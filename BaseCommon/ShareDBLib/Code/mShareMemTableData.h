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
	// 1 ����DB������ݼ�¼�����ڴ�
	// 2 Ϊÿ���ڴ�鿪��һ������߳�
	// 3 ��ȫ�ر�
	// 4 ÿ���ڴ��, ����һ����Ӧ������߳�
	//NOTE:�߳���,ֻ��Ӧ�ñ�ǩ��Ϊ eShareMemory_Free �� ���Լ������, �������߼��߳���, ���úü�¼KEY��, �����ñ�ʶΪeShareMemory_Used
	// DB���� Ҳֻ�ܶ� ��ʶ Ϊ eShareMemory_Used �Ľ��м����޸�
	// DB ��������, ��GetRecord �ӵ�ǰ����л�ȡ, ������, �ȶ�KEY�Ƿ�һ��, ��һ�����(SQL�Ѿ��䴦��)
	//  �̳� virtual void setNull(), ����ִ�к�, ��� Ϊ useCount==1 ���������
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

		AutoField	    mField;		// ���������¼�ڴ�
		AutoTable		mTable;
		Auto<TempSaveIndexRecord>			mRecord;
		Auto< TempLoadRecord>					mCreateRecord;
		ShareMemDBManager						*mpDBMgr;

		Int64													mLastMaxKey;				// ��ǰ����������ֵ���͵����KEY
		EasyMap<short, bool>						mKeyHashSlotList;			// �����¼KEY��Ӧ�Ĺ�ϣSLOT
		EasyHash<AString, SHARE_PTR>		mStringRecordHash;		// ���м�¼��ϣ�б�
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