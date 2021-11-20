
#ifndef _INCLUDE_LOGTHREAD_H_
#define _INCLUDE_LOGTHREAD_H_

#include "WorkThread.h"
#include "Lock.h"
#include <list>
#include "ThreadEvent.h"
#include "Common.h"
#include "DataStream.h"
#include "Auto.h"


class BaseCommon_Export_H LogTask
{
public:
	virtual ~LogTask() {}

public:
	char szLogBuffer[LOG_INFO_MAX_LENGTH];

	bool mbPrint;
};

//-------------------------------------------------------------------------
class BaseCommon_Export_H  LogThreadManager : public WorkThread
{ 
	friend class ThreadLog;
public:
	LogThreadManager()
		: mpLogFile(NULL)
	{
		mWaitEvent = event_create(false, true);
	}

	virtual ~LogThreadManager()
	{
		Close();
		if (mpLogFile!=NULL)
			fclose(mpLogFile);
		mpLogFile = NULL;

		for (int i = 0; i < mLogPool.size(); ++i)
		{
			delete mLogPool[i];
		}
		mLogPool.clear(true);

		for (int i = 0; i < mFreeList.size(); ++i)
		{
			delete mFreeList[i];
		}
		mFreeList.clear(true);

		for (int i = 0; i < mLogList.size(); ++i)
		{
			delete mLogList[i];
		}
		mLogList.clear(true);
	}

public: 
	virtual LogTask* NewLogTask() { return new LogTask(); }	

public:
	void AppendLog(LogTask *logTask)
	{
		mLogListLock.lock();
		mLogList.push_back(logTask);
		mLogListLock.unlock();
		event_set(mWaitEvent);
	}

	LogTask* AlloctLogTask()
	{
		CsLockTool  t(mFreeListLock);

		if (mFreeList.empty())
		{
			mPoolLock.lock();
			mFreeList.swap(mLogPool);
			mPoolLock.unlock();
		}

		if (!mFreeList.empty())
		{
			LogTask *p = mFreeList.back();
			mFreeList.pop_back();
			//!!!memset(p->szLogBuffer, 0, LOG_INFO_MAX_LENGTH);
			//PRINT(" @ last count [%u]\n", mFreeList.size());
			return p;
		}
		
		return NewLogTask();
	}

	virtual bool needWait(){ return true; }

	virtual void Print(const char *szInfo) { TableTool::PrintInfo(szInfo); }

public:
	virtual void backProcess() {}
	virtual void backWorkThread(void)
	{
		Array<LogTask*> tempList;

		while (IsActive())
		{
			backProcess();
			if (needWait())
				event_timedwait(mWaitEvent, 10000);

			mLogListLock.lock();
			tempList.swap(mLogList);
			mLogListLock.unlock();

			for (int i=0; i<tempList.size(); ++i)
			{
				LogTask *p = tempList[i];
//#if __SERVER__ && __WINDOWS__ && !DEVELOP_MODE
				if (p->mbPrint)
					Print(p->szLogBuffer);
//#endif
				__writeLog(p->szLogBuffer, p);
				//delete *it;
			}
			if (mpLogFile!=NULL)
			{
				fflush(mpLogFile);
			}
			mPoolLock.lock();
			for (int i = 0; i < tempList.size(); ++i)
			{
				mLogPool.push_back(tempList[i]);
			}
			mPoolLock.unlock();

			tempList.clear(false);
		}
	}

	virtual void onBeforeClose(void) 
	{
		event_set(mWaitEvent);
	}

	virtual void __writeLog(const char *szLog, LogTask *pLogTask)
	{
		if (mpLogFile!=NULL)
		{
			fprintf(mpLogFile, "%s", szLog);
			//fflush(mpLogFile);
		}
	}

protected:
	FILE								*mpLogFile;
	CsLock								mLogListLock;
	Array<LogTask*>		mLogList;		// 当前使用的
	event_handle			mWaitEvent;

	Array<LogTask*>		mLogPool;		// 从后台线程收集不再使用的
	CsLock							mPoolLock;

	Array<LogTask*>		mFreeList;		// 在主线通过与mLogPool交换回来不用的

	CsLock					mFreeListLock;
};



#endif //_INCLUDE_LOGTHREAD_H_