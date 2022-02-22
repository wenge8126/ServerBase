#include "NetNode.h"
#include "NetUnit.h"
#include "CoroutineTool.h"

namespace NetCloud
{

	void NetCloud::tNetNode::OnReceive(ANodeConnect scrNodeConnect, NodePacket *pData)
	{
		AUnit  unit = FindUnit(pData->mTargetID);
		if (unit)
		{
			if (pData->OnBeforeRun(unit.getPtr()))
				return;
			//OnReceiveProcess(targetAddress, pData);
			unit->OnReceiveProcess(pData);
			return; 
		}
		// 消息不是本节点时, 如果节点为Gate则转发数据, 一般节点则直接发向对应的Gate, 避免一直不能到达目标			
		SendToGate(pData->mTargetID, pData);

		if (scrNodeConnect)
		{
			DEBUG_LOG(">>> Process scr node %s packet %s", scrNodeConnect->GetAddress().dump().c_str(), pData->GetMsgName());
			// 通知此地址已经无效
			NotifyInvalidNode(scrNodeConnect, pData->mTargetID);
		}
		//else
			// 有可能直接接收的Gate传来的

	}

	void tNetNode::Process()
	{
		mWaitRequestManager->Process();
		if (mWaitRunPacket.size() > 0)
		{
			for (int i = 0; i < mWaitRunPacket.size(); ++i)
			{
				Auto<NodePacket> pNodePak = mWaitRunPacket[i];
				if (pNodePak)
					RunLocalPacket(pNodePak.getPtr());
			}
			mWaitRunPacket.clear(false);
		}
	}

	bool tNetNode::SendToGate(UnitID targetID, Packet *p)
	{
		AGateConnect gate = FindGateByID(targetID);
		if (gate)
		{
			NodePacket *pNodePak = dynamic_cast<NodePacket*>(p);
			if (pNodePak != NULL)
				pNodePak->OnBeforeSend(gate.getPtr());
			gate->SendToGate(targetID, p);
			return true;
		}
		ERROR_LOG("No exist gate of %lld", targetID);
		return false;
	}

	bool tNetNode::SendToTarget(NodePacket *pData)
	{
		if (pData->mbBroadcast!=eBroadcastNone)
		{
			BroadcastMsg(pData);
			return true;
		}

		UnitID targetID = pData->mTargetID;
		ANodeConnect nodeNode = FindNodeConnectByID(targetID);
		if (nodeNode)
		{
			pData->OnBeforeSend(nodeNode.getPtr());
			if (nodeNode->SendToNode(targetID, pData))
				return true;
		}
		// 是否在本节点
		tNetUnit *pUnit = FindUnit(pData->mTargetID);
		if (pUnit != NULL)
		{
			if (CORO == 0)
				RunLocalPacket(pData);
			else
				mWaitRunPacket.push_back(pData);
			
			return true;
		}
		// 直接发送到对应的Gate
		AGateConnect gateConn = FindGateByID(targetID);
		if (gateConn)
		{
			pData->OnBeforeSend(gateConn.getPtr());
			gateConn->SendToGate(targetID, pData);

			RequestNodeInfo(gateConn, targetID);
			return true;
		}
		else
			ERROR_LOG("No exist gate of id  %s", targetID.dump().c_str());
		return false;
	}

	void tNetNode::OnReceiveGate(NodePacket *pData)
	{
		if (pData->mbBroadcast)
		{
			OnReceiveBroadcastMsg(pData);
			return;
		}

		// 一般应该为该节点的消息
		AUnit  unit = FindUnit(pData->mTargetID);
		if (unit)
		{
			unit->OnReceiveProcess(pData);
		}
		else
			ERROR_LOG("No exist unit of Gate send msg");
	}

	void tNetNode::RunLocalPacket(NodePacket *pData)
	{
		tNetUnit *pUnit = FindUnit(pData->mTargetID);
		if (pUnit == NULL)
		{
			ERROR_LOG("Run local packet %s fail, target unit no exist %s", pData->GetMsgName(), pData->mTargetID.dump().c_str());
			return;
		}
		// 必须反序列出, 才可以正常获取在本节点执行的消息(发送与接受不同的包)
		Auto<NodePacket> pNodePak = CreatePacket(pData->GetFactory()->GetPacketID());
		pNodePak->mTargetID = pData->mTargetID;
		pNodePak->mSenderID = pData->mSenderID;

		pNodePak->mbBroadcast = 0;
		mTempLoopData.CleanUp();
		if (pData->Write(mTempLoopData) == TRUE)
		{
			pNodePak->Read(mTempLoopData, mTempLoopData.dataSize(), NULL);
			pNodePak->OnBeforeRun(pUnit);
			pUnit->OnReceiveProcess(pNodePak.getPtr());
		}
		else
			ERROR_LOG("%s write error", pData->GetMsgName());
	}

	tNetNode::tNetNode()
	{

		mWaitRequestManager = MEM_NEW WaitRequestManager();
		mDBResultData = MEM_NEW NiceData();
	}

	tNetNode::~tNetNode()
	{
		for (int i = 0; i < mWaitRunPacket.size(); ++i)
		{
			mWaitRunPacket[i]._free();
		}
		mWaitRequestManager->Close();
		mWaitRequestManager._free();
	}

}