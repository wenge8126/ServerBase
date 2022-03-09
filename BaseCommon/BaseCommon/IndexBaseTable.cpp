
#include "IndexBaseTable.h"
#include "Common.h"
#include "IndexDBRecord.h"
#include "IntEasyMapIndex.h"
#include "Int64MapIndex.h"
#include "StringHashIndex.h"
#include "StringEasyHashIndex.h"
#include "Md5Tool.h"
#include "MemoryTableFieldIndex.h"
#include "MultipleStringIndex.h"

#include "TableFieldInfo.h"
#include "ListRecordIndex.h"

BaseTable::BaseTable(INIT_FIELD_MODE initMode)
{
	if (initMode == eInitPoolField)
		mOwnerPtr->mField = MEM_NEW  MemoryTableFieldIndex(this);
	else if (initMode==eInitCreateField)
		mOwnerPtr->mField = MEM_NEW BaseFieldIndex();	
}

AutoIndex BaseTable::NewRecordIndex( FIELD_TYPE indexKeyType, bool bHash, bool bMultKey )
{
	switch (indexKeyType)
	{
	//case FIELD_FLOAT:
	case FIELD_STRING:
	case FIELD_CHAR_STRING:
		if (bMultKey)
		{
			return MEM_NEW STL_MultipleStringIndex();
		}
		if (bHash)
			return MEM_NEW StringHashIndex();		
		else
			return MEM_NEW StringEasyHashIndex();

	case FIELD_BYTE:
	case FIELD_SHORT:
	case FIELD_INT:
		if (bMultKey)
			return MEM_NEW MultipleIntMapIndex();
		else
			return MEM_NEW IntEasyMapIndex();

	case FIELD_INT64:
	case FIELD_UINT64:
		if (bMultKey)
			return MEM_NEW MultipleInt64MapIndex();
		else
			return MEM_NEW Int64MapIndex();

	default:
		{
			FieldInfo info = FieldInfoManager::getFieldInfo(indexKeyType);
			if (info!=NULL)
			{
				DEBUG_LOG("ERROR: δʵ�� [%s] ���͵�����", info->getTypeString() );
			}
			else
			{
				DEBUG_LOG("ERROR: �������ַ��ֶ����� [%d]", indexKeyType);
			}
		}
	}

	return AutoIndex();
}

bool BaseTable::SetMainIndex( int indexCol, bool bHash, bool bMultiple)
{
	FieldInfo info = mOwnerPtr->mField->getFieldInfo(indexCol);
	if (info != NULL)
	{
		AutoIndex index = NewRecordIndex((FIELD_TYPE)info->getType(), bHash, bMultiple);
		if (index)
		{
			index->SetIndexField(info->getName().c_str(), indexCol);
			mMainKeyIndex = index;
			return true;
		}
		else
			ERROR_LOG("Create main index fail : table %s, field %s, type %s", GetTableName(), info->getName().c_str(), info->getTypeString());
	}
	else
		ERROR_LOG("Create main index fail : table %s no exist field %d", GetTableName(), indexCol);

	return false;
}

bool BaseTable::SetMainIndex( const char *szFieldName, bool bHash, bool bMultiple )
{
	return SetMainIndex(GetField()->getFieldCol(szFieldName), bHash, bMultiple);
}

ARecord BaseTable::CreateRecord( float fIndex, bool bReplace )
{
	ARecord r = NewRecord();
	int col = mMainKeyIndex->IndexFieldCol();
	r->_set(col, fIndex);
	if (AppendRecord(r, bReplace))
		return r;
	ERROR_LOG("ERROR: %s ���뵽������ʧ�� >%f", GetTableName(), fIndex);
	return ARecord();
}

ARecord BaseTable::CreateRecord( const char* szIndex, bool bReplace )
{
	ARecord r = NewRecord();
	int col = mMainKeyIndex->IndexFieldCol();

	r->_set(col, szIndex);
	if (AppendRecord(r, bReplace))
		return r;
	ERROR_LOG("ERROR: %s ���뵽������ʧ�� >%s", GetTableName(), szIndex);
	return ARecord();
}

ARecord BaseTable::CreateRecord( Int64 nIndex, bool bReplace )
{
	ARecord r = NewRecord();
	int col = mMainKeyIndex->IndexFieldCol();
	r->_set(col, nIndex);
	if (AppendRecord(r, bReplace))
		return r;
	ERROR_LOG("ERROR: %s ���뵽������ʧ�� >%llu", GetTableName(), (UInt64)nIndex);
	return ARecord();
}


void BaseTable::ClearAll()
{
	if (mMainKeyIndex && mMainKeyIndex->GetCount()>0)
	{
		//NOTE: ʹ��AutoPtr, ���м�¼�����ط���������Ȼ���Դ���, �ұ����Ȼָ��ǰ���
		mMainKeyIndex->ClearAll(this);
	}
	tBaseTable::ClearAll();
}

void BaseTable::Init()
{
	tBaseTable::Init();
	mMainKeyIndex.setNull();
}

bool BaseTable::AppendRecord( ARecord scrRecord, bool bReplace, bool bRestore )
{
	Data d = scrRecord->getIndexData();
	if (d.empty())
		return false;	

	if (bReplace)
	{
		ARecord exist = mMainKeyIndex->GetRecord(d);
		if (exist)
		{
			OnReplaceRecord(exist, scrRecord);
			RemoveRecord(exist);
		}
	}
	else if (!mMainKeyIndex->IsMultiple())
	{
		if ( mMainKeyIndex->GetRecord(d) )
			return false;
	}
	scrRecord->SetInTable(true);
	return mMainKeyIndex->InsertRecord(scrRecord);
}

