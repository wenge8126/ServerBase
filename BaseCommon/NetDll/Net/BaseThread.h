/********************************************************************
	created:	2013/09/07
	created:	7:9:2013   11:19
	filename: 	C:\Work\BaseCommon\ServerBase\Base\BaseThread.h
	file path:	C:\Work\BaseCommon\ServerBase\Base
	file base:	BaseThread
	file ext:	h
	author:		Yang WenGe
	
	purpose:	�����߳�, һ�㴦�����߼�, Ϊ����һ��������ʵ�ֶ�������߳�

	NOTE:		1 �����ر�: NotifyThreadClose() ʱ��������
				2 �������Ҫ�ȴ�, ���ܵ��ô˶���� NotifyThreadClose �� CanClose
				3 �ȴ��¼���ɺ�, ���� CanClose ����߳������ر�
*********************************************************************/
#ifndef _INCLUDE_BASETHREAD_H_
#define _INCLUDE_BASETHREAD_H_

#include "WorkThread.h"

#include "EventCenter.h"
//#include "NetHandle.h"
#include "NetHead.h"

#include "TimeManager.h"
#include "CallBack.h"

//-------------------------------------------------------------------------
#define LOGIC_PROCESS_ONCE_TIME		(10000)

//-------------------------------------------------------------------------
namespace Logic
{
	class tEventCenterManager;
}

typedef CallBack<float>		ProcessFunction;

class tSafeCheck;
class tNetHandle;
class tDBManager;
class DataTableManager;
//-------------------------------------------------------------------------
class Net_Export BaseThread : public WorkThread
{
    friend class tBaseServer;

public:
	// Main thread wait work thread run, Until call 'CanStop' finish work thread
	virtual void MainWaitWork();
	
	// Average loop ( microsecond 2021/4/11 һ��ѭ��ʹ�ö���΢��)
	virtual double GetAverageLoopTime(){ return (double)((TimeManager::NowTick()- mLoopStartMilSecond) / mTotalLoopCount); }
	virtual float TotalLoopOneSecond() {
		UInt64 second = (UInt64)((TimeManager::NowTick() - mLoopStartMilSecond)*0.001); return second > 0 ? (float)(mTotalLoopCount / second) : 0;
	}
	virtual float TotalLowLoopOneSecond() {
		UInt64 second = (UInt64)((TimeManager::NowTick() - mLoopStartMilSecond)*0.001);  return second >0 ? (float)(mTotalLowLoopCount / second) : 0;	
	}
	virtual UInt64 ControlCheckLoopSecond() const;

	AutoEvent WaitTime(ProcessFunction  processFun, float waitTimeSecond);
	AutoEvent StartProcess(ProcessFunction  processFun, float tickTimeSecond);

public:
	virtual void Process(void*) = 0;
	virtual void OnStart(void*) = 0;
	virtual void OnStop(void*) = 0;

    virtual int OnceTime(void) const { return LOGIC_PROCESS_ONCE_TIME;  }
	virtual int OnceLowTime() const { return 10000; }

	virtual StateDataType GetRunStateInfo(tNiceData &infoData) { return 0; }

	// ����ÿ������ѭ���������ٱ��ʱ���
	virtual float WarnLoopLimitRate() const { return 0.8f; }
	// ����ÿ������ѭ���������ٱ�����Ϊ����
	virtual float ErrorLoopLimitRate() const { return 0.3f; }

	// ʣ����п����ڴ���ڶ���M���б���
	virtual int WarnMemoryLimitM() { return 800; }

	// ʣ����п����ڴ���ڶ���M��Ϊ����
	virtual int ErrorMemoryLimitM() { return 500; }

	virtual int WarnDiskLimitSizeM() { return 2000; }
	virtual int ErrorDiskLimitSizeM() { return 1000; }

	virtual bool StartError() { return false; }
	virtual bool LogicError() { return false; }
	virtual bool LogicMaintain() { return false; }

	virtual void LowProcess(int spaceTime)	{ DEBUG_LOG("Low once time %d, loop %llu", spaceTime, mLowRunCount);  }

	virtual void Log(const char* szInfo, ...);
	virtual tDBManager* GetDBWork() { return NULL; }

protected:
	// ����׼���رն��� NOTE: �˹��ܱ���ֻ���߳�ѭ���ڵ���
	virtual bool NotifyThreadClose(){  CanClose(); return true; }

	virtual void onExceptionExist();

	virtual void OnOnceLoopEnd() { };

public:
	BaseThread(const char *threadIndexName);
	virtual ~BaseThread();

	virtual void InitEventCenter(AutoEventCenter eventCenter);
	AutoEventCenter GetMainEventCenter(void) const { return mMainEventCenter; }

	AString GetThreadName() const { return mThreadName; }

	static void LogRun(const char *szLogName, const char *szInfo);

public:
	virtual void start() { InitThread(); }

	virtual VOID run(void);

