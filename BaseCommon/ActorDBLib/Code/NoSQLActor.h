#ifndef _INCLUDE_NOSQL_ACTOR_H_
#define _INCLUDE_NOSQL_ACTOR_H_

#pragma once

#include "NoSQLComponent.h"

namespace NetCloud
{

	class NoSQLActor : public Actor
	{
	public:
		virtual void Init() override
		{
			AddComponent("NoSQLComponent");
			//AddComponent("DBMySQLComponent");
		}

	public:
		virtual void RegisterMsg(ActorManager *pActorMgr) override
		{
			REG_COMPONENT(pActorMgr, NoSQLComponent);
			//REG_COMPONENT(pActorMgr, DBMySQLComponent);
		}
	};
	//-------------------------------------------------------------------------
	class NoSQLUserActor : public Actor
	{

	public:
		virtual void Init() override
		{
			AddComponent("NoSQLUserComponent");
		}

	public:
		virtual void RegisterMsg(ActorManager *pActorMgr) override
		{
			REG_COMPONENT(pActorMgr, NoSQLUserComponent);
		}
		//-------------------------------------------------------------------------
	};
}

#endif //_INCLUDE_NOSQL_ACTOR_H_
