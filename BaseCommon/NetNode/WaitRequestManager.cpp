#include "WaitRequestManager.h"
#include "RequestPacket.h"
#include "ResponsePacket.h"

namespace NetCloud
{

	void WaitRequestManager::OnResponseResult(tResponseResultPacket *pResultPacket)
	{
		HandPacket waitPacket = FindServerEvent((EVENT_ID)pResultPacket->GetEventID());
		Auto<tRequestPacket> requestPacket = waitPacket;
		if (requestPacket)
		{
			FreeServerEvent(requestPacket.getPtr());
			requestPacket->SetFinish(true);
			requestPacket->OnResponse(pResultPacket);
		}
		else
			DEBUG_LOG("Response result %d, but wait request packet no exist", pResultPacket->GetPacketID());
	}


	void WaitRequestManager::OnResponseResultPartData(tCloudPacket *pResultPacket, tNetConnect* pConnect)
	{
		HandPacket waitPacket = FindServerEvent((EVENT_ID)pResultPacket->GetEventID());
		Auto<tRequestPacket> requestPacket = waitPacket;
		if (requestPacket)
		{
			requestPacket->OnResponsePartData(pResultPacket, pConnect);
		}
		else
			DEBUG_LOG("Response result %d, but wait request packet no exist", pResultPacket->GetPacketID());
	}

	class Net_Export WaitRequestTimer : public tTimer
	{
	public:
		WaitRequestTimer(tRequestPacket *pPacket)
			: mPacket(pPacket)
		{}

		virtual void onTime() override
		{
			Auto<tRequestPacket> p = mPacket;
			if (p)
			{
				if (!p->IsFinish())
					p->OnTime();
			}
		}

	protected:
		HandPacket mPacket;
	};


#define _MOVE_BIT 20
	static const UINT msMaxPosValue = ~((0xFFFFFFFF >> _MOVE_BIT) << _MOVE_BIT);
	void WaitRequestManager::AllotEventIDAndWait(tRequestPacket *serverEvent, int waitMilSecond)
	{
		if (serverEvent->GetEventID() > 0)
		{
			ERROR_LOG("%d Now packet event ID != 0, may be no call InitData", serverEvent->GetPacketID());
			return;
		}
		UINT x = 0;
		if (!mIDStack.empty())
		{
			x = mIDStack.pop();
			///自增高12位
			UINT pos = x & msMaxPosValue;
			UINT code = (x >> _MOVE_BIT) + 1;
			x = (code << _MOVE_BIT) + pos;
			AssertNote(mWaitRequestPacketHash[pos] == NULL, "Event no free id %u", pos);
			mWaitRequestPacketHash[pos] = serverEvent;
			INFO_LOG(" [%d] Start wait pos [%u], code [%u]", serverEvent->GetPacketID(), pos, code);
		}
		else
		{
			AssertNote(mWaitRequestPacketHash.size() < msMaxPosValue, "Max wait event count must less 104万");

			x = mWaitRequestPacketHash.size();
			mWaitRequestPacketHash.push_back(serverEvent);
			INFO_LOG(" [%d] Start wait pos [%u], code [%u]", serverEvent->GetPacketID(), mWaitRequestPacketHash.size() - 1, 0);
		}
		serverEvent->SetEventID( x );

		Auto<WaitRequestTimer>		pTime = MEM_NEW WaitRequestTimer(serverEvent);
		pTime->Wait(mTimeCenter.getPtr(), waitMilSecond);
	}

	void WaitRequestManager::FreeServerEvent(tRequestPacket *serverEvent)
	{
		if (serverEvent->GetEventID() <= 0)
			return;

		UINT pos = serverEvent->GetEventID()&msMaxPosValue;

		if (pos >= 0 && (DSIZE)pos < mWaitRequestPacketHash.size())
		{
			Packet *p = mWaitRequestPacketHash[pos];
			if (p == serverEvent)
			{
				mWaitRequestPacketHash[pos] = NULL;
				mIDStack.push(serverEvent->GetEventID());
				INFO_LOG(" [%d] Free wait pos [%u], code [%u]", serverEvent->GetPacketID(), pos, serverEvent->GetEventID() >> _MOVE_BIT);
				serverEvent->SetEventID( 0 );
			}
		}
	}

	Packet* WaitRequestManager::FindServerEvent(EVENT_ID id)
	{
		UINT x = id & msMaxPosValue;
		if (x > 0 && (DSIZE)x < mWaitRequestPacketHash.size())
		{
			Packet *p = mWaitRequestPacketHash[x];
			if (p != NULL && (EVENT_ID)p->GetID() == id)
			{
				return p;
			}
		}
		return NULL;
	}
}