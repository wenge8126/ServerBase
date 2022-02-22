#ifndef _INCLUDE_UDPBYTCPNET_H_
#define _INCLUDE_UDPBYTCPNET_H_

#include "IOCPServerNet.h"
#include "IOCPConnect.h"
#include "CEvent.h"

#define  _WAIT_RESEND_TIME          60              // �ڴ�ʱ����δ���յ��ظ�������ط�
//-------------------------------------------------------------------------*/
enum
{
	UDP_PART_PACKET	 = 100,
	UDP_RESPONSE_PACKET = 101,
	UDP_REQUEST_RESEND_PACKET = 102,
	UDP_CHECK_KEY,
	TCP_NOTIFY_CHECK_KEY,		// ����˷���ͻ���
	TCP_NOTIFY_READY_OK,
	TCP_RESPONSE_EMPTY_PART,
};

//-------------------------------------------------------------------------*/
/*/ UDP���Tcpʵ���޶�������
����Ч������ (ǰ����ResourcesServer�ϴ��ļ�) 2017.9.28
	1 ���������ݷ�����ɺ�, ��ʱ�ȴ���Ӧ���, ���δ��Ӧ��Ӧ�������ʹ��TCP������͵���ŷ����Է�, 
		�Է��յ����ʱ������UDPӦ���Ѿ�ȫ�����յ�, ͨ�������б������δ���յ����ݰ�,�����ط�
	2 �����յ����ݰ�, �Ҵ����������Ϊ��ʱ, �����յ��������ŷ����Է�, �Է����պ�����ȴ��ط�������
	3 �����յ����ݰ���, ��������б�Ϊ��, ������Ƿ������������������Ϊ3 �İ�, �����������ط�

�ŵ�:
	1 ʹ�÷���, ����Ϊһ���TCP����, UDP������TCP������, �Զ����UDP׼��������ӹ�
	2 �����Ϣ��Ӧ�ٶ�, Լ 20(wifi)~80(4G)
	3 ȷ���޶�ʧ���ݴ���, �ɼ̳�OnConnectFault() �����������

NOTE: ����ʹ�� uint δʵ���������, һ�����ӿɴ���Լ4200G, �����Ͽ���������ʱP2P��������

�Ż��޸� (2017.9.30)
	1 ����һ�����ͽ����򼰷���ʱ��Ͱ������뵽�����б�
	2 ����һ�����ݺ󣬼����Ѵ�����ź��ѽ������UDP�ظ�
	3 �ȴ��жϵ�һ�������δ�ظ����ط�һ�Σ����Ƴ��ж��ط��б�
//-------------------------------------------------------------------------*/
class Net_Export UDPByTcpConnect : public IOCPServerConnect
{
	friend class UDP_RequestResendPart;
	friend class UDP_NotifySendCode;
	friend class UDPPartPacket;

public:
	static void RegisterMsgPacket(tNetProtocol *pPro);

public:
	// ���ӳ�������, TCP�����Ѿ����ֳ�ʱ
	virtual void OnConnectFault() { NOTE_LOG("[%s] connet happy fault, now stop net work",  GetIp(), GetPort()); }

	virtual bool Process() override
	{
		_udpConnectProcess();
		return IOCPServerConnect::Process();		
	}

	virtual void _udpConnectProcess();

public:
	virtual HandPacket _createPartPacket() = 0;
	virtual bool _sendByUDP(HandPacket dataPacket) = 0;
	virtual bool _sendByTcp(HandPacket dataPacket) = 0;
	virtual bool UdpReadyOk() const { return mbUDPReadyOk;  }

	virtual void StartSendUDPCheck(UInt64 checkKey);

	void _sendUdpCheck(UInt64 checkKey)
	{
		Auto<DataPacket> pak = _createPartPacket();
		pak->GetData()->seek(0);
		pak->GetData()->write((byte)UDP_CHECK_KEY);
		pak->GetData()->write(checkKey);
		_sendByUDP(pak);
	}

	virtual void SetUDPReadyOk(bool bOk)
	{
		mbUDPReadyOk = bOk;
		mWaitResponeOkEvent._free();
	}

	// �ӹ����еķ�����Ϣ��,ʹ��UDP����
	virtual bool Send(int packetID, const Packet *msgPacket) override
	{
		if (!UdpReadyOk())
			return IOCPServerConnect::Send(packetID, msgPacket);

		// �ְ�
		bool b = GetNetHandle()->GetNetProtocol()->WritePacket(packetID, msgPacket, &mUDPSendData);
		if (b)
		{
			_sendAllData();
			return true;
		}
		else
			ERROR_LOG("UDP send [%s] WritePacket fail", msgPacket->GetMsgName());
		return false;
	}

public:
	virtual void _sendAllData();

	// UDP�������ݴ���
	virtual void OnReceiveUDPData(DataBuffer *receiveData, int size, bool bUdp);

	void _CheckProcessWaitReceivePartList();

	// ����ϲ����ս��յ�������
	void _ProcessUDPReceive();

	// �����ط�, UDP��TCPͬʱ����
	void _requestResend(uint nPartCode);

	//// NOTE: ��TCP֪ͨ��ǰ�������а��������ţ���ΪTCP�ٶ�����UDP,��ʱ�����϶��Ѿ����յ����а�,����δ���յ���ȫ�������ط�����
	//void OnNotifyLastSendCode(uint lastSendCode)
	//{
	//	if (lastSendCode>mReceiveCode)
	//	{			
	//		for (uint i=mReceiveCode+1; i<=lastSendCode; ++i)
	//		{
	//			if (!mReceivePartDataList.exist(i))
	//				_requestResend(i);
	//		}
	//	}
	//}

