#ifndef _INCLUDE_CONFIGTABLE_H_
#define _INCLUDE_CONFIGTABLE_H_

#include "IndexDBRecord.h"
#include "IndexBaseTable.h"

//-------------------------------------------------------------------------*
class BaseCommon_Export ConfigTable : public SkipBaseTable
{
public:
	ConfigTable()
		: SkipBaseTable(eInitCreateField){}

public:
	// 取得有序的记录索引迭代, 用于有序遍历
	virtual ARecordIt GetRecordIt() override;
	virtual ARecordIt GetRecordIt(ARecord targetRecord) override;

public:
	// 优化查寻速度， 主要是字符串KEY使用哈希字符串索引，数字KEY，通过ModifyIDIndex 转换为ID数组索引
	virtual AutoIndex NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey);

	virtual bool SetMainIndex(int indexCol, bool bHash, bool bMultiple = false) override;

	// 获取有序记录索引, 参数可以为空
	virtual AutoIndex GetIndex(const char *szFieldName) override
	{
		return mMainKeyOrderIndex;
	}

	bool AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore) override;

	virtual void OnRecordDataChanged(ARecord re, int col, Int64 scrValue) override;

	virtual void OnRecordDataChanged(ARecord re, int col, const char *scrValue) override;

public:
	AutoIndex		mMainKeyOrderIndex;			// 主键有序索引, 主索引一般为Hash无序
};


//-------------------------------------------------------------------------*

#endif //_INCLUDE_CONFIGTABLE_H_

// 使用ID快速索引代码, 当前已经停止使用

//bool BaseTable::SetIDMainIndex( UInt64 minKey, UInt64 maxKey )
//{
//	if (GetField()->getCount()<=0)
//	{
//		DEBUG_LOG("ERROR: 当前字段为空");
//		return false;
//	}
//
//	AutoIndex index = pIndex;
//	if (!pIndex->SetKeyRangeLimit(minKey, maxKey))
//		return false;
//	index->SetIndexField(GetField()->getFieldInfo(0)->getName().c_str());
//	mMainKeyIndex =index;
//	mMainIndexFieldCol = 0;
//	return true;
//}
//
//bool BaseTable::ModifyIDIndex(int limitMaxID, int &maxID)
//{
//	if (!mMainKeyIndex)
//		return false;
//
//	if (mMainKeyIndex->GetCount()<=0)
//		return false;
//
//	FieldInfo info = GetField()->getFieldInfo(mMainIndexFieldCol);
//	if (info!=NULL && (info->getType()==FIELD_INT || info->getType()==FIELD_INT) )
//	{
//		int minID = INT_MAX;
//		maxID = -minID;
//
//		for (ARecordIt it = mMainKeyIndex->GetRecordIt(); (*it); ++(*it))
//		{
//			if (!it->GetRecord())
//				continue;
//			int key = it->GetRecord()->get(mMainIndexFieldCol);
//			if (key<minID)
//				minID = key;
//
//			if (key>maxID)
//				maxID = key;
//		}
//
//		if (maxID>limitMaxID || maxID<minID || minID<0)
//			return false;
//
//		AutoIndex index = mMainKeyIndex;
//		SetIDMainIndex(minID, maxID+1);
//
//		for (ARecordIt it = index->GetRecordIt(); (*it); ++(*it))
//		{
//			if (it->GetRecord())
//				AppendRecord(it->GetRecord(), true);
//		}
//		GREEN_LOG("[%s] Succeed changed use id index key >[%d]~[%d]", GetTableName(), minID, maxID);
//		return true;
//	}
//
//	return false;
//}