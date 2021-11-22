#include "WebMsg.h"

#include "CWssServerNet.h"
#include "WebLoginThread.h"
#include "CommonDefine.h"
//#include "DBOperate.h"
#include "MsgStruct.h"
#include "ConfigEnum.h"

#include "MsgData.h"
#include "Md5Tool.h"
#include "TestDefine.h"

// 开服时,不使用SDK二次验证, 避免登陆长时间阻塞, 影响转化率
#define USE_SDK_ACCOUNT_CHECK			0

#define SET_ALLOW_OPERATE(op) mAllowOperateList.insert(#op, true);

class ToClientEvent : public Logic::BaseEvent
{
public:
	ToClientEvent()
	{
		mData = MEM_NEW NiceData();
	}

public:
	virtual void SetData(AutoNice &scrData) { mData = scrData; }
	virtual tNiceData& GetData(void) { return *mData; }
	virtual const tNiceData& GetData(void) const { return *mData; }

	virtual NICEDATA_TYPE _RestoreDataType(DataStream *scrData) override
	{
		NICEDATA_TYPE niceType = BaseEvent::_RestoreDataType(scrData);
		if (niceType != NICEDATA)
		{
			ERROR_LOG("[%s] 必须恢复 NiceData 类型的数据, 当前 [%d]", GetEventName(), niceType);
			return NULL_NICEDATA;
		}
		return niceType;
	}

public:
	AutoNice			mData;
};

#if TEST_SERVER
class CL_TestLoginEvent : public CL_LoginEvent
{
public:
	virtual bool _DoEvent() override;

	virtual void SendResponce(AutoEvent hRespEvent) override;

};
#endif

void WebMsg::Register(BaseWebServer *pNet)
{
	pNet->RegisterMsg(WEB_REQUST_DB, MEM_NEW Logic::EventFactory<CL_DBOpreate>());
	pNet->RegisterMsg(CL_Login, MEM_NEW Logic::EventFactory<CL_LoginEvent>());
	pNet->RegisterMsg(LC_Notify, MEM_NEW Logic::EventFactory<Logic::CEvent>());
	pNet->RegisterMsg(LC_EventMsg, MEM_NEW Logic::EventFactory<ToClientEvent>());
	pNet->RegisterMsg(CL_EventMsg, MEM_NEW Logic::EventFactory<CL_EventMsgEvent>());
	pNet->RegisterMsg(CL_RequestResource, MEM_NEW Logic::EventFactory<CL_RequestResourceEvent>());
	pNet->RegisterMsg(CL_CodeActive, MEM_NEW Logic::EventFactory<CL_CodeActiveEvent>());
#if TEST_SERVER
	pNet->RegisterMsg(CL_TestLogin, MEM_NEW Logic::EventFactory <CL_TestLoginEvent>());
#endif
	CL_DBOpreate::InitAllowList();
}

EasyHash<AString, bool> CL_DBOpreate::mAllowOperateList;
void CL_DBOpreate::InitAllowList()
{
	// true 无登陆也可以请求, false 登陆与否, 都不允许请求, 不在此列表的 默认为 必须登陆后 才可执行
	mAllowOperateList.insert("t_load_configtable", true);
	mAllowOperateList.insert("t_load_configlist", true);
	mAllowOperateList.insert("RQ_NetPing", true);
	mAllowOperateList.insert("d_init_game_data", false);
	mAllowOperateList.insert("d_chat", false);

	SET_ALLOW_OPERATE(e_save_scene_path);
	
}

//-------------------------------------------------------------------------
int CL_LoginEvent::msTotalNowLogin = 0;
//-------------------------------------------------------------------------

