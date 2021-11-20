
#ifndef _INCLUDE_TABLEUPDATELOADER_H_
#define _INCLUDE_TABLEUPDATELOADER_H_

#include "BaseTable.h"
#include "IndexBaseTable.h"

//-------------------------------------------------------------------------
// ���ֱ������
// 1 ����ȫ������, �����ֶ���Ϣ, ȫ����¼����
// 2 ֻ����������¼����, �������ֶ���Ϣ
// 3 ֻ������¼�¼����, �������ֶ���Ϣ
enum TABLE_SAVE_TYPE
{
	eSaveAll = 0,
	eOnlySaveRecordData = 1,
	eOnlyUpdateRecordData = 2,
	eSaveDBRecordData = 3,
};
//-------------------------------------------------------------------------
// ����ʶ���������������ݣ�����ֻ�����޸ĵ�����
class UpdateDataBuffer : public DataBuffer
{
public:
	UpdateDataBuffer()
	{}

	UpdateDataBuffer(DSIZE initSize)
		: DataBuffer(initSize)
	{}
};
// ʶ��ֻ�����¼��������
class SaveRecordDataBuffer : public DataBuffer
{
public:
	SaveRecordDataBuffer()
	{}

	SaveRecordDataBuffer(DSIZE initSize)
		: DataBuffer(initSize)
	{}
};

// DBȫ��¼�ָ�
class SaveDBRecordBuffer : public DataBuffer
{
public:
	SaveDBRecordBuffer()
	{}

	SaveDBRecordBuffer(DSIZE initSize)
		: DataBuffer(initSize)
	{}
};

//-------------------------------------------------------------------------

class TableDataLoader : public TableLoader
{	
public:
	virtual bool LoadTable(tBaseTable *pTable, DataStream *scrData) override
	{
		byte dataType = eSaveAll;

		READ(scrData, dataType);		
		if (dataType<eSaveAll || dataType>eSaveDBRecordData)
		{
			ERROR_LOG("�ָ�������ݱ������ʹ��� [%d]", dataType);
			return false;
		}
		if (eSaveAll==dataType)
		{
			AString tableName;
			if (!scrData->readString(tableName))
				return false;
			pTable->SetTableName(tableName.c_str());
			pTable->ClearAll();
			AutoField newField = MEM_NEW BaseFieldIndex();
			if (!newField->restoreFromData(scrData))
			{
				ERROR_LOG("�ָ��ֶ���Ϣʧ�� [%s]", tableName.c_str());
				return false;
			}
			newField->_updateInfo();

			if (pTable->GetField())
			{
				if (pTable->GetField()->GetCheckCode()!=newField->GetCheckCode())
					pTable->InitField(newField);
				else
				{
					LOG_RED
					DEBUG_LOG("!!! WARN: [%s] Already exist same field, May be not need eSaveAll, Can use SaveRecordDataBuffer save data!!!\r\n%s", 
						tableName.c_str(), pTable->GetField()->ToString().c_str());
					LOG_WHITE
				}
			}
			else 
				pTable->InitField(newField);
			pTable->SetMainIndex(0, false);
		}
		else if(eSaveDBRecordData == dataType)
		{
			int code = 0;
			if (!scrData->read(code))
				return false;
			if (code != pTable->GetField()->GetCheckCode())
			{
				ERROR_LOG("%s Field check fail: now %d != %d", pTable->GetTableName(), code, pTable->GetField()->GetCheckCode());
				return false;
			}
		}
		// �ڼ�¼����ʱ, ɾ����¼
		if (eOnlyUpdateRecordData==dataType)
		{
			if (!pTable->ApplyDeleteInfo(scrData))
			{
				ERROR_LOG("�ָ�ʹ��ɾ����¼��Ϣʧ�� [%s]", pTable->GetTableName());
				return false;
			}
		}
		if (pTable->GetField()->getCount()<=0)
		{
			ERROR_LOG("[%s]�ָ�����¼ǰ, ����ֶ�Ϊ��", pTable->GetTableName());
			return false;
		}
		// �ָ���¼
		int count;
		READ(scrData, count);
		for (int i =0; i<count; ++i)
		{						
			if (eOnlyUpdateRecordData==dataType)
			{
				AString key;
				if (!scrData->readString(key))
					return false;

				ARecord re = pTable->GetRecord(key.c_str());
				if (!re)
				{
					//DEBUG_LOG("WARN: �ָ����¼�¼����ʱ, ��ȡ������Ҫ���µļ�¼[%s] at table [%s]", key.c_str(), pTable->GetTableName());
					re = pTable->CreateRecord(key.c_str(), true);
				}

				if (!re->updateFromData(scrData))
				{
					ERROR_LOG("�ָ���¼��������ʧ��");
					return false;
				}
			}			
			else if (eSaveDBRecordData == dataType)
			{
				AString key;
				if (!scrData->readString(key))
					return false;
				ARecord re = pTable->GetRecord(key.c_str());
				if (re)
				{
					re->ReloadData(key.c_str());
					if (!re->restoreData(scrData))
					{
						ERROR_LOG("�ָ���¼����ʧ��");
						return false;
					}
					re->FullAllUpdate(true);					
				}
				else
				{
					ARecord re = pTable->_NewRecord();
					re->_alloctData(0);
					if (!re->restoreData(scrData))
					{
						ERROR_LOG("�ָ���¼����ʧ��");
						return false;
					}
					re->FullAllUpdate(true);

					pTable->AppendRecord(re, true, true);
					pTable->InsertDBNewRecord(re);
				}
			}			
			else
			{
				ARecord re = pTable->NewRecord();
				if (!re->restoreData(scrData))
				{
					ERROR_LOG("�ָ���¼����ʧ��");
					return false;
				}
				pTable->AppendRecord(re, true, true);
			}
		}
		return true;
	}

