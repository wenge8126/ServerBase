#include "AsyncNode.h"

#include "AsyncGate.h"



AsyncNode::AsyncNode()
{
	mNodeNet = MEM_NEW MeshNet(0);
	mNodeNet->mNodeClientNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eNodeMsg_UnitList, RQ_RequestUnitListInfo, RS_UnitInfoList, AsyncNode>(this));
	mNodeNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eNodeMsg_UnitList, RQ_RequestUnitListInfo, RS_UnitInfoList, AsyncNode>(this));

	mNodeNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory < eGateMsg_NotifyNodeInfo, MS_NotifyNodeInfo, AsyncNode>(this));
}

void AsyncNode::ConnectGate(const char *szGateIP, int nPort)
{
	CoroutineTool::AsyncCall([&]()
	{
		auto connect = mNodeNet->mNodeClientNet->AwaitConnect(szGateIP, nPort, 10);
		if (connect)
		{
			RQ_RequestGateInfo req;
			RS_GateListInfo resp;
			if (tNetProcess::Await(connect.getPtr(), eGateMsg_RequestGateInfo, req, resp, 10))
			{
				mGateCount = resp.mGateCount;
				// 加入Gate列表
				MeshNet::AConnectData gateData = connect->GetUserData();
				mGateList.Append(gateData->mNodeCode, gateData);

				for (int i = 0; i < resp.mGateList.size(); ++i)
				{
					if (!mGateList[i])
					{
						NetAddress addr(resp.mGateList[i]);
						ConnectGate(addr.Ip().c_str(), addr.GetPort());
					}
				}
			}
		}
		else
			ERROR_LOG("Connect gate fail : [%s:%d]", szGateIP, nPort);
	});
}

void AsyncNode::On(tNetConnect *pConnect, MS_NotifyNodeInfo &msg)
{
	CoroutineTool::AsyncCall([&]()
	{
		NetAddress addr(msg.mNodeKey);
		HandConnect conn = mNodeNet->AwaitConnectNode(addr.Ip().c_str(), addr.GetPort(), 6000);
		if (conn)
		{
			RQ_RequestUnitListInfo req;
			RS_UnitInfoList resp;
			if (tNetProcess::Await(conn.getPtr(), eNodeMsg_UnitList, req, resp, 6000))
			{
				auto d = conn->GetUserData();
				for (int i = 0; i < resp.mUnitList.size(); ++i)
				{
					UInt64 key = resp.mUnitList[i];
					mUnitList.insert(key, d);
				}
			}
		}
	});
}
