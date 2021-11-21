
#include "ShareMemMsg.h"
#include "TimeManager.h"

//-------------------------------------------------------------------------
ShareMemAO WillSendTempBuffer::_AllotBuffer(UINT size)
{
	ShareMemAO mem;
	AString name;
	static thread_local int 	gsCode = 0;

	bool bOk = false;
	for (int i = 0; i < 2; ++i)
	{
		name.Format("__WillSendBuffer_%d_%d", mNameKey, ++gsCode);

		int keyID = MAKE_INDEX_ID(name.c_str());

		if (mem.Create(keyID, size) == TRUE)
		{
			bOk = true;
			memset(mem.GetDataPtr(), 0, size);
			DEBUG_LOG("Succeed create share memory for wait send buffer, key = %d", keyID);
			if (mpShareKeySpace != NULL)
				*mpShareKeySpace = keyID;
			else
				ERROR_LOG("Save key share space ptr is null");
			//m_Buffer = (CHAR*)mShareMem.GetDataPtr();	
			//AssertEx(m_Buffer != NULL, "Allot buffer share memory fail");
			if (mpMsgHander != NULL)
				mpMsgHander->OnWaitSendShareBufferChanded(keyID);
			break; 
		}
	}
	if (!bOk)
	{
		ERROR_LOG("AllotBuffer create share memory fail ");
	}
	return mem;
}

bool WillSendTempBuffer::_write(void *scr, DSIZE len)
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
		if (!Resize(len - nFree + 1))
		{
			//    return 0;
			ERROR_LOG("Loop free size too small %d < write size %d", nFree, len);
			return false;
		}
        head = Head();
        tail = Tail();
        nFree = ((head <= tail) ? (mBufferLen - tail + head - 1) : (head - tail - 1));
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

BOOL WillSendTempBuffer::Resize(INT size)
{
	__ENTER_FUNCTION_FOXNET

		Assert(size != 0);
#ifdef __LINUX__
	size = std::max(size, (int)(mBufferLen >> 1));
#else
	size = MAX(size, (int)(mBufferLen >> 1));
#endif

	UINT newBufferLen = mBufferLen + size;

	if (newBufferLen > mMaxLength)
	{
		ERROR_LOG("Resize %u more then max %u", newBufferLen, mMaxLength);
		return FALSE;
	}

	UINT len = 0;

	if (mpBuffer != NULL)
		len = dataSize();

	if (size < 0)
	{
		if (newBufferLen < 0 || newBufferLen < len)
		{
			AssertEx(0, "��������仺��ֵ����С����򶼲���С�ڵ�ǰ�Ѿ����ڵ����ݴ�С");
			return FALSE;
		}
	}
	ShareMemAO newMem = _AllotBuffer(newBufferLen + sizeof(int) * 2);
	if (newMem.GetKey() == 0)
	{
		ERROR_LOG("Allot share memory fail");
		return FALSE;
	}
	CHAR * newBuffer = newMem.GetDataPtr() + sizeof(int) * 2;

	if (mpBuffer != NULL)
	{
		int head = Head();
		int tail = Tail();
		if (head < tail)
		{
			memcpy(newBuffer, &mpBuffer[head], tail - head);
		}
		else if (head > tail)
		{
			memcpy(newBuffer, &mpBuffer[head], mBufferLen - head);
			memcpy(&newBuffer[mBufferLen - head], mpBuffer, tail);
		}
	}
	mShareMem.Destory();
	mShareMem = newMem;

	mpBuffer = newBuffer;
	mBufferLen = newBufferLen;

	mpHead = (int*)mShareMem.GetDataPtr();
	mpTail = mpHead + 1;

	_setHead(0);
	_setTail(len);

	DEBUG_LOG("�ɹ����·��仺���С > [%u]", newBufferLen);

	return TRUE;

	__LEAVE_FUNCTION_FOXNET

		return FALSE;
}

//-------------------------------------------------------------------------

