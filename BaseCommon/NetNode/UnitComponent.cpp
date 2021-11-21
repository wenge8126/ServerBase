#include "UnitComponent.h"
#include "EventCenter.h"

namespace NetCloud
{

	bool NetCloud::NetUnit::OnReceiveProcess(NodePacket *pNodePacket)
	{
		Auto<TransferPacket> tranPacket = pNodePacket;
		if (tranPacket && tranPacket->mTargetPacketID == PACKET_EVENT)
		{
			tranPacket->mData.seek(0);
			AutoEvent evt = GetEventCenter()->RestoreMsg(&tranPacket->mData);
			if (evt)
			{				
				evt->_OnEvent(this, typeid(tNetUnit));
				evt->_OnEvent(&pNodePacket->mSenderID, typeid(UnitID));
				//evt["TARGET_ID"] = (UInt64)pNodePacket->mSenderID;
				evt->DoEvent();
			}
			else
				ERROR_LOG("Msg event resore fail");
			return true;
		}		
		else
		{
			//ERROR_LOG("Receive node NodePacket fail, is not TransferPacket && mTargetPacketID != PACKET_EVENT");
		}
		return false;
	}


}