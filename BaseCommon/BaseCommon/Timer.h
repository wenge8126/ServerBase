#pragma once
#ifndef  _INCLUDE_TIMER_H_
#define _INCLUDE_TIMER_H_

#include "Auto.h"
#include "BaseCommon.h"
#include "PoolList.h"
#include "AsyncAutoFunction.h"
#include "AsyncFunction.h"
#include "PoolLoop.h"
#include <vector>

//-------------------------------------------------------------------------
class TimerCenter;
class BaseCommon_Export tTimer : public AutoBase
{
public:
	static bool AWaitTime(Int64 milSecord);

public:
	virtual void onTime() = 0;
	virtual void update() { onTime(); }
	virtual void StopUpdate() { mbLoop = 0; }
	virtual void StopWait() { mbRun = 0;  }

	virtual void Wait(TimerCenter *pMgr, Int64 milSecond);
	virtual void Loop(TimerCenter *pMgr, Int64 milSecond);

	virtual void Wait(Int64 milSecond);
	virtual void Loop(Int64 milSecond);

public:
	tTimer()
		: mMilSecond(0)
		, mbLoop(0)
		, mbRun(0)
		, mStartMilSecond(0) 
	{}

	struct 
	{
		UInt64  mMilSecond;			
		UInt64  mbLoop		: 1;
		UInt64	mbRun			: 1;
		UInt64	mStartMilSecond : 62;
	};
};

typedef Auto<tTimer>	ATimer;

//-------------------------------------------------------------------------
class BaseCommon_Export  TimerCenter : public Base<TimerCenter>
{
public:
	static ATimer WaitTime(int milSecond, AsyncAutoFunction waitCall);

	static ATimer WaitTime(int milSecond, AsyncFunction waitCall);

public:
	void Start(tTimer *pTimer, Int64 milSecond);

	void _append(tTimer *pTimer, UInt64 time);

	void Process();

	UInt64 Now();

	void ClearAll()
	{
		WaitList  temp;
		temp.swap(mWaitEventList);		
		mRunList.clear();
	}

	virtual void OnForceFreeBefore() override { ClearAll(); }

public:
	TimerCenter()
		: mRunList(1024*1024)
	{
		msAllCenterList.push_back(this);
	}

	static TimerCenter* GetMe()
	{
		thread_local static Hand<TimerCenter>  sTimerCenter = MEM_NEW TimerCenter();
		return sTimerCenter.getPtr();
	}

	static void ClearAllTimer()
	{
		for (int i = 0; i < msAllCenterList.size(); ++i)
		{
			if (msAllCenterList[i])
				msAllCenterList[i]->ClearAll();
		}
	}

	static void CloseClear()
	{
		for (int i=0; i<msAllCenterList.size(); ++i)
		{
			if (msAllCenterList[i])
				msAllCenterList[i]->ClearAll();
			msAllCenterList[i]._free();
		}
		msAllCenterList.clear();
	}

protected:
	typedef PoolList<ATimer>		WaitList;
	WaitList									mWaitEventList;
	PoolLoop<ATimer>					mRunList;

	static std::vector< Hand<TimerCenter> > msAllCenterList;
};
//-------------------------------------------------------------------------

#endif // ! _INCLUDE_TIMER_H_

