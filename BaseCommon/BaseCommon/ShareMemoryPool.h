/********************************************************************
	created:	2012/02/24
	created:	24:2:2012   23:00
	filename: 	
	file base:	EasyMemory
	file ext:	h
	author:		杨文鸽

	purpose:	共享内存池
	动态大块内存池方案: (2020.1.18)
1 按顺序分配一大块内存, 头部保存信息, 信息最后字节设置为1, 表示使用中, 之后空间为可用空间,
开头0表示未使用, 后面表示块大小 (表示一个最大块), 最后一个字节设置为1, 表示已经使用, 到些字节之间表示块的可使用空间
2 块按 32大小递增大小切分, 块以块的大小为索引, 保存在 EasyMap(此时不可再使用内存池分配)
3 分配时, 先找到对应的块对象, 不存在,或者不存在空闲的块, 则直接从大块内存中分配, 分配后, 将后面未使用的块进行更新信息
4 分配的新块开头1字节表示已经使用,再1字节表示分配数组号, 后4字节保存分配的块的大小
5 释放时, 按块的大小进行与相邻的块进行合并分析(相邻的块之间使用两个字节隔开,代表是否被使用)
6 未使用的, 进行合并, 信息更新到头部和尾部, 然后再找到对应的块索引, 不存在则新建, 加入到块索引, 索引链表以信息后的空间保存下一个块的地址
*********************************************************************/
#ifndef _INCLUDE_SHAREMEMORYPOOL_H_
#define _INCLUDE_SHAREMEMORYPOOL_H_


#include "Assertx.h"
#include "Lock.h"
#include <math.h>
#include "EasyMap.h"
#include "ShareMemAO.h"
#include "ArrayIndex.h"
#include "BaseCommon.h"
//-------------------------------------------------------------------------
#define POOL_USE_MULTI_THREAD		1
#define DEBUG_MODE							1

#define BLOCK_2_BASE						3		// 4:16 5:32 6:64
#define BLOCK_SIZE								((int)pow(2, BLOCK_2_BASE))		// 16 32 64
#define BIG_BLOCK_SIZE(x)					( ( (UINT)(x+BLOCK_SIZE-1)>>BLOCK_2_BASE ) * BLOCK_SIZE )

#define BLOCK_OFFSET						(4)
#define BLOCK_INFO_SIZE					(8)			// 前后各4个字节 表示是否使用和大小

#define BLOCK_BEFORE(ptr)					( (BlockInfo*)((char*)ptr-BLOCK_OFFSET) )
#define BLOCK_BACK(ptr)					( (BlockInfo*)( (char*)ptr + ( (BlockInfo*)((char*)ptr-BLOCK_OFFSET) )->size() ) )

#define SET_BLOCK_USE(ptr)				{ BLOCK_BEFORE(ptr)->setUse(true); BLOCK_BACK(ptr)->setUse(true); }
#define SET_BLOCK_FREE(ptr)				{ BLOCK_BEFORE(ptr)->setUse(false); BLOCK_BACK(ptr)->setUse(false); }
// 设置大小后, 才可以设置是否被使用
#define SET_BLOCK_SIZE(ptr, size)		{ BLOCK_BEFORE(ptr)->setSize(size); BLOCK_BACK(ptr)->setSize(size); }
#define SET_BLOCK_INIT(ptr, size)		{ BLOCK_BEFORE(ptr)->set(false, size); BLOCK_BACK(ptr)->set(false, size); }
#define GET_MEM_SIZE(ptr)				( BLOCK_BEFORE(ptr)->size() )
#define MEM_IS_USED(ptr)					( BLOCK_BEFORE(ptr)->isUse() )

#define SAVE_SIZE_SPACE	sizeof(size_t)

#pragma pack(push)
#pragma pack(4)

struct BlockInfo
{
	int   block;

	BlockInfo()
		: block(0)
	{}

