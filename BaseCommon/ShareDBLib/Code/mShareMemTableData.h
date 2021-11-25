#pragma once

#ifndef _INCLUDE_SHAREMEMTABLEDATA_H_
#define _INCLUDE_SHAREMEMTABLEDATA_H_

#include "IndexDBRecord.h"
#include "ArrayIndex.h"
#include "ShareMemAO.h"
#include "ShareDBLib.h"
#include "mDBRecordShareMemoryPool.h"
#include "MySqlDBTool.h"

class tDBTool;

namespace NetCloud
{
	class TempLoadRecord : public IndexDBRecord
	{
	public:
		char        *mpStateData = NULL;

	public:
		TempLoadRecord(AutoTable hOwnerTable)
			: IndexDBRecord(hOwnerTable)
		{

		}
		~TempLoadRecord()
		{
		}

		void SetRecordData(char *szRecordData)
		{
			mRecordData = szRecordData;
		}

		char* GetRecordData() { return mRecordData; }

		virtual DSIZE _getStateDataSize() override
		{
			return IndexDBRecord::_getStateDataSize() + sizeof(ShareUpdateInfo);
		}

		virtual ShareUpdateInfo *getUpdateInfo()
		{
			return (ShareUpdateInfo*)(IndexDBRecord::_getStateData() + _getStateDataSize() - sizeof(ShareUpdateInfo));
		}

		virtual char* _getStateData() override
		{
			if (mpStateData != NULL)
				return mpStateData;
			return IndexDBRecord::_getStateData();
		}
	};
	//-------------------------------------------------------------------------
	// 1 保存DB表格数据记录共享内存
	// 2 为每个内存块开启一个落地线程
	// 3 安全关闭
	// 4 每个内存块, 开启一个对应的落地线程
	//NOTE:线程内,只对应用标签不为 eShareMemory_Free 的 尝试加锁落地, 创建在逻辑线程内, 设置好记录KEY后, 再设置标识为eShareMemory_Used
	// DB进程 也只能对 标识 为 eShareMemory_Used 的进行加锁修改
	// DB 加锁技巧, 在GetRecord 从当前表格中获取, 加锁后, 比对KEY是否一至, 不一至清除(SQL已经冷处理)
	//  继承 virtual void setNull(), 基类执行后, 如果 为 useCount==1 则解锁处理
	//-------------------------------------------------------------------------
	class ShareMemDBManager;
	class ShareDBLib_Export ShareMemTableData : public AutoBase
	{
	public:
		AString			mTableName;
		AString			mKeyFieldName;
		AString			mType;
		bool		        mbKeyIsString;
		AutoNice		mExtParam;

		AutoField	    mField;		// 用来分配记录内存
		AutoTable		mTable;
		Auto<TempSaveIndexRecord>			mRecord;
		Auto< TempLoadRecord>					mCreateRecord;
		ShareMemDBManager						*mpDBMgr;

		Int64													mLastMaxKey;				// 当前表格内最大数值类型的最大KEY
		EasyMap<short, bool>						mKeyHashSlotList;			// 保存记录KEY对应的哈希SLOT
		EasyHash<AString, SHARE_PTR>		mStringRecordHash;		// 所有记录哈希列表
		EasyHash<Int64, SHARE_PTR>			mRecordHash;

		SQLDataArray										mTempSQLData;

	public:
		ShareMemTableData(const AString &tableName, ShareMemDBManager *pMgr)
			: mTableName(tableName)
			, mpDBMgr(pMgr)
			, mLastMaxKey(0)
		{

		}

		~ShareMemTableData();

		const char* GetTableName() const { return mTableName.c_str(); }

	public:
		bool InitField(const AString &strFieldInfo, AString &resultInfo);

		void InitExtParam(AutoNice extNice)
		{
			mExtParam = extNice;
			AString slotData = extNice["SLOT"];

			SetSlotData(slotData, mKeyHashSlotList);
		}

		static void SetSlotData(const AString &slotData, EasyMap<short, bool> &resultSlotMap);

		AString LoadAllRecord(tDBTool *pMySql);

		void MakeKeyValue(AString &keyValue, AString &sqlKeyValue)
		{
			if (mbKeyIsString)
				sqlKeyValue.Format("'%s'", keyValue.c_str());
			else
				sqlKeyValue = keyValue;
		}

		FieldInfo MakeSQLValue(int nCol, AString &dataValue, AString &sqlDataValue)
		{
			FieldInfo info = mField->getFieldInfo(nCol);
			if (info == NULL)
			{
				ERROR_LOG("Table %s field no exist >col = %d", mTableName.c_str(), nCol);
				return NULL;
			}
			if (info->getType() == FIELD_STRING)
				sqlDataValue.Format("'%s'", dataValue.c_str());
			else
				sqlDataValue = dataValue;
			return info;
		}

		//bool AppendShareMemory(int index, int recordDataSize);

		SHARE_PTR NewCreateRecord(const AString &key);

		SHARE_PTR GrowthNewRecord();

		bool DeleteRecord(UInt64  recordDataPtr, const AString &recordKey);

		void UpdateShareRecord(const  AString &recordKey, Buffer &recordStateData, uint nowUpdateCount);
	};

	typedef Auto<ShareMemTableData> AShareMemTableData;
}


#endif //_INCLUDE_SHAREMEMTABLEDATA_H_