	virtual bool SaveTable(tBaseTable *pTable, DataStream *resultData, const char *szIndexField = NULL)
	{
		bool bUpdate = dynamic_cast<UpdateDataBuffer*>(resultData)!=NULL;
		bool bOnlySaveRecord = dynamic_cast<SaveRecordDataBuffer*>(resultData)!=NULL;
		bool bSaveDBRecord = dynamic_cast<SaveDBRecordBuffer*>(resultData) != NULL;

		byte dataType = eSaveAll;
		if (bUpdate)
			dataType = eOnlyUpdateRecordData;
		else if (bOnlySaveRecord)
			dataType = eOnlySaveRecordData;
		else if (bSaveDBRecord)
			dataType = eSaveDBRecordData;

		resultData->write(dataType);

		if (eSaveAll==dataType)
		{
			resultData->writeString(pTable->GetTableName());
			if (!pTable->GetField()->saveToData(resultData))
			{
				ERROR_LOG("�����ֶ���Ϣʧ��");
				return false;
			}
		}
		else if (eSaveDBRecordData == dataType)
		{
			resultData->write(pTable->GetField()->GetCheckCode());
		}

		// ����ɾ����¼��Ϣ
		if (eOnlyUpdateRecordData==dataType)
		{
			if (!pTable->SaveDeleteInfo(resultData, true))
			{
				ERROR_LOG("����ɾ����¼��Ϣʧ�� [%s]", pTable->GetTableName());
				return false;
			}
		}

		DSIZE nowPos = resultData->tell();
		resultData->write((int)0);
		uint count = 0;
		for (ARecordIt it = pTable->GetRecordIt(); *it; ++(*it))
		{
			ARecord re = *it;				
			if (!re)
				continue;

			if (eOnlyUpdateRecordData == dataType)
			{
				if (re->NeedUpdate())
				{
					++count;
					Data key = re->getIndexData();
					resultData->writeString(key.string());
					if (!re->saveUpdateData(resultData))
					{
						ERROR_LOG("���м�¼��������ʧ��");
						return false;
					}
				}
			}			
			else if (eSaveDBRecordData == dataType)
			{
				++count;
				Data key = re->getIndexData();
				resultData->writeString(key.string());
				if (!re->saveData(resultData))
				{
					ERROR_LOG("���м�¼��������ʧ��");
					return false;
				}
			}
			else
			{
				++count;
				if (!re->saveData(resultData))
				{
					ERROR_LOG("���м�¼����ʧ��");
					return false;
				}
			}

		}
		DSIZE last = resultData->tell();
		resultData->seek(nowPos);
		resultData->write(count);
		resultData->seek(last);

		return true;
	}
};



//class OnlySaveRecordLoader : public TableDataLoader
//{	
//public:
//	virtual bool NeedSaveAll(){ return false; }
//};

//-------------------------------------------------------------------------

#endif //_INCLUDE_TABLEUPDATELOADER_H_