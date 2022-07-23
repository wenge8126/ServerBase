#include "ClientActor.h"



void ClientActor::On(SC_ActorRequestClientMsg &reqMsg, CS_ResponceServerActorMsg &clientResponse, UnitID sender, int)
{
	// 异步请求客户端再回复
	SC_ServerRequestClientMsg req;
	
	req.mRequestMsgData = reqMsg.mRequestMsgData;
	req.mClientActorID = reqMsg.mClientActorID;
	req.mRequestMsgName = reqMsg.mRequestMsgName;
	reqMsg.mRequestMsgData.setNull();

	AutoNice resp = tNetProcess::Await(mpClientConnect, SC_ServerRequestClientMsg::PACKET_REQUEST_CLIENTACTOR, req, 16000);
	if (resp)
	{
		NOTE_LOG("=====");
		
		clientResponse.mResponseMsgData = MEM_NEW DataBuffer();
		resp->serialize(clientResponse.mResponseMsgData.getPtr());
		
		
		NOTE_LOG("$$$$$$$ client response : %s", resp->dump().c_str());
	}
	else
	{
		ERROR_LOG("Test request fail");
	}
}
