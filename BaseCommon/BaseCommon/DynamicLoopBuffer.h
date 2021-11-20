#pragma once
#ifndef _INCLUDE_DYNAMICLOOPBUFFER_H_
#define _INCLUDE_DYNAMICLOOPBUFFER_H_

#include "DataStream.h"
#include "BaseCommon.h"
#include "MemoryLock.h"
#include "ShareMemAO.h"
//-------------------------------------------------------------------------
// 使用增加缓存块的方式, 动态增加
// 读写支持多线程, 支持
//-------------------------------------------------------------------------

class BaseCommon_Export_H tDynamicLoopBuffer : public tLoopStream
{
public:
	virtual UINT	Write(const CHAR* buf, UINT len)
	{
		if (_write((void*)buf, len))
			return len;
		return 0;
	}
	virtual UINT	Read(CHAR* buf, UINT len)
	{
		if (_read((void*)buf, len))
			return len;
		return 0;
	}

	virtual DSIZE tell(void) const
	{
		ERROR_LOG("tDynamicLoopBuffer can not tell");
		return 0;
	}
	virtual bool seek(DSIZE absolutePosition)
	{
		ERROR_LOG("tDynamicLoopBuffer can not seek");
		return false;
	}
	virtual bool setDataSize(DSIZE dataSize)
	{
		ERROR_LOG("tDynamicLoopBuffer can not setDataSize");
		return false;
	}

	virtual DSIZE size(void) const
	{
		ERROR_LOG("tDynamicLoopBuffer can not size");
		return 0;
	}
	virtual DSIZE dataSize(void) const
	{
		ERROR_LOG("tDynamicLoopBuffer can not dataSize");
		return 0;
	}
	virtual char* data(void)
	{
		ERROR_LOG("tDynamicLoopBuffer can not data");
		return NULL;
	}

	virtual bool resize(DSIZE destSize, char fullValue = 0)
	{
		ERROR_LOG("tDynamicLoopBuffer can not resize");
		return false;
	}


public:
	struct  DataPosInfo
	{
		union
		{
			struct {
				uint mArrayIndex;
				uint mPos;
			};
			UInt64			value;
		};

		DataPosInfo() {}
		DataPosInfo(UInt64 val) : value(val) {}
		DataPosInfo(uint arrayIndex, uint pos) : mArrayIndex(arrayIndex), mPos(pos) {  }

		DataPosInfo(const DataPosInfo &other)
		{
			value = other.value;
		}

		DataPosInfo& operator = (const DataPosInfo &other)
		{
			value = other.value;
		}
	};

	struct BlockInfo
	{
	protected:
		char *mPtr;

	public:
		uint mSize;
		uint mPreviousIndex;		// 上一个
		uint mNextIndex;
		uint mArrayIndex;
		char *mReceivePtr;		// 接收端的数据指针, 因为获取到的共享内存的数据指针进程间是不相同

		void Link(BlockInfo *pPreviousInfo, tDynamicLoopBuffer *pOwnerBuffer)
		{
			uint next = pPreviousInfo->mNextIndex;

			pPreviousInfo->mNextIndex = mArrayIndex;
			mPreviousIndex = pPreviousInfo->mArrayIndex;

			mNextIndex = next;
			BlockInfo *pNextBlock = pOwnerBuffer->getBlock(next);
			pNextBlock->mPreviousIndex = mArrayIndex;
		}

		char* GetDataPtr()
		{
			return mPtr;
		}

		void SetDataPtr(void *ptr)
		{
			mPtr = (char*)ptr;
		}
	};

	struct ArrayInfo
	{
		char mLock;
		UInt64 mHeadPosInfo;
		UInt64 mTailPosInfo;
		uint mArrayCount;
		UInt64 mTotalMemorySize;		// 统计分配共享内存空间
		UInt64 mDebugCount;			// 用于调试显示业务层数量信息(如投递的更新数量)
	};

	enum 
	{
		eWaitLock = 0,
		eWriteLock = 1,
		eReadLock = 2,
	};

public:
	tDynamicLoopBuffer()
		: mpBufferArrayAndInfo(NULL)
	{

	}

	virtual bool Init(int key) = 0;

	void *getArrayBuffer() { return mpBufferArrayAndInfo; }

	virtual char* GetBlockDataPtr(BlockInfo *pBlock)
	{
		return pBlock->GetDataPtr();
	}

