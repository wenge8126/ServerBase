#pragma once

#ifndef _INCLUDE_SHAREMEMMSG_H_
#define _INCLUDE_SHAREMEMMSG_H_

#include "ShareMemAO.h"
#include "NetHead.h"
#include "Hand.h"
#include "DataStream.h"
#include "LoopDataStream.h"
#include "LoopDataBuffer.h"
#include "TimeManager.h"

#if __LINUX__
#include <thread>
struct mt {
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
};
#endif
//-------------------------------------------------------------------------
// 共享内存互相传数据时, 保存不能及时写入到转换缓存的空间, 使用共享内存的目的可以保证发送方DUMP后,数据安全
//???  对方还不能得到数据开始及结尾, 尚不可获取
// 可以继承 LoopData, 实现可增加空间功能, 重写 _write _AllotBuffer
class ShareMemMsgHander;
class WillSendTempBuffer : public LoopData
{
public:
	WillSendTempBuffer(int nameKey, UINT BufferSize = DEFAULTSOCKETINPUTBUFFERSIZE, UINT MaxBufferSize = DISCONNECTSOCKETINPUTSIZE)
		: LoopData(0, 0)
		, mpShareKeySpace(NULL)
		, mpMsgHander(NULL)
		, mMaxLength(MaxBufferSize)
		, mNameKey(nameKey)
	{
	}

	void InitMsgHander(ShareMemMsgHander *pMsgHander) { mpMsgHander = pMsgHander; }
	void SetShareKeySpacePtr(int *pSpacePtr) { mpShareKeySpace = pSpacePtr; }


	virtual bool _write(void *scr, DSIZE len) override;

	virtual bool resize(DSIZE destSize, char fullValue = 0) override { return Resize(destSize-mBufferLen)==TRUE; }

	BOOL Resize(INT size);

	void SetNameKey(int nameKey) { mNameKey = nameKey; }

protected:
	virtual ShareMemAO _AllotBuffer(UINT size);

protected:
	ShareMemMsgHander	*mpMsgHander;
	int									*mpShareKeySpace;		// 用于保存分配共享空间的KEY
	UINT								mMaxLength;
	int									mNameKey;
	ShareMemAO					mShareMem;
};

//-------------------------------------------------------------------------
// 检查恢复, 用于检查共享内存中待发送,未完全转移发送的数据
class RestoreWillSendTempBuffer : public LoopData
{
public:
	RestoreWillSendTempBuffer()
		: LoopData(0, 0)
	{
	}

	// 使用此恢复成功之后, 即可读取待发送数据
	bool Restore(int key)
	{
		UINT size = 0;
		if (mShareMem.Attach(key, size) == TRUE)
		{
			mpBuffer = mShareMem.GetDataPtr()+sizeof(int)*2;
			mBufferLen = size- sizeof(int) * 2;
			mpHead = (int*)mShareMem.GetDataPtr();
			mpTail = mpHead + 1;
			return true;
		}
		return false;
	}

protected:
	ShareMemAO	mShareMem;

};
//-------------------------------------------------------------------------*
//-------------------------------------------------------------------------*
class ShareMemLoopData :  public LoopData
{
public:
	WillSendTempBuffer	mShareWillSendBuffer;

public:
	ShareMemLoopData()
		:mShareWillSendBuffer(0)
	{
	}

	void SetNameKey(int nameKey) { mShareWillSendBuffer.SetNameKey(nameKey);  }

protected:
	virtual DataStream& GetWaitSendBuffer() override { return mShareWillSendBuffer; }
};
//-------------------------------------------------------------------------
// 发送和接收 Buffer
class ShareMemMsgBuffer : public MemBase
{
    friend class ShareMemMsgHander;
    friend class ShareMemSQL;

public:
    // bCreate create share memory, other attach share memory
    // Set bCreate true for send buffer
    ShareMemMsgBuffer()
		: mbServerCreate(false)
#if __WINDOWS__
        , mpClientLock(NULL)       
#endif
        , mSize(0)
    {
    }

    ~ShareMemMsgBuffer()
    {
#if __LINUX__
        if (mShareMem.GetDataPtr() != NULL)
        {
            if (mbServerCreate)
            {
                pthread_mutex_destroy( &getMutex() );
            }
            else
            {
                // Client close unlock
                pthread_mutex_unlock( &getMutex() );
            }
        }
#else
		if (mpClientLock != NULL)
			delete mpClientLock;
		mpClientLock = NULL;
#endif

		mShareMem.Destory();
    }
    
