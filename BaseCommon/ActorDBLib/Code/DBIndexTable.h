#ifndef _INCLUDE_DBINDEXTABLE_H_
#define _INCLUDE_DBINDEXTABLE_H_

#pragma once

#include "ActorDBLib.h"
#include "LogicDBTable.h"
#include <unordered_map>

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// 索引功能表格, 所有常驻内存
	// NOTE: 目录只支持Int64 主键
	//-------------------------------------------------------------------------
	class ActorDBLib_Export_H  DBIndexTable : public LogicDBTable
	{		
	public:
		typedef std::unordered_map<Int64, ARecord> RecordMap;

	protected:
		RecordMap	mRecordMap;

	public:
		DBIndexTable(bool bShareSQL)
			: LogicDBTable(bShareSQL)
		{

		}

		virtual void ApplyExt(AutoNice extParam) override
		{
			LogicDBTable::ApplyExt(extParam);
			if (mbStringKey)
			{
				ERROR_LOG("目前不支持字符串DBIndexTable");
			}
			else
			{
				NOTE_LOG("*** [%s] use index mode, then record alway in memory, Start load all record ...", GetTableName());
				LoadAllRecord(NULL);
				NOTE_LOG("*** [%s] use index mode, Finish load all record %lld", GetTableName(), mRecordMap.size());
			}
		}

		RecordMap& GetRecordMap() { return mRecordMap; }

	public:
		virtual void InsertDBNewRecord(ARecord newRecord) override
		{
			LogicDBTable::InsertDBNewRecord(newRecord);
			mRecordMap.insert(RecordMap::value_type(newRecord[0], newRecord));
		}

		virtual ARecord GrowthNewRecord(DataStream *recordData /* = NULL */)  override
		{
			ARecord newRe = LogicDBTable::GrowthNewRecord(recordData);
			if (newRe)
			{
				mRecordMap.insert(RecordMap::value_type(newRe[0], newRe));
			}
			return newRe;
		}

		virtual ARecord GetRecord(const char* szIndex) override
		{			
				Int64 key = TOINT64(szIndex);
				ARecord re;
				auto it = mRecordMap.find(key);
				if (it != mRecordMap.end())
				{
					re = it->second;
					if (re)
					{						
						return re;
					}
				}
				re = LogicDBTable::GetRecord(szIndex);
				if (re)
					mRecordMap.insert(RecordMap::value_type(key, re));

				return re;			
		}

		virtual ARecord GetRecord(Int64 key) override
		{
			ARecord re;
			auto it = mRecordMap.find(key);
			if (it != mRecordMap.end())
			{
				re = it->second;
				if (re)
				{
					return re;
				}
			}
			re = LogicDBTable::GetRecord(key);
			if (re)
				mRecordMap.insert(RecordMap::value_type(key, re));

			return re;
		}

		virtual bool LoadAllRecord(DBUserComponent *pComponent) override;

		virtual bool DeleteRecord(ARecord record) override;
	};

}


#endif //_INCLUDE_DBINDEXTABLE_H_