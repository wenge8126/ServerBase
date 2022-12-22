#pragma once

#ifndef _INCLUDE_DBUSERCOMPONENT_H_
#define _INCLUDE_DBUSERCOMPONENT_H_

#include "Actor.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
	使用Actor DB功能组件
	1 可以不依赖DBActor, 要求必须使用DBActoryManager
	2 由结构表格导出中间数据表结构类, 方便直接操作DB记录
	//-------------------------------------------------------------------------*/

	class ActorDBLib_Export_H DBUserComponent : public Component
	{
	public:
		virtual const char* GetTableName() const { return "None"; }
		virtual bool CheckTable(AutoTable t) = 0;
		virtual void OnLoadRecord(ARecord record){}

		virtual bool GetDataInfo(const char *szKey, const AString &checkMD5, Int64 &size, AString &md5) { NOTE_LOG("GetDataInfo no override"); return false; }

		virtual AutoData GetData(const char *szKey, const AString &checkMD5, Int64 position, int size) { NOTE_LOG("GetData no override"); return false; }

		virtual ARecord ReadyRecord(const char *szKey)
		{
			ARecord re;
			if (IsValid(szKey))
				re = mDataRecord;
			else
				re = LoadRecord(szKey);
			return re;
		}

		virtual AutoData GetRecordData(const char *szKey)
		{
			ARecord re = ReadyRecord(szKey);

			if (re)
			{
				AutoData d = MEM_NEW DataBuffer();
				if (re->saveData(d.getPtr()))
					return d;

				ERROR_LOG("Save record fail : %s", szKey);
			}
			else
				WARN_LOG("No exist record :  %s", szKey);
			return AutoData();
		}		

		virtual bool SaveRecordByData(const char *szKey, AutoData d, bool bGrowthKey, AString &resultKey)
		{
			if (!mDBTable ||!d)
				return false;

			d->seek(0);
			if (bGrowthKey)
			{
				ARecord newRe = mDBTable->GrowthNewRecord(d.getPtr());
				if (newRe)
					resultKey = newRe[0].string();
				return newRe;
			}

			ARecord re = mDBTable->NewRecord();
			re->_alloctData(0);
			if (!re->restoreData(d.getPtr()))
			{
				ERROR_LOG("Restore record fail : %s", szKey);
				return false;
			}

			ARecord existRe = ReadyRecord(szKey);
			if (!existRe)
				mDBTable->InsertDBNewRecord(re);
			else
			{
				for (int i = 1; i < mDBTable->GetField()->getCount(); ++i)
				{
					existRe->set(i, re->get(i));
				}
				existRe->SaveUpdate();
			}
			resultKey = szKey;
			return true;
		}

		AutoData GetTableFieldData()
		{
			return mFieldData;
		}

		virtual void Awake() override
		{
			if (!InitTable(GetTableName()))
				ERROR_LOG("DB component init table fail : %s", GetTableName());
		}

		virtual bool LoadMaxKey(AString &maxKey)
		{
			Auto<LogicDBTable> t = mDBTable;
			if (t)
				return  t-> LoadMaxKey(maxKey);
			ERROR_LOG("Table no exist : %s", GetTableName());
			return false;
		}

	public:
		bool IsValid() { return mDataRecord; }
		bool IsValid(const char *szKey)
		{
			if (mDataRecord && mDataRecord[0].string() == szKey)
				return true;
			return false;
		}
		bool ResetRecord(const char *szKey)
		{
			LowUpdate();
			mDataRecord = LoadRecord(szKey);
			return mDataRecord;
		}

		bool ResetRecord(Int64 key)
		{
			LowUpdate();
			mDataRecord = LoadRecord(key);
			return mDataRecord;
		}

		bool ResetRecord(ARecord record)
		{
			if (mDataRecord == record)
				return record;

			LowUpdate();
			mDataRecord = record;
			return mDataRecord;
		}

		bool InitTable(const char *szTableName)
		{
			AutoTable t = GetDBMgr()->GetTable(szTableName);
			
			if (t && CheckTable(t))
			{
				mDBTable = t;
				mFieldData = MEM_NEW DataBuffer();
				mDBTable->GetField()->saveToData(mFieldData.getPtr());
				return true;
			}
			return false;
		}

		AutoTable GetTable() { return mDBTable; }

		AutoDBManager GetDBMgr()
		{
			return mpActor->GetDBMgr();
		}

		ARecord LoadRecord(const char *szKey)
		{
			if (!mDBTable)
			{
				ERROR_LOG("Must init table");
				return ARecord();
			}

			ARecord re = mDBTable->GetRecord(szKey);
			if (re)
			{
				mKey = szKey;
				mDataRecord = re;
				return re;
			}
			ERROR_LOG("No exist %s in table %s", szKey, mDBTable->GetTableName());

			return ARecord();
		}

		ARecord LoadRecord(Int64 nKey)
		{
			if (!mDBTable)
			{
				ERROR_LOG("Must init table");
				return ARecord();
			}
			ARecord re = mDBTable->GetRecord(nKey);
			if (re)
			{
				mKey = STRING(nKey);
				mDataRecord = re;
				return re;
			}
			ERROR_LOG("No exist %s in table %s", STRING(nKey), mDBTable->GetTableName());

			return ARecord();
		}

		virtual ARecord GrowthNewRecord()
		{
			mDataRecord = mDBTable->GrowthNewRecord();
			return mDataRecord;
		}

	public:
		virtual void LowUpdate() override
		{			
			if (mDataRecord)
				mDataRecord->SaveUpdate();
		}

	protected:
		AString		mKey;
		ARecord	mDataRecord;
		AutoTable mDBTable;
		AutoData  mFieldData;
	};

}

#endif //_INCLUDE_DBUSERCOMPONENT_H_
