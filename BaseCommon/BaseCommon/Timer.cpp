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
			// �Ƴ��Ѿ����ڵĵȴ�
			mWaitEventList.erase(it);
			//NOTE_LOG("Remove now exist wait");
			bDelete = true;
			continue;
		}
		
		if (bInserted && bDelete)
			break;

		if (!bInserted && t->mStartMilSecond + t->mMilSecond > time)
		{
			// �������ӵ���ǰλ�õ�ǰ��, ���Կ�����ȷ�ж�֮���Ϊ֮ǰ�ĵȴ�,����ɾ��
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
	//NOTE: ����MFC Time������MessageBox����ʱ��, �첽��ı�ִ��˳��, ��: onTime ��ʹ���� yield (), ���������첽����, 
	// ִ����ɺ�, ����ſ����첽����֮��Ĵ���ִ��, ����ѭ����, �ٴ�ִ�е��˴�, ��ʱ mRunList����Ȼ��ֵ, ��˻��ٴ�ִ�� onTime, ��ִ�ж���
	// �������������̲�����MessageBox��ʽ, �������ߺ�, ���ڵ�ǰЭ�̼���ִ��Time����(MFC�ں���ô������)
	
	for (ATimer t = mRunList.value(); !mRunList.empty();  t = mRunList.value())
	{
		mRunList._remove(mRunList.__getRoot()); // ���Ƴ����MFC MessageBox ����, �Ƚ����ͷ�

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