bool CL_LoginEvent::_DoEvent()
{
	if (!mNetConnect)
		return true;

	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
	// 状态高位表示bad, 返回逻辑服务器火爆繁忙
	if ((pThread->mLogicDBState & 0xFF00) > 0)
	{
		int error = EnumNoticeConfig::eLogicServerBuy;
		
		// 只有逻辑维护时, 返回游戏服务器维护
		if (!( (pThread->mLogicDBState & (~eLogicMaintain)) > 0) )
			error = eLogicDBMaintain;

		GetResponseEvent()["RESULT"] = false;
		AString info = "Login fail, Run operate fail > ";
		info += error;
		GetResponseEvent()["ERROR"] = info;
		GetResponseEvent()["ERROR_CODE"] = error;
		Finish();
		return true;
	}

	if (msTotalNowLogin >= 3)
	{
		NOTE_LOG("!!!!!! Now login count %d >= 3", msTotalNowLogin);
	}

	++msTotalNowLogin;

	AutoNice sdkData = (tNiceData*)get("SDK");
	if (sdkData)
	{
		// 如果开区时间未到, 则直接返回错误
		//WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
		//if (!pThread->IsOpen())
		//{
		//	GetResponseEvent()["ERROR_CODE"] = EnumNoticeConfig::eServerError;
		//	GetResponseEvent()["ERROR"] = u8"服务区未开放";
		//	Finish();
		//	return true;
		//}
		//SDKLogin(sdkData);
	}
	else
		TestAccountLogin();
	
	return true;
}


void CL_LoginEvent::TestAccountLogin()
{
	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());

	UInt64 value = pThread->mLoginActor->GetID().value;
	
	GetData()["SERVER_ID"] = pThread->mServerInfo.SERVER_ID;
	mAccount = get("ACCOUNT");

	if (mAccount.length() <= 0 || strstr(mAccount.c_str(), SDK_KEY) == mAccount.c_str())
	{
		GetResponseEvent()["ERROR"] = "SDK reqeust check fail";
		GetResponseEvent()["ERROR_CODE"] = EnumNoticeConfig::eAccountCheckFail;
		Finish();
		return;
	}

	int nResult = eNoneError;
	AutoNice checkResult; // = pThread->mAccountUserUnit->AwaitRequestDB("t_account", "", "g_check_account", &GetData(), 10000, &nResult);
	if (checkResult)
	{
		GetResponseEvent()["NOTICE"] = checkResult["NOTICE"];
		CheckLoginResult(checkResult, nResult);
		return;
	}
	else
	{
		GetResponseEvent()["ERROR_CODE"] = EnumNoticeConfig::eServerError;
		GetResponseEvent()["RESULT"] = false;
		GetResponseEvent()["ERROR"] = "Login fail, Request login fail";
		Finish();
	}
}