	BlockInfo(bool bUse, DSIZE size)
	{
		block = bUse ? abs(size) : -abs(size);
	}

	void set(bool bUse, DSIZE size)
	{
		block = bUse ? abs(size) : -abs(size);
	}

	void setUse(bool bUse)
	{
		block = bUse ? abs(block) : -abs(block);
	}

	void setSize(DSIZE size)
	{
		size = abs(size);
		bool bUse = block > 0;
		block = bUse ? size : -size;
	}

	bool isUse()
	{
		return block >= 0;
	}

	DSIZE size()
	{
		return abs(block);
	}
};
#pragma pack(pop)
//-------------------------------------------------------------------------
struct ShareMemNode
{
	void *mpPreviou;
	void *mpNext;
};
//-------------------------------------------------------------------------

struct ShareMemoryBucket
{
	unsigned char		*mpNewPointer;
	DSIZE mBlockSize;

	ShareMemoryBucket()
		: mpNewPointer(NULL)
		, mBlockSize(0)
	{

	}

	void Append(void *pMem)
	{
		SET_BLOCK_FREE(pMem);
		// 链接到开头
		if (mpNewPointer != NULL)
		{
			ShareMemNode &pNowNode = *reinterpret_cast<ShareMemNode*>(mpNewPointer);
			pNowNode.mpPreviou = pMem;
		}

		ShareMemNode &pNode = *reinterpret_cast<ShareMemNode*>(pMem);
		pNode.mpNext = mpNewPointer;
		pNode.mpPreviou = NULL;
		mpNewPointer = static_cast<unsigned char*>(pMem);
		//*reinterpret_cast<unsigned char**>(pMem) = mpNewPointer;
		//mpNewPointer = static_cast<unsigned char*>(pMem);
	}

	void* Pop()
	{
		if (mpNewPointer == NULL)
			return NULL;

		unsigned char *rp = mpNewPointer;
		if (mpNewPointer != NULL)
		{
			ShareMemNode &pNowNode = *reinterpret_cast<ShareMemNode*>(mpNewPointer);
			mpNewPointer = (unsigned char*)pNowNode.mpNext;
			if (pNowNode.mpNext != NULL)
			{
				ShareMemNode *pNextNode = reinterpret_cast<ShareMemNode*>(pNowNode.mpNext);
				pNextNode->mpPreviou = NULL;
			}
		}
		SET_BLOCK_USE(rp);
		//由于头4个字节被“强行”解释为指向下一内存块的指针，这里m_NewPointer就指向了下一个内存块，以备下次分配使用。 
		//mpNewPointer = *reinterpret_cast<unsigned char**>(rp);
		return rp;
	}
};

//-------------------------------------------------------------------------
// 分配的所有块BigMemoryBlock根据块的地址保存在EasyMap
// 分配时, 找到对应的块或最小的大于块, 按最小化分裂, 分裂后, 剩余的空间更新块信息, 追加到新的对应块索引
// 释放时, 分析左右合并, 然后加入对应的块中
// 内存块空间: SIZEID********SIZE (size = 4 + 2 + size + 4), 每个块的前后各4个字节表示是否使用和大小
// NOTE: 传出的地址是, 共享内存 key+偏移空间 64位 SHARE_PTR, 前32位KEY, 后32位偏移大小
//			空间的块使用双向链表方式链接在一起, 便于合并移除
//			目前一个进程只支持使用一个此实例
//-------------------------------------------------------------------------
class BaseCommon_Export ShareMemoryPool
{
public:
	ShareMemoryPool(bool bInitCreateMem, const char *szName, DSIZE initBigBlockSize = 16 * 1024 * 1024)
		: mNowIndex(0)
		, mNowBigBucketSize(0)
		, mbInitCreate(bInitCreateMem)
		, mShareMemName(szName)
	{
		mNowBigBucketSize = BIG_BLOCK_SIZE(initBigBlockSize);
	}

	virtual ~ShareMemoryPool()
	{
		
	}

