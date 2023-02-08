#include "Timer.h"
#include "CoroutineTool.h"

#ifndef __LINUX__
# include "windows.h"
#else
# include "TimeManager.h"
#endif

//-------------------------------------------------------------------------

class  _WaitTimer : public tTimer
{
	CoroID	mCoreID;

public:
	virtual void onTime()
	{
		RESUME(mCoreID);
	}

	virtual void Wait(Int64 milSecond) override
	{
		mCoreID = CORO;
		tTimer::Wait(milSecond);
		YIELD
	}
};

bool tTimer::AWaitTime(Int64 milSecord)
{
	if (CORO == 0)
	{
		ERROR_LOG("tTimer::AWaitTime must in coroution function");
		return false;
	}
	Auto<tTimer> t = MEM_NEW _WaitTimer();
	t->Wait(milSecord); 
	return true;
}

//-------------------------------------------------------------------------
void tTimer::Wait(TimerCenter *pMgr, Int64 milSecond)
{
	if (milSecond < 0)
	{
		GREEN_LOG("Wait milSecond %lld<0, now set 0", milSecond);
		milSecond = 0;
	}
		
	mbRun = 1;
	pMgr->Start(this, milSecond);
}

void tTimer::Wait(Int64 milSecond)
{
	Wait(TimerCenter::GetMe(), milSecond);
}

void tTimer::Loop(TimerCenter *pMgr, Int64 milSecond)
{
	if (milSecond < 0)
	{
		WARN_LOG("Loop time %d<0, now set 0", milSecond);
		milSecond = 0;
	}
	mbLoop = 1; 
	pMgr->Start(this, milSecond);
}

void tTimer::Loop(Int64 milSecond)
{
	Loop(TimerCenter::GetMe(), milSecond);
}


//-------------------------------------------------------------------------
std::vector< Hand<TimerCenter> > TimerCenter::msAllCenterList;
//-------------------------------------------------------------------------
void TimerCenter::Start(tTimer *pTimer, Int64 milSecond)
{
	UInt64 now = Now();
	pTimer->mStartMilSecond = now;
	if (milSecond < 0)
		pTimer->mMilSecond = 0;
	else
		pTimer->mMilSecond = milSecond;

	UInt64 time = now + milSecond;
	_append(pTimer, time);
}

void TimerCenter::_append(tTimer *pTimer, UInt64 time)
{
	bool bInserted = false;
	bool bDelete = false;
	for (auto it = mWaitEventList.begin(); it;  )
	{
		ATimer &t = *it;
		if (t.getPtr() == pTimer)
		{
			// 移除已经存在的等待
			mWaitEventList.erase(it);
			//NOTE_LOG("Remove now exist wait");
			bDelete = true;
			continue;
		}
		
		if (bInserted && bDelete)
			break;

		if (!bInserted && t->mStartMilSecond + t->mMilSecond > time)
		{
			// 由于增加到当前位置的前面, 所以可以正确判断之后的为之前的等待,继续删除
			mWaitEventList.insert(it, pTimer);
			bInserted = true;			
		}
		
		++it;
	}

	if (!bInserted)
		mWaitEventList.push_back(pTimer);
}

void TimerCenter::Process()
{
	UInt64 now = Now();
	for (auto it = mWaitEventList.begin(); it; )
	{
		ATimer &t = *it;
		if (now >= t->mStartMilSecond + t->mMilSecond)
		{
			mRunList.insert(t);
			mWaitEventList.erase(it);
		}
		else
			break;
	}
	//NOTE: 由于MFC Time函数与MessageBox调整时序, 异步会改变执行顺序, 如: onTime 中使用了 yield (), 激活其他异步函数, 
	// 执行完成后, 会接着开启异步函数之后的代码执行, 如主循环内, 再次执行到此处, 此时 mRunList中依然有值, 因此会再次执行 onTime, 被执行二次
	// 理论上其他进程不会像MessageBox方式, 阻塞主线后, 会在当前协程继续执行Time函数(MFC内核怎么做到呢)
	
	for (ATimer t = mRunList.value(); !mRunList.empty();  t = mRunList.value())
	{
		mRunList._remove(mRunList.__getRoot()); // 先移除解决MFC MessageBox 问题, 先进行释放

		if (!t)
			continue;

		if (t->mbLoop>0)
		{
			t->update();
			if (t->mbLoop)
			{
				t->mStartMilSecond += t->mMilSecond;
				_append(t.getPtr(), t->mStartMilSecond + t->mMilSecond);
			}
		}
		else if (t->mbRun>0)
			t->onTime();
	}
}

UInt64 TimerCenter::Now()
{
#	ifdef __LINUX__
	return TimeManager::NowTick();
#	else		
	return GetTickCount64();
#	endif //__LINUX__
}
//-------------------------------------------------------------------------
class _WaitAutoTimer :public tTimer
{
public:
	AsyncAutoFunction mACall;

public:
	virtual void onTime() override
	{
		mACall();
	}
};

class _WaitHandTimer :public tTimer
{
public:
	AsyncFunction mCall;

public:
	virtual void onTime() override
	{
		mCall();
	}
};

ATimer TimerCenter::WaitTime(int milSecond, AsyncAutoFunction waitCall)
{
	Auto<_WaitAutoTimer>  timer = MEM_NEW _WaitAutoTimer();
	timer->mACall = waitCall;
	timer->Wait(milSecond);
	return timer;
}

ATimer TimerCenter::WaitTime(int milSecond, AsyncFunction waitCall)
{
	Auto<_WaitHandTimer>  timer = MEM_NEW _WaitHandTimer();
	timer->mCall = waitCall;
	timer->Wait(milSecond);
	return timer;
}
