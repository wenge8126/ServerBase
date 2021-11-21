

#include "BaseThread.h"

#include "TableManager.h"

#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#include "BaseNetManager.h"
#include "Assertx.h"
#include "EventFactory.h"
#include "ThreadEvent.h"
#include "CEvent.h"
#include "CoroutineTool.h"
#include "Timer.h"

#include "SystemInfo.h"
#include "FileDataStream.h"
#include <stdarg.h>
#include "Dump.h"
//--------------------------------------------------------------------------------------------------------
#define SHOW_LOOP_COUNT			0

void OnException(BaseThread *pThread)
{
	pThread->NotifyException();
}
//--------------------------------------------------------------------------------------------------------
BaseThread::BaseThread(const char *threadIndexName)
: mActive(true)	
, mRunCount(0)
, mQuestClose(false)
, mCloseEvent(NULL)
, mbStop(false)
, mbClosing(false)
, mLowRunCount(0)
, mbException(false)
, mbLooping(false)
, mThreadName(threadIndexName)
, mRestartCount(0)
, mTotalLoopCount(0)
, mTotalLowLoopCount(0)
, mExceptionCount(0)
{	
	//mTimeManager.Init();
	mBeginTime =TimeManager::SysMicrosecond();
	mLoopStartMilSecond = TimeManager::NowTick();
	//CRand::SetSpeed (TimeManager::Now()&0xFFFFFFFF, TimeManager::NowTick());   

	mCloseEvent = event_create(false, false);
}
//--------------------------------------------------------------------------------------------------------

BaseThread::~BaseThread()
{
	_Release();

	event_destroy(mCloseEvent);
}

void BaseThread::LogRun(const char *szLogName, const char *szInfo)
{
	FileDataStream		logFile(szLogName, FILE_WRITE);
	if (!logFile)
	{
		logFile.open(szLogName, FILE_CREATE);
	}
	logFile.seekEnd();
	logFile._write((void*)szInfo, strlen(szInfo));
	logFile._write((void*)"\r\n", 2);
	logFile.close();
}

//--------------------------------------------------------------------------------------------------------

