
#ifndef _INCLUDE_ASYNCLOOP_H_
#define _INCLUDE_ASYNCLOOP_H_

#pragma once

#include "Timer.h"


class Async
{
public:
	template<typename FUN>
	static bool AwaitLoop(FUN checkFun, Int64 onceMilSecond, Int64 overMilSecond)
	{
		Int64 now = TimerCenter::GetMe()->Now();
		while (true)
		{
			if (checkFun())
				break;

			if (TimerCenter::GetMe()->Now() - now >= overMilSecond)
				return false;

			tTimer::AWaitTime(onceMilSecond);
		}

		return true;
	}

	template<typename FUN>
	class _ATimer : public tTimer
	{
	public:
		virtual void onTime() override
		{
			mOnFun();
		}

	public:
		FUN		mOnFun;
	};

	template<typename FUN>
	static void Wait(FUN onFun, Int64 overMilSecond)
	{
		Auto<_ATimer> t = MEM_NEW _ATimer();
		t->mOnFun = onFun;
		t->Wait(overMilSecond);
	}
};


#endif //_INCLUDE_ASYNCLOOP_H_