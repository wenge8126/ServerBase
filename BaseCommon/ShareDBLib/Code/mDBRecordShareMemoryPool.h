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
// ��¼��ʹ�õĹ����ڴ��
// NOTE: 1 �ڴ�����С�������9�����򱣴治�˿��ָ�룬��������ָ�����, ָ��ǰ��һ���ֽ�������ʶ��¼״̬(Ŀǰδʹ��)
// 2 ����Ϊ������������¼������֧�� 32 * 0xFFFE, DBServerָ��Ϊ1000��Լ6500��
// 3 ����ؿ������� 0XFFFE, ��ֱ�Ӵ�ϵͳ����, �ؿ��ʶΪ 0xFFFF, 
//   ������ʱ��Ҫ���,��Ӱ������ٶ�
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
			NOTE_LOG("��¼�ڴ��[%u] Size[%llu = %llu] ����[%d] �ͷ�[%d], ϵͳ����[%llu]", mObjectSize, mAllotCount * mObjectSize, mAllotCount, mNewCount, mFreeCount, mSysCount);
#endif
		}

		// ��������, �ر�����߳�, DB����Ҳ�Ѿ��ر�
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
			//����ͷ4���ֽڱ���ǿ�С�����Ϊָ����һ�ڴ���ָ�룬����m_NewPointer��ָ������һ���ڴ�飬�Ա��´η���ʹ�á� 
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

		// �������
		void ProcessRecordDataSave(MySqlDBTool  *pDBTool);

		// �ر�ʱ, Ҫ����뱣�� 	 NOTE: �˺��������ڹر�ʱ, DB����Ҳ�Ѿ��ر�, ���Բ���Ҫ�ټ���, ֱ�Ӽ�����
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
