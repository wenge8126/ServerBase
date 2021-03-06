
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
				DEBUG_LOG("ERROR: 未实现 [%s] 类型的索引", info->getTypeString() );
			}
			else
			{
				DEBUG_LOG("ERROR: 不存在字符字段类型 [%d]", indexKeyType);
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
	ERROR_LOG("ERROR: %s 加入到主索引失败 >%f", GetTableName(), fIndex);
	return ARecord();
}

ARecord BaseTable::CreateRecord( const char* szIndex, bool bReplace )
{
	ARecord r = NewRecord();
	int col = mMainKeyIndex->IndexFieldCol();

	r->_set(col, szIndex);
	if (AppendRecord(r, bReplace))
		return r;
	ERROR_LOG("ERROR: %s 加入到主索引失败 >%s", GetTableName(), szIndex);
	return ARecord();
}

ARecord BaseTable::CreateRecord( Int64 nIndex, bool bReplace )
{
	ARecord r = NewRecord();
	int col = mMainKeyIndex->IndexFieldCol();
	r->_set(col, nIndex);
	if (AppendRecord(r, bReplace))
		return r;
	ERROR_LOG("ERROR: %s 加入到主索引失败 >%llu", GetTableName(), (UInt64)nIndex);
	return ARecord();
}


void BaseTable::ClearAll()
{
	if (mMainKeyIndex && mMainKeyIndex->GetCount()>0)
	{
		//NOTE: 使用AutoPtr, 所有记录其他地方的引用依然可以存在, 且表格依然指向当前表格
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
	// 终端新建记录, 需要全部同步到DB
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
				ERROR_LOG("恢复记录数据失败");
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

// 删除DB数据库中的记录, 且只能通过此功能删除, 原因, 当子表移除记录后, 更新数据中得不到被删除的信息
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
			//!!! NOTE 任何地方千万不可使用 Auto<tFieldInfo> 临时使用, 否则会自动释放掉 tFieldInfo
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
		ERROR_LOG("字段哈希字符长度超过 1024 * 10 >%s", str.c_str());
		return 0;
	}

	return MAKE_INDEX_ID(str.c_str());
}
//-------------------------------------------------------------------------
// 不会组合保存在表格中的记录, 可以自由使用
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