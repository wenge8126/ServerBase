#ifndef _INCLDUE_NOSQLCOMPONECNT_H_
#define _INCLDUE_NOSQLCOMPONECNT_H_

#pragma once
#include "Component.h"
#include "PoolLoop.h"

namespace NetCloud
{
	typedef int			DB_HASH;

	class NoSQLData : public AutoBase
	{
	public:
		UInt64			mActiveMilSecond = 0;
		Buffer			mData;
		DB_HASH				mFieldHash = 0;
	};
	typename Auto<NoSQLData> ANoSQLData;

	class NoSQLComponent : public Component
	{
	public:
		virtual void LowUpdate() override
		{
			CheckCool();
		}

		void CheckCool()
		{
			int count = 100;
			UInt64 nowSec = TimeManager::Now();

			for (int i = 0; i < count; ++i)
			{
				auto pNow = mActiveList.__getRoot();

				if (TimeManager::Now() - pNow->mValue->mActiveMilSecond > 3600)
				{
					pNow->mValue->mData._free();
					pNow->mValue->mActiveMilSecond = 0;
					mActiveList._remove(pNow);
				}
				else
					mActiveList.loop();
			}
		}

	public:
		FastHash<AString, ANoSQLData>	mNoSQLDataList;
		FastHash<DB_HASH, AutoData>	mFieldList;
		PoolLoop<ANoSQLData>				mActiveList;
	};

	class DBMySQLComponent : public MySQLComponent
	{
	public:
		LogicDBTable mFieldTable;
		LogicDBTable mDataTable;
	};

	// 使用端结构
	class tDBData : public AutoBase
	{
	public:
		Int64 GetID() { return MAKE_INDEX_ID(mKey.c_str()); }

		UnitID GetNoSQLID(int dbCode, int nDBCount) 
		{
			return UnitID(dbCode, GetID() % nDBCount);
		}

	public:
		AString			mKey;
		ARecord		mData;
		AutoField		mField;
	};

	// 使用端
	class NoSQLUserComponent : public Component
	{
	public:
		bool Init(tDBData *pDBData)
		{
			return false;
		}

		bool Save(tDBData *pDBData)
		{
			return false;
		}

		bool Load(tDBData *pDBData)
		{
			return false;
		}
	};
}
#endif //_INCLDUE_NOSQLCOMPONECNT_H_
