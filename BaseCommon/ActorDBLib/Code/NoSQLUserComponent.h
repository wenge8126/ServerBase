#ifndef _INCLUDE_NOSQLUSERCOMPONENT_H_
#define _INCLUDE_NOSQLUSERCOMPONENT_H_
#pragma once

#include "Component.h"
#include "Actor.h"
#include "IndexDBRecord.h"
#include "NoSQLComponent.h"

/*-------------------------------------------------------------------------
���ڶ��¼(���ʼ�, ��Ʒ), KEY Ϊ ID_�����±�, ����Ʒ��������:
1 ���������ʹ��ArrayList, �����Ϣ�ﱣ�浱ǰ��Ʒ��¼����
2 ������Ʒʱ, ����PlayerID_�±�, ��ȡ(�ɸ��ݿͻ�������, �ֲ���ȡ)
3 ɾ��ʱ, ��������Ƶ���ɾ������Ʒ��,
�ȱ���ɾ��λ�ø��ºõ��������,
��ɾ�����λ�ü�¼(�ɺ���, �´�������Ʒʱ���ɻḲ�Ǳ���)
4 ��ƷID����, ����ʱ, ֱ�ӱ������һ�ʹ��EashMap����
//------------------------------------------------------------------------*/

namespace NetCloud
{

	//-------------------------------------------------------------------------
	// ʹ�ö�	
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
	// NiceData ����ʹ��
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
	// ��¼��ֵ
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
	// ���¼(���ʼ�, ��Ʒ), KEY Ϊ ID_�����±�
	class ActorDBLib_Export_H RecordArray : public AutoBase
	{
	public:
		void Init(AString key, int idCol)
		{
			mKey = key;
			mIDCol = idCol;
		}

		// ���ڱ�����Ϣ
		int GetCount() { return mRecordArray.size(); }

		// ����Insert�����, �ٱ���, ��Ҫ����������KEY
		void Insert(ARecord  record, RecordNoSQLUserComponent *pUser)
		{
			// �������ID
			int lastID = 0;
			for (int i=0; i<mRecordArray.size(); ++i)
			{
				ARecord re = mRecordArray[i];
				int id = re[mIDCol];
				if (id > lastID)
					lastID = id;
			}
			record->set(mIDCol, lastID+1);

			AString key;
			key.Format("%s_%d", mKey.c_str(), (int)mRecordArray.size());
			//record->set(0, key);
			mRecordArray.add(record);
			mRecordIndex.insert(record[mIDCol], mRecordArray.size() - 1);

			pUser->InitRecord(record);
			pUser->Save(key);
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
					// �ƶ���ɾ��λ����
					ARecord lastRecord = mRecordArray[*pos];
					AString key = mKey;
					key.Format("%s_%d", mKey.c_str(), *pos);
					//lastRecord->set(0, key);
					pUser->InitRecord(lastRecord);
					pUser->Save(key);
					// ���ɾ���ĺ���ɾ��, �´����Ӻ�ᱻ����
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

		AString GetRecordKey(ARecord re)
		{
			AString key;
			int id = re[mIDCol];
			int *pos = mRecordIndex.findPtr(id);
			if (pos != NULL)
			{
				if (*pos < mRecordArray.size() && mRecordArray[*pos] == re)
					key.Format("%s_%d", mKey.c_str(), *pos);
				else
					ERROR_LOG("%d Is not same record at array", id);
			}
			else
				ERROR_LOG("No find record pos id : %d", id);
			return AString();
		}

		bool LoadAll(int count, RecordNoSQLUserComponent *pUser)
		{
			mRecordArray.clear(false);
			mRecordIndex.clear(false);
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
					mRecordIndex.insert(pUser->mDataRecord[mIDCol], mRecordArray.size() - 1);
					NOTE_LOG("Item : %s \r\n%s", key.c_str(), pUser->mDataRecord->ToNiceData()->dump().c_str());
				}
				else
					break;
			}
			pUser->mDataRecord.setNull();
			return mRecordArray.size() == count;
		}

	protected:
		ArrayList<ARecord>				mRecordArray;
		EasyMap<Int64, int>				mRecordIndex;
		AString		mKey;
		int			mIDCol = 1;					// ID�ڼ�¼�е���
	};
	//-------------------------------------------------------------------------
}


#endif