	virtual void OnCreateShareMemory(const AString &name) {}

public:
	ShareMemoryBucket* AllocShareMemory(DSIZE size)
	{
		size = BIG_BLOCK_SIZE(size);
		DSIZE newSize = size;
		//前后加上已经被使用的标识, 用于合并分析时, 中止判断
		newSize += BLOCK_INFO_SIZE; // 前后加上标识
		newSize += BLOCK_INFO_SIZE; // 块的标识

		AString keyName;
		keyName.Format("%s_%d", mShareMemName.c_str(), ++mNowIndex);
		int key = MAKE_INDEX_ID(keyName.c_str());
		ShareMemAO *pMem = MEM_NEW ShareMemAO();
		if (pMem->Create(key, newSize) == TRUE)
		{
			OnCreateShareMemory(keyName);
			memset(pMem->GetDataPtr(), 0, newSize);
			mShareMemoryList.Append(mNowIndex, pMem);
			char *ptr = pMem->GetDataPtr();
			((BlockInfo*)ptr)->set(true, 0);	 // 标识使用中, 判断不可合并

			SET_BLOCK_SIZE(ptr + BLOCK_INFO_SIZE, size);
			SET_BLOCK_FREE(ptr + BLOCK_INFO_SIZE);

			((BlockInfo*)(ptr + BLOCK_INFO_SIZE+size+BLOCK_OFFSET))->set(true, 0); // 标识使用中, 判断不可合并

			_AppendFreeBlock( ptr + BLOCK_OFFSET * 2);
			return mBlockList.findPtr(size);
		}
		else
		{
			delete pMem;
			ERROR_LOG("Create share memory %d fail", key);
			return NULL;
		}
	}

	void Split(void *pMem, DSIZE size)
	{
		DSIZE blockSize = BLOCK_BEFORE(pMem)->size();
		if (blockSize - size >= 64)
		{
			// 分离后, 大小减少 8字节
			DSIZE lastSize = blockSize - size - BLOCK_INFO_SIZE;
			// 设置新的大小
			SET_BLOCK_SIZE(pMem, size);
			void *pLast = (char*)pMem + size + BLOCK_INFO_SIZE;
			SET_BLOCK_SIZE(pLast, lastSize);
			SET_BLOCK_FREE(pLast);
			_AppendFreeBlock(pLast);
		}
	}

	void* Merge(void *pLeftMem, void *pRightMem)
	{
		DSIZE size = GET_MEM_SIZE(pLeftMem);
		DSIZE size2 = GET_MEM_SIZE(pRightMem);
		SET_BLOCK_INIT(pLeftMem, size + size2 + BLOCK_INFO_SIZE);
		return pLeftMem;
	}

	void* AnalysisMergeFree(void *pMem)
	{
		BlockInfo *pLeftInfo = (BlockInfo*)((char*)pMem - BLOCK_INFO_SIZE);
		if (!pLeftInfo->isUse())
		{
			void *pLeft = (char*)pLeftInfo - pLeftInfo->size();
			// 把左边空闭的块从链表中移除
			_RemoveFormFree(pLeft);
			pMem = Merge(pLeft, pMem);
		}
		DSIZE size = GET_MEM_SIZE(pMem);
		BlockInfo *pRightInfo= (BlockInfo*)((char*)pMem + size +  BLOCK_OFFSET);
		if (!pRightInfo->isUse())
		{
			void *pRight = (char*)pRightInfo + BLOCK_OFFSET;
			_RemoveFormFree(pRight);
			pMem = Merge(pMem, pRight);
		}
		return pMem;
	}

	SHARE_PTR NewPtr(DSIZE size);

	void FreePtr(SHARE_PTR mem)
	{
		void *pFreeMem = ToMemPtr(mem);

		void *pMem = AnalysisMergeFree(pFreeMem);	

		SET_BLOCK_FREE(pMem);

		_AppendFreeBlock(pMem);
	}