bool ShareMemMsgBuffer::Init(SM_KEY key, DSIZE &size, bool bCreate)
{
    mbServerCreate = bCreate;

	int nKey = (int)key;
	mSendBuffer.SetNameKey(mbServerCreate ? nKey : -nKey);
	
    mSize = size;
    if (bCreate)
    {
#if __LINUX__
        // �ѹ����������ڻ������
        if (mShareMem.Create(key, size+sizeof(mt)) == TRUE)
        {
            memset(mShareMem.GetDataPtr(), 0, size + sizeof(mt));

            mt *mm = (mt*)mShareMem.GetDataPtr() + size;
            pthread_mutexattr_init(&mm->mutexattr);                                  //��ʼ��mutex���Զ���
            pthread_mutexattr_setpshared(&mm->mutexattr, PTHREAD_PROCESS_SHARED);    //�޸�����Ϊ���̼乲��
            pthread_mutex_init(&mm->mutex, &mm->mutexattr);                          //��ʼ��һ��mutex��
        }
#else
        if (mShareMem.Create(key, size+1) == TRUE)
            memset(mShareMem.GetDataPtr(), 0, size);
#endif          
        else
        {
            ERROR_LOG("Share memory create fail");
            return false;
        }
    }
    else
    {
		UINT shareSize = 0;
        if (mShareMem.Attach(key, shareSize) == FALSE)
        {
            ERROR_LOG("Share memory attck fail > %u : size %d", key, size);
            return false;
        }

		{
			//NOTE: �����һ���ֽ�Ϊ��ʶ, ����Ѿ������ӹ�, �򲻿��ٴ����� 2021/4/8
			if (mShareMem.GetDataPtr()[shareSize - 1] != 0)
			{
				mShareMem.Destory();
				ERROR_LOG("%d Share memory net already connect, can not reconnect", key);
				return false;
			}

			mShareMem.GetDataPtr()[shareSize - 1] = 1;
		}
		 

		WARN_LOG("Exist share memory size %u != %d", shareSize, size);
#if __LINUX__
		size = shareSize - 1 - sizeof(mt);
		mSize = shareSize - 1 - sizeof(mt);
#else
		size = shareSize - 1;
		mSize = shareSize - 1;
#endif
		
#if __WINDOWS__
        AString str;
        str.Format("_share_msg_%d", key);
		if (mpClientLock != NULL)
			delete mpClientLock;
        mpClientLock = new MtLock(str.c_str());
        //mpClientLock->lock();
        if (!mpClientLock->trylock())
#elif __LINUX__
        if (!pthread_mutex_trylock(&getMutex()) )
#endif
        {
            mShareMem.Destory();
            ERROR_LOG("Already exist attack share msg >%d", key);
            return false;
        }
    }

    // �����˵� ���ͻ��漴ʱ���ͻ���, ������ ��Ϊ�Է��ķ��ͱ���Ϊ���˵Ľ���, ���ԶԶ˻���Ϊ����
    if (bCreate)
        _setBuffer(mSendBuffer, mReceiveBuffer, mSize);
    else
        _setBuffer(mReceiveBuffer, mSendBuffer, mSize);
    
	// ��ʼ8���ֽڱ���˫�������͵ķ��乲���ڴ��KEY
	mSendBuffer.mShareWillSendBuffer.SetShareKeySpacePtr( ( (int*)mShareMem.GetDataPtr() ) + ( bCreate ? 0 : 1 ) );
	mSendBuffer.mShareWillSendBuffer.Resize(DEFAULTSOCKETINPUTBUFFERSIZE);

    return true;
}

void ShareMemMsgBuffer::_setBuffer(LoopData &leftBuffer, LoopData &rightBuffer, DSIZE size)
{
	// ��ʼ4���ֽڱ�������͵ķ��乲���ڴ��KEY
	int offSetByBegin = sizeof(int) * 2;
	size -= offSetByBegin;

    {
        char *pBuffer = mShareMem.GetDataPtr()+ offSetByBegin;
        // for send, before bExistData, pHead, pTail
        leftBuffer.mpBuffer = pBuffer + sizeof(bool) + sizeof(int) * 2;
        leftBuffer.mBufferLen = (int)(size * 0.5f) - (sizeof(bool) + sizeof(int) * 2);
        leftBuffer.mbExistData = (bool*)pBuffer;
        leftBuffer.mpHead = (int*)(pBuffer + sizeof(bool));
        leftBuffer.mpTail = (int*)(pBuffer + sizeof(bool) + sizeof(int));
    }

    {
        char *pBuffer = mShareMem.GetDataPtr() + offSetByBegin + (int)(size*0.5f);
        // for send, before bExistData, pHead, pTail
        rightBuffer.mpBuffer = pBuffer + sizeof(bool) + sizeof(int) * 2;
        rightBuffer.mBufferLen = (int)(size * 0.5f) - (sizeof(bool) + sizeof(int) * 2);
        rightBuffer.mbExistData = (bool*)pBuffer;
        rightBuffer.mpHead = (int*)(pBuffer + sizeof(bool));
        rightBuffer.mpTail = (int*)(pBuffer + sizeof(bool) + sizeof(int));
    }
}

bool ShareMemMsgBuffer::ExistConnect()
{
	// ���г��Լ���, ���������ס,˵�������ڶԶ�����
#if __WINDOWS__		
		// �������������Ϊ��Ա����, ���Է��쳣�˳���, ��ʹ�˴�����, �Է�Ҳ����ʧ��
		//if (mpClientLock == NULL)
		//{
		//	AString str;
		//	str.Format("_share_msg_%d", mShareMem.GetKey());
  //          mpClientLock = new MtLock(str.c_str());
		//}
		//if (mpClientLock->trylock())
		//{
		//	mpClientLock->unlock();
		//	return false;
		//}
	AString str;
	str.Format("_share_msg_%d", mShareMem.GetKey());
	MtLock      mtLock(str.c_str());
	if (mtLock.trylock())
	{
		mtLock.unlock();
		return false;
	}

	return true;

#elif __LINUX__
	if (pthread_mutex_trylock(&getMutex()))
	{
		pthread_mutex_unlock(&getMutex());
		return false;
	}
	return true;
#endif
}

//-------------------------------------------------------------------------
int ShareMemMsgHander::GetWaitSendShareMemKey(int &createWaitSendKey)
{
	if (mShareMemMsgBuffer.mbServerCreate)
	{
		createWaitSendKey = *(int*)mShareMemMsgBuffer.mShareMem.GetDataPtr();
		return *(((int*)mShareMemMsgBuffer.mShareMem.GetDataPtr()) + 1);
	}
	else
	{
		createWaitSendKey = *(((int*)mShareMemMsgBuffer.mShareMem.GetDataPtr()) + 1);
		return *(int*)mShareMemMsgBuffer.mShareMem.GetDataPtr();
	}
}
//-------------------------------------------------------------------------