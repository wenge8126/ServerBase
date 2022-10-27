#pragma once

#include "SCActor.h"
#include "ServerMsg.h"

using namespace NetCloud;

class GameCenterActor : public SCActor
{
public:
	void On(LW_RequestEnterServer &request, WL_ResponseEnterServer &response, UnitID sender, int)
	{
		// 分配后, 请求GameServerActor 创建PlayerActor
		response.mServerID = 1;

		WG_RequestCreatePlayerActor requestCreatePlayerActor;
		requestCreatePlayerActor.mDBID = request.mDBID;
		GW_ResponseCreatePlayer responseResult;
		if (Await({ Actor_GameServer, 1 }, requestCreatePlayerActor, responseResult, GATE_REQUEST_OVERMILSECOND))
		{
			response.error = eErrorCodeNone;
		}
		else
			response.error = eErrorCode_GateNetMsgOverTime;
	}

	void RegisterMsg() override
	{
		REG_ACTOR_MSG(GameCenterActor, LW_RequestEnterServer, WL_ResponseEnterServer);
	}
};
