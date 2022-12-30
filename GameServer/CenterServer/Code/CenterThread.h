#pragma once

#ifndef _INCLUDE_WEBLOGINTHREAD_H_
#define _INCLUDE_WEBLOGINTHREAD_H_

#include "CoroutineTool.h"
#include "TaskManager.h"
//#include "NetNode.h"
#include "BaseThread.h"

#include "SDK.h"

#include "CRunConfig.h"
#include "TaskSystem.h"
#include "ActorManager.h"
#include "HttpComponect.h"

using namespace NetCloud;


template<bool>
class WebPlayer;

class CenterThread : public ServerThread
{
public:
	CenterThread();
	~CenterThread()
	{
		mServerList._free();
	}

	virtual AString GetTitle() override;
	virtual void SetTitle(const AString &title);

	
	static const char* GetAppName();


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

class CenterActorManager : public NetCloud::DBActorManager
{
public:
	CenterActorManager(CenterThread *pThread, const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
		: NetCloud::DBActorManager(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
		, mpThread(pThread)
	{
	}

	virtual int NeedCacheTable(const AString &tableName) override
	{
		if (tableName == "t_account" 
			|| tableName == "t_user" 
			|| tableName == "t_userdata" 
			|| tableName == "t_goods" 
			|| tableName=="t_commodity"
			|| tableName=="t_commodity_data1"
			|| tableName=="t_template"
			)
			return 60;

		return 0;
	}

public:
	CenterThread  *mpThread = NULL;
};




#endif //_INCLUDE_WEBLOGINTHREAD_H_