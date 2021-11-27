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
	// 逻辑使用的表, 主要是提供表的记录保存读取功能, 
	// 不再索引记录, 记录保存在Actor中
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
		// 获取记录, 是异步从SQL中进行加载, 可能不存在
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

		// 创建记录后, 不再加入到索引
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
