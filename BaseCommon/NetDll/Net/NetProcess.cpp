#include "NetProcess.h"
#include "AsyncProtocol.h"


AutoNice tNetProcess::Await(tNetConnect *pConnect, int msgID, tRequestMsg &req, int overMilSecond)
{
	if (CORO == 0)
	{
		ERROR_LOG("AwaitConnect must in coro");
		return AutoNice();
	}
	Auto< AsyncProtocol> protocol = pConnect->GetNetHandle()->GetNetProtocol();
	AutoNice resp;
	AWaitResponse pWait = protocol->AllotEventID();
	req.SetPackectID(msgID);
	req.SetRequestID(pWait->mRequestMsgID);
	if (!pConnect->Send(&req, false))
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
			ERROR_LOG("Restore response msg data fail %d : msg ID %u", req.GetPacketID(), pWait->mRequestMsgID);
			return AutoNice();
		}
	}
	protocol->FreeServerEvent(pWait.getPtr());
	return resp;
}
