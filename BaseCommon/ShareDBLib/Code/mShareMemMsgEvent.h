#pragma once
#ifndef _INCLUDE_SHAREMEMMSGEVENT_H_
#define _INCLUDE_SHAREMEMMSGEVENT_H_

#include "ShareMemNet.h"
#include "ClientEvent.h"
#include "DBDefine.h"

// 共享内存进程用到的消息事件

namespace NetCloud
{
	//-------------------------------------------------------------------------
	class ShareMemSQLNet : public ShareMemNet
	{
	public:
		ShareMemSQLNet()
			: ShareMemNet(true)
		{}

	public:
		ShareMemDBManager *mpDBMgr;
	};

	//-------------------------------------------------------------------------
	class tShareMemResponseEvent : public Logic::tClientEvent
	{
	public:
		ShareMemDBManager*  GetDBMgr()
		{
			if (!mNetConnect)
			{
				return NULL;
			}
			Hand<ShareMemSQLNet> net = mNetConnect->GetNetHandle();
			return net->mpDBMgr;
		}
	};
	//-------------------------------------------------------------------------
	class DM_ReqeustDBInfo : public tShareMemResponseEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			int sqlKey = get(SHARE_SQL_KEY);

			if (GetDBMgr()->mSQLKey != 0)
			{
				GetResponseEvent()["ERROR"] = "SQL update already init";
				ERROR_LOG("* DB SQL : Already init %s = %d, now can not init agin %d", SHARE_SQL_KEY, GetDBMgr()->mSQLKey, sqlKey);
				Finish();
				return true;
			}

			NOTE_LOG("* DB SQL : %s = %d", SHARE_SQL_KEY, sqlKey);

			GetDBMgr()->InitDBUpdate(sqlKey);

			// 回复MYSQL连接参数
			AutoNice sqlParam = MEM_NEW NiceData();
			sqlParam->append(GetDBMgr()->mInitParam, true);
			GetResponseEvent()["_SQL_PARAM_"] = sqlParam.getPtr();

			AutoData listData = MEM_NEW DataBuffer(1024);
			auto &list = GetDBMgr()->mTableInfoMap;
			listData->write((int)list.size());
			for (auto it = list.begin(); it; ++it)
			{
				AShareMemTableData tableData = it.get();
				listData->writeString(it.key());
				listData->writeString(it.get()->mType);
				if (!it.get()->mField->saveToData(listData.getPtr()))
				{
					ERROR_LOG("%s save field error", it.key().c_str());
					return false;
				}
				AutoNice extData = tableData->mExtParam;
				if (!extData)
					extData = MEM_NEW NiceData();
				extData->serialize(listData.getPtr());
				Auto< NewCreateShareMemFieldIndex> index = it.get()->mField;
				if (index)
					index->mRecordMemPool.SaveMemPtrData(listData.getPtr());
				else
					listData->write((DSIZE)0);
			}
			AutoEvent respEvent = GetResponseEvent();
			respEvent["MEM_LIST_DATA"] = listData.getPtr();;
			
			respEvent["RESULT"] = true;

			Finish();
			return true;
		}
	};
	//-------------------------------------------------------------------------
	// 创建记录
	class DM_CreateRecord : public tShareMemResponseEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			AString tableName = get("TABLE_NAME");
			AString recordKey = get("RECORD_KEY");

			SHARE_PTR ptr;
			if (get("IS_GROWTH"))
				ptr = GetDBMgr()->GrowthNewRecord(tableName);
			else
				ptr = GetDBMgr()->CreateRecord(tableName, recordKey);
			
			GetResponseEvent()["RECORD_PTR"] = ptr.ptr;
			Finish();
			return true;
		}
	};
	//-------------------------------------------------------------------------
	// 创建表格
	class DM_CreateTable : public tShareMemResponseEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			AString tableName = get("TABLE_NAME");
			AString type = get("TYPE");

			if (GetDBMgr()->GetTableData(tableName))
			{
				GetResponseEvent()["ERROR"] = "Aleady exist table";
				Finish();
				return true;
			}

			AutoData fieldData = (DataStream*)get("FIELD_DATA");
			AutoField field = MEM_NEW BaseFieldIndex();
			fieldData->seek(0);
			if (field->restoreFromData(fieldData.getPtr()))
			{
				if (field->CheckSame( get("FIELD_CODE") ) )
				{
					AString info;
					if (GetDBMgr()->CreateDBTable(tableName, get("TYPE").string(), field, (tNiceData*)get("EXT_PARAM"), info))
					{
						GetResponseEvent()["RESULT"] = true;

						AutoData memListData = MEM_NEW DataBuffer(128);
						Auto< NewCreateShareMemFieldIndex> index = GetDBMgr()->GetTableData(tableName)->mField;
						if (index)						
							index->mRecordMemPool.SaveMemPtrData(memListData.getPtr());
						else
							memListData->write((DSIZE)0);
						GetResponseEvent()["MEM_LIST_DATA"] = memListData.getPtr();					
					}
					NOTE_LOG("%s create result > %s", tableName.c_str(), info.c_str());
					GetResponseEvent()["ERROR"] = info;
				}
				else
				{
					AString err;
					err.Format("%s Field code check fail, now %d!=%d", tableName.c_str(), field->GetCheckCode(), (int)get("FIELD_CODE"));
					ERROR_LOG(err.c_str());
					GetResponseEvent()["ERROR"] = err;
				}
			}
			else
			{
				ERROR_LOG("%s field data restore fail", tableName.c_str());
				GetResponseEvent()["ERROR"] = "field data restore fail";
			}
			Finish();
			return true;
		}
	};
	//-------------------------------------------------------------------------
	// 删除记录
	class DM_NotifyDeleteRecord : public Logic::tBaseNetEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			ShareMemDBManager *pMgr = GetDBMgr();
			UInt64 ptr = get("RECORD_PTR");
			if (pMgr != NULL)
				GetDBMgr()->DeleteRecord(get("TABLE_NAME"), ptr, get("RECORD_KEY"));
			else
				ERROR_LOG("Get ShareMemDBManager NULL");
			return true;
		}

		ShareMemDBManager*  GetDBMgr()
		{
			if (!mNetConnect)
			{
				return NULL;
			}
			Hand<ShareMemSQLNet> net = mNetConnect->GetNetHandle();
			return net->mpDBMgr;
		}
	};
	//-------------------------------------------------------------------------
	// 通知动态扩展了更新共享内存, 接收后, 需要及时附加, 并保存, 避免未能及时处理获取, 发送端快速退出释放
	class DM_NotifyCreateUpdateShareMemory : public DM_NotifyDeleteRecord
	{
	public:
		virtual bool _DoEvent() override
		{
			int key = get("MEM_KEY");
			if (key != 0)
			{
				ShareMemDBManager *pMgr = GetDBMgr();
				UInt64 ptr = get("RECORD_PTR");
				if (pMgr != NULL)
				{
					ShareMemDBManager::AShareMemAO mem = MEM_NEW ShareMemAO();
					uint size = 0;
					if (mem->Attach(key, size))
					{
						pMgr->mTempAttchShareMemList.push_back(mem);
						NOTE_LOG("Succeed attach update share memory %d", key);
					}
					else
						ERROR_LOG("%s Attack share memory fail", GetEventName());
				}
				else
				ERROR_LOG("Get ShareMemDBManager NULL");
			}
			else
				ERROR_LOG("%s No set MEM_KEY", GetEventName());
			return true;
		}
	};
	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_SHAREMEMMSGEVENT_H_
