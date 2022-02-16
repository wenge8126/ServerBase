
#ifndef _INCLUDE_ASYNCLOOP_H_
#define _INCLUDE_ASYNCLOOP_H_

#pragma once

#include "Timer.h"


class AsyncLoop
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

};


#endif //_INCLUDE_ASYNCLOOP_H_