	virtual void SetBlockDataPtr(BlockInfo *pBlock, void *ptr)
	{
		pBlock->SetDataPtr(ptr);
	}

public:
	ArrayInfo &getArrayInfo()
	{
		return *(ArrayInfo*)mpBufferArrayAndInfo;
	}

	const ArrayInfo &getArrayInfo() const
	{
		return *(ArrayInfo*)mpBufferArrayAndInfo;
	}

	DataPosInfo getDataHeadInfo() const
	{
		return DataPosInfo(getArrayInfo().mHeadPosInfo);
	}

	DataPosInfo getDataTail() const
	{
		return DataPosInfo(getArrayInfo().mTailPosInfo);
	}

	BlockInfo* getBlock(uint arrayIndex)
	{
		return (BlockInfo*)(getBlockArrayBeginPtr() + arrayIndex);
	}

	BlockInfo* getBlockArrayBeginPtr()
	{
		return (BlockInfo*)((char*)mpBufferArrayAndInfo + sizeof(ArrayInfo));
	}

protected:
	// 块指针保存的列表, 前8个数据为当前数据的开头(4字节数组下标, 4字节偏移位置), 后8个字节为数据结束位置, 再4个字节为当前块数量, 再4个字节为已经分配的总长度
	void *mpBufferArrayAndInfo;

};

// 数据填充发送端
class BaseCommon_Export_H DynamicGrowLoopBuffer : public tDynamicLoopBuffer
{
public:
	DynamicGrowLoopBuffer(uint blockMaxNumber, uint  blockInitSize = 2048)
		: mMaxBlockNumber(blockMaxNumber)
		, mAllBufferSize(0)
		, mBlockSize(blockInitSize)
	{
	}

	virtual bool Init(int key) override
	{
		if (mpBufferArrayAndInfo == NULL)
		{
			size_t size = sizeof(ArrayInfo) + sizeof(BlockInfo)*(mMaxBlockNumber+1);
			mpBufferArrayAndInfo = ALLOCT_NEW(size);
			memset(mpBufferArrayAndInfo, 0, size);
			BlockInfo *pInfo = createBufferBlock(mBlockSize);
			BlockInfo *pInfo2 = createBufferBlock(mBlockSize);
			pInfo->mPreviousIndex = pInfo2->mArrayIndex;
			pInfo->mNextIndex = pInfo2->mArrayIndex;

			pInfo2->mPreviousIndex = pInfo->mArrayIndex;
			pInfo2->mNextIndex = pInfo->mArrayIndex;

			//BlockInfo *pInfo3 = createBufferBlock(blockInitSize);
			//pInfo3->Link(pInfo);
		}
		return true;
	}
	~DynamicGrowLoopBuffer()
	{
		if (mpBufferArrayAndInfo != NULL)
		{
			uint count = getArrayInfo().mArrayCount;
			for (uint i = 0; i < count; ++i)
			{
				BlockInfo *pInfo = getBlock(i);
				Allot::freePtr(GetBlockDataPtr(pInfo));
			}
			ALLOCT_FREE(mpBufferArrayAndInfo);
			mpBufferArrayAndInfo = NULL;
		}
	}

protected:
	virtual void* AllocMemory(uint size) {
		return ALLOCT_NEW(size);
	}

	virtual BlockInfo* createBufferBlock(uint size)
	{
		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eWriteLock);

		uint &nowBlockCount = getBlockCount();
		if (nowBlockCount >= mMaxBlockNumber)
		{
			ERROR_LOG("Now already max block count %u", nowBlockCount);
			return NULL;
		}
		void *ptr = AllocMemory(size);
		if (ptr == NULL)
		{
			ERROR_LOG("Alloc share memory fail");
			return NULL;
		}
		getArrayInfo().mTotalMemorySize += size;
		BlockInfo *pInfo = getBlock(nowBlockCount);
		SetBlockDataPtr(pInfo, (char*)ptr);
		pInfo->mSize = size;
		pInfo->mArrayIndex = nowBlockCount++;

		pInfo->mPreviousIndex = -1;
		pInfo->mNextIndex = -1;

