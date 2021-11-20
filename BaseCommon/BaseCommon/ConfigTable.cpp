
#include "ConfigTable.h"
#include "StringHashIndex.h"
#include "StringEasyHashIndex.h"
#include "IntEasyMapIndex.h"
#include "IntEasyHashIndex.h"
#include "Int64HashIndex.h"
#include "MultipleStringIndex.h"

ARecordIt ConfigTable::GetRecordIt()
{
	if (mMainKeyOrderIndex) return mMainKeyOrderIndex->GetRecordIt();
	if (mMainKeyIndex) return mMainKeyIndex->GetRecordIt();
	ERROR_LOG("No exist main index, may be load fail");
	thread_local static AutoIndex msIndex = MEM_NEW IntEasyMapIndex();
	return msIndex->GetRecordIt();
}

ARecordIt ConfigTable::GetRecordIt(ARecord targetRecord)
{
	if (!targetRecord)
		return ARecordIt();
	if (mMainKeyOrderIndex) return mMainKeyOrderIndex->GetRecordIt(targetRecord);
	if (mMainKeyIndex)  return mMainKeyIndex->GetRecordIt(targetRecord);
	ERROR_LOG("No exist main index, may be load fail");
	return ARecordIt();
}

AutoIndex ConfigTable::NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey)
{
	if (bMultKey)
		ERROR_LOG("[%s] 配置表格不支持多键值, 当前使用唯一键值索引", GetTableName());

	switch (indexKeyType)
	{
		//case FIELD_FLOAT:
	case FIELD_STRING:
	case FIELD_CHAR_STRING:
		//StringEasyHashIndex已经优化为EasyHash StringHashIndex();
		return MEM_NEW StringEasyHashIndex(); 

	case FIELD_BYTE:
	case FIELD_SHORT:
	case FIELD_INT:
		return MEM_NEW IntEasyHashIndex();	// NOTE: 使用EasyHash<int, ARecord> 2017.6.20

	case FIELD_INT64:
	case FIELD_UINT64:
		return MEM_NEW Int64HashIndex();

	default:
		{
			FieldInfo info = FieldInfoManager::getFieldInfo(indexKeyType);
			if (info!=NULL)
			{
				DEBUG_LOG("ERROR: [%s]未实现 [%s] 类型的索引", GetTableName(), info->getTypeString() );
			}
			else
			{
				DEBUG_LOG("ERROR: [%s]不存在字符字段类型 [%d]", GetTableName(), indexKeyType);
			}
		}
	}

	return AutoIndex();
}

bool ConfigTable::SetMainIndex(int indexCol, bool bHash, bool bMultiple /*= false*/)
{
	if (SkipBaseTable::SetMainIndex(indexCol, bHash, bMultiple))
	{
		if (mMainKeyIndex->IsStringHashKey())
			mMainKeyOrderIndex = MEM_NEW STL_MultipleStringIndex();
		else
			mMainKeyOrderIndex = MEM_NEW IntEasyMapIndex();
		mMainKeyOrderIndex->SetIndexField(GetField()->getFieldInfo(indexCol)->getName().c_str(), indexCol);
		return true;
	}
	return false;
}

bool ConfigTable::AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore)
{
	if (SkipBaseTable::AppendRecord(scrRecord, bReplace, true))
	{
		mMainKeyOrderIndex->InsertRecord(scrRecord);
		return true;
	}
	return false;
}

void ConfigTable::OnRecordDataChanged(ARecord re, int col, Int64 scrValue)
{
	SkipBaseTable::OnRecordDataChanged(re, col, scrValue);
	if (mMainKeyIndex && col == mMainKeyIndex->IndexFieldCol())
	{
		if (mMainKeyOrderIndex->RemoveRecord(scrValue, re))
			mMainKeyOrderIndex->InsertRecord(re);
		else
			ERROR_LOG("%s No exist record %s in index %s, record :\r\n%s", GetTableName(), STRING(scrValue), mMainKeyOrderIndex->mIndexFieldName.c_str(), re->ToNiceData()->dump().c_str());
	}
}

void ConfigTable::OnRecordDataChanged(ARecord re, int col, const char *scrValue)
{
	SkipBaseTable::OnRecordDataChanged(re, col, scrValue);
	if (mMainKeyIndex && col == mMainKeyIndex->IndexFieldCol())
	{
		if (mMainKeyOrderIndex->RemoveRecord(scrValue, re))
			mMainKeyOrderIndex->InsertRecord(re);
		else
			ERROR_LOG("%s No exist record %s in index %s, record :\r\n%s", GetTableName(), scrValue, mMainKeyOrderIndex->mIndexFieldName.c_str(), re->ToNiceData()->dump().c_str());
	}
}
