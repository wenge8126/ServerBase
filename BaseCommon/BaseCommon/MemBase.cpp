
#include "MemBase.h"
#include "Common.h"

#if __WINDOWS__
#include <malloc.h>
#include <Windows.h>
#endif


#include "AutoString.h"
#include "Assertx.h"

#if MEMORY_DEBUT_TEST
#	include "LogEvent.h"
//---------------------------------------------------------------------------------------

EasyLog		gLog;

#define  SAVE_MEMORY_LOG gLog.log
#endif

//---------------------------------------------------------------------------------------
// ���ܶ���ʹ�õĿռ��С������ʹ�õ�ַǰһ���ֽڱ���ռ����������Կռ��8�ֽ�
struct _HandUse { void *p; int coount; };
Bucket		mUseMemPool(sizeof(_HandUse), 2048);					// For auto hand

Allot	Allot::msAllocation;
//---------------------------------------------------------------------------------------


int GetBitNum(size_t num)
{
	int n = 0;
	while(num>=2)
	{
		num = num >> 1;
		++n;
	}
	AssertEx(num==1 && n>=1, "����Ҫ���Ĵ�С��2��N��ڤ");
	return n;
}

//---------------------------------------------------------------------------------------

Allot::Allot()
{
#if MEMORY_DEBUT_TEST
	mAllNewCount = 0;
	mAllFreeCount = 0;

	mUsePoolSize = 0;
	mFreePoolSize = 0;

	mUseSystemSize = 0;
	mFreeSystemSize = 0;
#endif

#if USE_SYSTEM_MEMORY
	mMemoryPool = NULL;
#else	
	for (int i=0; i<TRUCK_MAX_NUM; ++i)
	{
		// NOTE: �ں����1�����ڱ���������, ���������ݽṹ�ڴ����
		mPointPtrList[i].setBlockSize((i+1)*BASE_BLOCK_SIZE + 1);
	}
#	if	USE_BIG_POOL
	mMemoryPool = new MemoryPool(MEMORY_POOL_SIZE);
#	else
	mMemoryPool = NULL;
#	endif
#endif


}
//---------------------------------------------------------------------------------------
void* Allot::_smallMalloc(size_t &size)
{
	//if (size>MAX_OBJECT_SIZE)
	//	return NULL;
	size = GET_BLOCK_SIZE(size);
	size_t index = (size>>BIT_NUM)-1;		
	AssertEx(index<0xff, "�������ɳ�ʱ255");
	byte *p = (byte*)mPointPtrList[index].newPtr();
	*p = (byte)index;
	//size -= 1;
	return p+1;
}
//---------------------------------------------------------------------------------------
size_t Allot::_freeMalloc(void *ptr)
{
	byte index = *( (byte*)ptr - 1 );
	//for (int i=0; i<TRUCK_MAX_NUM; ++i)
	//{
	//	if (mPointPtrList[i].deletePtr(ptr))
	//		return (i+1)*BASE_BLOCK_SIZE;
	//}
	if (index<0xFF)
	{
		mPointPtrList[index].deletePtr((byte*)ptr - 1);
		return (index+1)*BASE_BLOCK_SIZE;
	}
	return 0;
}
//---------------------------------------------------------------------------------------

void* Allot::allot( size_t &size )
{
	if (size > 1024 * 1023)
	{
		int b = 2;
	}

#if USE_SYSTEM_MEMORY
	return ::malloc(size);
#endif

#if MEMORY_DEBUT_TEST
	if (size==0)
	{
		PRINT("XXX Error: �����ڴ�ռ�Ϊ��");
	}
#endif
	AssertEx(size!=0, "will malloc size is zero");
	void *p = NULL;
	size_t resultSize = 0;
#if USE_SMALL_POOL
	if (size<=MAX_OBJECT_SIZE)
	{		
		p = _smallMalloc(size);
		AssertEx(p, "small point malloc error");
		resultSize = size+1;
	}
	else 
#endif
	{		
#if USE_BIG_POOL
		p = mMemoryPool->allocateBytes(size+1, resultSize);
#else
		p = ::malloc(size+1);
		if (p!=NULL)
		{
			resultSize = size;
#if MEMORY_DEBUT_TEST
			mUseSystemSize += size;
#endif
		}
#endif
		AssertEx((p && resultSize>=size),  "pool malloc error");
        if (p==NULL)
            return NULL;
		*(byte*)p = 0xFF;
		p = (byte*)p+1;
	}

#if MEMORY_DEBUT_TEST

#if MULTI_THREAD_LOCK
	mLock.lock();
#endif
	mUsePoolSize += resultSize;
#if MULTI_THREAD_LOCK
	mLock.unlock();
#endif

#endif

	return p;
}
//---------------------------------------------------------------------------------------
void Allot::recordInfo( void *p, const char* szFile, unsigned int nLine, const char* szFunction, size_t size )
{
#if MEMORY_DEBUT_TEST
#	if MULTI_THREAD_LOCK
	mLock.lock();
#	endif
	MemInfo info = {szFile, (int)nLine, szFunction, size};
	mMemInfoList[p] = info;

	char szTemp[1024];
	sprintf_s(szTemp, 1024, "%s_%u:%s", szFile, nLine, szFunction);
	auto it = mNewInfo.find(szTemp);
	if (it != mNewInfo.end())
	{
		it->second.mNewCount += size;
		++it->second.mAllotTime;
	}
	else
	{
		sNEW_INFO s;
		s.mNewCount = size;
		s.mLastCount = 0;
		s.mFreeCount = 0;
		s.mAllotTime = 1;
		mNewInfo[szTemp] = s;
	}
#	if MULTI_THREAD_LOCK
	mLock.unlock();
#	endif
#endif
}
//---------------------------------------------------------------------------------------

