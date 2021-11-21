
#ifndef _INCLUDE_NETCONNECT_H_
#define _INCLUDE_NETCONNECT_H_

#include "NetHead.h"

#include "AutoString.h"
#include "MemBase.h"
#include "Hand.h"

#include "NetHandle.h"
//----------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------
namespace Logic
{
	class tEvent;
}

class SocketInputStream;
class SocketOutputStream;
class Socket;
class Packet;
class tNetHandle;
//----------------------------------------------------------------------------------------------
// ��������
//----------------------------------------------------------------------------------------------
class Net_Export tConnect : public tNetConnect
{
public:
	//friend class UDPServerSocket;
	friend class EventClientNet;

public:
	virtual bool SendEvent(Logic::tEvent *pEvent);
	virtual int  SendSize(void);

public:
	friend class Player;

public:
	virtual int GetNetID() const { return mConnectID; }
	virtual void SetNetID(int id) { mConnectID = id; }

	virtual bool CheckSameConnect(tNetConnect *pConnect) { return false; }

	virtual tNetHandle* GetNetHandle(void) = 0;
	virtual void OnConnected(void) = 0;
	virtual void OnClose(void) {}

	virtual void SetState(NET_CONNECT_STATE netState){ mNetState = netState; }
	virtual bool IsState(NET_CONNECT_STATE netState){ return mNetState==netState; }

	virtual void Log(const char *szInfo, ...);

public:
	tConnect();
	virtual ~tConnect();

	friend class tServerNet;


public:
	virtual bool Connect(const char *ip, unsigned int port, bool bWait = false);
	virtual bool TryConnect( void );
	virtual void Close();


	virtual AString GetHost(void);
	virtual int GetPort(void);

	virtual bool HasNetError();

public:
	virtual bool Send ( const Packet* pPacket, bool );
	virtual bool Send(const char* szData, size_t length);

	virtual bool Process(void);	

protected:
	virtual void CreateSocket(void);
	virtual void _CreateSocket(void);
	Socket*	GetSocket(void);

	virtual bool ProcessOutput();
	virtual bool ProcessInput();

	virtual void OnReceiveData(void);

	virtual bool _ReceiveData();

public:
	virtual void OnSucceedSendEvent(Logic::tEvent *pEvt, Packet *p){}

public:
	// �������Ӱ�ȫ���, ��Ҫ�Ƿ���������ʹ��
	virtual void SetPass(void){ mNetState = NET_CONNECT_PASS; }
	virtual bool IsPass(void) { return mNetState==NET_CONNECT_PASS || mNetState==NET_CONNECT_OK; }

	virtual void SetOK(void){ mNetState = NET_CONNECT_OK; }
	virtual bool IsOk(void){ return mNetState==NET_CONNECT_OK; }

protected:
	virtual void SetConnectBegin(void){ mNetState = NET_CONNECT_BEGIN; }

public:
	virtual void SetUse(bool bUse) {};
	virtual bool IsUse(void) { return true; }

public:
	virtual void SetNeedRemove( const char *szInfo );
	virtual AString& GetRemoveInfo(void){ return mRemoveInfo; }
	virtual AString  GetIpPort(void);

	virtual UInt64 GetAddrIndexKey(void);

	virtual UINT GetSendBufferDataSize();

	virtual uint GetSendDataSize(){ return mSendTotalSize; }

protected:
	NET_CONNECT_STATE	mNetState;
	Socket				*mSocket;
	SocketInputStream	*mSocketInputStream;
	SocketOutputStream	*mSocketOutputStream;
	// һ���������棬 һ���������ͣ������������ݱȽϴ�ʱ�����糬��512�������ѹ������
	// ���͹���|ԭ��С��2B����ѹ����Ĵ�С��2B�����ʾδѹ��������
	// һ�η��Ͳ��꣬����һ�η��ͣ�ֱ�������꣬��ʱ�к�������ݴ����������û�ȴ����������Ϣ����
	// ��ȡʱ���з�����ȡ��������С���ٶ�ȡ���ݴ�С+2B��Ȼ����н�ѹ����
	// �����������������������ݰ������ܶ���1����Ȼ��ֱ�ӽ��ִ��OK
	SocketOutputStream	*mOutputZipBuffer;
	SocketInputStream	*mInputUNZipBuffer;

	AString				mRemoveInfo;

	uint				mSendTotalSize;

private:
	int					mConnectID;

};

typedef Hand<tConnect>	AConnect;

#endif //_INCLUDE_NETCONNECT_H_