VOID BaseThread::run( void )
{
	UInt64 now = TimeManager::SysMicrosecond();
	UInt64 second = (UInt64)(now * 0.000001);
	AString logString;
	logString.Format("%s %d > %s start", TimeManager::ToTimeString(second).c_str(), (int)(now*0.001 - second * 1000), GetThreadName().c_str());
	LogRun("./RunConfig/Start.log", logString.c_str());

#if __WINDOWS__
	Dump::msOnExceptionFunction = std::bind(OnException, this);
	// 通过命令行参数,获取上一次重启次数与异常次数
	NiceData lineParam;
	int x = SystemInfo::ReadCommandLineParam(::GetCommandLine(), lineParam);
	mRestartCount = lineParam["restart"];
	mExceptionCount = lineParam["exception"];
#endif
	CRand::SetSeed((uint)TimeManager::Now());

	mbClosing = false;
	mbLooping = false;
	Log("Thread *%d* begin ready", mThreadID);
	//mCloseEvent = ::CreateEvent(NULL, FALSE, FALSE, "EXIT_SERVER");

	OnStart(NULL);
	if (mQuestClose)
	{
		mbStop = true;
		NotifyThreadClose();
		//Log("逻辑线程初始开始失败，直接退出");
		return;
	}
	Log("Thread *%d* start ", mThreadID );	

	mBeginTime = TimeManager::SysMicrosecond();
	UInt64 uLastLowTime = mBeginTime;
	UInt64 uLowNextTime = mBeginTime+OnceLowTime();
	UInt64 endTime = 0;

	bool bNeedLimitLoop = OnceTime() > 0;
#if DEVELOP_MODE
	uint limitInitCount = -1;
#else
	UInt64 limitInitCount = -1;
#endif
	limitInitCount -= 10000;
	if (OnceTime()>0)
		limitInitCount =  limitInitCount / OnceTime();		// Max loop count in max time 
	if (bNeedLimitLoop)	
		limitInitCount = limitInitCount * OnceTime() / OnceLowTime();			// Low limit count from fast count use time

	int onceSecondLoopCount = 1000000 / OnceTime();

#if SHOW_LOOP_COUNT
	UInt64 tempTestTime = mBeginTime;
	UInt64 tempTestCount = 0;
#endif
	mCurrentLoopBeginSecond = TimeManager::Now();
	mLoopStartMilSecond = TimeManager::NowTick();
	mbLooping = true;
	while (IsActive())
	{
		TimeManager::ThreadSetCurrentTime();
		
		mCurrentLoopBeginSecond = TimeManager::Now();

		if (mQuestClose)
		{
            mCloseLock.lock();
			mQuestClose = false;            
			mbClosing = true;
            
            NOTE_LOG("Thread %u now closing ...", mThreadID);
			if (NotifyThreadClose())
			{
                 NOTE_LOG("Thread %u now ready close finish", mThreadID);
				setActive(false);
                mCloseLock.unlock();
				break;
			}
            NOTE_LOG("WARN: Thread %u now ready close NOT finish", mThreadID);
			mbClosing = false;
            mCloseLock.unlock();
            continue;
		}

		//try{
		if (mMainEventCenter)
			mMainEventCenter->ProcessEvent();
		Process(NULL);
		TimerCenter::GetMe()->Process();
		CoroutineTool::CheckFinish();
		//}
		//catch (std::exception &e)
		//{
		//	ERROR_LOG("Thread [%d] process error >%s", mThreadID, e.what());
		//}
		//catch (...)
		//{
		//	ERROR_LOG("Thread [%d] process error, Please check dump file", mThreadID);
		//}

		++mRunCount;
		++mTotalLoopCount;
		OnOnceLoopEnd();

		endTime = TimeManager::SysMicrosecond();
		if (endTime > uLowNextTime)
		{			
			++mLowRunCount;
			++mTotalLowLoopCount;
			LowProcess((int)(endTime-uLastLowTime));
			uLastLowTime = endTime;
			uLowNextTime = mBeginTime + (mLowRunCount+1)*OnceLowTime();

			if (mLowRunCount > limitInitCount)
			{
				DEBUG_LOG("Init once loop time %llu, fast loop %llu", mLowRunCount, mRunCount);
				mBeginTime = TimeManager::SysMicrosecond();
				uLastLowTime = mBeginTime;
				uLowNextTime = mBeginTime + OnceLowTime();				
				mRunCount = 1;
				mLowRunCount = 1;				
			}
		}

		if (bNeedLimitLoop)
		{
			//int lastTime = (int)(mRunCount * OnceTime() - (endTime - mBeginTime));
			//if (lastTime >= 1000)
			//	TimeManager::Sleep(1); // (int)(lastTime*0.001f));

			if (TotalLoopOneSecond() > onceSecondLoopCount)
			{
				UInt64 waitTime = (UInt64)((float)(TotalLoopOneSecond() - onceSecondLoopCount) / onceSecondLoopCount);
				if (waitTime <= 0)
					waitTime = 1;
				TimeManager::Sleep((int)waitTime);
			}

			// 以下是时间溢出 （到3000年~v~）
			//UInt64 n = TimeManager::SysMicrosecond();
			//if (endTime >= mBeginTime)
			//{
			//	int lastTime = (int)(mRunCount * OnceTime() - (endTime - mBeginTime));
			//	if (lastTime >= 1000)
			//		TimeManager::Sleep((int)(lastTime*0.001f));
			//}
			//else
			//{
			//	mBeginTime = n;
			//	mRunCount = 0;
			//}
		}

#if SHOW_LOOP_COUNT
		if (TimeManager::SysMicrosecond() - tempTestTime >= 1000000)
		{
			PRINT("Loop 1 sec : %lld\r\n", mRunCount - tempTestCount);
			tempTestCount = mRunCount;
			tempTestTime = TimeManager::SysMicrosecond();
		}
#endif
	}
	
	TimerCenter::GetMe()->ClearAll();

	LOG("*** Thread *%u* stop", mThreadID);
	OnStop(NULL);
	_Release();

	LOG("=== Thread *%u* closed OK", mThreadID);

	mbStop = true;
    event_set(mCloseEvent);
}