void Allot::release( void *ptr )
{
	if (NULL==ptr)
		return;

#if USE_SYSTEM_MEMORY
	::free(ptr);
	return;
#endif

#if MEMORY_DEBUT_TEST
	char szTemp[1024];
#	if MULTI_THREAD_LOCK
	mLock.lock();
#	endif
	size_t recordSize = 0;
	MemInfoMap::iterator it = mMemInfoList.find(ptr);
	if (it!=mMemInfoList.end())
	{
		recordSize = it->second.mSize;
		
		sprintf_s(szTemp, 1024, "%s_%u:%s", it->second.mFile, it->second.mLine, it->second.mFunction);

		mMemInfoList.erase(it);			
	}
	else
	{
		AssertEx(0, "no memory record info, may be is use MEM_NEW create object");
#if __LINUX__
		throw std::exception();
#else
		throw std::exception("no memory record info, may be is use MEM_NEW create object");
#endif
	}
#	if MULTI_THREAD_LOCK
	mLock.unlock();
#	endif
#endif

	size_t size = 0;

#if USE_SMALL_POOL
	size = _freeMalloc(ptr);
	if (size<=0)
#endif
	{
#if USE_BIG_POOL
		size = mMemoryPool->deallocateBytes(ptr);
		if (size<=0)
		{
			AssertEx(0,  "no memory record info, may be is not use MEM_NEW create object");
#	ifdef __WINDOWS__
			throw std::exception("no memory record info, may be is not use MEM_NEW create object");
#	else
			throw (1);
#	endif
		}
#else
		::free((byte*)ptr-1);
#	if MEMORY_DEBUT_TEST
        if (!(recordSize>=MAX_OBJECT_SIZE))
            size = recordSize;
		AssertEx(recordSize>=MAX_OBJECT_SIZE, "������Ӧ����С�ڴ���з���, ������û��ʹ�� MEM_NEW ����");
		size = recordSize;

		mFreeSystemSize += size;
#	endif

#endif
	}
	else
		size += 1;	// ���ױ�־��������

#if MEMORY_DEBUT_TEST

	//if (size<=MAX_OBJECT_SIZE)
	//{
	//	if (size!=GET_BLOCK_SIZE(recordSize))
	//	{
	//		//AssertEx(0, "�ͷŴ�С������С�����");
	//	}
	//}
	//else if (size!=recordSize)
	//{
	//	//AssertEx(0, "�ͷŴ�С������С�����");
	//}
#	if MULTI_THREAD_LOCK
		mLock.lock();
#	endif
		mFreePoolSize += size;

		auto it2 = mNewInfo.find(szTemp);
		if (it2!=mNewInfo.end())
			it2->second.mFreeCount += size;
		else
			PRINT("XXX ERROR: [%s]δ�ҵ� new ��Ϣ��¼ \r\n", szTemp);

#	if MULTI_THREAD_LOCK
		mLock.unlock();
#	endif

#endif

}
//---------------------------------------------------------------------------------------
char szInfo[4][256];

const char* getInfo(size_t s, size_t index)
{
#pragma warning(push)
#pragma warning(disable:4996)
	if (s>1024*1024)
		sprintf(szInfo[index], "[%.6f]MB", (float)s/1024/1024);
	else if (s>1024)
		sprintf(szInfo[index], "[%.6f]KB", (float)s/1024);
	else
		sprintf(szInfo[index], "[%llu]B", (UInt64)s);
	return szInfo[index];
#pragma warning(pop)
}

