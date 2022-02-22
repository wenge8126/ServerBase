#include "AsyncProtocol.h"
#include "DefinePacketFactory.h"


//-------------------------------------------------------------------------
#define _MOVE_BIT 24
static const UINT msMaxPosValue = ~((0xFFFFFFFF >> _MOVE_BIT) << _MOVE_BIT);

AsyncProtocol::AsyncProtocol()
{
	EventNetProtocol  x;
	RegisterNetPacket(MEM_NEW DefinePacketFactory<ResponseMsgPacket, PACKET_RESPONSE_MSG>());
}

AWaitResponse AsyncProtocol::AllotWaitID()
{
	UINT x = 0;
	if (!mIDStack.empty())
	{
		x = mIDStack.pop();
		// 自增高8位
		UINT pos = x & msMaxPosValue;
		UINT code = (x >> _MOVE_BIT) + 1;
		x = (code << _MOVE_BIT) + pos;
		AssertEx(mEventList[pos], "Event no free id %u", pos);
		AWaitResponse p = mEventList[pos];
		p->mRequestMsgID = x;
		return p;
		//INFO_LOG(" [%s] Start wait pos [%u], code [%u]", serverEvent->GetEventNameInfo().c_str(), pos, code); 
	}
	else
	{
		AssertNote(mEventList.size() < msMaxPosValue, "Max wait event count must less  pow(2, 24) ");

		x = mEventList.size();
		AWaitResponse resp = MEM_NEW WaitResponse();
		resp->mRequestMsgID = x;
		mEventList.push_back(resp);
		return resp;
		//INFO_LOG(" [%s] Start wait pos [%u], code [%u]", serverEvent->GetEventNameInfo().c_str(), mEventList.size()-1, 0); 
	}
}

void AsyncProtocol::FreeWaitID(WaitResponse *pWaitResponse)
{
	//if (pWaitResponse->mRequestMsgID <= 0)
	//	return;

	UINT pos = pWaitResponse->mRequestMsgID & msMaxPosValue;

	if (pos >= 0 && (DSIZE)pos < mEventList.size())
	{
		AWaitResponse p = mEventList[pos];
		if (p.getPtr() == pWaitResponse)
		{
			//mEventList[pos] = NULL;
			mIDStack.push(pWaitResponse->mRequestMsgID);
			p->Init();
			INFO_LOG(" [%s] Free wait pos [%u], code [%u]", serverEvent->GetEventNameInfo().c_str(), pos, serverEvent->mID >> _MOVE_BIT);
		}
	}
}

AWaitResponse AsyncProtocol::FindWaitResponse(MSG_ID id)
{
 	UINT x = id & msMaxPosValue;
	if (x >= 0 && (DSIZE)x < mEventList.size())
	{
		AWaitResponse p = mEventList[x];
		if (p && p->mRequestMsgID == id)
		{
			return p;
		}
	}
	return NULL;
}

UINT ResponseMsgPacket::Execute(tNetConnect* pConnect)
{
	Auto< AsyncProtocol> protocol = pConnect->GetNetHandle()->GetNetProtocol();
	AWaitResponse pWait = protocol->FindWaitResponse(mRequestID);
	if (pWait)
	{
		pWait->mResponsePacket = this;
		if (pWait->mWaitCoroID>0)
			RESUME(pWait->mWaitCoroID);
	}
	else
		WARN_LOG("Wait msg no exist : %u", mRequestID);

	return 0;
}
