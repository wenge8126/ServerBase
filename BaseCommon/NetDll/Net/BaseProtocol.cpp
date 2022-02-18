#include "BaseProtocol.h"



bool AsyncProtocol::ProcessReceivePacket(tNetConnect *pConnect, Packet *pPacket)
{

	if (pPacket->GetPacketID() == PACKET_RESPONSE_MSG)
	{
		Auto<ResponseMsgPacket> p = pPacket;
		AWaitResponse pWait = FindWaitResponse(p->mRequestID);
		if (pWait)
		{
			pWait->mResponsePacket = pPacket;
			RESUME(pWait->mWaitCoroID);
		}
		else
			WARN_LOG("Wait msg no exist : %u", p->mRequestID);

		return true;
	}
	return EventNetProtocol::ProcessReceivePacket(pConnect, pPacket);
}

AutoNice AsyncProtocol::Await(tNetConnect *pConnect, HandPacket req, int overMilSecond)
{
	AutoNice resp;
	AWaitResponse pWait = AllotEventID();

	if (!pConnect->Send(req.getPtr(), false))
		return resp;
	pWait->mWaitCoroID = CORO;
	pWait->Wait(overMilSecond);

	YIELD;

	if (pWait->mResponsePacket)
	{
		Auto< ResponseMsgPacket> pResp = pWait->mResponsePacket;
		resp = MEM_NEW NiceData();
		pResp->mData.seek(0);
		if (!resp->restore(&pResp->mData))
		{
			ERROR_LOG("Restore response msg data fail %d : msg ID %u", req->GetID(), pWait->mRequestMsgID);
			return AutoNice();
		}
	}
	FreeServerEvent(pWait.getPtr());
	return resp;
}
//-------------------------------------------------------------------------
#define _MOVE_BIT 24
static const UINT msMaxPosValue = ~((0xFFFFFFFF >> _MOVE_BIT) << _MOVE_BIT);

AsyncProtocol::AWaitResponse AsyncProtocol::AllotEventID()
{
	UINT x = 0;
	if (!mIDStack.empty())
	{
		x = mIDStack.pop();
		// 自增高8位
		UINT pos = x & msMaxPosValue;
		UINT code = (x >> _MOVE_BIT) + 1;
		x = (code << _MOVE_BIT) + pos;
		AssertNote(mEventList[pos] == NULL, "Event no free id %u", pos);
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

void AsyncProtocol::FreeServerEvent(WaitResponse *pWaitResponse)
{
	if (pWaitResponse->mRequestMsgID <= 0)
		return;

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

AsyncProtocol::AWaitResponse AsyncProtocol::FindWaitResponse(MSG_ID id)
{
	UINT x = id & msMaxPosValue;
	if (x > 0 && (DSIZE)x < mEventList.size())
	{
		AWaitResponse p = mEventList[x];
		if (p && p->mRequestMsgID == id)
		{
			return p;
		}
	}
	return NULL;
}
