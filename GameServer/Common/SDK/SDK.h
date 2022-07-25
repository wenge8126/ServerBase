#ifndef _INCLUDE_SDK_H_
#define _INCLUDE_SDK_H_

/*/-------------------------------------------------------------------------/
运营平台SDK接入
一般会有各种阻塞交互，实现异步功能
//-------------------------------------------------------------------------*/
#include "TaskManager.h"
#include "Event.h"
#include "HttpClient.h"
#include "CallBack.h"

#include "Component.h"

//  区别开发账号还是平台账号
#define SDK_KEY "_sdk_" 
// 支付码
#define SDK_SECRET "1f693ff72076e0bf5a9546448645c43d" 
// 网址
#define SDK_URL "https://juhesdk.3975ad.com/api/oauth/verify_token" 
// 激活码网址 正式
#define CODE_URL "https://activecode.3975ad.com/api/activecode/exchange/app_id/"
// 激活码网址 测试
#define CODE_URL_TEST "http://juhesdktest2.fgcq.info/api/activecode/exchange/app_id/278"
// 移动教室项目appid 
#define APPID 278
typedef std::string (*Dllfun)(const char *, std::string&, bool bIs360);
typedef bool (*DllNotifyFun)(const char *szSid, int zoneID, const char *zoneName, int roleID, const char *roleName, unsigned __int64 roleCreateTime, int roleLevel);

//-------------------------------------------------------------------------*/
class TaskGetRequestEx;
class CommonLib_Export SDK : public TaskManager
{
public:
	bool AwaitRequest(const char *szHttpAddr, AString &response, const char *postString = NULL);

public:
	void RequestPost(const char *szHttpAddr, const char *postString, CallBack<const std::string&> mCallBack);
	void RequestGet(const char *requestString, CallBack<const std::string&> mCallBack);
	void RequestGet(const char *requestString, CallBack<TaskGetRequestEx*> mCallBack, const AString &attachValue);

	void RequestCheckUCLogin(const char  *sid, AutoEvent requestEvt, bool bIs360);
	void NotifyLoginData(const char *szSid, int roleDB, const AString &roleName, UInt64 roleCreateTime, int roleLevel, AutoEvent requestEvt, bool bIs360);

	std::string CheckUCLogin(const char  *sid, std::string &info, bool bIs360);
	bool NotifyData(const char *szSid, int zoneID, const char *zoneName, int roleID, const char *roleName, unsigned __int64 roleCreateTime, int roleLevel);

public:
	SDK();
	~SDK();

	enum
	{
		eTaskNone,
		eTaskCheckUCLogic,
		eTaskNotifyLogic,
		eTaskPostRequest,
		eTaskGetRequest,
		eTaskGetRequestEx,
	};

	static void AnalysisHttpParam(const AString &requestData, NiceData &resultMsg);

protected:
	friend class TaskPostRequest;
	friend class TaskGetRequest;
	friend class TaskGetRequestEx;
	// 只能在后台线程内使用
	CHttpClient		mHttpClient;
//protected:
//	__int64 mDllHander;
//	Dllfun mCheckLoginFun;
//	DllNotifyFun mNotifyFun;
};

//-------------------------------------------------------------------------
// 异步请求组件
class HttpReqeustComponent : public NetCloud::Component
{
public:
	bool AwaitRequest(const char *szHttpAddr, AString &response, const char *postString = NULL)
	{
		return mSDK.AwaitRequest(szHttpAddr, response, postString);
	}

public:
	void RequestPost(const char *szHttpAddr, const char *postString, CallBack<const std::string&> mCallBack)
	{
		mSDK.RequestPost(szHttpAddr, postString, mCallBack);
	}
	void RequestGet(const char *requestString, CallBack<const std::string&> mCallBack)
	{
		mSDK.RequestGet(requestString, mCallBack);
	}

public:
	void Start() override
	{
		mSDK.InitThread();
		StartUpdate(0.001f);
	}

	void Update(float onceTime) override
	{
		mSDK.Process();
	}