		return pInfo;
	}

	uint& getBlockCount()
	{
		return getArrayInfo().mArrayCount;
	}

	// 只能最后一个块内跳, 可以跳的块的最后
	void skipDataTail(DSIZE size)
	{
		DataPosInfo posInfo = getDataTail();
		BlockInfo *tailBlock = getBlock(posInfo.mArrayIndex);
		if (posInfo.mPos + size <= tailBlock->mSize)
		{
			posInfo.mPos += size;
			setDataTail(posInfo.value);
		}
		else
			ERROR_LOG("Now tail data size %u < pos %u + size %u", tailBlock->mSize, posInfo.mPos, size);
	}

	// 得到数据尾部之后的连续的空间, 如果已经满了, 则判断下一个块是否存在数据, 存在, 则新增加块节点
	char* getTailSeriesSpace(DSIZE &size)
	{
		DataPosInfo posInfo = getDataTail();
		BlockInfo *tailBlock = getBlock(posInfo.mArrayIndex);
		if (posInfo.mPos < tailBlock->mSize)
		{
			size = (DSIZE)(tailBlock->mSize - posInfo.mPos);
			return (char*)GetBlockDataPtr(tailBlock) + posInfo.mPos;
		}

		DataPosInfo headPosInfo = getDataHeadInfo();

		// 尾已经是当前块的最后, 移动到下一个块, 偏移位置为0
		BlockInfo *pNextBlock = getBlock(tailBlock->mNextIndex);
		if (pNextBlock->mArrayIndex == headPosInfo.mArrayIndex)
		{
			BlockInfo *pNew = createBufferBlock(mBlockSize);
			if (pNew == NULL)
				return NULL;
			pNew->Link(tailBlock, this);
			posInfo.mArrayIndex = pNew->mArrayIndex;
			posInfo.mPos = 0;
			setDataTail(posInfo.value);
			size = (DSIZE)pNew->mSize;
			return GetBlockDataPtr(pNew);
		}
		else
		{
			posInfo.mArrayIndex = pNextBlock->mArrayIndex;
			posInfo.mPos = 0;
			setDataTail(posInfo.value);

			size = (DSIZE)pNextBlock->mSize;
			return GetBlockDataPtr(pNextBlock);
		}
	}

	void setDataTail(UInt64 info)
	{
		getArrayInfo().mTailPosInfo = info;
	}

public:
	// 直接填充
	virtual bool _write(void *scrData, DSIZE dataSize) override
	{
		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eWriteLock);

		char *pData = (char*)scrData;
		while (true)
		{
			DSIZE freeSize = 0;
			void *pPtr = getTailSeriesSpace(freeSize);
			if (pPtr == NULL)
			{
				LOG("Write fail : Now buffer full and max block");
				return false;
			}
			if (freeSize >= dataSize)
			{
				memcpy(pPtr, pData, dataSize);
				skipDataTail(dataSize);
				return true;
			}
			memcpy(pPtr, pData, freeSize);
			skipDataTail(freeSize);

			pData += freeSize;
			dataSize -= freeSize;
		}

	}

	virtual bool _read(void *destData, DSIZE readSize)
	{
		ERROR_LOG("DynamicGrowLoopBuffer can not read");
		return false;
	}

	virtual bool peek(void *dest, DSIZE len)
	{
		ERROR_LOG("DynamicGrowLoopBuffer can not peek");
		return false;
	}

	virtual bool	skip(DSIZE len)
	{
		ERROR_LOG("DynamicGrowLoopBuffer can not skip");
		return false;
	}

	virtual void clear(bool bFreeBuffer = false) override
	{
		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eWriteLock);

		DataPosInfo headInfo = getDataHeadInfo();
		setDataTail(headInfo.value);
	}

	void UpdateDebugCount(UInt64 debugCount)
	{
		getArrayInfo().mDebugCount = debugCount;
	}

	UInt64 GetDebugCount() const
	{
		return getArrayInfo().mDebugCount;
	}

protected:
	uint mMaxBlockNumber;
	uint mAllBufferSize;
	uint mBlockSize;
};

//-------------------------------------------------------------------------
// 数据消费端
class BaseCommon_Export_H DigestDynamicLoopBuffer : public tDynamicLoopBuffer
{
public:
	DigestDynamicLoopBuffer(void *pArrayInfo)
	{
		mpBufferArrayAndInfo = pArrayInfo;
	}

	virtual bool Init(int key) override { return false;  }

public:
	void skipDataHead(uint size)
	{
		DataPosInfo headInfo = getDataHeadInfo();
		BlockInfo *pHeadBlock = getBlock(headInfo.mArrayIndex);
		if (headInfo.mPos + size <= pHeadBlock->mSize)
		{
			headInfo.mPos += size;
			setHeadInfo(headInfo.value);
		}
		else
			ERROR_LOG("Now head pos %u + size %u < all size %u", headInfo.mPos, size, pHeadBlock->mSize);
	}

