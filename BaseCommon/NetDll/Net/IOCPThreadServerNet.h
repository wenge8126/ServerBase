#ifndef _INCLUDE_IOCPTHREADSERVERNET_H_
#define _INCLUDE_IOCPTHREADSERVERNET_H_

#include "MemBase.h"
#include "NetHead.h"
#include "Array.h"
#include "WorkThread.h"
#include "LoopDataStream.h"

//-------------------------------------------------------------------------
// ֧������ת�Ƶ�IOCP��������
// NOTE:  ���Ӱ�һ�ζ˿ڻ᲻�ܸ��£���˱���ʹ�ö��߽��ռ�����
// 1 �������պ󣬰󶨶˿ڣ� Ȼ����䵽ָ���߳��е����������, ֻ��Ҫ����һ���߳�������������
// 2 ÿ���߳��еĹ���֧��ת�ƹ���
// 3 Ϊ��֧�ֶ���,��ʹ���κ����ܶ���
//-------------------------------------------------------------------------
class IOCPThreadConnect;
class ListenThreadEx;
class IOCPProcessThread;
class Net_Export IOCPThreadServerNet :  public WorkThread
{
	friend class IOCPThreadConnect;
	friend class IOCPProcessThread;
public:	
	virtual bool SendData(int connectID, void *checkParam, const char *pSendData, int sendSize);

	virtual void ProcessReceive(int connectID, void *checkParam, void *processTool);

	virtual bool CheckConnect(IOCPThreadConnect *pConn, void *checkParam){ return true; }

public:
	virtual IOCPThreadConnect* CreateConnect();

	// ׷������ NOTE: ��Ҫ��֤�̰߳�ȫ
	// virtual bool AppendConnect(IOCPThreadConnect *pConnect);

	// NOTE: �����߳��ڻص�
	void OnAcceptConnect(size_t connectSocket);

	bool InitStart(const char *szIp, int port, int threadCount = 2);

	void Close();

	// NOTE: �����ر���������
	bool CloseAllConnect(int waitSecondTime = 10);

	void RemoveConnect(IOCPThreadConnect *pConn, bool pDelete = true);

	//void _RemoveConnect(IOCPThreadConnect *pConn, bool pDelete);

	//NOTE: �����̵߳���, ֻ�������������Ƴ�
	void Process();

	virtual void backWorkThread() override;

public:
	IOCPThreadServerNet()
		: mpListen(NULL)
	{

	}
	~IOCPThreadServerNet()
	{
		Close();
	}

protected:
	struct ConnectData
	{
		IOCPThreadConnect	*mpConnect;
		CsLock	mConnectLock;

		ConnectData()
			: mConnectLock()
			, mpConnect(NULL)
		{

		}
	};
	Array<ConnectData> mConnectList;
	Array<IOCPProcessThread*> mIOCPThreadList;

	CsLock	mLock;

private:
	ListenThreadEx		*mpListen;
};
//-------------------------------------------------------------------------
struct SendOverlapped;
struct RecvOverlapped;
//-------------------------------------------------------------------------
class Net_Export  IOCPThreadConnect :  public MemBase
{
public:
	virtual void ProcessReceive(void *processTool){}

public:
	// ���뵱ǰ���� NOTE: ����ȷ���̰߳�ȫ
	//virtual bool LeaveNet()
	//{
	//	mConnectLock.lock();
	//	mpNet->RemoveConnect(this, false);
	//	mConnectLock.unlock();
	//	return true;
	//}

	// �ڽ����̻߳ص�
	virtual void OnReceiveData(void *pData, int  size);

	// NOTE: �����Ѵ�����ʵ��ֱ���ͷŵ�, ����ж˿�δ���, �������ɺ��ڽ����߳��ڽ���ɾ��
	void Close()
	{
		mpNet->RemoveConnect(this, true);
	}

	bool Send( void *sendData, size_t size )
	{
		CsLock *pLock = getLock();
		if (pLock==NULL)
			return false;

		pLock->lock();
		bool b = mSendData.Write((const char*)sendData, size)==size;
		_SendTo();
		pLock->unlock();
		return b;
	}

	void _SendTo();
	void _PostReceive();

	bool IsRemove(){ return false; }

	void SetRemove(bool bRemove);

	bool CheckRemove();

	const char* GetIp(){ return  mIp; }
	int GetPort(){ return mPort; }
	int GetLocalIPPort(char *ip);

	int GetNetID() const { return mNetID; }

	CsLock* getLock();

	void lock()
	{
		CsLock *pLock = getLock();
		if (pLock!=NULL)
			pLock->lock();
	}

	void unlock()
	{
		CsLock *pLock = getLock();
		if (pLock!=NULL)
			pLock->unlock();
	}

public:
	IOCPThreadConnect(IOCPThreadServerNet *pNet);
	virtual ~IOCPThreadConnect();

public:
	SendOverlapped		*mSendOverlapped;
	RecvOverlapped		*mRecvOverlapped;	

	//CsLock mRevLock;
	//CsLock mSendLock;
	LoopDataStream		mSendData;

	size_t mSocketID;
	char mIp[20];
	int mPort;

	UInt64 mLastSendTime;
	bool mNeedRemove;
	IOCPThreadServerNet *mpNet;
	int mNetID;
};
//-------------------------------------------------------------------------
// IOCP �����̣߳� ʵ������������ڿ������������������
class IOCPProcessThread : public WorkThread
{
public:
	IOCPProcessThread(ListenThreadEx *pListen)
		: mpListen(pListen)
	{

	}

	virtual void onBeforeClose(void) override 
	{ 
#if __LINUX__
		int r = 10;
		_ForceClose(&r);
#else
		_ForceClose(10); 
#endif
	};

	virtual void backWorkThread(void);

	void Log(const char *sz, ...){}

protected:
	ListenThreadEx	*mpListen;
};
//-------------------------------------------------------------------------
#endif //_INCLUDE_IOCPTHREADSERVERNET_H_