void CL_LoginEvent::CheckLoginResult(AutoNice resultData, int  nResult)
{
	if (nResult != eNoneError)
	{
		GetResponseEvent()["RESULT"] = false;
		AString info = "Login fail, Run operate fail > ";
		info += nResult;
		GetResponseEvent()["ERROR"] = info;
		GetResponseEvent()["ERROR_CODE"] = EnumNoticeConfig::eAccountCheckFail;
		Finish();
		return;
	}

	if (!mNetConnect)
		return;

	int checkResult = resultData["RESULT"];
	if (checkResult != eNoneError)
	{
		GetResponseEvent()["RESULT"] = false;
		AString info = "Login fail, Run operate fail > ";
		info += checkResult;
		GetResponseEvent()["ERROR"] = info;
		GetResponseEvent()["ERROR_CODE"] = checkResult;
		Finish();		
		return;
	}

	AString checkIp = resultData["CHECK_BLACK_IP"];
	if (checkIp.length() > 0)
	{
		if (checkIp != mNetConnect->GetIp())
		{
			GetResponseEvent()["RESULT"] = false;
			AString info = "Login fail, Run operate fail > ";
			info += eAccountInBlackList;
			GetResponseEvent()["ERROR"] = info;
			GetResponseEvent()["ERROR_CODE"] = eAccountInBlackList;
			Finish();
		}
	}

	Int64 id = resultData["ID"];
			
	Hand<BaseWebServer> net = mNetConnect->GetNetHandle();
	Hand<BaseWebConnect> existConn = net->GetConnect(id);
	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
	UInt64 value = pThread->mLoginActor->GetID().value;
	if (id > 0 && existConn && existConn != mNetConnect) // 本Login进程内, 通知顶号 设置在初始化数据gamedata 之后
	{
		//pThread->mLoginActor->AwaitRequestDB("t_data", "", "d_data_remove", resultData.getPtr(), 10000);
		//if (existConn)
		//	existConn->ForeClose();
		WARN_LOG("account:%s Ding Hao playerID:%lld unitId:%lld", resultData["ACCOUNT"].c_str(), id, value);
	}
	else
	{
		WARN_LOG("account:%s Login playerID:%lld  unitId:%lld", resultData["ACCOUNT"].c_str(), id, value);
	}

	
	GetResponseEvent()["RESULT"] = true;			

	//// 初始化数据, 如果出生则进行创建
	resultData["ACCOUNT"] = mAccount;
	int nInitResult = 0;
	
	resultData["LOGIN_UNIT_ID"] = value;

	AutoNice initGameData; // = pThread->mLoginActor->AwaitRequestDB("t_data", STRING(id), "d_init_game_data", resultData.getPtr(), 20000, &nInitResult);
	if (nInitResult != eNoneError || !initGameData)
	{
		GetResponseEvent()["RESULT"] = false;
		GetResponseEvent()["ERROR_CODE"] = EnumNoticeConfig::eServerError;
	}
	else
	{
		if (!mNetConnect)
			return;

		GetResponseEvent()["OPT"] = initGameData["OPT"];
		id = initGameData["ID"];
		if (id <= 0)
		{
			GetResponseEvent()["ERROR_CODE"] = EnumNoticeConfig::eServerError;
			GetResponseEvent()["RESULT"] = false;
			Finish();
			mNetConnect->SetRemove(true);
			return;
		}
		Hand<BaseWebConnect> player = mNetConnect;
		if (!player)
		{
			Finish();
			return;
		}
		player->InitNetUnit(id);
		GetResponseEvent()["ID"] = id;
		if (initGameData["IS_BIRTH"])
		{
			NiceData bindParam;
			bindParam["ACCOUNT"] = mAccount;			
			bindParam["SERVER_ID"] = pThread->mServerInfo.SERVER_ID;
			bindParam["DBID"] = id;
			
			AutoNice saveID; // = pThread->mAccountUserUnit->AwaitRequestDB("t_account", "", "g_account_bind_dbid", &bindParam, 10000, &nResult);
			if (!saveID || !saveID["RESULT"])
			{
				ERROR_LOG("Bind account fail > %s : %s", mAccount.c_str(), STRING(id));
			}
			else
				GetResponseEvent()["RESULT"] = true;
		}
		else
			GetResponseEvent()["RESULT"] = true;
	}

	Finish();
}
//-------------------------------------------------------------------------
bool CL_DBOpreate::_DoEvent()
{
	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
	AString opFuntionName = get("DB");
	if (opFuntionName.length() <= 0)
	{
		GetResponseEvent()["RESULT"] = false;
		GetResponseEvent()["ERROR"] = "OP name is empty";
		Finish();
		return true;
	}

	bool *pbAllow = mAllowOperateList.findPtr(opFuntionName);
	if (pbAllow != NULL)
	{
		if (!*pbAllow)
		{
			ERROR_LOG("No allow DB opereate > %s", opFuntionName.c_str());
			GetResponseEvent()["RESULT"] = false;
			GetResponseEvent()["ERROR"] = "No allow DB opereate";
			Finish();
			return true;
		}
	}
	else
	{
		Hand<BaseWebConnect> player = mNetConnect;
		if (!player || player->mDBID <= 0)
		{
			DEBUG_LOG("Player login fail or no login, can not run DB opreate");
			GetResponseEvent()["RESULT"] = false;
			GetResponseEvent()["NEED_LOGIN"] = true;
			GetResponseEvent()["ERROR"] = "Player no login, or login fail";
			Finish();
			return TRUE;
		}
		GetData()["_ID_"] = player->mDBID;
	}

	GetData().remove("DB");
	//DEBUG_LOG("** %s ****** %s",  opFuntionName.c_str(), GetData().dump().c_str());
	bool bRe = false; // pThread->mLoginActor->RequestDB("t_data", "", opFuntionName.c_str(), &GetData(), 120000, CloudDBCall(&CL_DBOpreate::OnLoad, this));
	if (!bRe)
	{
		GetResponseEvent()["RESULT"] = false;
		GetResponseEvent()["ERROR"] = "Reques DB fail";
		Finish();		
	}
	return true;
}

