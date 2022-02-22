#include "AsyncGate.h"



//class GateNetProtocol : public AsyncProtocol
//{
//public:
//	virtual HandPacket CreatePacket(PacketID packetID) override
//	{
//		HandPacket pak = AsyncProtocol::CreatePacket(packetID);
//		if (pak)
//			return pak;
//		Auto<GateTransferPacket> p = mTransferPacketFactory->CreatePacket();
//		p->mGate = mGate->mpProcess;
//		return p;
//	}
//
//public:
//	GateNetProtocol()
//	{
//		mTransferPacketFactory = MEM_NEW DefinePacketFactory<GateTransferPacket, 0>();
//	}
//
//public:
//	AProcess mGate;
//	AutoPacketFactory mTransferPacketFactory;
//};

AsyncGate::AsyncGate(size_t maxConnectCount /*= NET_CONNECT_MAX*/, int threadNum /*= _IOCP_THREAD_NUM*/)
{
	mGateNet = MEM_NEW MeshNet(maxConnectCount, threadNum);
	
	mGateNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW GateTransferPacketFactory(this));
	mGateNet->mNodeClientNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW GateTransferPacketFactory(this));

	mGateNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eN2G_RequestGateInfo, NG_RequestGateInfo, GN_ResponseGateListInfo, AsyncGate>(this));
	mGateNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory<eN2G_AppendUnit, NG_AppendUnit, AsyncGate>(this));
}