	void _checkResend()
	{
		if (mReceivePartDataList.empty())
			return;
		uint lastCode = mReceivePartDataList.last()->mKey;
		for (uint i=mReceiveCode+1; i<lastCode; ++i)
		{
			if (!mReceivePartDataList.exist(i) && (lastCode-i)>3)
				_requestResend(i);
		}
	}

	HandPacket _findSendPacket(int nCode)
	{
		for (auto it=mSendPacketList.begin(); it; ++it)
		{
			if ((*it).mCode==nCode)
				return (*it).mPartPacket;
		}
		return HandPacket();
	}

public:
	UDPByTcpConnect()
		: mSendCode(0)
		, mReceiveCode(0)
		, mbUDPReadyOk(false)
		, mTargetAddr(0)
		, mUDPReceiveData(1024*8)
	{

	}
	~UDPByTcpConnect()
	{
		mWaitResponeOkEvent._free();
	}

public:
	UInt64			mTargetAddr;						// Զ�˵�ַ

protected:
	// UDP �����͵�������
	LoopDataStream		mUDPSendData;
	ThreadLoopData		mUDPReceiveData;
	struct SendPartData
	{
		uint				mCode;
		UInt64			mSendTime;
		HandPacket	mPartPacket;
		bool				mbResend;

		SendPartData()
			: mbResend(false)
			, mCode(0)
			, mSendTime(0) {}
	};
	PoolList<SendPartData>				mSendPacketList;
	//EasyMap<uint, HandPacket>		mSendPacketList;
	struct RevPartData
	{
		HandPacket	mPartPacket;
		bool				mbResponse;

		RevPartData()
			: mbResponse(false){}
	};
	EasyMap<uint, RevPartData>		mReceivePartDataList;
	EasyMap<uint, AutoEvent>			mWaitResendList;
	//EasyMap<uint, AutoEvent>			mWaitSendLastSendCodeList;
	uint		mSendCode;
	uint		mReceiveCode;
	AutoEvent	mWaitResponeOkEvent;
	bool mbUDPReadyOk;
};

//-------------------------------------------------------------------------*/
class UDPPartPacket : public DataPacket
{
public:	
	virtual UINT Execute( tNetConnect* pConnect );
	virtual	PacketID_t	GetPacketID( ) const { return UDP_PART_PACKET; }

public:
	UDPPartPacket()
	{
		mData = MEM_NEW DataBuffer(MAX_UDP_MSG_SIZE+60);
	}
};

//-------------------------------------------------------------------------*/
class UDP_RequestResendPart : public DataPacket
{
public:
	virtual UINT Execute( tNetConnect* pConnect );
	virtual	PacketID_t	GetPacketID( ) const { return UDP_REQUEST_RESEND_PACKET; }

	UDP_RequestResendPart(){ mData = MEM_NEW DataBuffer(64); }
};

//-------------------------------------------------------------------------*/
class TM_WaitTcpResponseResendPart : public Logic::CEvent
{
public:
	virtual bool _OnTimeOver() override
	{
		ERROR_LOG("[%s:%d] Connect may be disconnect, part code >%u", mConnect->GetIp(), mConnect->GetPort(), mWaitCode);
		Hand<UDPByTcpConnect> conn = mConnect;
		if (conn)
			conn->OnConnectFault();
		return true;
	}

public:
	int mWaitCode;
	HandConnect mConnect;
};
class TM_WaitSendUDPCheckEvent : public Logic::CEvent
{
public:
	virtual bool _DoEvent() override
	{
		_OnTimeOver();
		return true;
	}

	virtual bool _OnTimeOver() override
	{
		if (++mCount<=10)
		{
			mpConnect->_sendUdpCheck(mCheckKey);
			WaitTime(2);
		}
		else
			ERROR_LOG("���Է���UDP׼����֤��ʱ, ����10��");
		return true;
	}

public:
	TM_WaitSendUDPCheckEvent()
		: mCount(0){}

	virtual void InitData() override
	{
		Logic::CEvent::InitData();
		mCount = 0;
	}

public:
	int mCount;
	UDPByTcpConnect *mpConnect;
	UInt64	mCheckKey;
};
//-------------------------------------------------------------------------*/

//-------------------------------------------------------------------------*
// ׼��ʱ�õ�����Ϣ
// 1 TCP���Ӻ�, ���͵��ͻ��˵�Ҫ��UDP��ʼ������֤
class TCP_NotifyUDPCheckKeyMsg : public DataPacket
{
public:
	virtual UINT Execute( tNetConnect* pConnect )
	{
		mData->seek(0);
		UInt64 checkKey = 0;
		mData->read(checkKey);
		Hand<UDPByTcpConnect> conn = pConnect;
		conn->StartSendUDPCheck(checkKey);
		return 0;
	}
	virtual	PacketID_t	GetPacketID( ) const { return TCP_NOTIFY_CHECK_KEY; }

public:
	TCP_NotifyUDPCheckKeyMsg()
	{
		mData = MEM_NEW DataBuffer(16);
	}
};
// 2 UDP�����׼���ú�,֪ͨ�ͻ���UDP׼��OK, ��ʼUDP����
class TCP_NotifyUDPReadyOkMsg : public DataPacket
{
public:
	virtual UINT Execute( tNetConnect* pConnect );
	virtual	PacketID_t	GetPacketID( ) const { return TCP_NOTIFY_READY_OK; }

	TCP_NotifyUDPReadyOkMsg(){ mData = MEM_NEW DataBuffer(); }
};
//-------------------------------------------------------------------------*/
#endif //_INCLUDE_UDPBYTCPNET_H_