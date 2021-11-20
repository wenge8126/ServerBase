/********************************************************************
	created:	2012/02/24
	created:	24:2:2012   23:00
	filename: 	
	file base:	EasyMemory
	file ext:	h
	author:		���ĸ�

	purpose:	�����ڴ��
	��̬����ڴ�ط���: (2020.1.18)
1 ��˳�����һ����ڴ�, ͷ��������Ϣ, ��Ϣ����ֽ�����Ϊ1, ��ʾʹ����, ֮��ռ�Ϊ���ÿռ�,
��ͷ0��ʾδʹ��, �����ʾ���С (��ʾһ������), ���һ���ֽ�����Ϊ1, ��ʾ�Ѿ�ʹ��, ��Щ�ֽ�֮���ʾ��Ŀ�ʹ�ÿռ�
2 �鰴 32��С������С�з�, ���Կ�Ĵ�СΪ����, ������ EasyMap(��ʱ������ʹ���ڴ�ط���)
3 ����ʱ, ���ҵ���Ӧ�Ŀ����, ������,���߲����ڿ��еĿ�, ��ֱ�ӴӴ���ڴ��з���, �����, ������δʹ�õĿ���и�����Ϣ
4 ������¿鿪ͷ1�ֽڱ�ʾ�Ѿ�ʹ��,��1�ֽڱ�ʾ���������, ��4�ֽڱ������Ŀ�Ĵ�С
5 �ͷ�ʱ, ����Ĵ�С���������ڵĿ���кϲ�����(���ڵĿ�֮��ʹ�������ֽڸ���,�����Ƿ�ʹ��)
6 δʹ�õ�, ���кϲ�, ��Ϣ���µ�ͷ����β��, Ȼ�����ҵ���Ӧ�Ŀ�����, ���������½�, ���뵽������, ������������Ϣ��Ŀռ䱣����һ����ĵ�ַ
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
#define BLOCK_INFO_SIZE					(8)			// ǰ���4���ֽ� ��ʾ�Ƿ�ʹ�úʹ�С

#define BLOCK_BEFORE(ptr)					( (BlockInfo*)((char*)ptr-BLOCK_OFFSET) )
#define BLOCK_BACK(ptr)					( (BlockInfo*)( (char*)ptr + ( (BlockInfo*)((char*)ptr-BLOCK_OFFSET) )->size() ) )

#define SET_BLOCK_USE(ptr)				{ BLOCK_BEFORE(ptr)->setUse(true); BLOCK_BACK(ptr)->setUse(true); }
#define SET_BLOCK_FREE(ptr)				{ BLOCK_BEFORE(ptr)->setUse(false); BLOCK_BACK(ptr)->setUse(false); }
// ���ô�С��, �ſ��������Ƿ�ʹ��
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
		// ���ӵ���ͷ
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
		//����ͷ4���ֽڱ���ǿ�С�����Ϊָ����һ�ڴ���ָ�룬����m_NewPointer��ָ������һ���ڴ�飬�Ա��´η���ʹ�á� 
		//mpNewPointer = *reinterpret_cast<unsigned char**>(rp);
		return rp;
	}
};

//-------------------------------------------------------------------------
// ��������п�BigMemoryBlock���ݿ�ĵ�ַ������EasyMap
// ����ʱ, �ҵ���Ӧ�Ŀ����С�Ĵ��ڿ�, ����С������, ���Ѻ�, ʣ��Ŀռ���¿���Ϣ, ׷�ӵ��µĶ�Ӧ������
// �ͷ�ʱ, �������Һϲ�, Ȼ������Ӧ�Ŀ���
// �ڴ��ռ�: SIZEID********SIZE (size = 4 + 2 + size + 4), ÿ�����ǰ���4���ֽڱ�ʾ�Ƿ�ʹ�úʹ�С
// NOTE: �����ĵ�ַ��, �����ڴ� key+ƫ�ƿռ� 64λ SHARE_PTR, ǰ32λKEY, ��32λƫ�ƴ�С
//			�ռ�Ŀ�ʹ��˫������ʽ������һ��, ���ںϲ��Ƴ�
//			Ŀǰһ������ֻ֧��ʹ��һ����ʵ��
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
		//ǰ������Ѿ���ʹ�õı�ʶ, ���ںϲ�����ʱ, ��ֹ�ж�
		newSize += BLOCK_INFO_SIZE; // ǰ����ϱ�ʶ
		newSize += BLOCK_INFO_SIZE; // ��ı�ʶ

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
			((BlockInfo*)ptr)->set(true, 0);	 // ��ʶʹ����, �жϲ��ɺϲ�

			SET_BLOCK_SIZE(ptr + BLOCK_INFO_SIZE, size);
			SET_BLOCK_FREE(ptr + BLOCK_INFO_SIZE);

			((BlockInfo*)(ptr + BLOCK_INFO_SIZE+size+BLOCK_OFFSET))->set(true, 0); // ��ʶʹ����, �жϲ��ɺϲ�

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
			// �����, ��С���� 8�ֽ�
			DSIZE lastSize = blockSize - size - BLOCK_INFO_SIZE;
			// �����µĴ�С
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
			// ����߿ձյĿ���������Ƴ�
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
			// ˵���ǵ�һ��, ��Ҫ���µ�һ��ָ���Ա
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

	AString								mShareMemName;			// ʹ�ô�����_���, ���������ڴ�KEY
	int									mNowIndex;
	int									mNowBigBucketSize;		
	bool									mbInitCreate;					// ����ǳ�ʼ����������ʱ�ͷ�, ����Ϊ�ָ�ģʽ, ����ʱ, �������ͷ�


};

//-------------------------------------------------------------------------

#endif //_INCLUDE_SHAREMEMORYPOOL_H_