	void* ToMemPtr(SHARE_PTR mem)
	{
		ShareMemAO  *p = mShareMemoryList.Find(mem.key);
		if (p != NULL)
			return (void*)(p->GetDataPtr() + mem.pos);
		return NULL;
	}

	bool Restore()
	{
		mNowIndex = 1;
		AString keyName;
		while (true)
		{
			keyName.Format("%s_%d", mShareMemName.c_str(), mNowIndex);
			int key = MAKE_INDEX_ID(keyName.c_str());
			ShareMemAO *pMem = MEM_NEW ShareMemAO();
			UINT memSize = 0;
 			if (pMem->Attach(key, memSize) == TRUE)
			{
				mShareMemoryList.Append(mNowIndex++, pMem);
				DSIZE totalSize = BLOCK_INFO_SIZE;
				char *pHead = pMem->GetDataPtr();
				void *pBlock = pHead + BLOCK_INFO_SIZE;
				do
				{
					DSIZE size = GET_MEM_SIZE(pBlock);
					if (size <= 0 || pBlock<pHead || pBlock>=pHead+ memSize)
						break;
 					totalSize += size + BLOCK_INFO_SIZE;
					if (!MEM_IS_USED(pBlock))
						_AppendFreeBlock(pBlock);
					pBlock = (char*)pBlock + size + BLOCK_INFO_SIZE;
				} while (true);

				if (totalSize != memSize)
				{
					ERROR_LOG("Memory size check fail %d != %u", totalSize, memSize);
					return false;
				}
			}
			else			
				break;
		}
		return true;
	}

	SHARE_PTR GetSharePtr(void *pMem)
	{
		for (int i=0; i<mShareMemoryList.size(); ++i)
		{
			ShareMemAO *pShareMem = mShareMemoryList[i];
			if (pShareMem != NULL)
			{
				if (pMem >= pShareMem->GetDataPtr() && pMem < pShareMem->GetDataPtr() + pShareMem->GetSize())
				{
					SHARE_PTR  ptr;
					ptr.key = i;
					ptr.pos = (DSIZE)((char*)pMem - pShareMem->GetDataPtr());
					return ptr;
				}
			}
		}
		return SHARE_PTR();
	}

	void  _RemoveFormFree(void *pMem)
	{		
		ShareMemNode &node = *(ShareMemNode*)pMem;

		if (node.mpNext!=NULL)
			((ShareMemNode*)node.mpNext)->mpPreviou = node.mpPreviou;		

		if (node.mpPreviou != NULL)
			((ShareMemNode*)node.mpPreviou)->mpNext = node.mpNext;
		else
		{
			// 说明是第一个, 需要更新第一个指针成员
			DSIZE size = GET_MEM_SIZE(pMem);
			ShareMemoryBucket *pBucket = mBlockList.findPtr(size);
			AssertNote(pBucket != NULL, "Must bucket %d exist", size);
			pBucket->mpNewPointer = (unsigned char*)node.mpNext;
		}
	}

	void _AppendFreeBlock(void *pMem)
	{
		DSIZE size = GET_MEM_SIZE(pMem);
		ShareMemoryBucket *pBucket = mBlockList.findPtr(size);
		if (pBucket == NULL)
		{
			mBlockList.insert(size, ShareMemoryBucket());
			pBucket = mBlockList.findPtr(size);
		}
		pBucket->Append(pMem);
	}

protected:
	ArrayIndex<ShareMemAO>											mShareMemoryList;
	EasyMap<int, ShareMemoryBucket, 256, false, false>	mBlockList;

	AString								mShareMemName;			// 使用此名称_序号, 产生共享内存KEY
	int									mNowIndex;
	int									mNowBigBucketSize;		
	bool									mbInitCreate;					// 如果是初始创建才析构时释放, 否则为恢复模式, 析构时, 不进行释放


};

//-------------------------------------------------------------------------

#endif //_INCLUDE_SHAREMEMORYPOOL_H_