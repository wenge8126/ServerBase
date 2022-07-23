#pragma once
#ifndef _INCLUDE_SCACTOR_H_
#define _INCLUDE_SCACTOR_H_

#include "Actor.h"
#include "ServerClientMsg.h"

// 具有直接与客户端Actor通讯功能
class ActorDBLib_Export_H SCActor : public NetCloud::Actor
{
public:
	template<typename RespMsg>
	bool AwaitClient(UnitID clientActorID, tBaseMsg &requestMsg, RespMsg &responseMsg, int overMilSecond)
	{
		SC_ActorRequestClientMsg msg;
		msg.mClientActorID = clientActorID;
		msg.mRequestMsgName = requestMsg.GetMsgName();
		msg.mRequestMsgData = MEM_NEW DataBuffer();
		requestMsg.serialize(msg.mRequestMsgData.getPtr());

		CS_ResponceServerActorMsg resp;

		if (Await(UnitID(Actor_Client, 1), msg, resp, overMilSecond) && resp.mResponseMsgData)
		{
			resp.mResponseMsgData->seek(0);
			responseMsg.restore(resp.mResponseMsgData.getPtr());

			return true;
		}
		else
			ERROR_LOG("AwaitClient fail");

		return false;
	}
};

#endif // _INCLUDE_SCACTOR_H_