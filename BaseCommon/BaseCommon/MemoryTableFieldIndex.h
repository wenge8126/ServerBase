
#ifndef _INCLUDE_MEMORYTABLEFIELDINDEX_H_
#define _INCLUDE_MEMORYTABLEFIELDINDEX_H_

#include "IndexBaseTable.h"
#include "RecordMemPool.h"
#include "EasyStack.h"
/*/-------------------------------------------------------------------------/
NOTE: �����ͷŵļ�¼, �Ż���¼restoreʱ���ٶ�
	��¼���½�ʱ�����ÿ���ֶν���һ�γ�ʼ������ʱ�Ϻ�ʱ, ����ֱ��ʹ�û���ļ�¼
//-------------------------------------------------------------------------*/

#define DEBUG_RECORD_POOL		0

class BaseCommon_Export MemoryTableFieldIndex : public BaseFieldIndex
{
	//friend class MemoryRecord;
public:
	MemoryTableFieldIndex(tBaseTable *pTable)
        : mpTable(pTable)
#if DEBUG_RECORD_POOL
		, mUseCount(0)
		, mFreeCount(0)
#endif
	{

	}
	~MemoryTableFieldIndex();

	virtual INIT_FIELD_MODE getFieldMode() const { return eInitPoolField; }

	virtual BaseRecord* TryGetFreeRecord() override
	{
#if DEBUG_RECORD_POOL
		BaseRecord *p = mFreeRecordPool.pop();
		if (p!=NULL)
			++mUseCount;
		return p;
#else
		return mFreeRecordPool.pop();
#endif
	}

public:
	virtual void* _allotRecordData(uint size) override
	{
		if (size<=mRecordMemPool.getBlockSize())
			return mRecordMemPool.newPtr();
		AssertNote(0, "����ռ�[%u]������¼���趨��С[%llu]", size, mRecordMemPool.getBlockSize());
		return NULL;
	}

	virtual void _freeRecordData(void *pRecordData) override
	{
		mRecordMemPool.deletePtr((unsigned char*)pRecordData);
	}

	// NOTE: ��ִ��֮ǰ��һ��Ҫ��������صļ�¼�ͷ�
	virtual void _updateInfo() override
	{		
        AssertNote(mpTable==NULL || mpTable->GetRecordCount()<=0, "Clear record data error : table exist record");
		_ClearFreeRecord();
		BaseFieldIndex::_updateInfo();
		mRecordMemPool.clear();
		mRecordMemPool.setBlockSize(getDataLength()+(getCount()+7)/8);
	}

protected:
	virtual void FreeRecord(BaseRecord *pRecord) override;
	void _ClearFreeRecord();

protected:
	RecordMemPool			mRecordMemPool;
	EasyStack<BaseRecord*>	mFreeRecordPool;
    tBaseTable                      *mpTable;

#if DEBUG_RECORD_POOL
	int						mFreeCount;
	int						mUseCount;
#endif
};


#endif //_INCLUDE_MEMORYTABLEFIELDINDEX_H_