void BaseThread::loopProcess()
{	
	if (mMainEventCenter)
		mMainEventCenter->ProcessEvent();
	Process(NULL);
	TimerCenter::GetMe()->Process();
	CoroutineTool::CheckFinish();
}

// 初始网络,可重复调用此功能,为当前线程准备多个网络连接或服务
//bool BaseThread::AppendNet(tNetHandle *pServerTool)
//{
//	AssertEx(pServerTool, "网络注册工具为空"); 
//	tNetHandle *hNetTool = pServerTool;
//	if ( hNetTool )
//	{
//		// 注意, 这里是允许插入多个相同主键的元素
//		mNetMap.insert(MAKE_INDEX_ID(pServerTool->GetIndexName()), pServerTool);
//	}
//	return NULL!=hNetTool;
//}

void BaseThread::Log(const char* szInfo, ...)
{
#if DEVELOP_MODE
	va_list va;
	va_start(va, szInfo);
	TableTool::DebugLog(va, szInfo);
#endif
}



void BaseThread::_Release( void )
{
	mMainEventCenter.setNull();
}


class ProcessEvent : public Logic::BaseEvent
{
	friend class BaseThread;

protected:
	ProcessFunction mProcessFunction;

public:
	virtual void SetData(AutoNice &scrData) { AssertNote(0, "Can not call ProcessEvent SetData"); }
	virtual tNiceData& GetData(void) { static NiceData d; return d; }
	virtual const tNiceData& GetData(void) const { static NiceData d;  return d; }

	virtual bool Update(float onceTime) override
	{
		mProcessFunction(onceTime, true);
		return true;
	}

	virtual bool _OnTimeOver() override
	{
		mProcessFunction(0, true);
		return true;
	}
};

void BaseThread::InitEventCenter( AutoEventCenter eventCenter )
{
	//AssertEx (eventCenter, "EventCenter is null.");

	mMainEventCenter = eventCenter;
	if (mMainEventCenter)
	{
		mMainEventCenter->RegisterEvent("__ProcessEvent_", MEM_NEW Logic::EventFactory<ProcessEvent>());
	}
}

//void BaseThread::Close()
//{
//    mCloseLock.lock();
//	mQuestClose = true;
//    mCloseLock.unlock();
//	WorkThread::Close();
//}

void BaseThread::CanClose()
{
    // 线程完成关闭后, 才进行设置此事件
	//event_set(mCloseEvent);
    //NOTE_LOG("Thread %u close event ok", mThreadID);
}


void BaseThread::NotifyException()
{
	mQuestClose = true;
	mbException = true;

	NotifyThreadClose();
	mQuestClose = true;
	mbException = true;
	NOTE_LOG("Now work thread sleep, please manager thread force this thread!");
	for (int i=0; i<30; ++i)
	{
		loopProcess();
		LowProcess(10);
		TimeManager::Sleep(100);
	}
	while (!ReadyClose())
	{
		loopProcess();
		LowProcess(10);
		TimeManager::Sleep(100);
	}
	//_ForceClose(1);
	event_set(mCloseEvent);
	mbStop = true;
	LOG("异常退出");
}

void BaseThread::MainWaitWork()
{
	event_wait(mCloseEvent);
}

UInt64 BaseThread::ControlCheckLoopSecond() const
{
	if (mQuestClose || mbStop || mbClosing || !mbLooping)
		return 0;

	return TimeManager::Now() - mCurrentLoopBeginSecond;
}

AutoEvent BaseThread::WaitTime(ProcessFunction processFun, float waitTimeSecond)
{
	if (mMainEventCenter)
	{
		Hand<ProcessEvent> evt = mMainEventCenter->StartEvent("__ProcessEvent_");
		if (evt)
		{
			evt->mProcessFunction = processFun;
			evt->WaitTime(waitTimeSecond);
			return evt;
		}
	}
	return AutoEvent();
}

AutoEvent BaseThread::StartProcess(ProcessFunction processFun, float tickTimeSecond)
{
	if (mMainEventCenter)
	{
		Hand<ProcessEvent> evt = mMainEventCenter->StartEvent("__ProcessEvent_");
		if (evt)
		{
			evt->mProcessFunction = processFun;
			evt->StartUpdate(tickTimeSecond);
			return evt;
		}
	}
	return AutoEvent();
}

