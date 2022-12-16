#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"

#include "DefineMsgFactory.h"

#include "SCActor.h"
#include "MsgData.h"
#include "DBUser_t_account.h"
#include "ServerMsg.h"
#include "t_account.h"
#include "DBUser_t_resources.h"
#include "ServerToolMsg.h"


using namespace NetCloud;

//-------------------------------------------------------------------------
// 客户端请求或回复的消息
// 服务器LogicActor 收到后, 使用异步等待 ClientActor 再次异步请求至目标Actor
// ClientActor 接收到回复后, LogicAtor 构建新的回复消息, 回复至客户端
// Client 再根据等待着的Actor 返回回复结果
//-------------------------------------------------------------------------

class CenterThread;

//-------------------------------------------------------------------------
class AccountDBComponent : public DBUser_t_account
{
public:
	virtual void OnLoadRecord(ARecord record) override;
};

// 可动态更新的资源组件, 如 配置, UI图片等
class ResourcesComponent : public DBUser_t_resources
{
public:
	EasyMap<AString, ARecord> mResourcesList;
	ARecord mResourceListRecord;

public:
	void On(TR_UpdateSaveResource &req, RT_ReponseSaveResult &response, UnitID sender, int)
	{
		req.DumpLog();
		NOTE_LOG(" DATA size %d", req.mResourceData->dataSize());
		
		ARecord existRe = mResourcesList.find(req.mIndexName);
		if (existRe)
		{
			ResetRecord(existRe);
		}
		else
		{
			mDataRecord = LoadRecord(req.mIndexName.c_str());
			if (!mDataRecord)
				mDataRecord = mDBTable->CreateRecord(req.mIndexName.c_str(), true);
		}
		DATA() = req.mResourceData.getPtr();
		SetMD5(req.mMD5.c_str());
		SetTIME((int)TimeManager::Now());

		LowUpdate();

		if (req.mIndexName == CONFIG_LIST_INDEX)
		{
			mResourceListRecord = mDataRecord;
		}
		

		response.mInfo = u8"测试成功";
	}

	void On(CR_LoadResource &req, RC_ResponseResource &response, UnitID, int)
	{
		if (mResourceListRecord && req.mIndexName == CONFIG_LIST_INDEX)
		{
			ResetRecord(mResourceListRecord);
			response.mResourceData = dataDATA();
			response.mMD5 = strMD5();
			return;
		}

		ARecord existRe = mResourcesList.find(req.mIndexName);
		if (existRe)
		{
			ResetRecord(existRe);
			if (strMD5() == req.mMD5)
			{
				NOTE_LOG("%s 缓存有效, 直接返回", req.mIndexName.c_str());
				response.mResourceData = dataDATA();
				response.mMD5 = strMD5();
				return;
			}
		}

		if (ResetRecord(req.mIndexName.c_str()))
		{
			mResourcesList.insert(strNAME(), mDataRecord);

			response.mResourceData = dataDATA();
			response.mMD5 = strMD5();

			if (req.mIndexName == CONFIG_LIST_INDEX)
			{
				mResourceListRecord = mDataRecord;
			}
		}
		else
			response.mErrorCode = 1;
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(ResourcesComponent, TR_UpdateSaveResource, RT_ReponseSaveResult);
		REG_COMP_MSG(ResourcesComponent, CR_LoadResource, RC_ResponseResource);
	}
};

//-------------------------------------------------------------------------
// 用于接受处理客户端连接, 并响应客户端请求服务器Actor消息
//-------------------------------------------------------------------------
class AccountCenterActor : public SCActor
{
	struct AccountData 
	{
		Int64 mDBID;
		AString  mPassword;
	};

public:
	Hand< DBUser_t_account> mAccountDBUser;
	FastHash<AString, AccountData> mAccountHash;

public:
	// 创建帐号
	void On(AC_RequestCreateAccount &req, CA_ResponseCreateAccount &response, UnitID sender, int)
	{
		AccountData *pExistAccount = mAccountHash.findPtr(req.mAccount);
		if (pExistAccount != NULL)
		{
			response.mErrorCode = eErrorCode_ExistAccount;
			response.mDBID = pExistAccount->mDBID;
			return;
		}

		ARecord record = mAccountDBUser->GetTable()->GrowthNewRecord();
		if (record)
		{
			t_account tRecord(record);
			tRecord.ACCOUNT() = req.mAccount;
			//??? tRecord.PASSWORD() = req.mPassword;
			tRecord.INFO(true) = req.mInfoData.getPtr();
			record->SaveUpdate();
			response.mErrorCode = eErrorCodeNone;
			response.mDBID = record->get(0);
			AccountData d;
			d.mDBID = response.mDBID;
			//??? d.mPassword = req.mPassword;
			mAccountHash.insert(req.mAccount, d);
		}
		else
		{
			response.mErrorCode = eErrorCode_DBCreateFail;
		}

	}

	void On(AC_RequestAccountData &req, CA_ResponseAccountData &response, UnitID sender, int)
	{
		AccountData *pExistAccount = mAccountHash.findPtr(req.mAccount);
		if (pExistAccount != NULL)
		{
			//mAccountDBUser->LoadRecord(*pDBID);
			response.mErrorCode = eErrorCodeNone;
			response.mDBID = pExistAccount->mDBID;
			response.mPassword = pExistAccount->mPassword;
			return;
		}
		response.mErrorCode = eErrorCode_NoExistAccount;
	}

	void On(MSG_Test &req, SC_ResponseTest &response, UnitID sender, int)
	{
		NOTE_LOG("Http request : %s", req.dump().c_str());

		response.mInfo = "OK FOR HTTP response";
	}

public:
	CenterThread* GetLoginThread();


	virtual void Init() override
	{
		SCActor::Init();

		mAccountDBUser = AddComponent("AccountDBComponent");

		// 加载所有的
		Auto<LogicDBTable> t = mAccountDBUser->GetTable();

		if (!t)
		{
			ERROR_LOG("Load t_account table fail");
			return;
		}

		t->LoadAllRecord(mAccountDBUser.getPtr());
		NOTE_LOG("Load total account : %d", mAccountHash.size());

		// 资源组件
		AddComponent("ResourcesComponent");

		
	}

	virtual void OnLoadRecord(ARecord record)
	{
		AccountData d;
		d.mDBID = record->get(0);
		d.mPassword = record->get(2);
		AString account = record->get(1);
		mAccountHash.insert(account, d);
	}

public:
	void On(RQ_CreateDBTable &msg, RS_CreateDBTable &resp, UnitID senderID, int);

	void RegisterMsg() override
	{
		REG_ACTOR_MSG(AccountCenterActor, RQ_CreateDBTable, RS_CreateDBTable);
		REG_COMPONENT(AccountDBComponent);
		REG_COMPONENT(ResourcesComponent);		

		REG_ACTOR_MSG(AccountCenterActor, AC_RequestCreateAccount, CA_ResponseCreateAccount);
		REG_ACTOR_MSG(AccountCenterActor, AC_RequestAccountData, CA_ResponseAccountData);

		REG_ACTOR_MSG(AccountCenterActor, MSG_Test, SC_ResponseTest);
	}
};

