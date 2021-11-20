
#include "ShareMemoryPool.h"


SHARE_PTR ShareMemoryPool::NewPtr(DSIZE size)
{
	size = BIG_BLOCK_SIZE(size);
	size_t pos = mBlockList._findInsertPos(size);
	void *p = NULL;
	while(pos < mBlockList.size())
	{
		auto &v = mBlockList.getValue(pos);
		p = v.mVal.Pop();
		if (p != NULL)
		{
			if (v.mKey - size >= 64)
				Split(p, size);
			break;
		}
		mBlockList._remove(pos);
	}
	if (p == NULL)
	{
		if (!mbInitCreate)
		{
			ERROR_LOG("Now share memory pool is restore mode");
			return SHARE_PTR();
		}
		mNowBigBucketSize *= 2;
		if (mNowBigBucketSize < size)
			mNowBigBucketSize = size;

		ShareMemoryBucket *pBucket = AllocShareMemory(mNowBigBucketSize);
		p = pBucket->Pop();
		if (mNowBigBucketSize - size >= 64)
			Split(p, size);
		SET_BLOCK_USE(p);
	}

	return GetSharePtr( p );
}

