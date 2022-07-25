#pragma once

#include "Actor.h"
#include "MsgData.h"

class FFComp : public Component
{
public:
	void Notify(RQ_CheckAndCreateAccount &msg, UnitID sender, int)
	{
		LOG("cccccccc %s", msg.dump().c_str());
	}
};

class ToolActor : public NetCloud::Actor
{
public:
	virtual void Init() override
	{
		AddComponent("HttpReqeustComponent");
		AddComponent("FFComp");
	}

	void Notify(RQ_CheckAndCreateAccount &msg, UnitID sender)
	{
		LOG("+++++++ %s", msg.dump().c_str());
	}

	virtual void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_COMPONENT( FFComp);
		//pActorMgr->RegisterComponect("FFComp", MEM_NEW Logic::EventFactory<FFComp>());
		//pActorMgr->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
		REG_COMP_NOTIFY(FFComp, RQ_CheckAndCreateAccount);
		//pActorMgr->RegisterNotifyMsg("RQ_CheckAndCreateAccount", &Actor::OnComponentNotify<FFComp, RQ_CheckAndCreateAccount>);
	}
};


