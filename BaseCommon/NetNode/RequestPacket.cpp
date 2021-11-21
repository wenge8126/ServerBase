#include "RequestPacket.h"
#include "ResponsePacket.h"

namespace NetCloud
{
	//-------------------------------------------------------------------------*/

	void NetCloud::tRequestPacket::AllotEventIDAndWait(int overTimeMilSecond)
	{
		FreeID();

		if (mNetUnit)
		{
			auto waitMgr = mNetUnit->GetWaitReqeustMgr();
			if (waitMgr)
			{
				mWaitRequestManager = waitMgr;
				waitMgr->AllotEventIDAndWait(this, overTimeMilSecond);
			}
			else
				ERROR_LOG("WaitReqeustMgr is NULL");
		}
		else
			ERROR_LOG("Unit is NULL");
	}


	void tRequestPacket::FreeID()
	{
		if (mID != 0)
		{
			if (mWaitRequestManager)
				mWaitRequestManager->FreeServerEvent(this);
			else
				ERROR_LOG("WaitReqeustMgr is NULL");
		}
	}

}