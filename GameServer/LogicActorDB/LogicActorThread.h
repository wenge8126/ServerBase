#pragma once

#ifndef _DBSHAREMEMTHREAD_H_
#define _DBSHAREMEMTHREAD_H_

#include "BaseThread.h"
#include "CRunConfig.h"
#include "Vesion.h"
#include "ActorManager.h"

class LogicActorThread : public ServerThread
{
public:
    LogicActorThread();

	virtual AString GetTitle() override;
	virtual void SetTitle(const AString &title);

	static const char * GetAppName()
	{
		return CRunConfig<LogicActorDBConfig>::mConfig.title.c_str();
	}

public:
    virtual void Process(void*)
    {
		if (mActorManager)
			mActorManager->Process();
    }

	virtual void LowProcess(int spaceTime)
	{
		ServerThread::LowProcess(spaceTime);
		if (mActorManager)
			mActorManager->LowProcess();
	}

    virtual void OnStart(void*);
    virtual void OnStop(void*) 
    {

    }
	// 每秒1万次Loop
    virtual int OnceTime(void) const { return 20; }
	// 低速每秒约100次
    virtual int OnceLowTime() const { return 10000; }

    virtual bool NotifyThreadClose();

	virtual void NotifyException() override;

    virtual void DoCommand(const AString &commandString, StringArray &paramArray) override;

	virtual StateDataType GetRunStateInfo(tNiceData &info) override;


public:
	class CDBMgr : public NetCloud::ActorManager
	{
	public:
		LogicActorThread *mpThread;

		CDBMgr(LogicActorThread *pThread, const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
			: NetCloud::ActorManager(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
			, mpThread(pThread) {}
	};
	Auto<CDBMgr>			mActorManager;
	HandActor					mWorkerActor;
	HandActor					mAccountActor;

	bool								mbRequestClose = false;
	bool								mbNeedRestart = false;
};



//-------------------------------------------------------------------------*


#endif //_DBSHAREMEMTHREAD_H_