Allot::~Allot()
{
	_dump();
}

void Allot::_dump()
{
#if MEMORY_DEBUT_TEST
#	if MULTI_THREAD_LOCK
	CsLockTool sl(mLock);
#	endif
	SAVE_MEMORY_LOG("Memory use infomation...");
	SAVE_MEMORY_LOG("==============================================");
#if USE_SYSTEM_MEMORY==0 && USE_BIG_POOL
	SAVE_MEMORY_LOG("��̬�ڴ�ط���������� %s, "
		"�������� %s, "
		"�ͷų������� %s, "
		"last pool size %s",
		getInfo(mMemoryPool->_allPoolSize(), 0), 
		getInfo(mMemoryPool->_getMaxPoolSize(), 1), 
		getInfo(mMemoryPool->_freePoolSize(), 2),
		getInfo(mMemoryPool->_allPoolSize() - mMemoryPool->_freePoolSize(), 3)
		);
	size_t x = mMemoryPool->_alloctSize()-mMemoryPool->_freeSize();
	SAVE_MEMORY_LOG(		
		"All used %s, " 
		"All free %s, " 
		"last %s, "
		"now last pool size %s",
		getInfo(mMemoryPool->_alloctSize(), 0), 
		getInfo(mMemoryPool->_freeSize(), 1), 
		getInfo(x, 2),
		getInfo(mMemoryPool->_nowPoolSize(), 3)
		);

	MemoryPool::InfoList	infoList;
	mMemoryPool->_getNowPoolInfo(infoList);	

	if (!infoList.empty())
	{		
		SAVE_MEMORY_LOG("------------------------------");
		for (MemoryPool::InfoList::iterator it=infoList.begin(); it!=infoList.end(); ++it)
		{
			SAVE_MEMORY_LOG("Pool size %s, last %s",
				getInfo(it->first, 0),
				getInfo(it->second, 1)
				);
		}
	}
	std::list<size_t> &useList = mMemoryPool->_getUseList();
	if (!useList.empty())
	{
		SAVE_MEMORY_LOG("------------------------------");
		for (std::list<size_t>::iterator it=useList.begin(); it!=useList.end(); ++it)
		{
			SAVE_MEMORY_LOG("pool size %s", getInfo(*it, 0));
		}
	}

#endif
	SAVE_MEMORY_LOG("==============================================");
	SAVE_MEMORY_LOG("�ڴ�ط���[%u]MB, ʹ��[%u]B, FREE [%u], last [%u]b",
		MEMORY_POOL_SIZE/(1024*1024), mUsePoolSize, mFreePoolSize, mUsePoolSize-mFreePoolSize);
	SAVE_MEMORY_LOG("ϵͳ�ڴ� USE [%.3f]M=[%.3f]K, NEW [%u]K, FREE [%u]K"
		, (float)(mUseSystemSize-mFreeSystemSize)/1024/1024
		, (float)(mUseSystemSize-mFreeSystemSize)/1024
		, mUseSystemSize
		, mFreeSystemSize);
	SAVE_MEMORY_LOG("==============================================");
	SAVE_MEMORY_LOG("�ܼƷ���Ͱ[%d], ������ռ�[%d]", TRUCK_MAX_NUM, MAX_OBJECT_SIZE);
	SAVE_MEMORY_LOG("�ܼƷ���ָ������: [%u]�ܼ��ͷ�ָ������[%u]", mAllNewCount, mAllFreeCount);
	SAVE_MEMORY_LOG("----------------------------------------------");
	size_t leakSize = 0;
	size_t totalAllotSize = 0;
	size_t totalNewSize = 0;
	size_t totalFreeSize = 0;
	for(int i=0; i<TRUCK_MAX_NUM; ++i)
	{
		totalAllotSize += mPointPtrList[i].mTotalSize;
		totalNewSize += mPointPtrList[i].mNewCount * mPointPtrList[i].getBlockSize();
		totalFreeSize += mPointPtrList[i].mFreeCount * mPointPtrList[i].getBlockSize();
		leakSize += (mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount) * mPointPtrList[i].getBlockSize();
		if (mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount>0)
		{
			SAVE_MEMORY_LOG("Ͱ[%d]-->�ܼƷ�������[%u], �ͷ�[%u], δ�ͷ�����[%u], �ܼƷ���ռ�[%u], ***δ�ͷſռ�[%u]",
				i,
				mPointPtrList[i].mNewCount,
				mPointPtrList[i].mFreeCount,
				mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount,
				mPointPtrList[i].mTotalSize,
				(mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount) * mPointPtrList[i].getBlockSize()
				) ;
		}
		else
		{
			SAVE_MEMORY_LOG("Ͱ[%d]-->�ܼƷ�������[%u], �ͷ�[%u], �ܼƷ���ռ�[%u]",
				i,
				mPointPtrList[i].mNewCount,
				mPointPtrList[i].mFreeCount,				
				mPointPtrList[i].mTotalSize
				) ;
		}

	}
	SAVE_MEMORY_LOG("**********************************************");
	SAVE_MEMORY_LOG("Total allot memory size [%u]", totalAllotSize);
	SAVE_MEMORY_LOG("Total new memory size [%u]", totalNewSize);
	SAVE_MEMORY_LOG("Total free memory size [%u]", totalFreeSize);
	SAVE_MEMORY_LOG("Leak memory size [%u]	check [%d]", leakSize, (int)totalNewSize-totalFreeSize);
	SAVE_MEMORY_LOG("==============================================");
	for(int i=0; i<TRUCK_MAX_NUM; ++i)
	{
		if (mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount>0)
		{
			SAVE_MEMORY_LOG("Ͱ[%d]-->�ܼƷ�������[%u], �ͷ�[%u], δ�ͷ�����[%u], ʣ��ռ�[%u] �ܼƷ���ռ�[%u]",
				i,
				mPointPtrList[i].mNewCount,
				mPointPtrList[i].mFreeCount,
				mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount,
				(mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount)*mPointPtrList[i].getBlockSize(),
				mPointPtrList[i].mTotalSize
				) ;
		}
	}
	SAVE_MEMORY_LOG("==============================================");

	for (MemInfoMap::iterator it = mMemInfoList.begin(); it!=mMemInfoList.end(); ++it)
	{
		const char* p = (const char*)(it->first);
		SAVE_MEMORY_LOG("exist no free [%u] ->File:[%s] Line:[%d] Function[%s]", it->second.mSize, it->second.mFile, it->second.mLine, it->second.mFunction);
	}
#if DEBUT_STRING
	if (this == &msAllocation)
	{	
		SAVE_MEMORY_LOG("String debug ...");
		SAVE_MEMORY_LOG("***************************************************");
		AString::gDebugLock.lock();
		for (auto it=AString::gDebugString.begin(); it!=AString::gDebugString.end(); ++it)
		{
			const char *t = *it;
			SAVE_MEMORY_LOG("%s", t);
		}
		AString::gDebugLock.unlock();
	}
#endif
	SAVE_MEMORY_LOG("==============================================");
	SAVE_MEMORY_LOG("print memory use infomation finish.");
	SAVE_MEMORY_LOG("-------------------------NEW INFO -----------------------");
	for (auto it=mNewInfo.begin(); it!=mNewInfo.end(); ++it)
	{
		sNEW_INFO &info = it->second;
		info.mLastCount = (info.mNewCount-info.mFreeCount);
		if (info.mLastCount>0)
			SAVE_MEMORY_LOG("A[%d] L[%d] now[%d] new [%d] F[%d] > %s", info.mAllotTime, info.mLastCount, info.mNewCount-info.mFreeCount, info.mNewCount, info.mFreeCount, it->first.c_str());
	}
	SAVE_MEMORY_LOG("==============================================");

#endif

    //gLog.close();
}
//---------------------------------------------------------------------------------------
#include "TimeManager.h"

