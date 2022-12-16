#pragma once

#ifndef _INCLUDE_WEBLOGINTHREAD_H_
#define _INCLUDE_WEBLOGINTHREAD_H_

#include "CoroutineTool.h"
#include "TaskManager.h"
//#include "NetNode.h"
#include "BaseThread.h"

#include "SDK/SDK.h"
#include "LoginMsgData.h"
#include "CRunConfig.h"
#include "TaskSystem.h"
#include "ActorManager.h"
#include "HttpComponect.h"
#include "TcpComponent.h"
#include "WssWebComponent.h"
#include "NoSQLComponent.h"

#include "TestNoSQLUserComponent.h"
#include "Test2Component.h"
#include "ItemDataComponent.h"
#include "LoginNetComponect.h"
#include "HttpPostNet.h"

using namespace NetCloud;


template<bool>
class WebPlayer;

class WebLoginThread : public ServerThread
{
public:
	WebLoginThread();
	~WebLoginThread()
	{
		mServerList._free();
	}

	virtual AString GetTitle() override;
	virtual void SetTitle(const AString &title);

	static bool IsAccountWeb();
	static const char* GetAppName()
	{
		return CRunConfig<LoginConfig>::mConfig.title.c_str();
	}

	static bool IsWss();

public:

	virtual int OnceTime(void) const { return LOGIC_PROCESS_ONCE_TIME; }

	virtual int OnceLowTime() const { return 100000; }


	virtual void OnStart(void*);
	virtual void OnStop(void*) {}

	virtual bool NotifyThreadClose() override;

	virtual void Process(void*) override;

	virtual void LowProcess(int spaceTime) override
	{
		ServerThread::LowProcess(spaceTime);
		if (mActorManager)
			mActorManager->LowProcess();
	}

	virtual bool StartError() { return !mbStartOk; }
	virtual bool LogicError() { return false; }

	virtual void NotifyException() override;

	virtual void DoCommand(const AString &commandString, StringArray &paramArray) override;

	template<bool bSSL>
	void OnPlayerDisconnect(WebPlayer<bSSL> *pLayer);

	void OnLoginDBStart(int dbCode);

	void ReadyUpdate()
	{
		mbStartOk = false;
	}

	void ReadyOK()
	{
		mbStartOk = true;
		ServerThread::OnStart(NULL);
	}

	StateDataType GetRunStateInfo(tNiceData &info) override;
	
	// 获取服务器列表
	AString GetServerList();

	void UpdateOpenTime()
	{
		mOpenTime = TimeManager::AnalyzeTime(mServerInfo.OPEN_TIME);
	}

	bool IsOpen()
	{
		return mbStartOk && (mOpenTime <= 0 || mOpenTime<=TimeManager::Now());
	}


public:
	HandActor					mLoginActor;

	SDK								mSdkMgr;
	AutoNice						mServerList;
	AString							mServerListJson;
	g_register_login			mServerInfo;
	UInt64							mOpenTime = 0;
	bool								mbStartOk = false;
	bool								mbNeedRestart = false;
	int									mDBWorkerID = 0;
	StateDataType				mLogicDBState = 0;

	Hand<TaskSystem>	mTaskSystem;
	Auto<NetCloud::ActorManager>		mActorManager;

	HttpPostNet					mHttpClientNet;
};

//-------------------------------------------------------------------------

class LoginActorManager : public NetCloud::ActorManager
{
public:
	LoginActorManager(WebLoginThread *pThread, const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
		: NetCloud::ActorManager(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
		, mpThread(pThread)
	{
	}

public:
	WebLoginThread  *mpThread = NULL;
};

//class ClientProtocol : public BaseProtocol
//{
//public:
//	Auto<tBaseMsg> On()
//};

class PlayerItemComp : public ItemDataComponent
{
public:
	RecordArray		mItemData;
	AutoTable			mItemTable = CreateTable();

	PlayerItemComp()
	{
		mItemData.Init("Test");
	}

	ARecord CreateItem()
	{
		ARecord itemRe = mItemTable->NewRecord();
		mItemData.Insert(itemRe, this);
		return itemRe;
	}
};





#endif //_INCLUDE_WEBLOGINTHREAD_H_