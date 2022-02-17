/*/-------------------------------------------------------------------------
NOTE:
��ǰʹ�÷��ͺͽ��յĻ���ֱ��Ͷ�ݵ���ɶ˿��߳�
��ִ�����ڲ����ԶԷ��ͺͽ��յĻ�����ж�̬�޸Ŀռ��С
//-------------------------------------------------------------------------*/

#ifndef _INCLUDE_IOCPCONNECT_H_
#define _INCLUDE_IOCPCONNECT_H_

#include "NetHead.h"
#include "NetHandle.h"
#include "Lock.h"
#include "LoopList.h"
#include "LoopDataBuffer.h"

#include "Lock.h"
//-------------------------------------------------------------------------
// 

//-------------------------------------------------------------------------
struct SendOverlapped;
struct RecvOverlapped;
struct BaseOverlapped;
struct PacketData;
class EventData;

class NoResizeLoopBuffer : public LoopDataStream
{
public:
	virtual BOOL	Resize( INT size ) override { return FALSE; }

	NoResizeLoopBuffer(UINT BufferSize = DEFAULTSOCKETINPUTBUFFERSIZE, UINT MaxBufferSize = DISCONNECTSOCKETINPUTSIZE)
		: LoopDataStream(BufferSize, MaxBufferSize){}
};



class Net_Export IOCPConnect : public tNetConnect
{
	friend class IOCPBaseNet;
	friend class IOCPServerNet;
	friend class IOCPClientNet;
	friend class IOCPClientSetNet;
	friend class IocpOrEpollProcessNetThread;
	friend class MeshedNodeNet;
	friend class EventNetProtocol;

public:
	IOCPConnect(int revBufferLen = 0, int sendBufferLen = 0);
	virtual ~IOCPConnect();

	// ����˿��Ƿ������
	static bool PingOpenPort(const char *szIp, int nPort);

public:
	void Init(AutoNet ownerNet, UInt64 socketID, const char *szIp, int port)
	{
		mNet = ownerNet;
		mSocketID = socketID;
		mIp = szIp;
		mPort = port;
	}

	//NOTO: NET ID �������߼�����������趨, ���ӹ����в�ʹ�ô�ID, ���п����������б��±겻���
	virtual void SetNetID(int netID) 
	{ 
		mID = netID; 
	}
	virtual int GetNetID() const { return mID; }

	virtual void SetRemove(bool bNeedRemove);
	virtual bool IsRemove(){ return mNeedRemove; }
	virtual bool IsDisconnect() const { return mNeedRemove; }
	virtual const char* GetIp(){ return mIp.c_str(); }
	virtual int GetPort(){ return mPort; }
	virtual int GetLocalPort();
	virtual AString GetLocalIP();
	virtual AString GetRemoteAddr(int &port);
	virtual void Close(){ SetRemove(true); }

	virtual bool _checkOk() const { return true; }

public:
	virtual bool SendEvent(Logic::tEvent *pEvent);
	virtual bool Send(const Packet  *msgPacket, bool bEncrypt) override { return SendPacket(msgPacket, bEncrypt); }		// ���า�Ǻ�, ���Է�������Ϣ���Ƿ��Զ����зְ�����
	bool SendPacket(const Packet *msgPacket, bool bEncrypt);

	virtual tNetHandle* GetNetHandle(void){ return mNet.getPtr(); }

public:
	virtual void OnConnected(){}
	virtual void OnDisconnect(){ /*WARN_LOG("[%s:%d] disconnect!", GetIp(), GetPort());*/ }
	virtual void OnSucceedSendEvent(Logic::tEvent *pEvt, Packet *p) {}

	virtual void OnReceivePacket(Packet *pPacket);

	// Ping
	virtual void OnReceivePingOverTime();
	virtual int ReceivePingOverSecond() const { return 120; }
	virtual int OnceSendPingSecond() const { return 3; }

	virtual void OnPing(Packet *pingPacket) override;
	virtual void ProcessPing();

	virtual bool Process();

	void Log( const char *szInfo, ... );

private:
	// NOTE: ���������̵߳���, Ŀ��ʵ���������, 
	// ������ ʹ�� mSendLoopData.SendData() ���ݺ�, ʹ���ֶ����������¼�
	virtual void _SendTo();
	virtual void _PostReceive();

protected:
	virtual void _Free();
	virtual void _ProcessReceiveData();

	void _Start();

	void _Send(const char *szSendData, DWORD size)
	{
			mSendLoopData.mWaitSendBuffer._write((void*)szSendData, size);
			TrySend();
	}

	void TrySend();
	void TriggerThreadSend();

protected:
	int									mID;
	AutoNet							mNet;
	AString								mIp;
	int									mPort;

protected:
    bool									mNeedRemove;
	CsLock								mAppendOrRemoveLock;
    //bool									mbHaveReceiveEventMsg;
    //bool									mbHaveRevData;
	//LoopDataStream				mReceiveData;
	ThreadLoopData				mReceiveLoopData;				// No lock, NOTE: ���ڲ��ɱ仯��С, ����, �������ɴ��ڴ˿ռ�ֵ(�ɲ�����ʱ���������)
	ThreadLoopData				mSendLoopData;

	UInt64								mLastSendTime;

	UInt64								mSocketID;

	void*								mIocpHandle;
#if __WINDOWS__
	SendOverlapped				*mSendOverlapped;
	RecvOverlapped				*mRecvOverlapped;	
	BaseOverlapped				*mForSendNotifyOverlapped;
#else
	short								mNetThreadID;
#endif

	UInt64								mLastReceivePingTime;
	UInt64								mLastSendPingTime;

#if _DEBUG_NET_
	UInt64				mRemoveTime;
    UInt64              mReceiveSize;
    UInt64              mProcessSize;
	UInt64				mSendSize;
#endif
};
//-------------------------------------------------------------------------
class Net_Export IOCPServerConnect : public IOCPConnect
{
	friend class IOCPServerNet;

public:
	IOCPServerConnect(int revBufferLen = 0, int sendBufferLen = 0)
		: IOCPConnect(revBufferLen, sendBufferLen)
		, mbReceiveData(false)
		, mStartTime(0)		
	{

	}

protected:
	virtual void _ProcessReceiveData();
	virtual bool Process();	

	virtual bool _checkOk() const override { return mbReceiveData; }

	virtual void OnConnectSafeCodeError(const char *szIp, int port, int safeCode) 
	{
		DEBUG_LOG("Check safe code fail > %d != now %d", safeCode, GetNetHandle()->GetSafeCode());
	}

	// NOTE: һ�δ�������¼���ʱ����10����, �������������ͳ��, 1���ӳ���10�ν��жϿ�����
	virtual void OnProcessLongTime(){}

protected:
	bool					mbReceiveData;
	UInt64					mStartTime;
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_IOCPCONNECT_H_