#include "SDK.h"
#include <Windows.h>
#include "CommonDefine.h"
//#include "UCServerSDKTool.h"
//NOTE: ʵ�ʲ��ԣ���������ÿ���߳̽��м���DLL�����ֲ���������֤�����
// ����c#DLL���밲װnet4.0 ������������
//-------------------------------------------------------------------------*/
#define USE_SDK_CHECK	1

bool SDK::AwaitRequest(const char *szHttpAddr, AString &response, const char *postString /*= NULL*/)
{
	if (postString != NULL)
	{
		Auto<TaskPostRequest> t = StartTask(eTaskPostRequest);
		t->mHttpAddr = szHttpAddr;
		t->mReuestString = postString;
		t->AWait();
		response = t->mResponseString.c_str();
		return t->mbSucceed;
	}
	else
	{
		Auto<TaskGetRequest> t = StartTask(eTaskGetRequest);
		t->mReuestString = szHttpAddr;
		t->AWait();
		response = t->mResponseString.c_str();
		return t->mbSucceed;
	}
}

void SDK::RequestPost(const char *szHttpAddr, const char *postString, CallBack<const std::string&> mCallBack)
{
	Auto<TaskPostRequest> t = StartTask(eTaskPostRequest);
	t->mHttpAddr = szHttpAddr;
	t->mReuestString = postString;
	t->mCallBack = mCallBack;
}

void SDK::RequestGet(const char *requestString, CallBack<const std::string&> mCallBack)
{
	Auto<TaskGetRequest> t = StartTask(eTaskGetRequest);
	t->mReuestString = requestString;
	t->mCallBack = mCallBack;
}

void SDK::RequestGet(const char *requestString, CallBack<TaskGetRequestEx*> mCallBack, const AString &attachValue)
{
	Auto<TaskGetRequestEx> t = StartTask(eTaskGetRequestEx);
	t->mReuestString = requestString;
	t->mAttachValue = attachValue;
	t->mCallBack = mCallBack;
}

void SDK::RequestCheckUCLogin(const char *sid, AutoEvent requestEvt, bool bIs360)
{
#if USE_SDK_CHECK
	Auto<TaskCheckUCLogic> t = StartTask(eTaskCheckUCLogic);
	t->mWaitCallBackEvt = requestEvt;
	t->mSid = sid;
	t->mIs360 = bIs360;
#endif
}

void SDK::NotifyLoginData(const char *szSid, int roleDB, const AString &roleName, UInt64 roleCreateTime, int roleLevel, AutoEvent requestEvt, bool bIs360)
{
#if USE_SDK_CHECK
	//??? Ŀǰ��֧��360�������
	if (bIs360)
		return;

	Auto<NotifyLogicData> t = StartTask(eTaskNotifyLogic);
	t->mWaitCallBackEvt = requestEvt;
	t->mSid = szSid;
	t->mRoleDB = roleDB;
	//WARN_LOG("��ɫ����> [%s]", AString::getANIS(roleName.c_str()));
	t->mRoleName = AString::getANIS(roleName.c_str());
	t->mLevel = roleLevel;
	t->mCreateTime = roleCreateTime;
#endif
}

CsLock gLock;

