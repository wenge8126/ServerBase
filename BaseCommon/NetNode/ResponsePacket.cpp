#include "ResponsePacket.h"

namespace NetCloud
{

	UINT tResponsePacket::Execute(tNetConnect* pConnect)
	{
		tCloudPacket::Execute(pConnect);
		HandPacket p = CreatePacket(mFactoryPtr->mpFactory->GetResponsePacketID());
		Auto< tResponseResultPacket> respPacket = p;
		if (respPacket)
		{
			Response(respPacket.getPtr());
			respPacket->SetEventID(GetEventID());
			SendPacket(mSenderID, respPacket.getPtr());
		}
		else
			ERROR_LOG("xxxxxxxx %d is not ResponseResultPacket", mFactoryPtr->mpFactory->GetResponsePacketID());
		return 0;
	}

	UINT tResponseResultPacket::Execute(tNetConnect* pConnect)
	{
		tCloudPacket::Execute(pConnect);

		if (mNetUnit)
		{
			auto waitMgr = mNetUnit->GetWaitReqeustMgr();
			if (waitMgr)
			{
				waitMgr->OnResponseResult(this);
			}
			else
				ERROR_LOG("WaitReqeustMgr is NULL");
		}
		else
			ERROR_LOG("Unit is NULL");


		return 0;
	}

	UINT ResponseResultPartPacket::Execute(tNetConnect* pConnect)
	{
		tCloudPacket::Execute(pConnect);

		if (mNetUnit)
		{
			auto waitMgr = mNetUnit->GetWaitReqeustMgr();
			if (waitMgr)
			{
				waitMgr->OnResponseResultPartData(this, pConnect);
			}
			else
				ERROR_LOG("WaitReqeustMgr is NULL");
		}
		else
			ERROR_LOG("Unit is NULL");


		return 0;
	}

}
