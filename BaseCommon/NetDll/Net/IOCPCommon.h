#ifndef _INCLUDE_IOCPCOMMON_H_
#define _INCLUDE_IOCPCOMMON_H_

#ifdef NET_EXPORTS 
#include "NetHead.h"

#include "WorkThread.h"
#include "NetHandle.h"
#include "PoolList.h"
#include "Lock.h"
#include "SocketAPI.h"
#include "CoroutineTool.h"
//-------------------------------------------------------------------------
#define IoSend	1	// �����¼�
#define IoRecv	2	// �����¼�
#define IoExit	3	// �˳�ϵͳ
#define IoNotifySend 4	// ֪ͨ��������

#define MAX_PACKAGE_SIZE	(1024*16)		// ���η��ͻ���ܰ��Ĵ�С
//-------------------------------------------------------------------------

#if __WINDOWS__

struct BaseOverlapped : public OVERLAPPED
{
	UINT IoMode;
	WSABUF WsaBuf;

	//char buffer[MAX_PACKAGE_SIZE];	// ���ݵĻ���ָ��
	bool mbWaiting;

	BaseOverlapped()
	{
		mbWaiting = false;
		// init OVERLAPPED
		Internal=0;
		InternalHigh=0;
		Offset=0;
		OffsetHigh=0;
		hEvent=0;

		//memset(buffer, 0, MAX_PACKAGE_SIZE);
		// init SOverlapped
		IoMode=0;
		WsaBuf.buf=NULL;
		WsaBuf.len=0;
	}	
};


struct SendOverlapped : public BaseOverlapped
{
	SendOverlapped()
	{
		IoMode = IoSend;
	}
};


struct RecvOverlapped : public BaseOverlapped
{
	char buffer[MAX_PACKAGE_SIZE];	// ���ݵĻ���ָ��

	RecvOverlapped()
	{
		IoMode = IoRecv;
		WsaBuf.buf=buffer;
		WsaBuf.len=MAX_PACKAGE_SIZE;
	}
#if _DEBUG_NET_
	DSIZE mCurrentTile;
#endif
};
#endif //__WINDOWS__
//-------------------------------------------------------------------------
struct SocketInfo
{
	SOCKET mSocket;
	sockaddr mAddr;
};
//-------------------------------------------------------------------------
class IOCPServerNet;
class IOCPListenThread : public WorkThread
{
	friend class AcceptConnectTask;
public:
	IOCPListenThread(IOCPServerNet *pServerNet)
		: mListenSocket(INVALID_SOCKET)
		, mbHasConnect(false)
		, mpServerNet(pServerNet)
	{		

	}

	virtual ~IOCPListenThread()
	{
		Close();
	}

	bool InitStart(const char* szIp, int nPort);

	virtual void Close();

	void ErrorExit(const char *szInfo);

public:
	// MAIN thread call
	virtual void OnAcceptConnect(HandConnect newConnect);
	virtual HandConnect _CreateNewConnect(SocketInfo &socketID);

	virtual void Process();

public:
	// NOTE: �ں�̨�߳��ڵ���
	virtual void backWorkThread();
	void OnError(int netID, int error){}

protected:
	IOCPServerNet		*mpServerNet;
	SOCKET				mListenSocket;

	CsLock				mConnectListLock;
	Array<SocketInfo>	mConnectList;
	Array<SocketInfo>	mMainConnectList;
	bool				mbHasConnect;
};
//-------------------------------------------------------------------------
class ConnectNetThread : public WorkThread
{
public:
	ConnectNetThread();

	~ConnectNetThread();

public:
	virtual void StartConnect(const char *sIp, int nPort, int milSecondOverTime);

public:
	bool IsConnectFinish();
	bool IsOverTime();

public:
	virtual void backWorkThread(void);
	virtual void onBeforeClose(void)
	{

	}

public:
	SOCKET			mSocket;
	AString			mIP;
	int					mPort;
	bool					mbFinish;
	Auto<Waiter<HandConnect>>			mConnectWaiter;
	UInt64				mOverTime;
	HandConnect	mConnect;
};


//-------------------------------------------------------------------------
class NetListen : public IOCPListenThread
{
public:
	// ���ݵ�ǰ�������ӽ��з���
	void ProcessToNet(tNetHandle *destHandle);

	void AppendNet(tNetHandle *pNet)
	{
		mConnectListLock.lock();
		for (size_t i=0; i<mNetList.size(); ++i)
		{
			if (mNetList.get(i)==pNet)
			{
				mNetList._remove(i);
				break;
			}
		}
		mNetList.insert(pNet->GetConnectCount(), pNet);
		mConnectListLock.unlock();
	}

	void RemoveNet(tNetHandle *pNet)
	{
		mConnectListLock.lock();
		for (size_t i=0; i<mNetList.size(); ++i)
		{
			if (mNetList.get(i)==pNet)
			{
				mNetList._remove(i);
				break;
			}
		}
		mNetList.insert(pNet->GetConnectCount(), pNet);
		mConnectListLock.unlock();
	}

public:
	virtual void Process(){}

	NetListen()
		: IOCPListenThread(NULL)
		, mCheckCount(0)
	{

	}

protected:
	EasyMap<int, tNetHandle*>		mNetList;
	int mCheckCount;
};

//-------------------------------------------------------------------------*


#endif //NET_EXPORTS

class NetListen;
class Net_Export NetListenTool : public Base<NetListenTool>
{
public:
	bool InitStart(const char *szIp, int port);

	void ProcessToNet(tNetHandle *destHandle);

	void AppendNet(tNetHandle *pNet);

	void RemoveNet(tNetHandle *pNet);

public:
	NetListenTool();
	~NetListenTool();

protected:
	NetListen	*mpListen;
};
//-------------------------------------------------------------------------*

#endif //_INCLUDE_IOCPCOMMON_H_