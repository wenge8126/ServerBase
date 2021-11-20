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
	// ȡ������ļ�¼��������, �����������
	virtual ARecordIt GetRecordIt() override;
	virtual ARecordIt GetRecordIt(ARecord targetRecord) override;

public:
	// �Ż���Ѱ�ٶȣ� ��Ҫ���ַ���KEYʹ�ù�ϣ�ַ�������������KEY��ͨ��ModifyIDIndex ת��ΪID��������
	virtual AutoIndex NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey);

	virtual bool SetMainIndex(int indexCol, bool bHash, bool bMultiple = false) override;

	// ��ȡ�����¼����, ��������Ϊ��
	virtual AutoIndex GetIndex(const char *szFieldName) override
	{
		return mMainKeyOrderIndex;
	}

	bool AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore) override;

	virtual void OnRecordDataChanged(ARecord re, int col, Int64 scrValue) override;

	virtual void OnRecordDataChanged(ARecord re, int col, const char *scrValue) override;

public:
	AutoIndex		mMainKeyOrderIndex;			// ������������, ������һ��ΪHash����
};


//-------------------------------------------------------------------------*

#endif //_INCLUDE_CONFIGTABLE_H_

// ʹ��ID������������, ��ǰ�Ѿ�ֹͣʹ��

//bool BaseTable::SetIDMainIndex( UInt64 minKey, UInt64 maxKey )
//{
//	if (GetField()->getCount()<=0)
//	{
//		DEBUG_LOG("ERROR: ��ǰ�ֶ�Ϊ��");
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