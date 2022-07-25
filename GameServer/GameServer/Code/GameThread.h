#pragma once

#ifndef _INCLUDE_WEBLOGINTHREAD_H_
#define _INCLUDE_WEBLOGINTHREAD_H_

#include "CoroutineTool.h"
#include "TaskManager.h"
//#include "NetNode.h"
#include "BaseThread.h"

#include "SDK/SDK.h"

#include "CRunConfig.h"
#include "TaskSystem.h"
#include "ActorManager.h"
#include "HttpComponect.h"

using namespace NetCloud;


template<bool>
class WebPlayer;

class GameThread : public ServerThread
{
public:
	GameThread();
	~GameThread()
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


public:
#if _DEBUG
	virtual int OnceTime(void) const { return 100; }
#else
	virtual int OnceTime(void) const { return 100; }
#endif
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
		//mOpenTime = TimeManager::AnalyzeTime(mServerInfo.OPEN_TIME);
	}

	bool IsOpen()
	{
		return mbStartOk && (mOpenTime <= 0 || mOpenTime<=TimeManager::Now());
	}


public:
	HandActor					mLoginActor;

	AutoNice						mServerList;
	AString							mServerListJson;

	UInt64							mOpenTime = 0;
	bool								mbStartOk = false;
	bool								mbNeedRestart = false;
	int								mDBWorkerID = 0;
	StateDataType				mLogicDBState = 0;

	Hand<TaskSystem>	mTaskSystem;
	Auto<NetCloud::ActorManager>		mActorManager;
};

//-------------------------------------------------------------------------

class AccountActorManager : public NetCloud::ActorManager
{
public:
	AccountActorManager(GameThread *pThread, const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
		: NetCloud::ActorManager(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
		, mpThread(pThread)
	{
	}

public:
	GameThread  *mpThread = NULL;
};




#endif //_INCLUDE_WEBLOGINTHREAD_H_