	void setHeadInfo(UInt64 posInfo)
	{
		getArrayInfo().mHeadPosInfo = posInfo;
	}

	void* getHeadSeriesData(DSIZE &size)
	{
		DataPosInfo headInfo = getDataHeadInfo();
		DataPosInfo tailInfo = getDataTail();
		BlockInfo *pHeadBlock = getBlock(headInfo.mArrayIndex);
		if (headInfo.mArrayIndex == tailInfo.mArrayIndex)
		{
			size = (DSIZE)(tailInfo.mPos - headInfo.mPos);
			return (char*)GetBlockDataPtr(pHeadBlock) + headInfo.mPos;
		}
		else
		{
			size = (DSIZE)(pHeadBlock->mSize - headInfo.mPos);
			if (size <= 0)
			{
				// 检测下一个
				headInfo.mArrayIndex = pHeadBlock->mNextIndex;
				headInfo.mPos = 0;
				setHeadInfo(headInfo.value);
				return getHeadSeriesData(size);
			}
			return (char*)GetBlockDataPtr(pHeadBlock) + headInfo.mPos;
		}
	}

	virtual bool _read(void *destData, DSIZE readSize) override
	{
		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eReadLock);

		char *pData = (char*)destData;
		while (true)
		{
			DSIZE nowSize = 0;
			void *pDataPtr = getHeadSeriesData(nowSize);
			if (nowSize > 0)
			{
				if (nowSize >= readSize)
				{
					memcpy(pData, pDataPtr, readSize);
					skipDataHead(readSize);
					return true;
				}
				memcpy(pData, pDataPtr, nowSize);
				skipDataHead(nowSize);

				pData += nowSize;
				readSize -= nowSize;
			}
			else
				return false;
		}
	}

	virtual bool _write(void *destData, DSIZE readSize)
	{
		ERROR_LOG("DigestDynamicLoopBuffer can not _write");
		return false;
	}

	virtual bool peek(void *dest, DSIZE len) override
	{
		if (len < 0)
			return false;
		else if (len == 0)
			return true;

		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eReadLock);

		DataPosInfo headInfo = getDataHeadInfo();
		bool b = _read(dest, (uint)len);
		setHeadInfo(headInfo.value);
		return b;
	}

	virtual bool	skip(DSIZE len) override
	{
		if (len < 0)
			return false;
		else if (len == 0)
			return true;

		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eReadLock);

		DSIZE readSize = len;
		while (true)
		{
			DSIZE nowSize = 0;
			void *pDataPtr = getHeadSeriesData(nowSize);
			if (nowSize > 0)
			{
				if (nowSize >= readSize)
				{
					skipDataHead(readSize);
					return true;
				}
				skipDataHead(nowSize);

				readSize -= nowSize;
			}
			else
				return false;
		}
	}

	virtual void clear(bool bFreeBuffer = false) override
	{
		MemoryLock  memLock(&(getArrayInfo().mLock), eWaitLock, eReadLock);

		DataPosInfo tailInfo = getDataTail();
		setHeadInfo(tailInfo.value);
	}

	UInt64 GetDebugCount() const
	{
		return getArrayInfo().mDebugCount;
	}
};
//-------------------------------------------------------------------------
// 共享内存的环形动态缓存, 生产发送端
//-------------------------------------------------------------------------
class BaseCommon_Export_H ShareDynamicLoop : public DynamicGrowLoopBuffer
{
protected:
	AString		mKey;
	ShareMemAO				mArrayInfoShareMem;
	Array<ShareMemAO> mBlockShareMemList;
	MtLock		*mpOnlyOneLock;
	bool mbInitSucceed;

public:
	ShareDynamicLoop(uint blockMaxNumber, uint  blockInitSize = 2048)
		: DynamicGrowLoopBuffer(blockMaxNumber,  blockInitSize)
		, mpOnlyOneLock(NULL)
		, mbInitSucceed(false)
	{ 
	}
	~ShareDynamicLoop()
	{
		mpBufferArrayAndInfo = NULL;
		if (mpOnlyOneLock != NULL)
		{
			if (mbInitSucceed)
				mpOnlyOneLock->unlock();
			delete mpOnlyOneLock;
			mpOnlyOneLock = NULL;
		}
	}

public:
	// 通知消化端共享进程拿住共享内存, 避免未及时处理获取, 生产端退出, 共享内存被释放BUG
	//virtual void OnNotifyCreateShareMemory(int key) = 0;

public:
	void Close()
	{
		if (mpOnlyOneLock != NULL)
		{ 
			if (mbInitSucceed)
				mpOnlyOneLock->unlock();
			delete mpOnlyOneLock;
			mpOnlyOneLock = NULL;
		}
	}

