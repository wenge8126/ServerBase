#include "AsyncNode.h"

#include "AsyncGate.h"

class AsyncNodeMeshNet : public MeshNet
{
public:
	virtual void OnNodeClose(AConnectData closeNode) override
	{
		for (auto it = mpNode->mUnitNodeIndex.begin(); it; )
		{
			auto nodeData = it.get();
			if (!nodeData || nodeData == closeNode)
				it.erase();
			else
				++it;
		}
	}

public:
	AsyncNodeMeshNet(AsyncNode *pNode)
		: MeshNet(0)
		, mpNode(pNode) { }

public:
	AsyncNode *mpNode;
};

AsyncNode::AsyncNode()
{
	mNodeNet = MEM_NEW AsyncNodeMeshNet(this);

	mNodeNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW GateTransferPacketFactory(this));
	mNodeNet->mNodeClientNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW GateTransferPacketFactory(this));


	mNodeNet->mNodeClientNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eN2N_RequestUnitList, NN_RequestUnitListInfo, NN_ResponseUnitInfoList, AsyncNode>(this));
	mNodeNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eN2N_RequestUnitList, NN_RequestUnitListInfo, NN_ResponseUnitInfoList, AsyncNode>(this));

	mNodeNet->mNodeClientNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory < eG2N_NotifyNodeInfo, GN_NotifyNodeInfo, AsyncNode>(this));

	mNodeNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory<eN2N_BroadcastUnitNoExist, NN_BroadcastUnitNoExist, AsyncNode>(this));
	mNodeNet->mNodeClientNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory<eN2N_BroadcastUnitNoExist, NN_BroadcastUnitNoExist, AsyncNode>(this));

}

void AsyncNode::CloseNode()
{
	mNodeNet->mbClose = true;
	NG_NotifyNodeClose msg;
	for (int i = 0; i < mGateList.size(); ++i)
	{
		auto gateData = mGateList[i];
		if (gateData)
			gateData->mpConnect->Send(eN2G_NotifyNodeClose, &msg);
	}
	mNodeNet->StopNet();
	//CoroutineTool::AsyncCall([=]()
	//{
	//	tTimer::AWaitTime(3000);
	//	mNodeNet->StopNet();
	//});
}

void AsyncNode::ConnectGate(const char *szGateIP, int nPort, int overMilSecond)
{
	CoroutineTool::AsyncCall([=]()
	{
		AwaitConnectGate(szGateIP, nPort, overMilSecond);
	});
}

bool AsyncNode::AwaitConnectGate(const char *szGateIP, int nPort, int overMilSecond)
{
	auto connect = mNodeNet->mNodeClientNet->AwaitConnect(szGateIP, nPort, overMilSecond);
	if (connect)
	{
		NG_RequestGateInfo  req;
		req.mNodeKey = mNodeNet->mKey;
		GN_ResponseGateListInfo resp;
		if (tNetProcess::Await(connect.getPtr(), eN2G_RequestGateInfo, req, resp, overMilSecond))
		{
			mGateCount = resp.mGateCount;
			// 加入Gate列表
			MeshNet::AConnectData gateData = MEM_NEW  MeshNet::MeshConnectData();
			gateData->mpConnect = connect.getPtr();
			connect->SetUserData(gateData);

			mGateList.Append(gateData->mNodeCode, gateData);

			for (int i = mWaitAppendList.size() - 1; i >= 0; --i)
			{
				auto unit = mWaitAppendList[i];
				if (unit)
				{
					if (AppendUnit(unit))
						mWaitAppendList.removeAt(i);
				}
				else
					mWaitAppendList.removeAt(i);
			}

			for (int i = 0; i < resp.mGateList.size(); ++i)
			{
				if (!mGateList[i])
				{
					NetAddress addr(resp.mGateList[i]);
					ConnectGate(addr.Ip().c_str(), addr.GetPort(), overMilSecond);
				}
			}
			return true;
		}
	}
	else
		ERROR_LOG("Connect gate fail : [%s:%d]", szGateIP, nPort);
	return false;
}

bool AsyncNode::AppendUnit(AUnit unit)
{
	auto gate = GetGate(unit->GetID());
	if (gate)
	{
		NG_AppendUnit msg;
		msg.mUintID = unit->GetID();
		if (gate->mpConnect->Send(eN2G_AppendUnit, &msg))
		{
			unit->mNode = mAutoThis;
			mUnitList.insert(unit->GetID(), unit);
			return true;
		}
		else
			ERROR_LOG("Send MS_AppendUnit %s fail", unit->GetID().dump().c_str());
	}
	else
		ERROR_LOG("No exist unit %s gate %d", unit->GetID().dump().c_str(), unit->GetID().Hash(mGateCount));
	mWaitAppendList.push_back(unit);
	return false;
}

void AsyncNode::BroadcastUnitNoExist(UnitID id)
{
	NN_BroadcastUnitNoExist msg;
	msg.mNoExistUnitID = id;
	auto &nodeList = mNodeNet->GetMeshNodeList();
	for (auto it = nodeList.begin(); it; ++it)
	{
		auto nodeData = it.get();
		if (nodeData && nodeData->IsValid())
		{
			nodeData->mpConnect->Send(eN2N_BroadcastUnitNoExist, &msg);
		}
	}
}

void AsyncNode::On(tNetConnect *pConnect, GN_NotifyNodeInfo &msg)
{
	if (mNodeNet->GetKey()==msg.mNodeKey)
		return;

	CoroutineTool::AsyncCall([&]()
	{
		MeshNet::AConnectData nodeData =  mNodeNet->GetMeshNodeList().find(msg.mNodeKey);
		if (!nodeData)
		{
			NetAddress addr(msg.mNodeKey);
			HandConnect conn = mNodeNet->AwaitConnectNode(addr.Ip().c_str(), addr.GetPort(), 6000);
			if (conn)
				nodeData = conn->GetUserData();
		}

		if (nodeData && nodeData->IsValid())
		{
			NN_RequestUnitListInfo  req;
			NN_ResponseUnitInfoList resp;
			if (tNetProcess::Await(nodeData->mpConnect, eN2N_RequestUnitList, req, resp, 6000))
			{
				for (int i = 0; i < resp.mUnitList.size(); ++i)
				{
					UInt64 key = resp.mUnitList[i];
					mUnitNodeIndex.insert(key, nodeData);
				}
			}
		}
	});
}