void CL_DBOpreate::OnLoad(NetCloud::RequestDBOperatePak *pDB, AutoNice resultData, int nResult)
{
	if (!mNetConnect || !GetEventFactory() || !GetResponseEvent() || !GetResponseEvent()->GetEventFactory())
		return;

	if (nResult == eNoneError)
	{
		GetResponseEvent()->GetData().append(*resultData, true);

		GetResponseEvent()["RESULT"] = true;

		Auto<DataBuffer> respData = (DataStream*)resultData["DATA"];
		if (respData && respData->dataSize() >= 1024)
		{
			//AutoData zipData = MEM_NEW DataBuffer();
			//if (respData->ZipData(zipData, 0, 0, 0, MAX_SPEED) > 0)
			//{
			//	GetResponseEvent()->GetData()["DATA"] = zipData.getPtr();
			//	GetResponseEvent()->GetData()["_ZIP_"] = true;
			//}		

			// 使用后台任务进行压缩
			WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
			Auto<BackZipMsgTask> task = pThread->mTaskSystem->StartTask(eDBMsgTask_ZipData);
			task->mMsgData = respData;
			task->mCallEvent = GetSelf();

			return;
		}	
		
		//if (get("DB").string()=="RQ_FinishAllResearchTask")
		//	DEBUG_LOG("==== Send opreate response > %s\r\n%s\r\n----------------------\r\n%s", get("DB").string().c_str(), resultData->dump().c_str(), GetResponseEvent()->GetData().dump().c_str());
	}
	else
	{
		GetResponseEvent()["RESULT"] = false;
		GetResponseEvent()["ERROR"] = nResult;
		if (nResult == eDBMaintain)
			GetResponseEvent()["NEED_LOGIN"] = true;
	}
	Finish();
}

bool CL_EventMsgEvent::_DoEvent()
{
	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
	Hand<BaseWebConnect> player = mNetConnect;

	if (player->mDBID <= 0)
	{
		DEBUG_LOG("Player no login");
		return true;
	}

	AString msgEventName = get("NAME");
	//AutoNice msgData = (tNiceData*)get("MSG_DATA");
	//if (msgEventName == "CHAT")
	//{		
	//	msgData["_ID_"] = player->mDBID;
	//	pThread->mLoginActor->RequestDB("t_data", STRING(player->mDBID), "d_chat", msgData.getPtr(), 10000, CloudDBCall());
	//}
	//else
	{
		ERROR_LOG("%s no process", msgEventName.c_str());
	}

	return true;
}

//-------------------------------------------------------------------------
EasyHash<AString, CL_RequestResourceEvent::CacheData> CL_RequestResourceEvent::msCacheList;

// 请求资源数据
bool CL_RequestResourceEvent::_DoEvent()
{
	AString md5 = get("MD5");
	AString resName = get("NAME");

	CacheData *pData = msCacheList.findPtr(resName);
	if (pData != NULL && pData->mMD5 == md5)
	{
		GetResponseEvent()["DATA"] = pData->mData.getPtr();
		GetResponseEvent()["MD5"] = md5;
		Finish();
		return true;
	}

	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
	// t_resource
	//!!! pThread->mLoginActor->RequestDB("t_resource", "", "g_load_resource", &GetData(), 10000, CloudDBCall(&CL_RequestResourceEvent::OnLoadResource, this) );

	return true;
}

