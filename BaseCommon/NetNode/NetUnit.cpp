#include "NetUnit.h"
#include "EventCenter.h"

namespace NetCloud
{


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
		Auto<TransferPacket> pNodePak = mNode->CreateNodePacket();
		pNodePak->mTargetID = targetID;
		pNodePak->mSenderID = mID;
		pNodePak->mTargetPacketID = p->GetPacketID();
		pNodePak->mbBroadcast = bBroadcast;
		mNode->mTempLoopData.CleanUp();
		if (p->Write(mNode->mTempLoopData) == TRUE)
		{
			pNodePak->GetData()._write(mNode->mTempLoopData.data(), mNode->mTempLoopData.dataSize());
			return SendTo(pNodePak.getPtr());
		}
		else
			ERROR_LOG("%d write error", p->GetPacketID());
		return false;
	}

	bool tNetUnit::SendEvent(UnitID targetID, Logic::tEvent *pEvent, BROADCAST_MODE bBroadcast)
	{
		Auto<TransferPacket> pNodePak = mNode->CreateNodePacket();
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
			ERROR_LOG("%d is not NodePakcet", pNodePak->GetPacketID());
			return false;
		}
		pNodePak->mTargetID = targetID;
		pNodePak->mSenderID = mID;
		pNodePak->mbBroadcast = bBroadcast;
		return SendTo(pNodePak);
	}

	void tNetUnit::StartTransferNode()
	{
		if (mNode)
		{
			AGateConnect gate = mNode->FindGateByID(mID);
			if (gate)
			{
				AutoEvent evt = gate->StartEvent("NG_NotifyBeginJumpUnit");
				if (evt)
				{
					evt["UNIT_ID"] = (UInt64)mID;
					evt->Start();
					return;
				}
				ERROR_LOG("No exist NG_NotifyBeginJumpUnit ");
			}
			else
				ERROR_LOG("No exist Gate %lld type %d ", mID.id, mID.type);
		}
		else
			ERROR_LOG("No exist Node %lld type %d ", mID.id, mID.type);
	}

}
