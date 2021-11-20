#include "MemoryTableFieldIndex.h"
#include "IndexDBRecord.h"


MemoryTableFieldIndex::~MemoryTableFieldIndex()
{
	_ClearFreeRecord();
}

void MemoryTableFieldIndex::FreeRecord(BaseRecord *pRecord)
{
	if (mFreeRecordPool.size()>1000)
	{
		// ��������ʱй¶
		//NOTE_LOG("!!! WARN: ����ļ�¼�������࣬����δ���ظ�ʹ�ã������������");
		BaseFieldIndex::FreeRecord(pRecord);
		return;
	}
	pRecord->InitData();
	//!!! һ��Ҫ�Ѽ�¼�е��ֶ���Ϣ���������������������������ͷ�
	IndexDBRecord *p = dynamic_cast<IndexDBRecord*>(pRecord);
	AssertEx(p!=NULL, "Must is IndexDBRecord use FreeRecord");
	p->mOwnerTable.setNull();

	mFreeRecordPool.push(pRecord);
#if DEBUG_RECORD_POOL
	++mFreeCount;
#endif
}

void MemoryTableFieldIndex::_ClearFreeRecord()
{
#if DEBUG_RECORD_POOL
	//NOTE_LOG("---Field----\r\n%s\r\n------------", ToString().c_str());
	if (mUseCount>0)
		NOTE_LOG("###### field use [%d], free [%d], now [%llu] ===", mUseCount, mFreeCount, mFreeRecordPool.size());
#endif
	while (!mFreeRecordPool.empty())
	{
		BaseRecord *pRe = mFreeRecordPool.pop();
		IndexDBRecord *p = dynamic_cast<IndexDBRecord*>(pRe);
		//!!! �������ͷ�,���ܳط����ϵͳ�ڴ� OxFFFF ��ͷ��ʶ
		if (p->mRecordData!=NULL)
		{
			mRecordMemPool.deletePtr((unsigned char*)p->mRecordData);
			p->mRecordData = NULL;
		}
		MEM_DELETE pRe;
	}
}
