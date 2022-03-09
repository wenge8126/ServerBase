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
#include "PoolLoop.h"

#define NOSQL_DB_TYPE			(-10000)
#define NOSQL_DATA_COL		1
#define NOSQL_FIELD_HASH_COL	2
#define NOSQL_FIELD_TABLE	"t_field"
#define NOSQL_DATA_TABLE	"t_data"

namespace NetCloud
{
	typedef int			DB_HASH;
	//-------------------------------------------------------------------------
	class NoSQLData : public AutoBase
	{
	public:
		UInt64			mActiveMilSecond = 0;
		ARecord			mDataRecord;					// 第2字段 DATA, 第3字段 即是 FIELD_HASH值
	};
	typedef Auto<NoSQLData> ANoSQLData;
	//-------------------------------------------------------------------------
	class ActorDBLib_Export NoSQLActor : public Actor
	{
	public:
		virtual void Init() override
		{
			mFieldTable = GetDBMgr()->GetTable(NOSQL_FIELD_TABLE);
			mDataTable = GetDBMgr()->GetTable(NOSQL_DATA_TABLE);
			if (mFieldTable && mDataTable)
				NOTE_LOG("NoSQL DB table ready OK")
			else
				ERROR_LOG("NoSQL DB table ready fail : %s : %s", NOSQL_FIELD_TABLE, NOSQL_DATA_TABLE);
		}

		virtual void LowProcess() override
		{
			CheckCool();
		}

		void CheckCool()
		{
			if (mActiveList.empty())
				return;
			int count = 100;
			UInt64 nowSec = TimeManager::Now();
			auto *beginNode = mActiveList.__getRoot();
			auto *pNow = beginNode;
			for (int i = 0; i < count; ++i)
			{

				if (nowSec - pNow->mValue->mActiveMilSecond > 3)
				{
					Auto<LogicDBRecord> re = pNow->mValue->mDataRecord;
					re->FreeDataMemory();
					//pNow->mValue->mDataRecord->_freeData();
					pNow->mValue->mActiveMilSecond = 0;
					mActiveList._remove(pNow);
				}
				else
					mActiveList.loop();

				pNow = mActiveList.__getRoot();
				if (pNow == NULL || pNow == beginNode)
					break;
			}
		}

		void SaveData(const AString &key, DataStream  *data)
		{

		}

		ANoSQLData CheckReload(const AString &key, bool bCreate, DB_HASH fieldHash)
		{
			auto d = mNoSQLDataList.find(key);
			if (d)
			{
				if (!d->mDataRecord)
				{
					if (d->mDataRecord->ReloadData(key.c_str()))
						mActiveList.insert(d);
					else
					{
						ERROR_LOG("Data record %s reload fail", key.c_str());
						AssertNote(0, "Data record %s reload fail", key.c_str());
						return ANoSQLData();
					}
				}
			}
			else
			{
				d = MEM_NEW NoSQLData();
				ARecord re = mDataTable->GetRecord(key.c_str());
				if (!re)
				{
					if (bCreate)
					{
						re = mDataTable->CreateRecord(key.c_str(), true);
						re->set(NOSQL_FIELD_HASH_COL, fieldHash);
					}
					else
						return ANoSQLData();
				}
				d->mDataRecord = re;
				d->mActiveMilSecond = TimeManager::Now();
				mNoSQLDataList.insert(key, d);
				mActiveList.insert(d);
			}
			return d;
		}

		ARecord CheckExistFieldData(DB_HASH fieldHash)
		{
			ARecord fieldRe = mFieldList.find(fieldHash);
			if (fieldRe)
				return fieldRe;

			fieldRe = mFieldTable->GetRecord((Int64)fieldHash);
			if (fieldRe)
				mFieldList.insert(fieldHash, fieldRe);

			return fieldRe;
		}

		static void CopyData(ARecord re, AutoData data)
		{
			AutoData d = (DataStream*)re->get(NOSQL_DATA_COL);
			if (!d)
			{
				d = MEM_NEW DataBuffer();
				re->set(NOSQL_DATA_COL, d);
			}
			else
				d->clear(false);
			d->_write(data->data(), data->dataSize());
			re->NotifyChanged(NOSQL_DATA_COL);
		}

