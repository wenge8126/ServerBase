#pragma once

#include "Actor.h"
#include "MsgData.h"

class ToolActor : public NetCloud::Actor
{
public:
	virtual void Init() override
	{
		AddComponent("HttpReqeustComponent");
	}

	void Notify(RQ_CheckAndCreateAccount &msg, UnitID sender)
	{
		LOG("+++++++ %s", msg.dump().c_str());
	}

	virtual void RegisterMsg(ActorManager *pActorMgr)
	{
		//pActorMgr->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
		pActorMgr->RegisterActorMsg("RQ_CheckAndCreateAccount", &Actor::OnNotify<ToolActor, RQ_CheckAndCreateAccount>);
	}
};


