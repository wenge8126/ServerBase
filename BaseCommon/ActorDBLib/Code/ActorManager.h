#pragma once
#ifndef _INCLUDE_ACTORMANAGER_H_
#define _INCLUDE_ACTORMANAGER_H_

#include "Actor.h"
#include "DBTableManager.h"

namespace NetCloud
{
	class ActorManager;
	class ActorDBMgr : public DBTableManager
	{
	public:
		ActorManager *mpThread;

		ActorDBMgr(ActorManager *pThread)
			: mpThread(pThread) {}
	};

	class ActorDBLib_Export  ActorManager : public AutoBase
	{
	public:
		Hand<Actor> CreateActor(int actorType, Int64 id)
		{
			Auto<ActorFactory> fact = mFactoryList.find(actorType);
			if (fact)
			{
				Hand<Actor> actor = fact->_NewActor();
				actor->SetID(actorType, id);
				mNetNode->AppendUnit(actor.getPtr());
				return actor;
			}

			return Hand<Actor>();
		}

		void RegisterActor(int actorType, Auto<ActorFactory> factory)
		{
			factory->SetType(actorType);
			factory->mMgr = mSelfPtr;
			factory->RegisterMsg(this);
			mFactoryList.insert(actorType, factory);
		}

		void RegisterComponect(const char *szName, Hand<tEventFactory> factory);

		void RegisterMsg()
		{
			ActorRequestPacket::RegisterDBPacket(mNetNode.getPtr());
		}

		void RegisterActorMsg(const AString &msgName, pActorMsgCall  pFun)
		{
			mOnMsgFunctionList.insert(msgName, pFun);
		}


		void Close()
		{
			mShareDBManager->Close();
		
		}

		virtual void Process()
		{
			mShareDBManager->Process();
			mNetNode->Process();
		}

		virtual void LowProcess();

	public:
		ActorManager(const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2);
		~ActorManager()
		{
			mSelfPtr->mpMgr = nullptr;
		}

	public:
		FastHash<int, Auto<ActorFactory> >			mFactoryList;

		AutoActorMsgPtr										mSelfPtr;
		ANetNode													mNetNode;
		Auto<ActorDBMgr>									mShareDBManager;

		FastHash<AString, pActorMsgCall>			mOnMsgFunctionList;
	};

}

#endif //_INCLUDE_ACTORMANAGER_H_
