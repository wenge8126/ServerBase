#pragma once
#ifndef _INCLUDE_MEMORYLOCK_H_
#define _INCLUDE_MEMORYLOCK_H_

// 使用此锁效果未知, 且不支持嵌套, 直接在外部使用互斥锁
// 无任何作用的代码
#define MemoryLock
#define memLock(a, b, c)

//#include "TimeManager.h"



// 内存锁 (判断为可锁值时, 进行修改为锁定值)
//struct MemoryLock
//{
//public:
//	char	*mMemoryLockSpack;
//	char mWaitValue;
//	char mLockValue;
//	bool mbLock;
//
//public:
//	MemoryLock() : mMemoryLockSpack(NULL), mWaitValue(-1), mLockValue(-1), mbLock(false) {}
//	MemoryLock(char *szMem, char waitValue, char lockValue)
//	{
//		mMemoryLockSpack = szMem;
//		mWaitValue = waitValue;
//		mLockValue = lockValue;
//		mbLock = false;
//		//Lock(__FILE__, __LINE__);
//	}
//	~MemoryLock()
//	{
//		// 不是当前锁定的, 则不进行解锁, 用于锁嵌套时, 最外层锁定始终有效
//		//if (mbLock)
//			//UnLock();
//	}
//
//	void Init(char *szMem, char waitValue, char lockValue)
//	{
//		mMemoryLockSpack = szMem;
//		mWaitValue = waitValue;
//		mLockValue = lockValue;
//		mbLock = false;
//	}
//
//	void Clear()
//	{
//		mMemoryLockSpack = NULL;
//		mWaitValue = -1;
//		mLockValue = -1;
//		mbLock = false;
//	}
//
//	bool TryLock()
//	{
//		if (mMemoryLockSpack == NULL)
//			return true;
//
//		if (*mMemoryLockSpack == mLockValue)
//			return true;
//
//		if (*mMemoryLockSpack == mWaitValue)
//		{
//			*mMemoryLockSpack = mLockValue;
//			mbLock = *mMemoryLockSpack == mLockValue;
//			return mbLock;
//		}
//		return false;
//	}
//
//	void Lock(const char *szFile, int line)
//	{
//		if (mMemoryLockSpack == NULL)
//			return;
//		UInt64 nowTick = TimeManager::NowTick();
//		int x = 0;
//		while (true)
//		{
//			if (TryLock())
//			{
//				if (x > 0)
//				{
//					NOTE_LOG("***** Try count >%d\r\n", x);
//				}
//				return;
//			}
//			TimeManager::GetMe().Sleep(1);
//			++x;
//
//			if (TimeManager::NowTick() - nowTick > 1000)
//			{
//				WARN_LOG("Lock over time 1 sec, try %d. at [%s]:%d", x, szFile, line);
//				nowTick = TimeManager::NowTick();
//			}
//		}
//
//	}
//
//	bool UnLock()
//	{
//		if (mMemoryLockSpack == NULL)
//			return false;
//
//		if (*mMemoryLockSpack == mLockValue)
//		{
//			*mMemoryLockSpack = mWaitValue;
//			return *mMemoryLockSpack == mWaitValue;
//		}
//		return false;
//	}
//};


#endif //_INCLUDE_MEMORYLOCK_H_

