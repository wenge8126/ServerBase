#pragma once

#ifndef _DBSHAREMEMTHREAD_H_
#define _DBSHAREMEMTHREAD_H_

#include "BaseThread.h"
#include "CRunConfig.h"
#include "Vesion.h"
#include "ActorManager.h"

class NoSQLDBThread : public ServerThread
{
public:
    NoSQLDBThread();

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
    virtual int OnceTime(void) const { return LOGIC_PROCESS_ONCE_TIME; }
	// 低速每秒约100次
    virtual int OnceLowTime() const { return 2000000; }

    virtual bool NotifyThreadClose();

	virtual void NotifyException() override;

    virtual void DoCommand(const AString &commandString, StringArray &paramArray) override;

	virtual StateDataType GetRunStateInfo(tNiceData &info) override;


public:
	class CDBMgr : public NetCloud::DBActorManager
	{
	public:
		NoSQLDBThread *mpThread;

		CDBMgr(NoSQLDBThread *pThread, const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
			: NetCloud::DBActorManager(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
			, mpThread(pThread) {}
	};
	Auto<CDBMgr>			mActorManager;
	HandActor					mWorkerActor;
	HandActor					mNoSQLDBActor;

	bool								mbRequestClose = false;
	bool								mbNeedRestart = false;
};



//-------------------------------------------------------------------------*


#endif //_DBSHAREMEMTHREAD_H_