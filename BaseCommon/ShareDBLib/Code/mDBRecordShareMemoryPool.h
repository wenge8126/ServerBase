#pragma once

#ifndef _INCLUDE_DBRECORDSHAREMEMORYPOOL_H_
#define _INCLUDE_DBRECORDSHAREMEMORYPOOL_H_

#include "EasyMap.h"
#include "ShareMemAO.h"
#include "DataStream.h"
#include "ShareDBLib.h"
#include "IndexDBRecord.h"
#include "MySqlDBTool.h"

#define RECORD_POOL_DEBUG	1
//-------------------------------------------------------------------------*/
// 记录中使用的共享内存池
// NOTE: 1 内存分配大小必须大于9，否则保存不了块的指针，出现严重指针错误, 指针前面一个字节用来标识记录状态(目前未使用)
// 2 索引为短整数，最大记录数量数支持 32 * 0xFFFE, DBServer指定为1000，约6500万
// 3 如果池块数超过 0XFFFE, 则直接从系统分配, 池块标识为 0xFFFF, 
//   但分配时需要检查,会影响分配速度
//-------------------------------------------------------------------------*/
class tBaseTable;
class MySqlDBTool;

namespace NetCloud
{
	class ShareMemTableData;
	//class ShareMemReocrdData;

	class TempSaveIndexRecord : public IndexDBRecord
	{
	public:
		char        *mpStateData;

	public:
		TempSaveIndexRecord(AutoTable hOwnerTable)
			: IndexDBRecord(hOwnerTable)
			, mpStateData(NULL)
		{

		}
		~TempSaveIndexRecord()
		{
			mpStateData = NULL;
			mRecordData = NULL;
		}

		void SetRecordData(char *szRecordData)
		{
			mRecordData = szRecordData;
		}

		virtual char* _getStateData() override
		{
			return mpStateData;
		}

		virtual DSIZE _getStateDataSize() override
		{
			return IndexDBRecord::_getStateDataSize() + sizeof(ShareUpdateInfo);
		}

		virtual ShareUpdateInfo *getUpdateInfo()
		{
			return (ShareUpdateInfo*)(_getStateData() + _getStateDataSize() - sizeof(ShareUpdateInfo));
		}

	public:
		virtual void InitData() override
		{
			ERROR_LOG("Can not use InitData");
		}
		virtual void _alloctData(int extData) override
		{
			ERROR_LOG("Can not use _alloctData");
		}

	};


	struct DBRecordShareMemoryPool
	{
		friend class NewCreateShareMemFieldIndex;
	protected:
		ShareMemTableData					*mpMemTableData;
		unsigned char*						mpNewPointer;

		EasyMap<int, ShareMemAO*, 128, false, false>		mAllocList;
		uint										    mObjectSize;
		int										    mNowShareMemoryIndex;
		int											mNowAllocCount;
		SQLDataArray							mTempSQLData;

#if RECORD_POOL_DEBUG
		size_t			mNewCount;
		size_t			mFreeCount;
		size_t			mAllotCount;
		size_t			mSysCount;
#endif

	public:
		DBRecordShareMemoryPool(ShareMemTableData *pMemSql);

		~DBRecordShareMemoryPool()
		{
			clear();
#if RECORD_POOL_DEBUG
			NOTE_LOG("记录内存池[%u] Size[%llu = %llu] 分配[%d] 释放[%d], 系统分配[%llu]", mObjectSize, mAllotCount * mObjectSize, mAllotCount, mNewCount, mFreeCount, mSysCount);
#endif
		}

		// 最后检查落地, 关闭落地线程, DB进程也已经关闭
		void Close();

		void clear();

		void setBlockSize(size_t size)
		{
			mObjectSize = size + sizeof(byte);
			if (mObjectSize < 9)
				mObjectSize = 9;
		}

		size_t getBlockSize() const { return mObjectSize - 1; }

		void *newPtr()
		{
			if (mpNewPointer == NULL)
				MyAlloc();

			unsigned char *rp = mpNewPointer;
			//由于头4个字节被“强行”解释为指向下一内存块的指针，这里m_NewPointer就指向了下一个内存块，以备下次分配使用。 
			mpNewPointer = *reinterpret_cast<unsigned char**>(rp);

			*((unsigned char*)rp - 1) = eShareMemory_Used;

#if RECORD_POOL_DEBUG
			mNewCount++;
#endif

			return rp;
		}

		bool deletePtr(unsigned char *ptr)
		{
			*((unsigned char*)(ptr - 1)) = eShareMemory_Free;

			*reinterpret_cast<unsigned char**>(ptr) = mpNewPointer;
			mpNewPointer = static_cast<unsigned char*>(ptr);


#if RECORD_POOL_DEBUG
			mFreeCount++;
#endif
			return true;
		}

		virtual SHARE_PTR getShareMemoryPtr(const char *szRecordDataPtr);

		void* getShareMemory(SHARE_PTR memPtr);

		// 遍历落地
		void ProcessRecordDataSave(MySqlDBTool  *pDBTool);

		// 关闭时, 要求必须保存 	 NOTE: 此函数仅用于关闭时, DB进程也已经关闭, 所以不需要再加锁, 直接检查落地
		void ProcessRecordDataMustSave(MySqlDBTool *pDBTool);

		void _UpdateShareMemoryRecord(MySqlDBTool *pDBTool, char *szRecordData, AString &sqlString, SQLDataArray &fieldRecordData, char *szStateData, bool bInsert);

		void UpdateShareMemoryRecord(MySqlDBTool *pDBTool, int shareMemIndex, int offsetPos, char *szStateData, byte stateDataSize, bool bInsert);

		void SaveMemPtrData(DataStream *pDestData);

	protected:
		void MyAlloc();

		//void OnCreateShareMemory(int index);
	};
	//-------------------------------------------------------------------------*
}
#endif //_INCLUDE_RECORDSHAREMEMORYPOOL_H_
