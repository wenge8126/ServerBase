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
		virtual bool CheckTable(AutoTable t) = 0;

	public:
		bool InitTable(const char *szTableName)
		{
			AutoTable t = GetDBMgr()->GetTable(szTableName);
			if (CheckTable(t))
			{
				mDBTable = t;
				return true;
			}
			return false;
		}

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
	};

}

#endif //_INCLUDE_DBUSERCOMPONENT_H_
