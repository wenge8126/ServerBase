#pragma once

#ifndef _INCLUDE_LOOPDATA_H
#define _INCLUDE_LOOPDATA_H

#include "BaseCommon.h"
#include "LoopDataStream.h"
//-------------------------------------------------------------------------
// 异步交换循环缓存, 具有临时池, 如果发送缓存满, 则先写临时池
// 发送必须统一使用 SendData
//-------------------------------------------------------------------------
class BaseCommon_Export LoopData : public tLoopStream
{
public:
	char                  *mpBuffer;
	DSIZE                mBufferLen;
	int                     *mpHead;
	int                     *mpTail;
	bool                    *mbExistData;
	LoopDataStream          mWaitSendBuffer;        // Temp save will send data when send buffer full

public:
	LoopData(DSIZE willBufferSize = 4 * 1024, DSIZE willBufferMaxSize = 64 * 1024)
		: mpBuffer(NULL)
		, mBufferLen(0)
		, mpHead(NULL)
		, mpTail(NULL)
		, mbExistData(NULL)
		, mWaitSendBuffer(willBufferSize, willBufferMaxSize)
	{}

	// NOTE: Need call ProcessSendData in send thread
	virtual bool ProcessSendData();
	virtual DataStream& GetWaitSendBuffer() {	return mWaitSendBuffer;  }

public:
	virtual DSIZE tell(void) const override { ERROR_LOG("Can not call"); AssertEx(0, "LoopData can not use tell()"); return 0; }
	virtual bool seek(DSIZE absolutePosition) override { ERROR_LOG("Can not call"); AssertEx(0, "LoopData can not use seek()"); return false; }
	virtual bool setDataSize(DSIZE dataSize) override { ERROR_LOG("Can not call"); AssertEx(0, "LoopData can not use setDataSize()"); return false; }

	virtual DSIZE size(void) const override { return mBufferLen; }
	virtual DSIZE dataSize(void) const override;
	virtual char* data(void) override { return mpBuffer; }
	virtual bool resize(DSIZE destSize, char fullValue = 0) override { ERROR_LOG("Can not call"); AssertEx(0, "LoopData can not use resize()"); return false; }

	virtual int Head() const { return *mpHead; }
	virtual int Tail() const { return *mpTail; }
	virtual void _setTail(int tail) { *mpTail = tail; }
	virtual void _setHead(int head) { *mpHead = head; }

public:
	virtual bool SendData(char *szSendData, DSIZE sendSize);
	virtual DSIZE peekReadSize(int nBeforeHead);
	virtual DSIZE peekWriteSize(int nBeforeTail);	

	virtual char* GetDataPtr(int &nSize) override
	{
		int head = Head();
		int tail = Tail();
		if (head <= tail)
			nSize = tail - head;
		else
			nSize = mBufferLen - head;

		return mpBuffer + head;
	}
	// Get free space
	char*		GetFreeSpacePtr(int &nSize);

	//!!! 不再恢复已经写的数据 特别是后台无锁读取时, 改变此值特别有可能会使头数据在尾之后, 造成无效数据
	//void			_ForceRestoreTail(DSIZE formerTail) { _setTail(formerTail % mBufferLen); }

public:
	virtual bool _read(void *dest, DSIZE len) override;
	virtual DSIZE lastSize() override { return mBufferLen - dataSize() - 1; }
	virtual DSIZE lastDataSize(void) override { return dataSize(); }
	virtual bool peek(void *dest, DSIZE len);
	virtual bool skip(DSIZE len) override;
	virtual void clear(bool) override
	{
		_setHead(Tail());
	}

	virtual UINT	Write(const CHAR* buf, UINT len) override { if (_write((void*)buf, (DSIZE)len)) return len; return 0; }
	virtual UINT	Read(CHAR* buf, UINT len) override { if (_read(buf, (DSIZE)len)) return len; return 0; }

protected:
	virtual bool _write(void *scr, DSIZE len) override;
};

class BaseCommon_Export_H ThreadLoopData : public LoopData
{
public:
	ThreadLoopData(DSIZE dataBufferSize, DSIZE willBufferSize=4*1024, DSIZE willBufferMaxSize = 64*1024)
		: LoopData(willBufferSize, willBufferMaxSize)
	{
		mpBuffer = new char[dataBufferSize + sizeof(DSIZE) * 2 + 1];
		memset(mpBuffer, 0, dataBufferSize + sizeof(DSIZE) * 2 + 1);
		mBufferLen = dataBufferSize;

		mpHead = (int*)(mpBuffer + dataBufferSize);
		mpTail = mpHead + 1;
		mbExistData = (bool*)(mpTail + 1);
	}
	~ThreadLoopData()
	{
		delete[]  mpBuffer;
	}
	//virtual void _setTail(int tail) override
	//{
	//	*mpTail = tail; 
	//	NOTE_LOG("++++ H %d, T %d", Head(), Tail());
	//}
	//virtual void _setHead(int head) override 
	//{ 
	//	*mpHead = head; 
	//	NOTE_LOG("---- H %d, T %d", Head(), Tail());
	//}
};

#endif //_INCLUDE_LOOPDATA_H