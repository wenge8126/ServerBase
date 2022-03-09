#pragma once

#include "Actor.h"
#include "NoSQLComponent.h"



//class NoSQLActor : public NetCloud::NoSQLActor
//{
//public:
//	//virtual void Init() override
//	//{
//	//	AddComponent("NoSQLComponent");
//	//}
//
//	//
//
//	//virtual void RegisterMsg(ActorManager *pActorMgr) 
//	//{	
//	//	//REG_ACTOR_MSG(NoSQLActor, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount);
//
//	//	//REG_COMP_MSG(DDCom, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount);
//
//	//	//pActorMgr->RegisterActorMsg("RQ_CheckAndCreateAccount", &Actor::OnMsg<AccountActor, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount>);
//
//	//	//pActorMgr->RegisterActorMsg("RQ_CheckAndCreateAccount", &Actor::OnComponentMsg<DDCom, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount>);
//
//	//	REG_COMPONENT(pActorMgr, NoSQLComponent);
//	//}
//
//
//};

