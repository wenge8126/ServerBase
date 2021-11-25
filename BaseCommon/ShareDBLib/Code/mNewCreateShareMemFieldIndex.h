#pragma once

#ifndef _INCLUDE_NEWCREATESHAREMEMFIELDINDEX_H_
#define _INCLUDE_NEWCREATESHAREMEMFIELDINDEX_H_

#include "IndexBaseTable.h"
#include "mDBRecordShareMemoryPool.h"

/*/-------------------------------------------------------------------------/
动态分配共享内存记录池
NOTE: 只能在共享进程内完成新建记录
	
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
			AssertNote(0, "分配空间[%u]超过记录池设定大小[%llu]", size, mRecordMemPool.getBlockSize());
			return NULL;
		}

		virtual void _freeRecordData(void *pRecordData) override
		{
			mRecordMemPool.deletePtr((unsigned char*)pRecordData);
		}

		// NOTE: 在执行之前，一定要把所有相关的记录释放
		virtual void _updateInfo() override;

	public:
		DBRecordShareMemoryPool		mRecordMemPool;
	};

}
#endif //_INCLUDE_SHAREMEMORYFIELDINDEX_H_

