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

#define NOSQL_DB_TYPE		(-10000)

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
			if (resp.mFieldHash == 0 || mFieldList.find(resp.mFieldHash))
			{
				auto existData = mNoSQLDataList.find(resp.mKey);
				if (!existData)
				{
					existData = MEM_NEW NoSQLData();
					mNoSQLDataList.insert(resp.mKey, existData);
				}
				existData->mData.write(0, resp.mData->data(), resp.mData->dataSize());
				existData->mActiveMilSecond = TimeManager::Now();
				return;
			}
			CoroutineTool::AsyncCall([=]()
			{
				SQL_RequestFieldData reqMsg;
				reqMsg.mKey = resp.mKey;
				SQL_ResponseFieldData respField;
				if (mpActor->Await(senderID, reqMsg, respField, 10000))
				{
					if (respField.mFieldHash == resp.mFieldHash)
					{
						mFieldList.insert(resp.mFieldHash, respField.mData);
						auto existData = mNoSQLDataList.find(resp.mKey);
						if (!existData)
						{
							existData = MEM_NEW NoSQLData();
							mNoSQLDataList.insert(resp.mKey, existData);
						}
						existData->mData.write(0, ((DataStream*)resp.mData.getPtr())->data(), resp.mData->dataSize());
						existData->mActiveMilSecond = TimeManager::Now();
						return;
					}
				}
				ERROR_LOG("Save fail");
			});
		}

		void On(SQL_LoadNoSQLData &req, SQL_ResponseNoSQLData &resp, UnitID sender)
		{

		}

		void On(SQL_RequestFieldData &req, SQL_ResponseFieldData &resp, UnitID sender)
		{
			AutoData fieldData = mFieldList.find(req.mFieldHash);	
			if (fieldData)
				resp.mData = fieldData;
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
	class NoSQLUserRecord : public IndexDBRecord
	{
	public:
		virtual AutoField getField() const { return mField; }
		virtual tBaseTable* GetTable() override { return NULL; }

	public:
		AutoField mField;
	};
	// 使用端结构
	class ActorDBLib_Export_H tDBData : public AutoBase
	{
	public:
		Int64 GetID() { return MAKE_INDEX_ID(mKey.c_str()); }

		UnitID GetNoSQLID(int dbCode, int nDBCount) 
		{
			return UnitID(dbCode, GetID() % nDBCount);
		}

		void InitField(AutoField  field)
		{			
			mDataRecord = MEM_NEW NoSQLUserRecord();
			Auto<NoSQLUserRecord> re = mDataRecord;
			re->mField = field;
			mField = field;
			mFieldHash = MAKE_INDEX_ID(mField->ToString().c_str());
			mDataRecord->_alloctData(0);
		}

		virtual bool GetData(DataStream *pDestData)
		{
			if (mFieldHash == 0 && mNiceData)
			{
				mNiceData->serialize(pDestData);
				return true;
			}
			if (mFieldHash!=0 && mField && mDataRecord)
			{
				mDataRecord->saveData(pDestData);
				return true;
			}
			return false;
		}

	public:
		AString			mKey;
		AutoNice		mNiceData;
		ARecord		mDataRecord;
		AutoField		mField;
		DB_HASH		mFieldHash = 0;
	};
	//-------------------------------------------------------------------------
	// 使用端
	class ActorDBLib_Export NoSQLUserComponent : public Component
	{
	public:
		tDBData		mData;

	public:
		bool Init(tDBData *pDBData)
		{
			return false;
		}

		int GetNoSQLCount() { return 1; }

		bool Save(tDBData *pDBData)
		{
			SQL_SaveNoSQLData saveMsg;
			saveMsg.mData = MEM_NEW DataBuffer();
			if (pDBData->GetData(saveMsg.mData.getPtr()))
			{
				saveMsg.mFieldHash = pDBData->mFieldHash;
				saveMsg.mKey = pDBData->mKey;
				return mpActor->Send(pDBData->GetNoSQLID(NOSQL_DB_TYPE, GetNoSQLCount()), &saveMsg);
			}
			return false;
		}

		bool Load(tDBData *pDBData)
		{
			return false;
		}

		void On(SQL_RequestFieldData &req, SQL_ResponseFieldData &resp, UnitID sender)
		{
			resp.mData = MEM_NEW DataBuffer();
			
			if (mData.mField && mData.mField->saveToData(resp.mData.getPtr()))
			{
				resp.mFieldHash = mData.mFieldHash;
			}
			else
				ERROR_LOG("Field is Null");
		}

		virtual void RegisterMsg(ActorManager *pActorMgr)
		{
			REG_COMP_MSG(NoSQLUserComponent, SQL_RequestFieldData, SQL_ResponseFieldData);
		}
	};
	//-------------------------------------------------------------------------
}
#endif //_INCLDUE_NOSQLCOMPONECNT_H_