void CL_RequestResourceEvent::OnLoadResource(NetCloud::RequestDBOperatePak *pDB, AutoNice resultData, int nResult)
{
	if (resultData)
	{
		GetResponseEvent()->GetData().append(*resultData, true);
		if (nResult == eNoneError)
		{
			AutoData data = (DataStream*)resultData["DATA"];
			if (data)
			{
				AString resName = get("NAME");
				CacheData *pData = msCacheList.findPtr(resName);
				if (pData != NULL)
				{
					pData->mData = data;
					pData->mMD5 = resultData["MD5"];
				}
				else
				{
					CacheData d = { resultData["MD5"].string(), data };
					msCacheList.insert(resName, d);
				}
			}
		}
	}
	GetResponseEvent()["RESULT"] = nResult;
	Finish();
}

//void CL_RequestResourceEvent::SendResponce(AutoEvent hRespEvent)
//{
//	Hand< WEB_SendDataPartEvent> bigEvt = StartEvent("WEB_SendDataPartEvent");
//	bigEvt->mNetConnect = mNetConnect;
//	bigEvt->SendBigEvent(hRespEvent);
//}

//-------------------------------------------------------------------------

bool CL_CodeActiveEvent::_DoEvent()
{
	bool isSdk = get("SDK");
	AString code = get("CODE");

	Hand<BaseWebConnect> player = mNetConnect;
	WebLoginThread *pThread = ((WebLoginThread*)mNetConnect->GetUserData());
	int nResult = eNoneError;
	GetData()["_ID_"] = player->mDBID;
	AutoNice ret; // = pThread->mLoginActor->AwaitRequestDB("t_data", STRING(player->mDBID), "g_use_code", &GetData(), 10000, &nResult);
	if (ret && nResult == eNoneError)
	{
		AString str;
		str.Format("serial=%s&server_id=%d&server_name=%s&role_id=%lld&role_name=%s&ext=%s"
			, code.c_str()
			, pThread->mServerInfo.SERVER_ID
			, pThread->mServerInfo.SERVER_NAME.c_str()
			, player->mDBID
			, ret["role_name"].c_str()
			, code.c_str()
		);
		
		//NOTE_LOG("info ==> %s", str.c_str());
		AString str1;
		str1.Format("%s&key=%s"
			, str.c_str()
			, "31576c9057567746d480d09ba22124b9" //应用秘钥
		);

		MD5 md(str1.c_str());
		AString strMD5 = md.toString();
		strMD5.toLower();
		//NOTE_LOG("sign ==> %s", strMD5.c_str());
		AString newStr;
		newStr.Format("%s&version=%s&client_type=%d&sign=%s"
			, str.c_str()
			, pThread->mServerInfo.VERSION.c_str() // 扩展信息，研发发货时候原样返回
			, isSdk ? 1 : 2 // 客户端类型 1移动端 2pc端，2.0版本（包含2.0）才需要传递
			, strMD5.c_str() // 签名
		); 
		// NOTE_LOG("msg ==> %s", newStr.c_str());
	
		AString url(CODE_URL_TEST);
		int app_id = ret["app_id"];
		if (isSdk)
			url.Format("%s%d", CODE_URL, app_id);

		WARN_LOG("newStr ==> %s", newStr.c_str());
		NiceData tempCheckParam;
		AString response;
		if (pThread->mSdkMgr.AwaitRequest(url.c_str(), response, newStr.c_str()))
		{
			tempCheckParam.FullJSON(response.c_str());
			// NOTE_LOG("code Response ==> %s", tempCheckParam.dump().c_str());
			if ((int)tempCheckParam["code"] != 200)
			{
				GetResponseEvent()["ERROR"] = tempCheckParam["message"];
			}
			else
			{
				GetResponseEvent()["ERROR"] = u8"兑换码兑换成功";
			}
			
		}
	}

	Finish();
	return true;
}


#if TEST_SERVER

// 默认异步执行
bool CL_TestLoginEvent::_DoEvent()
{
	auto &config = CRunConfig<LoginConfig>::mConfig;
	static int nBeginAccount = config.worker_id;

	AString account = "Test_";
	account += nBeginAccount++;

	set("ACCOUNT", account);

	TestAccountLogin();
	return true;
}


void CL_TestLoginEvent::SendResponce(AutoEvent hRespEvent)
{
	hRespEvent["ACCOUNT"] = get("ACCOUNT");
	CL_LoginEvent::SendResponce(hRespEvent);
}

#endif