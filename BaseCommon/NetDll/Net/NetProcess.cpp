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
	AWaitResponse pWait = protocol->AllotWaitID();
	
	req.SetRequestID(pWait->mRequestMsgID);
	if (!pConnect->Send(msgID, &req))
		return resp;
	//pWait->mWaitCoroID = CORO;
	pWait->Wait(overMilSecond);
	
	//YIELD;
	HandPacket respPacket = pWait->mResonseWaiter.AWait(HandPacket());
	
	pWait->StopWait();
	if (respPacket)
	{
		Auto< ResponseMsgPacket> pResp = respPacket;
		resp = MEM_NEW NiceData();
		pResp->mData.seek(0);
		if (!resp->restore(&pResp->mData))
		{
			ERROR_LOG("Restore response msg data fail %s : msg ID %u", req.GetMsgName(), pWait->mRequestMsgID);
			return AutoNice();
		}
	}
	protocol->FreeWaitID(pWait.getPtr());
	return resp;
}

Auto<ResponseMsgPacket> tNetProcess::AwaitRequest(tNetConnect *pConnect, int msgID, tRequestMsg &req, int overMilSecond)
{
	if (CORO == 0)
	{
		ERROR_LOG("AwaitConnect must in coro");
		return AutoData();
	}
	Auto< AsyncProtocol> protocol = pConnect->GetNetHandle()->GetNetProtocol();
	Auto< ResponseMsgPacket> resp;
	AWaitResponse pWait = protocol->AllotWaitID();

	req.SetRequestID(pWait->mRequestMsgID);
	if (!pConnect->Send(msgID, &req))
		return resp;
	//pWait->mWaitCoroID = CORO;
	pWait->Wait(overMilSecond);

	//YIELD;
	HandPacket respPacket = pWait->mResonseWaiter.AWait(HandPacket());
	pWait->StopWait();
	if (respPacket)
	{
		Auto< ResponseMsgPacket> pResp = respPacket;

		pResp->mData.seek(0);
		resp = pResp;
	}

	protocol->FreeWaitID(pWait.getPtr());
	return resp;
}