	virtual bool Init(int shareKey) override
	{
		if (mpOnlyOneLock == NULL)
		{
			AString info;
			info.Format("_ShareDynamicLoopBuffer_%d", shareKey);
			mpOnlyOneLock = new MtLock(info.c_str());
			if (!mpOnlyOneLock->trylock())
			{
				ERROR_LOG("Key %d Send loop share memory buffer must only one in system! Please close other try agin", shareKey);
				return false;
			}
			mbInitSucceed = true;
		}
		else if (TOINT(mKey.c_str()) != shareKey)
		{
			ERROR_LOG("Init repeat %d, now key  %s", shareKey, mKey.c_str());
			return false;
		}

		mKey.setNull();
		mKey += shareKey;
		UInt64 _allocSize = sizeof(ArrayInfo) + sizeof(BlockInfo)*(mMaxBlockNumber + 1);
		uint size = (uint)_allocSize;
		if (_allocSize != size)
		{
			uint z = -1;
			ERROR_LOG("MaxBlockNumber too big, now %u > %u", mMaxBlockNumber, (z - sizeof(ArrayInfo)) / sizeof(BlockInfo));
			return false;
		}
		if (mArrayInfoShareMem.Create(shareKey, (uint)size) == FALSE)
		{
			if (mArrayInfoShareMem.Attach(shareKey, size) == TRUE)
			{
				NOTE_LOG("** Init attach share memory key %d", shareKey);

				uint uSize = 0;
				mpBufferArrayAndInfo = mArrayInfoShareMem.GetDataPtr();
				uint count = getArrayInfo().mArrayCount;
				for (uint i = 0; i < count; ++i)
				{
					BlockInfo *pBlock = getBlock(i);
					ShareMemAO mem;
					AString tempKey;
					tempKey.Format("%s_%d", mKey.c_str(), pBlock->mArrayIndex);
					int key = MAKE_INDEX_ID(tempKey.c_str());
					if (mem.Attach(key, uSize) == FALSE)
					{
						ERROR_LOG("Create share memory fail > %s : %d", tempKey.c_str(), key);
						return false;
					}
					else
						NOTE_LOG("Init attach share mem > %s : %d", tempKey.c_str(), key);
					void *ptr = mem.GetDataPtr();
					mBlockShareMemList.push_back(mem);
					//!!! NOTE: 千万不要修改设置接收端的数据指针
					SetBlockDataPtr( pBlock, ptr);
				}
				return true;
			}
			else
			{
				ERROR_LOG("Create share memory fail, may be already key %d", shareKey);
				return false;
			}
		}
		else
		{
			NOTE_LOG("** Init create share memory key %d", shareKey);
		}
		mpBufferArrayAndInfo = mArrayInfoShareMem.GetDataPtr();
		memset(mpBufferArrayAndInfo, 0, size);
		BlockInfo *pInfo = createBufferBlock(mBlockSize);
		BlockInfo *pInfo2 = createBufferBlock(mBlockSize);
		pInfo->mPreviousIndex = pInfo2->mArrayIndex;
		pInfo->mNextIndex = pInfo2->mArrayIndex;

		pInfo2->mPreviousIndex = pInfo->mArrayIndex;
		pInfo2->mNextIndex = pInfo->mArrayIndex;

		//BlockInfo *pInfo3 = createBufferBlock(blockInitSize);
		//pInfo3->Link(pInfo);			
		return true;
	}

	// NOTE: DB端发送端使用时, 可覆盖此函数, 异步请求在DB共享进程端创建, 然后附加, 这样, 即解决发送端异常释放太快, 接收端数据出错问题
	virtual void* AllocMemory(uint size) override 
	{
		ShareMemAO mem;
		AString tempKey;
		tempKey.Format("%s_%d", mKey.c_str(), getArrayInfo().mArrayCount);		
		int key = MAKE_INDEX_ID(tempKey.c_str());
		if (mem.Create(key, size) == FALSE)
		{
			ERROR_LOG("Create share memory fail > %s : %d", tempKey.c_str(), key);
			return NULL;
		}
#if DEVELOP_MODE
		else
			NOTE_LOG("New share meme alloc >%s : %d", tempKey.c_str(), key);
#endif
		void *ptr = mem.GetDataPtr();
		mBlockShareMemList.push_back(mem);
		////NOTE: 已经在接收线程,优化检查获取新增缓存, 不里不再需要通知  -- 通知消化端共享进程拿住共享内存, 避免未及时处理获取, 生产端退出, 共享内存被释放BUG
		//OnNotifyCreateShareMemory(key);
		return ptr;
	}

	
};

