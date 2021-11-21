#include "UDPServerClientNet.h"
#include "ServerIPInfo.h"

void UDPByTcpServerNet::OnReceiveUdpMsg(UInt64 scrAddr, DataStream *receiveData, int revSize)
{
	Hand<UDPByTcpConnect> conn = mUDPConnectList.find(scrAddr);
	if (conn)
		conn->OnReceiveUDPData((DataBuffer*)receiveData, revSize, true);
	else
	{
		// ����ǵ�һ�η������˵�KEY, ���������λ��
		receiveData->setDataSize(revSize);
		receiveData->seek(0);
		byte msgType = 0;
		if (receiveData->read(msgType) && msgType==UDP_CHECK_KEY)
		{							
			UInt64 checkCode;
			if (receiveData->read(checkCode))					
				StartReadyUDPConnect(checkCode, scrAddr);
		}	
	}
}

bool UDPByTcpServerNet::StartReadyUDPConnect(UInt64 checkKey, UInt64 scrAddr)
{
	NOTE_LOG("���յ� ��֤�ظ� %s > %llu", ServerIPInfo::GetAddrInfo(scrAddr).c_str(), checkKey);
	Hand<UDPByTcpServerConnect> p = mWaitUDPResponseList.find(checkKey);
	if (p)
	{
		p->mWaitResponseCheckKey._free();
		p->SetUDPReadyOk(true);
		mUDPConnectList.insert(scrAddr, p);
		mWaitUDPResponseList.erase(checkKey);

		p->mTargetAddr = scrAddr;
		// ʹ��UDP,TCPͬʱ���͵�һ��UDP�ɹ�����Ϣ
		p->SendFirstReadyOkMsg();

		return true;
	}
	return false;
}

HandConnect UDPByTcpServerNet::CreateConnect()
{
	return MEM_NEW UDPByTcpServerConnect();
}

bool UDPByTcpServerNet::OnAddConnect(tNetConnect *pConnect)
{
	IOCPServerNet::OnAddConnect(pConnect);
	// ����UDPУ��KEY, ������
	UInt64 key = GetEventCenter()->AutoAlloctKey();
	Hand<UDPByTcpServerConnect> conn = pConnect;
	conn->StartSendUDPReadyReqest(key);
	mWaitUDPResponseList.insert(key, pConnect);
	return true;
}

UDPByTcpServerNet::UDPByTcpServerNet()
{
	mUDPNet = MEM_NEW _UDPForTcpNet(this);
	UDPByTcpConnect::RegisterMsgPacket(GetNetProtocol());
}

void UDPByTcpClientConnect::OnConnected()
{
	IOCPConnect::OnConnected();
	//mUDPNet->Start(NULL, 0);
	mUDPNet->mpUDPReceiveThread->InitThread();
	mTargetAddr = ServerIPInfo::IP2Num(GetIp(), 0, GetPort());
}

UDPByTcpClientConnect::UDPByTcpClientConnect()
{
	mUDPNet = MEM_NEW _UDPForClientNet(this);
}

void UDPByTcpServerConnect::SendFirstReadyOkMsg()
{
	NOTE_LOG("[%s:%d] UDP׼��OK, ��Ŀ�귢��OK >%s", GetIp(), GetPort(), ServerIPInfo::GetAddrInfo(mTargetAddr).c_str());
	HandPacket p = GetNetHandle()->GetNetProtocol()->CreatePacket(TCP_NOTIFY_READY_OK);
	_sendByTcp(p);
}
