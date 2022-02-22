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
		// ��Ϣ���Ǳ��ڵ�ʱ, ����ڵ�ΪGate��ת������, һ��ڵ���ֱ�ӷ����Ӧ��Gate, ����һֱ���ܵ���Ŀ��			
		SendToGate(pData->mTargetID, pData);

		if (scrNodeConnect)
		{
			DEBUG_LOG(">>> Process scr node %s packet %s", scrNodeConnect->GetAddress().dump().c_str(), pData->GetMsgName());
			// ֪ͨ�˵�ַ�Ѿ���Ч
			NotifyInvalidNode(scrNodeConnect, pData->mTargetID);
		}
		//else
			// �п���ֱ�ӽ��յ�Gate������

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
		// �Ƿ��ڱ��ڵ�
		tNetUnit *pUnit = FindUnit(pData->mTargetID);
		if (pUnit != NULL)
		{
			if (CORO == 0)
				RunLocalPacket(pData);
			else
				mWaitRunPacket.push_back(pData);
			
			return true;
		}
		// ֱ�ӷ��͵���Ӧ��Gate
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

		// һ��Ӧ��Ϊ�ýڵ����Ϣ
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
		// ���뷴���г�, �ſ���������ȡ�ڱ��ڵ�ִ�е���Ϣ(��������ܲ�ͬ�İ�)
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