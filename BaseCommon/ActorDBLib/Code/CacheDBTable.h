#pragma once

#ifndef _INCLUDE_CACHEDBTABLE_H_
#define _INCLUDE_CACHEDBTABLE_H_

#include <unordered_map>
#include "LogicDBTable.h"

#define ONCE_CHECK_TIME		(1)
#define ONCE_CHECK_COUNT (100)
//-------------------------------------------------------------------------

namespace NetCloud
{

	class CachaDBRecord : public LogicDBRecord
	{
	public:
		CachaDBRecord() {}
		CachaDBRecord(tBaseTable *pTable)
			: LogicDBRecord(pTable)
		{
			Active();
		}

		bool CheckOver(UInt64 nowTime, int overSecond)
		{
			return (nowTime - mActiveTime > overSecond);
		}

		void Active()
		{
			mActiveTime = TimeManager::Now();
			//NOTE_LOG("%s : %s recond active %llu", GetTable()->GetTableName(), get(0).c_str(), mActiveTime);
		}

		virtual void SaveUpdate() override
		{
			LogicDBRecord::SaveUpdate();
			Active();
		}

	public:
		UInt64 mActiveTime = 0;
	};


	//-------------------------------------------------------------------------
	// 具有缓存功能的DB表格
	// 可以覆盖 DBManager > NeedCache(表名) 启用
	//-------------------------------------------------------------------------
	class ActorDBLib_Export CacheDBTable : public LogicDBTable
	{
		typedef std::unordered_map<std::string, ARecord>  StringRecordMap;
		typedef std::unordered_map<Int64, ARecord> IntRecordMap;

	public:
		ARecord _NewRecord()
		{
			return MEM_NEW CachaDBRecord(this);
		}

		ARecord NewRecord()
		{
			ARecord re = MEM_NEW CachaDBRecord(this);
			return re;
		}

		virtual ARecord GrowthNewRecord(DataStream *recordData /* = NULL */)  override
		{
			ARecord newRe = LogicDBTable::GrowthNewRecord(recordData);
			if (newRe)
			{
				if (mbStringKey)
					mStringKeyRecordList.insert(StringRecordMap::value_type(newRe[0].c_str(), newRe));
				else
					mDataRecordList.insert(IntRecordMap::value_type(newRe[0], newRe));
			}
			return newRe;
		}

		virtual ARecord GetRecord(const char* szIndex) override
		{

			if (mbStringKey)
			{
				Auto<CachaDBRecord> re;
				auto it = mStringKeyRecordList.find(szIndex);
				if (it != mStringKeyRecordList.end())
				{
					re = it->second;
					if (re)
					{
						re->Active();
						return re;
					}
				}

				re = LogicDBTable::GetRecord(szIndex);
				if (re)
					mStringKeyRecordList.insert(StringRecordMap::value_type(szIndex, re));

				return re;
			}
			else
			{
				Int64 key = TOINT64(szIndex);
				Auto<CachaDBRecord> re;
				auto it = mDataRecordList.find(key);
				if (it != mDataRecordList.end())
				{
					re = it->second;
					if (re)
					{
						re->Active();
						return re;
					}
				}
				re = LogicDBTable::GetRecord(szIndex);
				if (re)
					mDataRecordList.insert(IntRecordMap::value_type(key, re));

				return re;
			}

		}

		virtual ARecord GetRecord(Int64 key) override
		{
			if (mbStringKey)
			{
				Auto<CachaDBRecord> re;
				auto it = mStringKeyRecordList.find(STRING(key));
				if (it != mStringKeyRecordList.end())
				{
					re = it->second;
					if (re)
					{
						re->Active();
						return re;
					}
				}

				re = LogicDBTable::GetRecord(key);
				if (re)
					mStringKeyRecordList.insert(StringRecordMap::value_type(STRING(key), re));

				return re;
			}
			else
			{
				Auto<CachaDBRecord> re;
				auto it = mDataRecordList.find(key);
				if (it != mDataRecordList.end())
				{
					re = it->second;
					if (re)
					{
						re->Active();
						return re;
					}
				}

				re = LogicDBTable::GetRecord(key);
				if (re)
					mDataRecordList.insert(IntRecordMap::value_type(key, re));

				return re;
			}

		}

	public:
		CacheDBTable(bool bShareSQL, int activeOverSecond)
			: LogicDBTable(bShareSQL)
			, mActiveOverSecond(activeOverSecond)
		{}

		~CacheDBTable()
		{

		}

		virtual void ApplyExt(AutoNice extParam) override
		{
			LogicDBTable::ApplyExt(extParam);
			NOTE_LOG("*** [%s] use cache mode , Key is %s ***", GetTableName(), mbStringKey ? "STRING":"INT64");
		}

		virtual void LowProcess() override
		{
			LogicDBTable::LowProcess();

			if (TimeManager::Now() - mLastUpdateDataTimeSec >= ONCE_CHECK_TIME)
			{
				mLastUpdateDataTimeSec = TimeManager::Now();

				if (mbStringKey)
				{
					auto d = CheckCool(mStringKeyRecordList, mLastStringKey);
					if (d.empty())
						mLastStringKey.clear();
					else
						mLastStringKey = d.c_str();
				}
				else
				{
					auto d = CheckCool(mDataRecordList, mLastIntKey);
					if (d.empty())
						mLastIntKey = 0;
					else
						mLastIntKey = d;
				}
			}
		}

		template<typename KEY>
		Data CheckCool(std::unordered_map<KEY, ARecord> &list, KEY nowkey)
		{
			auto nowIt = list.find(nowkey);
			if (!mbNowHasKey || nowIt == list.end())
				nowIt = list.begin();
			int count = 0;
			UInt64 nowTime = TimeManager::Now();
			while (nowIt != list.end())
			{
				Auto<CachaDBRecord> re = nowIt->second;
				if (!re || re->CheckOver(nowTime, mActiveOverSecond))
				{
					NOTE_LOG("%s : %s recond cool %llu - %llu = %llu, then free", GetTableName(), re[0].c_str(), nowTime, re->mActiveTime, nowTime-re->mActiveTime);
					if (re)
						re->SaveUpdate();
					nowIt = list.erase(nowIt);
				}
				else
					++nowIt;
				if (++count > ONCE_CHECK_COUNT)
					break;
			}
			if (nowIt != list.end())
			{
				mbNowHasKey = true;
				return nowIt->second[0];
			}
			else
			{
				mbNowHasKey = false;
				return Data();
			}
		}

	protected:
		std::unordered_map<Int64, ARecord> mDataRecordList;
		std::unordered_map<std::string, ARecord> mStringKeyRecordList;
		Int64							mLastIntKey;
		std::string					mLastStringKey;
		bool							mbNowHasKey = false;
		UInt64						mLastUpdateDataTimeSec = 0;

		int								mActiveOverSecond = 0;
	};
	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_CACHEDBTABLE_H_