StateDataType ServerThread::GetRunStateInfo(tNiceData &infoData)
{
	//infoData["TITLE"] = GetTitle();
	infoData["THREAD"] = mThreadName;

	AString strInfo;
	strInfo.Format("%d/%d", mExceptionCount, mRestartCount);
	infoData["RESTART"] = strInfo;

	AString info;
	if (OnceTime() > 0)
		info.Format("%d/%d", (int)TotalLoopOneSecond(), (int)(1000000 / OnceTime()));
	else
		info.Format("%d/~", (int)TotalLoopOneSecond());
	infoData["TICK"] = info;

	info.Format("%2.f/%.2f", TotalLowLoopOneSecond(), (float)1000000 / OnceLowTime());
	infoData["LOWTICK"] = info;

	size_t totalSize = 0;
	size_t activePhys = 0;
	SystemInfo::SystemMem(totalSize, activePhys);

	size_t useMem = SystemInfo::ProcessMemory();

	infoData["USE_MEM"] = (useMem / 1024) / 1024.0f;

	info.Format("%.0f/%.0fM", (activePhys / 1024) / 1024.0f, (totalSize / 1024) / 1024.0f);
	infoData["FREE_MEM"] = info;

	StateData  baseSate;

	// 内存状态
	if (activePhys / 1024 / 1024 < ErrorMemoryLimitM())
		baseSate.open(EBASE_RUN_STATE::eMemoryTooBad);
	else if (activePhys / 1024 / 1024 < WarnMemoryLimitM())
		baseSate.open(EBASE_RUN_STATE::eMemoryWarn);

	// 循环状态	
	if (TotalLoopOneSecond()<1000 && TotalLoopOneSecond() / (1000000 / OnceTime()) < ErrorLoopLimitRate())
		baseSate.open(EBASE_RUN_STATE::eLoopTooBad);
	else if (TotalLoopOneSecond()<5000 && TotalLoopOneSecond() / (1000000 / OnceTime()) < WarnLoopLimitRate())
		baseSate.open(EBASE_RUN_STATE::eLoopWarn);
	
	if (TotalLowLoopOneSecond() / (1000000 / OnceLowTime()) < ErrorLoopLimitRate())
		baseSate.open(EBASE_RUN_STATE::eLowLoopTooBad);
	else if (TotalLowLoopOneSecond() / (1000000 / OnceLowTime()) < WarnLoopLimitRate())
		baseSate.open(EBASE_RUN_STATE::eLowLoopWarn);	


#if __WINDOWS__
	// 磁盘空间
	size_t totalDiskSize = 0, freeDiskSize = 0;

	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	SystemInfo::DiskSpaceInfo(FileDataStream::GetDiskDriveName(szDir).c_str(), totalDiskSize, freeDiskSize);

	info.Format("%llu/%lluM", freeDiskSize / 1024 / 1024, totalDiskSize / 1024 / 1024);
	infoData["DISK"] = info;

	if (freeDiskSize / 1024 / 1024 < ErrorDiskLimitSizeM())
		baseSate.open(EBASE_RUN_STATE::eDiskTooBad);
	else if(freeDiskSize / 1024 / 1024 < WarnDiskLimitSizeM())
		baseSate.open(EBASE_RUN_STATE::eDiskWarn);

#endif
	if (StartError())
		baseSate.open(EBASE_RUN_STATE::eStartError);

	if (LogicError())
		baseSate.open(EBASE_RUN_STATE::eLogicError);

	if (LogicMaintain())
		baseSate.open(EBASE_RUN_STATE::eLogicMaintain);

	StateDataType stateValue = baseSate;
	infoData["STATE"] = (uint)stateValue;

	return stateValue;
}

void BaseThread::onExceptionExist()
{
	NotifyThreadClose();
	DEBUG_LOG("ERROR: [%d]线程异常退出, 进程退出处理, 请检查DUMP信息", mThreadID);
	TableTool::SetLog(NULL);
	::exit(9999);
}
