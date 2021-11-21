
#ifndef _INCLUDE_TCPLISTENTHREAD_H_
#define _INCLUDE_TCPLISTENTHREAD_H_

#include "NetHead.h"
#include "WorkThread.h"
#include "NetHandle.h"
#include "PoolList.h"
#include "Lock.h"

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// �����߳�, ������������, ����ɹ�, �򴴽���������
class ServerSocket;
class Socket;
class Net_Export TCPListenThread : public WorkThread
{
	friend class AcceptConnectTask;
public:
	TCPListenThread()
		: mListenSocket(NULL)
		, mbHasConnect(false)
	{		
		
	}

	virtual ~TCPListenThread();

	void InitStart(const char* szIp, int nPort);

	virtual void Close()
	{
		_ForceClose(10);		
		WorkThread::Close();
	}

public:
	// MAIN thread call
	virtual void OnAcceptConnect(HandConnect newConnect) = 0;
	virtual HandConnect _CreateNewConnect() = 0;	

	virtual void Process();

public:
	// NOTE: �ں�̨�߳��ڵ���
	virtual void backWorkThread();

protected:
	ServerSocket		*mListenSocket;

	CsLock				mConnectListLock;
	PoolList<Socket*>	mConnectList;
	PoolList<Socket*>	mMainConnectList;
	bool				mbHasConnect;
};
//-------------------------------------------------------------------------

#endif //_INCLUDE_TCPLISTENTHREAD_H_