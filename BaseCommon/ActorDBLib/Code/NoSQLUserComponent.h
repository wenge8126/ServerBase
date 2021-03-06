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
		virtual int GetFieldHash() = 0;

	public:
		virtual int GetNoSQLCount() { return 1; }

		UnitID GetNoSQLID( const AString &key )
		{
			return UnitID(NOSQL_DB_TYPE, ((size_t)key) % GetNoSQLCount());
		}

		bool Save(const AString &key)
		{
			SQL_SaveNoSQLData saveMsg;
			saveMsg.mData = MEM_NEW DataBuffer();
			if (GetData(saveMsg.mData.getPtr()))
			{
				saveMsg.mFieldHash = GetFieldHash();
				saveMsg.mKey = key;
				saveMsg.SetAttachValue(GetEventFactory()->GetNameIndex());
				return mpActor->SendMsg(saveMsg, GetNoSQLID(key));
			}
			return false;
		}
	};
	//-------------------------------------------------------------------------
	// NiceData 数据使用
	class ActorDBLib_Export NiceNoSQLUserComponent : public NoSQLUserComponent
	{
	public:
		AutoNice		mNiceData;

	public:
		virtual void Awake() override
		{
			mNiceData = MEM_NEW NiceData();
		}

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
			SQL_LoadNoSQLData loadMsg;
			loadMsg.mbNeedField = false;
			loadMsg.mKey = key;
			SQL_ResponseNoSQLData resp;
			if (mpActor->Await(GetNoSQLID(key), loadMsg, resp, 10000))
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
		void Init(AString key)
		{
			mKey = key;
		}

		// 用于保存信息
		int GetCount() { return mRecordArray.size(); }

		// 调用Insert插入后, 再保存, 主要是重新设置KEY
		bool Insert(ARecord  record, RecordNoSQLUserComponent *pUser, int nIDCol = 0)
		{
			if (!mbLoaded)
			{
				ERROR_LOG("Now is not init load, Can not inset record");
				return false;
			}
			// 设置最大ID
			int lastID = 0;
			for (int i=0; i<mRecordArray.size(); ++i)
			{
				ARecord re = mRecordArray[i];
				int id = re[nIDCol];
				if (id > lastID)
					lastID = id;
			}
			record->set(nIDCol, lastID+1);

			AString key;
			key.Format("%s_%d", mKey.c_str(), (int)mRecordArray.size());
			mRecordArray.add(record);

			pUser->InitRecord(record);
			return pUser->Save(key);
		}

		bool Remove(ARecord  record, RecordNoSQLUserComponent *pUser, int nIDCol = 0)
		{
			if (!mbLoaded)
			{
				ERROR_LOG("Now is not init load, Can not inset record");
				return false;
			}
			int pos = FindArrayIndex(record);
			if (pos == NULL_POS)
			{
				ERROR_LOG("No exist record %s %d", record[0].c_str(), (int)record[nIDCol]);
				return false;
			}
			
			
			if (pos < mRecordArray.size() - 1)
			{
				// 移动到删除位置了
				ARecord lastRecord = mRecordArray[mRecordArray.size() - 1];
				AString key = mKey;
				key.Format("%s_%d", mKey.c_str(), pos);
				pUser->InitRecord(lastRecord);
				pUser->Save(key);
				// 最后删除的忽略删除, 下次增加后会被覆盖
			}
				
			return mRecordArray.removeAt((size_t)pos);						
		}

		virtual ARecord Find(int id, int nIDCol = 0, int *nArrayIndex = NULL)
		{
			for (int i=0; i<mRecordArray.size(); ++i)
			{
				ARecord re = mRecordArray[i];
				if ((int)re[nIDCol] == id)
				{
					if (nArrayIndex!=NULL)
						*nArrayIndex = i;
					return re;
				}
			}
			if (nArrayIndex != NULL)
				*nArrayIndex = NULL_POS;
			return ARecord();
		}

		virtual int FindArrayIndex(ARecord record)
		{
			for (int i = 0; i < mRecordArray.size(); ++i)
			{
				if (record == mRecordArray[i])				
				{					
					return i;
				}
			}
			return NULL_POS;			
		}


		AString GetRecordKey(ARecord re)
		{
			AString key;
		
			int pos = FindArrayIndex(re);
			if (pos != NULL_POS)
			{
				key.Format("%s_%d", mKey.c_str(), pos);								
			}
			else
				ERROR_LOG("No find record pos id : %d", re->ToNiceData()->ToJSON().c_str());
			return key;
		}

		bool LoadAll(int count, RecordNoSQLUserComponent *pUser)
		{
			mRecordArray.clear(false);
			AutoTable t;
			int i = 0;
			while (i<count)
			{
				if (t)
					pUser->InitRecord(t->CreateRecord(0, 0));
				AString key;
				key.Format("%s_%d", mKey.c_str(), i++);
				if (pUser->Load(key, !t))
				{
					t = pUser->mDataRecord->GetTable();
					mRecordArray.add(pUser->mDataRecord);
					NOTE_LOG("Item : %s \r\n%s", key.c_str(), pUser->mDataRecord->ToNiceData()->dump().c_str());
				}
				else
					break;
			}
			pUser->mDataRecord.setNull();
			mbLoaded = mRecordArray.size() == count;
			if (!mbLoaded)
				ERROR_LOG("Load count %d != %d", (int)mRecordArray.size(), count);
			return mRecordArray.size() == count;
		}

	protected:
		ArrayList<ARecord>				mRecordArray;
		AString		mKey;
		bool			mbLoaded = false;
	};
	//-------------------------------------------------------------------------
}


#endif
