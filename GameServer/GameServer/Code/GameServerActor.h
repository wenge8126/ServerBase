#pragma once

#include "SCActor.h"
#include "ServerMsg.h"

using namespace NetCloud;

class GameServerActor : public SCActor
{
public:
	void On(WG_RequestCreatePlayerActor &request, GW_ResponseCreatePlayer &response, UnitID sender, int)
	{
		GetMgr()->CreateActor(Actor_Player, request.mDBID);
		response.error = eErrorCodeNone;
	}

	void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_ACTOR_MSG(GameServerActor, WG_RequestCreatePlayerActor, GW_ResponseCreatePlayer);
	}
};
