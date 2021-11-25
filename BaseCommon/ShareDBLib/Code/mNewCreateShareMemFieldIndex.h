#pragma once

#ifndef _INCLUDE_NEWCREATESHAREMEMFIELDINDEX_H_
#define _INCLUDE_NEWCREATESHAREMEMFIELDINDEX_H_

#include "IndexBaseTable.h"
#include "mDBRecordShareMemoryPool.h"

/*/-------------------------------------------------------------------------/
��̬���乲���ڴ��¼��
NOTE: ֻ���ڹ������������½���¼
	
//-------------------------------------------------------------------------*/
namespace NetCloud
{
	class ShareMemTableData;

	class ShareDBLib_Export NewCreateShareMemFieldIndex : public BaseFieldIndex
	{
		
	public:
		NewCreateShareMemFieldIndex(ShareMemTableData *pMemTableData)
			: mRecordMemPool(pMemTableData)
		{

		}
		~NewCreateShareMemFieldIndex()
		{

		}

		virtual INIT_FIELD_MODE getFieldMode() const override { return eShareMemoryCreateField; }
		virtual SHARE_PTR getShareMemoryPtr(const char *szRecordDataPtr) override
		{
			return mRecordMemPool.getShareMemoryPtr(szRecordDataPtr);
		}

		void* getShareMemory(SHARE_PTR memPtr)
		{
			return mRecordMemPool.getShareMemory(memPtr);
		}

	public:
		virtual void* _allotRecordData(uint size) override
		{
			if (size <= mRecordMemPool.getBlockSize())
				return mRecordMemPool.newPtr();
			AssertNote(0, "����ռ�[%u]������¼���趨��С[%llu]", size, mRecordMemPool.getBlockSize());
			return NULL;
		}

		virtual void _freeRecordData(void *pRecordData) override
		{
			mRecordMemPool.deletePtr((unsigned char*)pRecordData);
		}

		// NOTE: ��ִ��֮ǰ��һ��Ҫ��������صļ�¼�ͷ�
		virtual void _updateInfo() override;

	public:
		DBRecordShareMemoryPool		mRecordMemPool;
	};

}
#endif //_INCLUDE_SHAREMEMORYFIELDINDEX_H_

