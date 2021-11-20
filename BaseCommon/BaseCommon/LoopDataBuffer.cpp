#include "LoopDataBuffer.h"

DSIZE LoopData::dataSize(void) const
{
	if (mpBuffer == NULL)
		return 0;
	int head = Head();
	int tail = Tail();
	if (head < tail)
		return tail - head;

	else if (head > tail)
		return size() - head + tail;

	return 0;
}

bool LoopData::SendData(char *szSendData, DSIZE sendSize)
{
	DataStream &waitData = GetWaitSendBuffer();
	if (waitData.dataSize() > 0)
		ProcessSendData();

	if (waitData.dataSize() <= 0)
	{
		DSIZE canSend = lastSize();
		if (sendSize < canSend)
			canSend = sendSize;
		else if (sendSize>canSend)
			waitData._write(szSendData + canSend, sendSize - canSend);
		if (!_write(szSendData, canSend))
		{
			ERROR_LOG("Code error, write loop data fail");
			return false;
		}
	}
	else
		waitData._write(szSendData, sendSize);
	return true;
}

bool LoopData::ProcessSendData()
{
	DataStream &waitData = GetWaitSendBuffer();
	if (waitData.dataSize() > 0)
	{
		bool bHaveSend = false;
		do
		{
			DSIZE canSend = lastSize();
			if (canSend <= 0)
				break;

			DSIZE sendSize = 0;
			char *szData = waitData.GetDataPtr(sendSize);

			if (sendSize < canSend)
				canSend = sendSize;

			if (!_write(szData, canSend))
				ERROR_LOG("Code error, write loop data fail");

			waitData.skip(canSend);
			//waitData._ForceRestoreHead(waitData.GetHead() + canSend);
			bHaveSend = true;

		} while (waitData.dataSize() > 0);
		return bHaveSend;
	}
	return false;
}

DSIZE LoopData::peekReadSize(int nBeforeHead)
{
	int head = nBeforeHead;
	int tail = Head();
	if (head < tail)
		return tail - head;

	else if (head > tail)
		return size() - head + tail;

	return 0;
}

DSIZE LoopData::peekWriteSize(int nBeforeTail)
{
	int head = nBeforeTail;
	int tail = Tail();
	if (head < tail)
		return tail - head;

	else if (head > tail)
		return size() - head + tail;

	return 0;
}

bool LoopData::_write(void *scr, DSIZE len)
{
	__ENTER_FUNCTION_FOXNET
		const char *buf = (const char*)scr;
	int head = Head();
	int tail = Tail();
	//					//
	//     T  H			//    H   T			LEN=10
	// 0123456789		// 0123456789
	// abcd...efg		// ...abcd...
	//					//

	int nFree = ((head <= tail) ? (mBufferLen - tail + head - 1) : (head - tail - 1));

	if (len > nFree)
	{
		//if (!Resize(len - nFree + 1))
		//    return 0;
		ERROR_LOG("Loop free size too small %d < write size %d", nFree, len);
		return false;
	}

	if (head <= tail)
	{
		//if( head==0 ) 
		//{
		//	nFree = mBufferLen - tail - 1;
		//	memcpy( &mpBuffer[tail], buf, len ) ;
		//} 
		//else 
		{
			nFree = mBufferLen - tail;
			if (len <= nFree)
			{
				memcpy(mpBuffer + tail, buf, len);
			}
			else
			{
				memcpy(mpBuffer + tail, buf, nFree);
				memcpy(mpBuffer, buf + nFree, len - nFree);
			}
		}
	}
	else
	{
		memcpy(&mpBuffer[tail], buf, len);
	}

	_setTail((tail + len) % mBufferLen);

	return true;

	__LEAVE_FUNCTION_FOXNET

		return 0;
}

char* LoopData::GetFreeSpacePtr(int &nSize)
{
	int head = Head();
	int tail = Tail();
	if (head <= tail)
	{
		nSize = mBufferLen - tail;

		if (head <= 0)
		{
			// NOTE: 只要是头为0, 则后面剩余空间就需要减1, 这个烂BUG, 测试很久才解决
			--nSize;
		}
	}
	else
		nSize = head - tail - 1;

	return mpBuffer + tail;
}

bool LoopData::_read(void *dest, DSIZE len)
{
	__ENTER_FUNCTION_FOXNET
		char *buf = (char *)dest;
	int head = Head();
	int tail = Tail();

	if (buf == NULL || len == 0)
		return 0;

	if (len > size())
		return 0;

	if (head < tail)
	{
		memcpy(buf, &mpBuffer[head], len);
	}
	else
	{
		int rightLen = mBufferLen - head;
		if (len <= rightLen)
		{
			memcpy(buf, &mpBuffer[head], len);
		}
		else
		{
			memcpy(buf, &mpBuffer[head], rightLen);
			memcpy(&buf[rightLen], mpBuffer, len - rightLen);
		}
	}

	_setHead((head + len) % mBufferLen);

	return len;

	__LEAVE_FUNCTION_FOXNET

		return 0;
}

bool LoopData::peek(void *dest, DSIZE len)
{
	__ENTER_FUNCTION_FOXNET
		char *buf = (char *)dest;
	int head = Head();
	int tail = Tail();

	if (buf == NULL || len == 0)
		return 0;

	if (len > dataSize())
		return 0;

	if (head < tail)
	{
		memcpy(buf, &mpBuffer[head], len);
	}
	else
	{
		int rightLen = mBufferLen - head;
		if (len <= rightLen)
		{
			memcpy(buf, &mpBuffer[head], len);
		}
		else
		{
			memcpy(buf, &mpBuffer[head], rightLen);
			memcpy(&buf[rightLen], mpBuffer, len - rightLen);
		}
	}

	return len;

	__LEAVE_FUNCTION_FOXNET

		return 0;
}

bool LoopData::skip(DSIZE len)
{
	__ENTER_FUNCTION_FOXNET
		int head = Head();

	if (len == 0)
		return FALSE;

	if (len > dataSize())
		return FALSE;

	_setHead((head + len) % mBufferLen);

	return TRUE;

	__LEAVE_FUNCTION_FOXNET

		return FALSE;
}