	virtual void _Release(void);

	bool IsActive(void)
	{
		return mActive;
	}

	virtual void setActive(bool bActive){ mActive = bActive; }

    virtual void OnDBReadySucceed(tDBManager *dbMgr) {  Log("DB �����ɹ�...\n*************************************************************"); }

    // NOTE: ֧�ֶ���ͬ��, �ŷ��߳̿�ʹ�ô˹������������ر�
    virtual void RequestClose()
    {
        if (mbClosing || mbStop)
            return;

        mCloseLock.lock();
        mQuestClose = true;
        NOTE_LOG("Thread %u request close", mThreadID);
        mCloseLock.unlock();
    }

	// �����ر�, ��ȷ�����ͷ�
	//virtual void Close();

	virtual void CanClose();

	virtual bool IsStoped() const { return mbStop; }
	virtual bool IsClosing() const { return mbClosing; }
	virtual bool IsNeedClose() const { return mQuestClose || mbStop || mbClosing; }
	virtual bool IsException() const { return mbException; }
	virtual bool ReadyClose() const { return true; }

	// NOTE: Input thread run
	virtual void ProcessCommand(const char *szCommand){ }	

	virtual void NotifyException();

public:
	virtual void backWorkThread(void) { run();  }

	virtual void loopProcess();

public:
	int									mRestartCount;
	int									mExceptionCount;

protected:	
	AString								mThreadName;
	bool									mActive;
	bool									mbClosing;
	bool									mbException;					// �������쳣, �����߳̽��д���
	bool									mbLooping;
	AutoEventCenter				mMainEventCenter;			// ��ǰ�̹߳��õ���Ҫ�¼�����, �����ݿ⼰ȫ�ֽ���

	UInt64								mBeginTime;					// �ӿ�������ǰ����ʱ��
	UInt64								mRunCount;
	UInt64								mLowRunCount;				// ��Ƶִ�д���	
	UInt64								mCurrentLoopBeginSecond;		// ��ǰѭ����ʼʱ��
	UInt64								mLoopStartMilSecond;	// ѭ����ʼ�ĺ���ʱ��(��Կ���ʱ��)
	UInt64								mTotalLoopCount;
	UInt64								mTotalLowLoopCount;

#if __LINUX__
	struct event_t*					mCloseEvent;
#else
	HANDLE							mCloseEvent;
#endif
    CsLock                             mCloseLock;

protected:
	bool									mQuestClose;
	bool									mbStop;

};
//-------------------------------------------------------------------------
// ���н����������
class Net_Export ServerThread : public BaseThread
{
public:
    ServerThread(const char *threadIndexName)
        : BaseThread(threadIndexName), mbHaveCommand(false) 
	{
		mLastSecondTime = TimeManager::Now();
	}

    virtual void DoCommand(const AString &commandString, StringArray &paramArray) = 0;

	virtual AString GetTitle() = 0;
	virtual void SetTitle(const AString &title) = 0;

	virtual StateDataType GetRunStateInfo(tNiceData &infoData);

public:
	virtual void OnStart(void*)
	{
		AString title = GetTitle();
		mTitleList[0] = ".  ";
		mTitleList[0] += title;
		mTitleList[1] = ".. ";
		mTitleList[1] += title;
		mTitleList[2] = "...";
		mTitleList[2] += title;
		SetTitle(title);
	}
    virtual void LowProcess(int spaceTime) override
    {
        if (mbHaveCommand)
        {
             StringArray  strList;
           AString commString;
            mCommandLock.lock();
            mCommandString.toLower();
            AString::Split(mCommandString.c_str(), strList, " ", 10);
            mCommandString.setNull();
            mbHaveCommand = false;
            mCommandLock.unlock();

			if (!strList.empty())
			{
				DoCommand(strList[0], strList);
			}
        }

		static int  mLast = 0;
		//static int sSpaceRate = OnceTime() > 0 ? (int)(1000000.f / OnceTime()) : 999999;
		//int flag = (int)((mRunCount / sSpaceRate) % 3);
		if (mLastSecondTime != TimeManager::Now())
		{
			mLastSecondTime = TimeManager::Now();
			mLast = ++mLast % 3;
			AString nowTitle = mTitleList[mLast];

			AString info;			
			info.Format("T:%d L:%.1f", (int)TotalLoopOneSecond(), TotalLowLoopOneSecond());
			nowTitle += info;
					
			SetTitle(nowTitle);
		}
    }
    virtual void ProcessCommand(const char *szCommand) override
    {
        mCommandLock.lock();
        mCommandString = szCommand;
        mbHaveCommand = true;
        mCommandLock.unlock();
    }

protected:
    AString mCommandString;
    bool mbHaveCommand;
    CsLock mCommandLock;
	UInt64	mLastSecondTime;
	Array<AString>	mTitleList;
};
//-------------------------------------------------------------------------
#endif //_INCLUDE_BASETHREAD_H_