	virtual void OnDestory() override
	{
		mSDK.Close();
	}

protected:
	SDK				mSDK;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------*/
class TaskCheckUCLogic : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		mResultAccount = (dynamic_cast<SDK*>(pTaskMgr))->CheckUCLogin(mSid.c_str(), mResultInfo, mIs360);
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{
		if (mWaitCallBackEvt)
			mWaitCallBackEvt->_OnEvent(this, typeid(TaskCheckUCLogic));
	}

	virtual void InitFree() override
	{
		mWaitCallBackEvt.setNull();
		mSid.setNull();
		mResultAccount.clear();
		mResultInfo.clear();
	}

public:
	AutoEvent			mWaitCallBackEvt;
	AString				mSid;
	bool				mIs360;

	std::string			mResultAccount;
	std::string			mResultInfo;

};

class NotifyLogicData : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		mResult = (dynamic_cast<SDK*>(pTaskMgr))->NotifyData(mSid.c_str(), 1, "野猪森林", mRoleDB, mRoleName.c_str(), mCreateTime, mLevel);
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{
		if (mWaitCallBackEvt)
			mWaitCallBackEvt->_OnEvent(this, typeid(NotifyLogicData));
	}

	virtual void InitFree() override
	{
		mWaitCallBackEvt.setNull();
		mSid.setNull();
		mRoleName.setNull();
		mCreateTime = 0;
		mLevel = 0;
		mRoleDB = 0;

		mResult = false;
	}

public:
	AutoEvent			mWaitCallBackEvt;
	AString				mSid;

	KEY					mRoleDB;
	AString				mRoleName;
	UInt64				mCreateTime;
	int					mLevel;

	bool				mResult;

};
//-------------------------------------------------------------------------*/
class TaskPostRequest : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		int re = (dynamic_cast<SDK*>(pTaskMgr))->mHttpClient.Post(mHttpAddr, mReuestString, mResponseString, 6);
		mbSucceed = re==0 || re==56;
		if (!mbSucceed)
			DEBUG_LOG("SDK Post fail %d> %s > %s ", re, mHttpAddr.c_str(), mReuestString.c_str());
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{
		mCallBack(mResponseString, mbSucceed);
	}

	virtual void InitFree() override
	{
		mCallBack.cleanup();
		mHttpAddr.clear();
		mReuestString.clear();
		mResponseString.clear();
		mbSucceed = false;
	}

	TaskPostRequest()
		: mbSucceed(false){}

public:
	CallBack<const std::string&> mCallBack;

	std::string			mHttpAddr;
	std::string			mReuestString;
	std::string			mResponseString;
	bool					mbSucceed;
};

//-------------------------------------------------------------------------*/
class TaskGetRequest : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		int re = (dynamic_cast<SDK*>(pTaskMgr))->mHttpClient.Get(mReuestString, mResponseString, 6);
		mbSucceed = re == 0 || re == 56;
		if (!mbSucceed)
			DEBUG_LOG("SDK Post fail %d> %s > %s ", re, mReuestString.c_str());
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{
		mCallBack(mResponseString, mbSucceed);
	}

	virtual void InitFree() override
	{
		mCallBack.cleanup();
		
		mReuestString.clear();
		mResponseString.clear();
		mbSucceed = false;
	}

	TaskGetRequest()
		: mbSucceed(false) {}

public:
	CallBack<const std::string&> mCallBack;
	
	std::string			mReuestString;
	std::string			mResponseString;
	bool					mbSucceed;
};
//-------------------------------------------------------------------------*/

class TaskGetRequestEx : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		int re = (dynamic_cast<SDK*>(pTaskMgr))->mHttpClient.Get(mReuestString, mResponseString, 6);
		mbSucceed = re == 0 || re == 56;
		if (!mbSucceed)
			DEBUG_LOG("SDK Post fail %d> %s > %s ", re, mReuestString.c_str());
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{
		mCallBack(this, mbSucceed);
	}

	virtual void InitFree() override
	{
		mCallBack.cleanup();

		mReuestString.clear();
		mResponseString.clear();
		mbSucceed = false;
	}

	TaskGetRequestEx()
		: mbSucceed(false) {}

public:
	CallBack<TaskGetRequestEx*> mCallBack;

	std::string			mReuestString;
	std::string			mResponseString;
	AString				mAttachValue;
	bool					mbSucceed;
};
//-------------------------------------------------------------------------*/
#endif //_INCLUDE_SDK_H_