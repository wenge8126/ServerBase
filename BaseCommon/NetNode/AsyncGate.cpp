#include "AsyncGate.h"

class GateTransferPacket : public TransferPacket
{
public:
	virtual UINT Execute(tNetConnect* pConnect) override
	{
		mGate->ProcessPacket(pConnect, this);
		return 0;
	}

public:
	Hand<AsyncGate> mGate;
};

class GateNetProtocol : public AsyncProtocol
{
public:
	virtual HandPacket CreatePacket(PacketID packetID) override
	{
		HandPacket pak = AsyncProtocol::CreatePacket(packetID);
		if (pak)
			return pak;
		Auto<GateTransferPacket> p = mTransferPacketFactory->CreatePacket();
		p->mGate = mGate->mpProcess;
		return p;
	}

public:
	GateNetProtocol()
	{
		mTransferPacketFactory = MEM_NEW DefinePacketFactory<GateTransferPacket, 0>();
	}

public:
	AProcess mGate;
	AutoPacketFactory mTransferPacketFactory;
};

AsyncGate::AsyncGate(size_t maxConnectCount /*= NET_CONNECT_MAX*/, int threadNum /*= _IOCP_THREAD_NUM*/)
{
	mGateNet = MEM_NEW MeshNet(maxConnectCount, threadNum);
	
	mGateNet->SetNetProtocol(MEM_NEW GateNetProtocol());

	mGateNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eGateMsg_RequestGateInfo, RQ_RequestGateInfo, RS_GateListInfo, AsyncGate>(this));
	mGateNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory<eGateMsg_AppendUnit, MS_AppendUnit, AsyncGate>(this));
}