	public:
		void Notify(SQL_SaveNoSQLData &resp, UnitID senderID, int nCompIndex)
		{
			//CoroutineTool::AsyncCall([=]()
			//{
			if (resp.mFieldHash == 0 || CheckExistFieldData(resp.mFieldHash))
			{
				auto existData = CheckReload(resp.mKey, true, resp.mFieldHash);
				if (!existData)
				{
					AssertNote(0, "Create or reload data fail : %s", resp.mKey.c_str());
					return;
				}
				CopyData(existData->mDataRecord, resp.mData);
				existData->mDataRecord->SaveUpdate();
				existData->mActiveMilSecond = TimeManager::Now();
				return;
			}

			SQL_RequestFieldData reqMsg;
			reqMsg.mKey = resp.mKey;
			reqMsg.SetAttachValue(nCompIndex);
			SQL_ResponseFieldData respField;
			if (Await(senderID, reqMsg, respField, 10000))
			{
				if (respField.mFieldHash == resp.mFieldHash)
				{
					ARecord fieldRe = mFieldTable->CreateRecord(resp.mFieldHash, true);
					CopyData(fieldRe, respField.mData);
					//fieldRe->set(NOSQL_DATA_COL, respField.mData);
					fieldRe->SaveUpdate();
					mFieldList.insert(resp.mFieldHash, fieldRe);

					auto existData = CheckReload(resp.mKey, true, resp.mFieldHash);
					if (!existData)
					{
						AssertNote(0, "Create or reload data fail : %s", resp.mKey.c_str());
						return;
					}
					CopyData(existData->mDataRecord, resp.mData);
					existData->mDataRecord->set(NOSQL_FIELD_HASH_COL, resp.mFieldHash);
					existData->mDataRecord->SaveUpdate();
					return;
				}
			}
			ERROR_LOG("Save fail");

			//});
		}

		void On(SQL_LoadNoSQLData &req, SQL_ResponseNoSQLData &resp, UnitID sender, int)
		{
			auto d = CheckReload(req.mKey, false, 0);
			if (!d)
			{
				DEBUG_LOG("No exist data %s", req.mKey.c_str());
				return;
			}


			resp.mData = (DataStream*)d->mDataRecord[NOSQL_DATA_COL];
			// 获取字段数据
			if (req.mbNeedField)
			{
				auto fieldRe = CheckExistFieldData((int)d->mDataRecord[NOSQL_FIELD_HASH_COL]);
				if (fieldRe)
				{
					resp.mFieldData = (DataStream*)fieldRe->get(NOSQL_DATA_COL);
				}
			}
		}

		void On(SQL_RequestFieldData &req, SQL_ResponseFieldData &resp, UnitID sender, int)
		{
			DB_HASH x = req.mFieldHash;
			if (x == 0)
			{
				auto d = CheckReload(req.mKey, false, 0);
				if (d)
				{
					x = (int)d->mDataRecord->get(NOSQL_FIELD_HASH_COL);
				}
				else
				{
					WARN_LOG("No exist field data %s : %d", req.mKey.c_str(), x);
					return;
				}
			}

			auto fieldRe = CheckExistFieldData(x);
			if (fieldRe)
				resp.mData = (DataStream*)fieldRe->get(NOSQL_DATA_COL);
			else
				WARN_LOG("No exist field data %s : %d", req.mKey.c_str(), x);
		}

		virtual void RegisterMsg(ActorManager *pActorMgr) override
		{
			REG_NOTIFY_MSG(NoSQLActor, SQL_SaveNoSQLData);
			REG_ACTOR_MSG(NoSQLActor, SQL_LoadNoSQLData, SQL_ResponseNoSQLData);
			REG_ACTOR_MSG(NoSQLActor, SQL_RequestFieldData, SQL_ResponseFieldData);
		}

		NoSQLActor()
			: mActiveList(100000)
		{

		}

	public:
		FastHash<DB_HASH, ARecord>	mFieldList;						// 字段记录不进行冷处理,加载后常驻内存
		FastHash<AString, ANoSQLData>	mNoSQLDataList;
		PoolLoop<ANoSQLData>				mActiveList;

		Auto<LogicDBTable> mFieldTable;
		Auto<LogicDBTable> mDataTable;
	};

}

#endif //_INCLDUE_NOSQLCOMPONECNT_H_
