/*/-------------------------------------------------------------------------
NOTE:
当前使用发送和接收的缓存直接投递到完成端口线程
在执行期内不可以对发送和接收的缓存进行动态修改空间大小
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

	// 评测端口是否监听中
	static bool PingOpenPort(const char *szIp, int nPort);

public:
	void Init(AutoNet ownerNet, UInt64 socketID, const char *szIp, int port)
	{
		mNet = ownerNet;
		mSocketID = socketID;
		mIp = szIp;
		mPort = port;
	}

	//NOTO: NET ID 可以由逻辑层进行自由设定, 连接管理中不使用此ID, 所有可以与连接列表下标不相符
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
	virtual bool Send(const Packet  *msgPacket, bool bEncrypt) override { return SendPacket(msgPacket, bEncrypt); }		// 子类覆盖后, 可以分析大消息包是否自动进行分包发送
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
	// NOTE: 仅在网络线程调用, 目的实现无锁设计, 
	// 工作线 使用 mSendLoopData.SendData() 数据后, 使用手动触发发送事件
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
	ThreadLoopData				mReceiveLoopData;				// No lock, NOTE: 由于不可变化大小, 所以, 最大包不可大于此空间值(可采用临时大包处理方案)
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

	// NOTE: 一次处理接收事件耗时超过10毫秒, 比如在这里进行统计, 1分钟出现10次进行断开处理
	virtual void OnProcessLongTime(){}

protected:
	bool					mbReceiveData;
	UInt64					mStartTime;
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_IOCPCONNECT_H_