#if DEVELOP_MODE 
 char Allot::gLogPathFile[256];
#endif

void Allot::setLogFile( const char* pathFileName )
{
#if MEMORY_DEBUT_TEST

	char	szPathFile[280];

	////SYSTEMTIME  sysTime;
	////GetLocalTime( &sysTime );
	//int hour = TimeManager::GetMe().GetHour(); //sysTime.wHour;
	//int minute = TimeManager::GetMe().GetMinute(); //sysTime.wMinute;
	//int second = TimeManager::GetMe().GetSecond(); //sysTime.wSecond;

	//sprintf( szPathFile, "%s_MemoryReport_[%2d��%2d��%2d].log ",pathFileName, hour,minute,second );
	sprintf( szPathFile, "MemLog/%s_MemoryReport.log ",pathFileName );
#if DEVELOP_MODE 
    strcpy (gLogPathFile, szPathFile);
#endif
	gLog.setFile(szPathFile, "wt");

#endif
}

void* Allot::newPtr( size_t size, const char* szFile, unsigned int nLine, const char* szFunction )
{
#if USE_SYSTEM_MEMORY
	return malloc(size); 
#endif

#if MEMORY_DEBUT_TEST
	int nowSize = msAllocation.mUsePoolSize;
	bool bNeedAdd = size<=MAX_OBJECT_SIZE-1;
#endif
	void *p = msAllocation.allot(size);
#if MEMORY_DEBUT_TEST
	if (p) 
	{
		if (bNeedAdd) 
			size += 1;
		//size = msAllocation.mUsePoolSize - nowSize;
		msAllocation.recordInfo(p, szFile, nLine, szFunction, size);
	}
#endif
	return p;
}

