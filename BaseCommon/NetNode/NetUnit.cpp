#include "NetUnit.h"
#include "EventCenter.h"
#include "AsyncNode.h"
#include "AsyncGate.h"

namespace NetCloud
{
	Hand<AsyncNode> GetNetNode(AProcess node)
	{
		if (node)
			return node->mpProcess;
		return Hand<AsyncNode>();
	}

	AutoEvent tNetUnit::StartEvent(const char *szEventName)
	{
		Logic::tEventCenter *pCenter = GetEventCenter();
		if (pCenter != NULL)
		{
			AutoEvent  evt = pCenter->StartEvent(szEventName);
			if (evt)
			{
				evt->OnEvent(this, typeid(tNetUnit));
				return evt;
			}
			else
				ERROR_LOG("No register event %s", szEventName);
		}
		else
			ERROR_LOG("Event center is NULL");

		return AutoEvent();
	}

	bool tNetUnit::Send(UnitID targetID, Packet *p, BROADCAST_MODE bBroadcast)
	{
		Auto<TransferPacket> pNodePak = CreateNodePacket(); // mNode->CreateNodePacket();
		pNodePak->mTargetID = targetID;
		pNodePak->mSenderID = mID;
		pNodePak->mTargetPacketID = p->GetFactory()->GetPacketID();
		pNodePak->mbBroadcast = bBroadcast;


		GetNetNode(mNode)->mTempLoopData.CleanUp();
		if (p->Write(GetNetNode(mNode)->mTempLoopData) == TRUE)
		{
			pNodePak->GetData()._write(GetNetNode(mNode)->mTempLoopData.data(), GetNetNode(mNode)->mTempLoopData.dataSize());
			return SendTo(pNodePak.getPtr());
		}
		else
			ERROR_LOG("%s write error", p->GetMsgName());
		return false;
	}

	bool tNetUnit::SendEvent(UnitID targetID, Logic::tEvent *pEvent, BROADCAST_MODE bBroadcast)
	{
		Auto<TransferPacket> pNodePak = CreateNodePacket(); //  mNode->CreateNodePacket();
		AutoEvent evt = pEvent;
		pNodePak->mData.seek(0);
		pEvent->GetEventCenter()->SerializeMsg(evt, &pNodePak->mData);
		pNodePak->mTargetID = targetID;
		pNodePak->mSenderID = mID;
		pNodePak->mTargetPacketID = PACKET_EVENT;
		pNodePak->mbBroadcast = bBroadcast;
		
		// NOTE_LOG("tNetUnit SendEvent, targetID:(%d,%d) mID:(%d,%d)", targetID.type, targetID.id, mID.type, mID.id);
		return SendTo(pNodePak.getPtr());
		
	}

	bool tNetUnit::StartSend(UnitID targetID, Logic::tEvent *pEvent)
	{
		if (pEvent == NULL)
			return false;
		pEvent->_OnEvent(this, typeid(tNetUnit));
		pEvent->_OnEvent(&targetID, typeid(UnitID));
		pEvent->Start();
		return true;
	}

	bool tNetUnit::SendPacket(UnitID targetID, Packet *pNodePacket, BROADCAST_MODE bBroadcast)
	{
		NodePacket *pNodePak = dynamic_cast<NodePacket*>(pNodePacket);
		if (pNodePak == NULL)
		{
			ERROR_LOG("%s is not NodePakcet", pNodePak->GetMsgName());
			return false;
		}
		pNodePak->mTargetID = targetID;
		pNodePak->mSenderID = mID;
		pNodePak->mbBroadcast = bBroadcast;
		return SendTo(pNodePak);
	}

	void tNetUnit::StartTransferNode()
	{
		//if (mNode)
		//{
		//	AGateConnect gate = mNode->FindGateByID(mID);
		//	if (gate)
		//	{
		//		AutoEvent evt = gate->StartEvent("NG_NotifyBeginJumpUnit");
		//		if (evt)
		//		{
		//			evt["UNIT_ID"] = (UInt64)mID;
		//			evt->Start();
		//			return;
		//		}
		//		ERROR_LOG("No exist NG_NotifyBeginJumpUnit ");
		//	}
		//	else
		//		ERROR_LOG("No exist Gate %lld type %d ", mID.id, mID.type);
		//}
		//else
			ERROR_LOG("No exist Node %lld type %d ", mID.id, mID.type);
	}

	Auto<NetCloud::TransferPacket> tNetUnit::CreateNodePacket()
	{
		if (mNode)
		{
			Hand<AsyncNode> node = mNode->mpProcess;
			if (node)
				return node->mNodeNet->GetNetProtocol()->CreatePacket(eNGN_TransferMsg);
		}

		return Auto<TransferPacket>();
	}

	void tNetUnit::SetID(int type, UInt64 id)
	{
		mID.type = type; 
		mID.id = id;

		if (mNode)
		{
			Hand<AsyncNode> node = mNode->mpProcess;
			if (node)
				node->AppendUnit(this);
		}
	}

	bool tNetUnit::SendTo(NodePacket *p)
	{
		p->OnStartSend(this);
		if (mNode)
		{
			Hand<AsyncNode> node = mNode->mpProcess;
			//return mNode->SendToTarget(p);				
			if (node)
				return node->Send(p->GetFactory()->GetPacketID(), p);
			ERROR_LOG("Node is not exist");
		}
		else
		{
			ERROR_LOG("No set node connect or gate connect");
		}
		return false;
	}

}