void BaseTable::OnRecordDataChanged( ARecord re, int col, Int64 scrValue )
{
	if (col==GetMainIndexCol())
	{	
		if (mMainKeyIndex->GetRecord(scrValue)==re && mMainKeyIndex->RemoveRecord(scrValue))
			mMainKeyIndex->InsertRecord(re);
	}	
}

void BaseTable::OnRecordDataChanged( ARecord re, int col, const char *scrValue )
{
	if (col==GetMainIndexCol())
	{	
		if (mMainKeyIndex->GetRecord(scrValue)==re && mMainKeyIndex->RemoveRecord(scrValue))
			mMainKeyIndex->InsertRecord(re);
	}	
}

ARecord BaseTable::NewRecord()
{
	AssertEx(GetField(), "Now table must exist FieldIndex");
	ARecord freeRe = GetField()->TryGetFreeRecord();
	IndexDBRecord *p = dynamic_cast<IndexDBRecord*>(freeRe.getPtr());
	if (p!=NULL)	
	{
		p->InitTablePtr(mOwnerPtr);	
		if (p->_getData()==NULL)
			p->_alloctData(0);
	}
	else
	{
		p =  MEM_NEW IndexDBRecord(GetSelf());
		p->_alloctData(0);
	}
	// �ն��½���¼, ��Ҫȫ��ͬ����DB
	//p->FullAllUpdate(true);
	return p;
}

ARecord BaseTable::_NewRecord()
{
	return MEM_NEW IndexDBRecord(GetSelf());
}

bool BaseTable::RemoveRecord( float fIndex )
{
	return mMainKeyIndex->RemoveRecord(fIndex);
}

bool BaseTable::RemoveRecord( const char* szIndex )
{		
	return mMainKeyIndex->RemoveRecord(szIndex);
}

bool BaseTable::RemoveRecord( Int64 nIndex )
{
	return mMainKeyIndex->RemoveRecord(nIndex);
}

bool BaseTable::RemoveRecord( ARecord record )
{
	return mMainKeyIndex->RemoveRecord(record);
}

ARecord BaseTable::GrowthNewRecord( DataStream *recordData )
{
	if (mMainKeyIndex)
	{
		ARecord re = NewRecord();
		if (recordData!=NULL)
		{
			recordData->seek(0);
			if (!re->restoreData(recordData))
			{
				ERROR_LOG("�ָ���¼����ʧ��");
			}
		}
		if (mMainKeyIndex->InsertLast(re))
		{
			re->FullAllUpdate(true);
			return re;
		}			
	}
	return ARecord();
}

ARecordIt BaseTable::GetRecordIt()
{
	if (mMainKeyIndex) return mMainKeyIndex->GetRecordIt(); 
	thread_local static AutoIndex msIndex = MEM_NEW IntEasyMapIndex();
	return msIndex->GetRecordIt();
}

BaseTable::~BaseTable()
{
	ClearAll();
}

// ɾ��DB���ݿ��еļ�¼, ��ֻ��ͨ���˹���ɾ��, ԭ��, ���ӱ��Ƴ���¼��, ���������еò�����ɾ������Ϣ
bool BaseTable::DeleteRecord( ARecord record )
{
	return RemoveRecord(record);

}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool NormalBaseTable::SetSkipUpdateField( int nFieldCol, bool bSkip )
{
	if (GetField())
	{
		GetField()->SetSkipUpdate(nFieldCol, bSkip);
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------
#define _TEMP_CHAR_BUFFER_LENGTH_ 32
int BaseFieldIndex::_generateCode() const
{
	//MD5	md(pFieldString);

	AString str;
	char temp[_TEMP_CHAR_BUFFER_LENGTH_];	
	for (int i = 0; i < getCount(); ++i)
	{
		FieldInfo info = getFieldInfo(i);
		str += info->getName();
		int t = (int)info->getType();
		sprintf_s(temp, _TEMP_CHAR_BUFFER_LENGTH_, "#%d_", t);
		str += temp;		
		if (t == FIELD_TABLE || t==FIELD_DB_TABLE)
		{
			//!!! NOTE �κεط�ǧ�򲻿�ʹ�� Auto<tFieldInfo> ��ʱʹ��, ������Զ��ͷŵ� tFieldInfo
			DBTableFieldInfo *f = dynamic_cast<DBTableFieldInfo*>(getFieldInfo(i));
			if (f!=NULL && f->mSubTableField)
			{
				sprintf_s(temp, _TEMP_CHAR_BUFFER_LENGTH_, "%d*", f->mSubTableField->GetCheckCode());
				str += temp;			
			}
		}
		str += "@";
	}

	if (str.length() > 1024 * 10)
	{
		ERROR_LOG("�ֶι�ϣ�ַ����ȳ��� 1024 * 10 >%s", str.c_str());
		return 0;
	}

	return MAKE_INDEX_ID(str.c_str());
}
//-------------------------------------------------------------------------
// ������ϱ����ڱ���еļ�¼, ��������ʹ��
//-------------------------------------------------------------------------
class FreeBaseRecord : public BaseRecord
{
public:
	AutoTable	mTable;

public:
	FreeBaseRecord() {};

	FreeBaseRecord(AutoTable t)
		: mTable(t)
	{

	}

public:
	virtual tBaseTable* GetTable() override { return mTable.getPtr(); }
	virtual AutoField getField() const override { return mTable ? mTable->GetField() : AutoField(); }
};

ARecord StructBaseTable::_NewRecord()
{
	FreeBaseRecord *pRe =  MEM_NEW FreeBaseRecord(GetSelf());	
	pRe->_alloctData(0);
	return pRe;
}

//-------------------------------------------------------------------------