void* Allot::newPtr( size_t size )
{
#if USE_SYSTEM_MEMORY
	return malloc(size);
#endif

	return msAllocation.allot(size);
}

void Allot::freePtr( void *p )
{
#if USE_SYSTEM_MEMORY
	return ::free(p);
#endif

	return msAllocation.release(p);
}

void Allot::freePtr( void *ptr, size_t size )
{
#if USE_SYSTEM_MEMORY
	return ::free(ptr);
#endif

	(void)size;
	freePtr(ptr);
}

size_t Allot::_getSize( void *ptr )
{
	byte *pIndex = (byte*)ptr - 1;
	
	if (*pIndex<0xFF)
	{		
		return (*pIndex+1)*BASE_BLOCK_SIZE - 1;
	}
	//for (int i=0; i<TRUCK_MAX_NUM; ++i)
	//{
	//	if (mPointPtrList[i].HasPoint(ptr))
	//		return (i+1)*BASE_BLOCK_SIZE;
	//}
	return 0;
}

//---------------------------------------------------------------------------------------
void Allot::dumpNewInfo()
{
#if MEMORY_DEBUT_TEST
#	if MULTI_THREAD_LOCK
	CsLockTool sl(mLock);
#	endif
	SAVE_MEMORY_LOG("------------------------POOL INFO----------------------");
	//size_t leakSize = 0;
	size_t totalAllotSize = 0;
	size_t totalNewSize = 0;
	size_t totalFreeSize = 0;
	for(int i=0; i<TRUCK_MAX_NUM; ++i)
	{
		totalAllotSize += mPointPtrList[i].mTotalSize;
		totalNewSize += mPointPtrList[i].mNewCount * mPointPtrList[i].getBlockSize();
		totalFreeSize += mPointPtrList[i].mFreeCount * mPointPtrList[i].getBlockSize();
		//leakSize += (mPointPtrList[i].mNewCount - mPointPtrList[i].mFreeCount) * (i+1) * BLOCK_INIT_NUM;
	}
	SAVE_MEMORY_LOG("use [%.3f]M = [%.3f]K, new [%.3f]M, free [%.3f]M, total [%.3f]M"
		, (float)(totalNewSize-totalFreeSize)/1024/1024
		, (float)(totalNewSize-totalFreeSize)/1024
		, (float)totalNewSize/1024/1024
		, (float)totalFreeSize/1024/1024
		, (float)totalAllotSize/1024/1024
		);

	SAVE_MEMORY_LOG("ϵͳ�ڴ� USE [%.3f]M=[%.3f]K, NEW [%u]K, FREE [%u]K"
		, (float)(mUseSystemSize-mFreeSystemSize)/1024/1024
		, (float)(mUseSystemSize-mFreeSystemSize)/1024
		, mUseSystemSize
		, mFreeSystemSize
		);

	SAVE_MEMORY_LOG("�ܼ��ڴ� USE [%.3f]M=[%.3f]K, NEW [%u]K, FREE [%u]K"
		, (float)(totalNewSize+mUseSystemSize-(totalFreeSize+mFreeSystemSize))/1024/1024
		,  (float)(totalNewSize+mUseSystemSize-(totalFreeSize+mFreeSystemSize))/1024
		, mUseSystemSize + totalNewSize
		, mFreeSystemSize + totalFreeSize
		);
	SAVE_MEMORY_LOG("-------------------------NEW INFO -----------------------");
	for (auto it=mNewInfo.begin(); it!=mNewInfo.end(); ++it)
	{
		sNEW_INFO &info = it->second;
		info.mLastCount = (info.mNewCount-info.mFreeCount);
		if (info.mLastCount > 0)
		{
			SAVE_MEMORY_LOG("[%d] now[%.3f]M=[%.3f]K new [%d] F[%d] > %s"
				, info.mLastCount
				, (float)(info.mNewCount - info.mFreeCount) / 1024 / 1024
				, (float)(info.mNewCount - info.mFreeCount) / 1024
				, info.mNewCount, info.mFreeCount
				, it->first.c_str());
		}
	}
	SAVE_MEMORY_LOG("==============================================");
#else
	PRINT("δ���ڴ���Բ��Կ��� MEMORY_DEBUT_TEST \r\n");
#endif
}
//---------------------------------------------------------------------------------------
