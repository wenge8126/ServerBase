#pragma once
#ifndef _INCLUDE_MEMORYLOCK_H_
#define _INCLUDE_MEMORYLOCK_H_

// ʹ�ô���Ч��δ֪, �Ҳ�֧��Ƕ��, ֱ�����ⲿʹ�û�����
// ���κ����õĴ���
#define MemoryLock
#define memLock(a, b, c)

//#include "TimeManager.h"



// �ڴ��� (�ж�Ϊ����ֵʱ, �����޸�Ϊ����ֵ)
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
//		// ���ǵ�ǰ������, �򲻽��н���, ������Ƕ��ʱ, ���������ʼ����Ч
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