    bool Init(SM_KEY key, DSIZE &size, bool bCreate);
    void _setBuffer(LoopData &leftBuffer, LoopData &rightBuffer, DSIZE size);

#if __LINUX__
	pthread_mutex_t& getMutex() {
		return ( (mt*)(mShareMem.GetDataPtr() + mSize) )->mutex;
	}
#endif

	// 检测是否存在对端连接, 用于当接收完成所有数据后, 检查待发送缓存中的数据
	bool ExistConnect();

	bool IsServerCreate() const { return mbServerCreate; }

protected:
	ShareMemAO				mShareMem;
	ShareMemLoopData		mSendBuffer;
	LoopData						mReceiveBuffer;
#if __WINDOWS__
	MtLock							*mpClientLock;      // 用于只保证一个接收角色的对端
#endif
	bool								mbServerCreate;
	DSIZE							mSize;
};
//-------------------------------------------------------------------------
class Net_Export ShareMemMsgHander : public Base<ShareMemMsgHander>
{
protected:
	ShareMemMsgBuffer mShareMemMsgBuffer;
	bool    mbInitOK;
	UInt64	mCheckOtherSendTime;		// 全部接收完成后的时间

public:
	ShareMemMsgHander()
		: mbInitOK(false)
	{
		mShareMemMsgBuffer.mSendBuffer.mShareWillSendBuffer.InitMsgHander(this);
		mCheckOtherSendTime = TimeManager::NowTick();
	}

	bool IsOK() const { return mbInitOK; }

	bool ExistConnect() { return mShareMemMsgBuffer.ExistConnect(); }

	int GetWaitSendShareMemKey(int &createWaitSendKey);

	virtual void OnWaitSendShareBufferChanded(int sendShareKey)
	{
		ERROR_LOG("No write OnWaitSendShareBufferChanded");
	}

public:
	virtual bool Init(SM_KEY memKey, DSIZE &bufSize, bool bCreateShareMem)
	{
		mbInitOK = mShareMemMsgBuffer.Init(memKey, bufSize, bCreateShareMem);
		if (!mbInitOK)
		{
			ERROR_LOG("Init share memory fail");
		}
		
		return mbInitOK;
	}

	//bool Send(const char* szSendData, DSIZE  sendSize)
	//{
	//    if (mbInitOK && mShareMemMsgBuffer.mSendBuffer._write((void*)szSendData, sendSize))
	//    {
	//        *mShareMemMsgBuffer.mSendBuffer.mbExistData = true;
	//        return true;
	//    }
	//    return false;
	//}

	bool HasReceiveData() const
	{
		return mShareMemMsgBuffer.mReceiveBuffer.dataSize() > 0;
	}

	// 创建端的 发送缓存即时发送缓存, 其他端 因为对方的发送必须为本端的接收, 所以对端缓存为互换
	LoopData& GetSendBuffer() { return mShareMemMsgBuffer.mSendBuffer; }
	LoopData& GetReceiveBuffer() { return mShareMemMsgBuffer.mReceiveBuffer; }

	virtual void Process()
	{
		mShareMemMsgBuffer.mSendBuffer.ProcessSendData();
		if (mbInitOK)
		{
			if (mShareMemMsgBuffer.mReceiveBuffer.dataSize() > 0)
			{
				PrcessReceiveData(mShareMemMsgBuffer.mReceiveBuffer);
				// 只在创建端, 比如SQL落地端进行检测接收对方待发送的数据, 达到数据安全完整
				if (mShareMemMsgBuffer.mbServerCreate)
				{
					mCheckOtherSendTime = TimeManager::NowTick();
				}
			}
			else if (mShareMemMsgBuffer.mbServerCreate)
			{
				if (TimeManager::NowTick() - mCheckOtherSendTime > 5000)
				{
					///??? 测试与DB进程连接
     //               DEBUG_LOG("Check once DB exist");
					//mCheckOtherSendTime = TimeManager::NowTick();
					//if (!mShareMemMsgBuffer.ExistConnect())
					//{
     //                   DEBUG_LOG("DB no exist, then process DB wait Send");
					//	OnRestoreOtherWillSendData();
					//}
				}
			}			
		}
    }

	virtual bool SendData(DataStream *pMsgData)
	{
		if (IsOK())
			return GetSendBuffer().SendData((char*)pMsgData->data(), pMsgData->dataSize());

		return false;
	}

    virtual void PrcessReceiveData(LoopData &loopData) { }

	virtual void OnRestoreOtherWillSendData() {}
};



#endif //_INCLUDE_SHAREMEMMSG_H_