std::string SDK::CheckUCLogin(const char *sid, std::string &info, bool bIs360)
{
	// QUICK SDK, ֱ��ʹ��CHttpClient ���л�ȡ
	std::string result;
	int re = -1;
	// ��������
	for (int i=0; i<2; ++i)
	{
		if (bIs360)
		{
			Array<AString> strList;
			if (AString::Split(sid, strList, " ", 2)>=2)
				re = mHttpClient.Post(strList[0].c_str(), strList[1].c_str(), result, 5);		
		}
		else
			re = mHttpClient.Get(sid, result, 5);
		if (re==0)
			break;
	}

	if (re!=0)
	{
		info = STRING(re);
		ERROR_LOG("SDK Request fail: Error code %d", re);
	}
	return result;
	//!!! SDKTool > return CheckLoginInfo(sid, info, bIs360);

//	CsLockTool l(gLock);
//
//#if !DEVELOP_MODE
//	if (mCheckLoginFun==NULL)
//	{
//#if _DEBUG
//		mDllHander=(__int64)LoadLibrary("UCServerSDKTool_d.dll");
//#else
//		mDllHander=(__int64)LoadLibrary("UCServerSDKTool.dll");
//#endif
//		if(mDllHander!=NULL)
//		{
//			mCheckLoginFun=(Dllfun)GetProcAddress((HINSTANCE)mDllHander, "CheckLoginInfo");
//			if(mCheckLoginFun==NULL)
//			{				
//				FreeLibrary((HINSTANCE)mDllHander);
//				mDllHander = NULL;
//				ERROR_LOG("CheckLoginInfo No find");
//			}
//		}
//		else
//			ERROR_LOG("���� SDK UCServerSDKTool dll ʧ��");
//	}
//
//	if(mCheckLoginFun!=NULL)
//	{
//		try{
//			return mCheckLoginFun(sid, info, bIs360);
//		}
//		catch(...)
//		{
//			ERROR_LOG("����CheckUCLogin����UC�ʺ�ʧ��");
//		}
//	}
//#endif
//	return std::string();
}

bool SDK::NotifyData(const char *szSid, int zoneID, const char *zoneName, int roleID, const char *roleName, unsigned __int64 roleCreateTime, int roleLevel)
{
	//???
	return false;

	//!!! SDKTool for uc 360, now use quick sdk, only use CHttpClient
	//return NotifyGameData(szSid, zoneID, zoneName, roleID, roleName, roleCreateTime, roleLevel);

//	CsLockTool l(gLock);
//
//#if !DEVELOP_MODE
//	if (mCheckLoginFun==NULL)
//	{
//#if _DEBUG
//		mDllHander=(__int64)LoadLibrary("UCServerSDKTool_d.dll");
//#else
//		mDllHander=(__int64)LoadLibrary("UCServerSDKTool.dll");
//#endif
//
//	}
//
//	if(mDllHander!=NULL && mNotifyFun==NULL)
//	{
//		mNotifyFun=(DllNotifyFun)GetProcAddress((HINSTANCE)mDllHander, "NotifyGameData");
//		if(mNotifyFun==NULL)
//		{				
//			//FreeLibrary(hdll);
//			//hdll = NULL;
//			ERROR_LOG("NotifyGameData No find");
//		}
//	}	
//
//	if(mNotifyFun!=NULL)
//	{
//		try{
//			return mNotifyFun(szSid, zoneID, zoneName, roleID, roleName, roleCreateTime, roleLevel);
//		}
//		catch(...)
//		{
//			ERROR_LOG("���� NotifyGameData ���µ�½��Ϣʧ��");
//		}
//	}
//#endif
//	return false;
}

SDK::SDK()
	//: mDllHander(NULL)
	//, mCheckLoginFun(NULL)
	//, mNotifyFun(NULL)
{
	RegisterTask(MEM_NEW DefineTaskFactory<TaskCheckUCLogic, eTaskCheckUCLogic>);
	RegisterTask(MEM_NEW DefineTaskFactory<NotifyLogicData, eTaskNotifyLogic>);
	RegisterTask(MEM_NEW DefineTaskFactory<TaskPostRequest, eTaskPostRequest>);
	RegisterTask(MEM_NEW DefineTaskFactory<TaskGetRequest, eTaskGetRequest>);
	RegisterTask(MEM_NEW DefineTaskFactory<TaskGetRequestEx, eTaskGetRequestEx>);
}
SDK::~SDK()
{
	//if (mDllHander!=NULL)
	//	FreeLibrary((HINSTANCE)mDllHander);
	//mDllHander = NULL;
}

void SDK::AnalysisHttpParam(const AString &requestData, NiceData &resultMsg)
{
	Array<AString> tempList;
	AString::Split(requestData.c_str(), tempList, "&", 100);
	for (int i = 0; i < tempList.size(); i++)
	{
		Array<AString> str;
		AString::Split(tempList[i].c_str(), str, "=", 2);

		if (str.size() == 2)
		{
			resultMsg[str[0].c_str()] = str[1].c_str();
		}
	}
}

//-------------------------------------------------------------------------*/