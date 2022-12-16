#pragma once

#ifndef _DBSHAREMEMTHREAD_H_
#define _DBSHAREMEMTHREAD_H_

#include "BaseThread.h"
#include "SQLUpdateManager.h"
#include "CRunConfig.h"
#include "Vesion.h"

class ShareSQLThread : public ServerThread
{
public:
    ShareSQLThread();

	virtual AString GetTitle() override;
	virtual void SetTitle(const AString &title);

	static const char * GetAppName()
	{
		return CRunConfig<ShareDBConfig>::mConfig.title.c_str();
	}

public:
    virtual void Process(void*)
    {
		if (mShareMemDBManager)
			mShareMemDBManager->Process();
    }

	virtual void LowProcess(int spaceTime)
	{
		ServerThread::LowProcess(spaceTime);
		//if (mShareMemDBManager)
			//mShareMemDBManager->LowProcess();
	}

    virtual void OnStart(void*);
    virtual void OnStop(void*) 
    {

    }
	// 每秒1万次Loop
    virtual int OnceTime(void) const { return LOGIC_PROCESS_ONCE_TIME; }
	// 低速每秒约100次
    virtual int OnceLowTime() const { return 10000; }

    virtual bool NotifyThreadClose();

	virtual void NotifyException() override;

    virtual void DoCommand(const AString &commandString, StringArray &paramArray) override;

	virtual StateDataType GetRunStateInfo(tNiceData &info) override;

public:
	class CShareMemMgr : public NetCloud::SQLUpdateManager
	{
	public:
		ShareSQLThread *mpThread;
		int mSaftCode = 0;

		CShareMemMgr(ShareSQLThread *pThread, int saftCode)
			: mpThread(pThread)
			, mSaftCode(saftCode)
		{}

		virtual int GetSafeCode() override { return mSaftCode; }
	};
	Hand<CShareMemMgr>			mShareMemDBManager;

	bool								mbRequestClose = false;
	bool								mbNeedRestart = false;
};



//-------------------------------------------------------------------------*


#endif //_DBSHAREMEMTHREAD_H_