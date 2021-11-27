#pragma once

#ifndef _INCLUDE_LOGICDBTABLE_H_
#define _INCLUDE_LOGICDBTABLE_H_

#include "IndexBaseTable.h"
#include "IndexDBRecord.h"
#include "ShareSQLUpdate.h"
#include "DBTableLoadSQL.h"

namespace NetCloud
{
	class DBTableManager;
	//-------------------------------------------------------------------------
	// �߼�ʹ�õı�, ��Ҫ���ṩ��ļ�¼�����ȡ����, 
	// ����������¼, ��¼������Actor��
	//-------------------------------------------------------------------------
	class LogicDBTable : public SkipBaseTable
	{
		friend class DBTableManager;
		friend class LogicDBRecord;

	public:
		LogicDBTable(bool bShareSQL);

		~LogicDBTable()
		{}

		virtual void Process() override { mDBDataLoadSQL->Process(); }
		virtual void LowProcess()  { mDBDataLoadSQL->LowProcess(); }

	public:
		// ��ȡ��¼, ���첽��SQL�н��м���, ���ܲ�����
		virtual ARecord GetRecord(const char* szIndex) override
		{
			ARecord re = NewRecord();
			re->_alloctData(0);
			re->_set(0, szIndex);
			if (re->ReloadData(szIndex))
				return re;

			return ARecord();
		}

		virtual ARecord GetRecord(Int64 key) override
		{
			ARecord re = NewRecord();
			re->_alloctData(0);
			re->_set(0, key);
			if (re->ReloadData(STRING(key)) )
				return re;

			return ARecord();
		}

		virtual ARecord GetRecord(float key) override
		{
			ERROR_LOG("Can not use float key");

			return ARecord();
		}

		virtual ARecord _NewRecord() override;

		virtual ARecord NewRecord() override;

		// ������¼��, ���ټ��뵽����
		virtual ARecord CreateRecord(const char* szIndex, bool bReplace) override
		{
			ARecord r = _NewRecord();
			r->_alloctData(0);
			r->_set(0, szIndex);
			InsertDBNewRecord(r);
			return r;
		}

		virtual ARecord CreateRecord(Int64 nIndex, bool bReplace) override
		{
			ARecord r = _NewRecord();
			r->_alloctData(0);
			r->_set(0, nIndex);
			InsertDBNewRecord(r);
			return r;
		}

		virtual ARecord GrowthNewRecord(DataStream *recordData = NULL) override;

		virtual void InsertDBNewRecord(ARecord newRecord);

		virtual bool DeleteRecord(ARecord record) override
		{
			return false;
		}

		virtual void ApplyExt(AutoNice extParam);

	protected:
		DBTableManager					*mpDB = NULL;
		AutoNice								mSQLParam;
		Hand<DBTableLoadSQL>		mDBDataLoadSQL;

		AString mKeyFieldName;
		bool mbStringKey = false;
	};

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	class LogicDBRecord : public IndexDBRecord
	{
	public:
		LogicDBRecord(){}
		LogicDBRecord(tBaseTable *pTable)
		{
			mOwnerTable = pTable->GetTablePtr();
		}

	public:
		virtual bool IsNull() const override { return mRecordData == NULL; }

		virtual bool ReloadData(const char *szIndex) override
		{
			Auto<LogicDBTable> t = GetTable();
			return (t->mDBDataLoadSQL->AwaitLoadRecord(szIndex, this));

		}

		virtual void SaveUpdate() override;


		void FreeDataMemory()
		{
			_freeData();
		}
	};

	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_LOGICDBTABLE_H_