// 共享内存动态增加环形缓存, 消化端
class ReceiveShareMemDynamicLoop : public DigestDynamicLoopBuffer
{
private:
	AString		mKey;
	ShareMemAO				mArrayInfoShareMem;
	Array<ShareMemAO> mBlockShareMemList;

public:
	ReceiveShareMemDynamicLoop()
		:DigestDynamicLoopBuffer(NULL)
	{

	}
	~ReceiveShareMemDynamicLoop()
	{
		mpBufferArrayAndInfo = NULL;

		AString info;
		for (int i=0; i<mBlockShareMemList.size(); ++i)
		{
			if (i!=0)
				info += " , ";
			info += STRING((int)mBlockShareMemList[i].GetKey());
		}
		NOTE_LOG("*** %d Now free share memory ------------------- \r\n %s\r\n---------------------------------", mKey.c_str(), info.c_str());
	}

	virtual bool Init(int sharekey) override
	{
		mKey.setNull();
		mKey += sharekey;
		UINT uSize = 0;
		if (mArrayInfoShareMem.Attach(sharekey, uSize) == FALSE)
		{
			ERROR_LOG("Share mem no exist %d", sharekey);
			return false;
		}

		mpBufferArrayAndInfo = mArrayInfoShareMem.GetDataPtr();
		uint count = getArrayInfo().mArrayCount;
		for (uint i=0; i<count; ++i)
		{
			BlockInfo *pBlock = getBlock(i);
			ShareMemAO mem;
			AString tempKey;
			tempKey.Format("%s_%d", mKey.c_str(), pBlock->mArrayIndex);
			int key = MAKE_INDEX_ID(tempKey.c_str());
			if (mem.Attach(key, uSize) == FALSE)
			{
				ERROR_LOG("Create share memory fail > %s : %d", tempKey.c_str(), key);
				return false; 
			}
			else
				NOTE_LOG("Append attach share memory %s : %d", tempKey.c_str(), key);
			void *ptr = mem.GetDataPtr();
			mBlockShareMemList.push_back(mem);
			pBlock->mReceivePtr = (char*)ptr;
		}

		return true;
	}

	virtual char* GetBlockDataPtr(BlockInfo *pBlock)
	{
		// 由于发送端是动态增加的, 接收进程内还未获取数据块指针
		if (pBlock->mReceivePtr == NULL)
		{
			ShareMemAO mem;
			AString tempKey;
			tempKey.Format("%s_%d", mKey.c_str(), pBlock->mArrayIndex);
			uint uSize = 0;
			int key = MAKE_INDEX_ID(tempKey.c_str());
			if (mem.Attach(key, uSize) == TRUE)
			{
				pBlock->mReceivePtr = mem.GetDataPtr();
				mBlockShareMemList.push_back(mem);
				NOTE_LOG("Append attach mem %s : %d", tempKey.c_str(), key);
			}
			else
			{
				ERROR_LOG("GetBlockDataPtr > Attach share memory fail > %s : %d", tempKey.c_str(), key);
#if __LINUX__
				throw(1);
#else
				throw(std::exception("Create share memory fail"));
#endif
			}
		}

		return pBlock->mReceivePtr;
	}

	virtual void SetBlockDataPtr(BlockInfo *pBlock, void *ptr) override
	{
		//pBlock->SetDataPtr(pBlock);
		AssertEx(0, "No call this function");
	}

	// 消化线程Process, 主要快速拿住新建的缓存共享块
	void Process()
	{
		if (getArrayInfo().mArrayCount != mBlockShareMemList.size())
		{
			LOG("**** Check attack all ...")
			uint count = getArrayInfo().mArrayCount;
			for (uint i = 0; i < count; ++i)
			{
				BlockInfo *pBlock = getBlock(i);
				GetBlockDataPtr(pBlock);
			}
			LOG("**** Check finish");
		}
	}
};

#endif //_INCLUDE_DYNAMICLOOPBUFFER_H_