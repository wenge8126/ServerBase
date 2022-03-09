#ifndef _INCLUDE_NOSQLUSERCOMPONENT_H_
#define _INCLUDE_NOSQLUSERCOMPONENT_H_
#pragma once

#include "Component.h"
#include "Actor.h"
#include "IndexDBRecord.h"
#include "NoSQLComponent.h"

/*-------------------------------------------------------------------------
对于多记录(如邮件, 物品), KEY 为 ID_数组下标, 如物品操作如下:
1 保存在组件使用ArrayList, 玩家信息里保存当前物品记录数量
2 加载物品时, 根据PlayerID_下标, 调取(可根据客户端请求, 分步获取)
3 删除时, 会把最后的移到到删除的物品上,
先保存删除位置更新好的最后数据,
再删除最后位置记录(可忽略, 下次增加物品时即可会覆盖保存)
4 物品ID递增, 查找时, 直接遍历查找或使用EashMap索引
//------------------------------------------------------------------------*/

namespace NetCloud
{

	//-------------------------------------------------------------------------
	// 使用端	
	class ActorDBLib_Export NoSQLUserComponent : public Component
	{
	public:
		virtual bool GetData(DataStream *pDestData) = 0;
		virtual bool Load(const AString &key, bool bNeedLoadField) = 0;
		virtual AString GetKey() = 0;
		virtual int GetFieldHash() = 0;

	public:
		virtual int GetNoSQLCount() { return 1; }

		UnitID GetNoSQLID()
		{
			return UnitID(NOSQL_DB_TYPE, ((size_t)GetKey()) % GetNoSQLCount());
		}

		bool Save()
		{
			SQL_SaveNoSQLData saveMsg;
			saveMsg.mData = MEM_NEW DataBuffer();
			if (GetData(saveMsg.mData.getPtr()))
			{
				saveMsg.mFieldHash = GetFieldHash();
				saveMsg.mKey = GetKey();
				saveMsg.SetAttachValue(GetEventFactory()->GetNameIndex());
				return mpActor->SendMsg(saveMsg, GetNoSQLID());
			}
			return false;
		}
	};
	//-------------------------------------------------------------------------
	// NiceData 数据使用
	class ActorDBLib_Export NiceNoSQLUserComponent : public NoSQLUserComponent
	{
	public:
		AString			mKey;
		AutoNice		mNiceData;

	public:
		virtual void Awake() override
		{
			mNiceData = MEM_NEW NiceData();
		}

		virtual AString GetKey() override { return mKey; }
		virtual int GetFieldHash() override { return 0; }

		virtual bool GetData(DataStream *pDestData) override
		{
			if (mNiceData->serialize(pDestData))
				return true;

			ERROR_LOG("Save data fail");
			return false;
		}

		virtual bool Load(const AString &key, bool) override
		{
			mKey = key;
			SQL_LoadNoSQLData loadMsg;
			loadMsg.mbNeedField = false;
			loadMsg.mKey = mKey;
			SQL_ResponseNoSQLData resp;
			if (mpActor->Await(GetNoSQLID(), loadMsg, resp, 10000))
			{
				resp.mData->seek(0);
				if (mNiceData->restore(resp.mData.getPtr()))
					return true;
				ERROR_LOG("Restore nicedata fail");
			}
			return false;
		}
	};
	//-------------------------------------------------------------------------
	// 记录数值
	class ActorDBLib_Export RecordNoSQLUserComponent : public NoSQLUserComponent
	{
	public:
		ARecord			mDataRecord;

	public:
		virtual bool InitRecord(ARecord recode)
		{
			mDataRecord = recode;	
			return true;
		}

		virtual AString GetKey() override 
		{
			if (mDataRecord)
				return mDataRecord[0].string();

			return AString();
		}

		virtual int GetFieldHash()
		{
			if (mDataRecord)
				return mDataRecord->getField()->GetCheckCode();
			return -1;
		}

		virtual bool GetData(DataStream *pDestData) override
		{
			if (!mDataRecord)
			{
				ERROR_LOG("No init create record");
				return false;
			}
			if (mDataRecord->saveData(pDestData))
				return true;

			ERROR_LOG("Save record data fail");
			return false;
		}

		virtual bool Load(const AString &key, bool bNeedLoadField) override;

		virtual AutoTable NewTable() { return MEM_NEW StructBaseTable(); }

	public:
		void On(SQL_RequestFieldData &req, SQL_ResponseFieldData &resp, UnitID sender, int)
		{
			resp.mData = MEM_NEW DataBuffer();
			
			if (mDataRecord && mDataRecord->getField()->saveToData(resp.mData.getPtr()))
			{
				resp.mFieldHash = mDataRecord->getField()->GetCheckCode();
			}
			else
			{
				resp.mData.setNull();
				ERROR_LOG("DataRecord or Field is Null");
			}
		}

		virtual void RegisterMsg(ActorManager *pActorMgr)
		{
			REG_COMP_MSG(RecordNoSQLUserComponent, SQL_RequestFieldData, SQL_ResponseFieldData);
		}
	};


	//-------------------------------------------------------------------------
	// 多记录(如邮件, 物品), KEY 为 ID_数组下标
	class ActorDBLib_Export_H RecordArray : public AutoBase
	{
	public:
		void Init(AString key, int idCol)
		{
			mKey = key;
			mIDCol = idCol;
		}

		// 用于保存信息
		int GetCount() { return mRecordArray.size(); }

		// 调用Insert插入后, 再保存, 主要是重新设置KEY
		void Insert(ARecord  record, RecordNoSQLUserComponent *pUser)
		{
			// 设置最大ID
			int lastID = 0;
			for (int i=0; i<mRecordArray.size(); ++i)
			{
				ARecord re = mRecordArray[i];
				int id = re[mIDCol];
				if (id > lastID)
					lastID = id;
			}
			record->set(mIDCol, lastID);

			AString key;
			key.Format("%s_%d", mKey.c_str(), (int)mRecordArray.size());
			record->set(0, key);
			mRecordArray.add(record);
			mRecordIndex.insert(record[mIDCol], mRecordArray.size() - 1);

			pUser->InitRecord(record);
			pUser->Save();
		}

		bool Remove(ARecord  record, RecordNoSQLUserComponent *pUser)
		{
			int *pos = mRecordIndex.findPtr(record[mIDCol]);
			if (pos == NULL)
			{
				ERROR_LOG("No exist record %s %d", record[0].c_str(), (int)record[mIDCol]);
				return false;
			}
			if (mRecordArray.removeAt((size_t)*pos))
			{
				if (*pos < mRecordArray.size())
				{
					// 移动到删除位置了
					ARecord lastRecord = mRecordArray[*pos];
					AString key = mKey;
					key.Format("%s_%d", mKey.c_str(), *pos);
					lastRecord->set(0, key);
					pUser->InitRecord(lastRecord);
					pUser->Save();
					// 最后删除的忽略删除, 下次增加后会被覆盖
				}
				return true;
			}
			return false;
		}

		ARecord Find(int id)
		{
			int *pos = mRecordIndex.findPtr(id);
			if (pos == NULL)
			{
				ERROR_LOG("No exist record %d", id);
				return ARecord();
			}
			return mRecordArray[*pos];
		}

	protected:
		ArrayList<ARecord>				mRecordArray;
		EasyMap<Int64, int>				mRecordIndex;
		AString		mKey;
		int			mIDCol = 1;					// ID在记录中的列
	};
	//-------------------------------------------------------------------------
}


#endif
