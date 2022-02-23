#ifndef _INCLDUE_NOSQLCOMPONECNT_H_
#define _INCLDUE_NOSQLCOMPONECNT_H_

#pragma once
#include "Component.h"
#include "PoolLoop.h"
#include "NoSQLNetMsg.h"
#include "NetCommon.h"
#include "FastHash.h"
#include "Actor.h"
#include "ActorManager.h"
#include "SQLComponent.h"
#include "PoolLoop.h"

namespace NetCloud
{
	typedef int			DB_HASH;
	//-------------------------------------------------------------------------
	class NoSQLData : public AutoBase
	{
	public:
		UInt64			mActiveMilSecond = 0;
		Buffer			mData;
		DB_HASH				mFieldHash = 0;
	};
	typedef Auto<NoSQLData> ANoSQLData;
	//-------------------------------------------------------------------------
	class ActorDBLib_Export NoSQLComponent : public Component
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
		void Notify(SQL_SaveNoSQLData &resp, UnitID senderID)
		{

		}

		void On(SQL_LoadNoSQLData &req, SQL_ResponseNoSQLData &resp, UnitID sender)
		{

		}

		void On(SQL_RequestFieldData &req, SQL_ResponseFieldData &resp, UnitID sender)
		{

		}

		virtual void RegisterMsg(ActorManager *pActorMgr) 
		{
			REG_COMP_NOTIFY(NoSQLComponent, SQL_SaveNoSQLData);
			REG_COMP_MSG(NoSQLComponent, SQL_LoadNoSQLData, SQL_ResponseNoSQLData);
			REG_COMP_MSG(NoSQLComponent, SQL_RequestFieldData, SQL_ResponseFieldData);
		}

		NoSQLComponent()
			: mActiveList(100000)
		{

		}

	public:
		FastHash<AString, ANoSQLData>	mNoSQLDataList;
		FastHash<DB_HASH, AutoData>	mFieldList;
		PoolLoop<ANoSQLData>				mActiveList;
	};

	class DBMySQLComponent : public MySQLComponent
	{
	public:
		Hand<LogicDBTable> mFieldTable;
		Hand<LogicDBTable> mDataTable;
	};
	//-------------------------------------------------------------------------
	// 使用端结构
	class ActorDBLib_Export_H tDBData : public AutoBase
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
	//-------------------------------------------------------------------------
	// 使用端
	class ActorDBLib_Export NoSQLUserComponent : public Component
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

		void On(SQL_RequestFieldData &req, SQL_ResponseFieldData &resp, UnitID sender)
		{

		}

		virtual void RegisterMsg(ActorManager *pActorMgr)
		{
			REG_COMP_MSG(NoSQLUserComponent, SQL_RequestFieldData, SQL_ResponseFieldData);
		}
	};
	//-------------------------------------------------------------------------
}
#endif //_INCLDUE_NOSQLCOMPONECNT_H_
