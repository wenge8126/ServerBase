#include "ClientActor.h"



void tClientActor::On(SC_ActorRequestClientMsg &reqMsg, CS_ResponceServerActorMsg &clientResponse, UnitID sender, int)
{
	// 异步请求客户端再回复
	SC_ServerRequestClientMsg req;
	
	req.mRequestMsgData = reqMsg.mRequestMsgData;
	req.mClientActorID = reqMsg.mClientActorID;
	req.mRequestMsgName = reqMsg.mRequestMsgName;
	reqMsg.mRequestMsgData.setNull();

	auto resp = tNetProcess::AwaitRequest(mpClientConnect, eMsg_RequestClientActor, req, 16000);
	if (resp)
	{				
		clientResponse.mResponseMsgData = MEM_NEW DataBuffer(resp->mData.dataSize());
		clientResponse.mResponseMsgData->_write(resp->mData.data(), resp->mData.dataSize());				
	}
	else
	{
		ERROR_LOG("Test request fail");
	}
}
