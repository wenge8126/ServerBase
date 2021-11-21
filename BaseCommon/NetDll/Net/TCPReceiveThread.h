/********************************************************************
	created:	2014/05/11
	created:	11:5:2014   14:45
	filename: 	C:\HomeGame\BaseCommon\NetDll\Net\TCPReceiveThread.h
	file path:	C:\HomeGame\BaseCommon\NetDll\Net
	file base:	TCPReceiveThread
	file ext:	h
	author:		YangWenge
	
	purpose:	�첽���繦��ģ��
	NOTE:		Select Mode ������ռ��쳣���, ���������ߴ���, δ����д
*********************************************************************/
#ifndef _INCLUDE_TCPRECEIVETHREAD_H_
#define _INCLUDE_TCPRECEIVETHREAD_H_

#include "NetHead.h"
#include "NetHandle.h"
#include "EasyList.h"
#include "WorkThread.h"
#include "Lock.h"

	struct fd_set;
	struct timeval;

class SocketInputStream;
class SocketOutputStream;
class Socket;

struct BackReceiveData : public MemBase
{
	friend class TCPThreadReceiveConnect;
	friend class TCPReceiveThread;
	friend class TCPThreadClientNet;
	friend class TCPListenThread;
	friend class ClientThreadConnect;

protected:
	Socket				*mSocket;
	bool				m_bNeedRemove;
	bool				m_bDisconnect;
	bool				mbNeedFree;
	bool				mHasReceiveData;
	bool				mHasSendData;

	SocketInputStream	*mBackThreadReceiveData;
	CsLock				mLock;
	
	SocketOutputStream	*mWillSendData;				// use at main thread
	SocketOutputStream	*mBackThreadSendData;
	CsLock				mSendDataLock;

public:
	~BackReceiveData();
	BackReceiveData();

	void InitReset();

public:
	void Disconnect(bool bDisconnect){ m_bDisconnect = bDisconnect; }
	bool IsDisconnect() const { return m_bDisconnect; }
	// ��̨�߳̽��ղ���
	// NOTE: call in back thread
	void ReceiveDataOnThread();
	void SendDataOnThread();

	void SetNeedFree()
	{
		mbNeedFree = true;
	}

};

class Net_Export TCPThreadReceiveConnect : public tNetConnect
{
	friend class TCPReceiveThread;
	friend class TCPThreadClientNet;
	friend class TCPListenThread;

public:
	TCPThreadReceiveConnect();
	virtual ~TCPThreadReceiveConnect();

public:
	virtual void SetNetID(int netID) { mNetID = netID; }
	virtual int GetNetID() const { return mNetID; }

public:
	virtual bool Connect(const char *ip, unsigned int port );
	virtual void Close(){ mBackData->Disconnect(true); }
	virtual void Log(const char *szInfo, ...);

public:
	virtual bool SendEvent(Logic::tEvent *pEvent);
	
	virtual bool Send(const Packet  *msgPacket, bool);

	virtual void SetNetHandle(AutoNet net){ mNet = net; }
	virtual tNetHandle* GetNetHandle(void) { return mNet.getPtr(); }

	virtual bool CheckSameConnect(tNetConnect *pConnect) { return pConnect==this; }

public:
	virtual void OnSucceedSendEvent(Logic::tEvent *pEvt, Packet *p) {}

	virtual void OnDisconnect();

	virtual void OnConnected();

	virtual void OnSafeCheckSucceed();

public:
	// NOTE: ���߳��ڵ���
	virtual bool InitCreate();	

	// ���߸��´�������
	virtual bool Process();
	virtual void ProcessReceiveData();
	virtual bool CheckSafeConnect();
	virtual void CheckSafeOverTime();

	virtual void SetRemove(bool bNeedRemove)
	{
		mNeedRemove = bNeedRemove;		
	}

	virtual bool CanRemove();

	virtual bool IsNeedRemove(){ return mNeedRemove; }
	virtual bool IsDisconnect() const { return mBackData->IsDisconnect(); }

public:
	Socket* GetSocket(){ return mBackData->mSocket; }
	virtual UInt64 GetIPKey();

	virtual const char* GetIp();
	virtual int GetPort();

	virtual uint GetReceiveDataSize(){ return mReceiveTotalSize; }

protected:
	BackReceiveData		*mBackData;

	SocketInputStream	*mMainThreadReceiveData;
	SocketOutputStream	*mMainThreadSendData;

	int					mNetID;

	AutoNet				mNet;
	bool				mbDataReceived;
	bool				mNeedRemove;
	UInt64				mStartTime;				
	uint				mReceiveTotalSize;
};


class TCPReceiveThread : public WorkThread
{
public:
	TCPReceiveThread();
	~TCPReceiveThread();

public:
	// ��������һ������
	virtual void AppendConnect(HandConnect connect);
	virtual int GetConnectCount(){ return mMainThreadConnectList.size(); }

	// ����������Ϣ��������ڽ����Ƴ�����, �����ƻ����б�
	virtual bool RemoveConnect(tNetConnect *pConnect);
	virtual void Process();

	virtual void Close(void);

	virtual EasyList<HandConnect>& GetConnectList(){ return mMainThreadConnectList; }

public:
	virtual void backWorkThread(void);

	virtual void CpuSleep();

protected:
	EasyList<BackReceiveData*>	mBackConnectDataList;
	EasyList<HandConnect>		mMainThreadConnectList;
	CsLock						mConnectDataListLock;

	EasyList<BackReceiveData*>	mEnterConnectDataList;
	bool						mHasEnterConnect;

protected:
	fd_set					*m_ReadFDs;
	fd_set					*m_WriteFDs;
	fd_set					*m_ExceptFDs;

	fd_set					*m_FDsBak;

	timeval					*m_Timeout;

	UInt64					m_MaxFD;			// �����߳�ʹ��

	int						mConnectCount;
};



#endif //_INCLUDE_TCPRECEIVETHREAD_H_