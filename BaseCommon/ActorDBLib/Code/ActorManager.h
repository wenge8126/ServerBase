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
		ActorManager *mpActorMgr;

		ActorDBMgr(ActorManager *pThread)
			: mpActorMgr(pThread) {}
	};

	class ActorDBLib_Export  ActorManager : public AutoBase
	{
		friend class ProcessComponent;

	public:
		Hand<Actor> CreateActor(int actorType, Int64 id)
		{
			Auto<ActorFactory> fact = mFactoryList.find(actorType);
			if (fact)
			{
				Hand<Actor> actor = fact->_NewActor();
				actor->SetID(actorType, id);
				mNetNode->AppendUnit(actor.getPtr());
				actor->Init();
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

		void RegisterActorMsg(const AString &notifyMsgName, pActorNotifyMsgCall  pFun)
		{
			mOnNotifyMsgFunctionList.insert(notifyMsgName, pFun);
		}


		virtual void Process()
		{
			mNetNode->Process();

			for (int i=0; i<mProcessComponectList.size(); )
			{
				AProcessComponect comp = mProcessComponectList[i];
				if (comp)
				{
					comp->Process();
					++i;
				}
				else
					mProcessComponectList.removeAt(i);
			}
		}

		virtual void LowProcess();

	public:
		ActorManager(const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2);
		~ActorManager()
		{
			mSelfPtr->mpMgr = nullptr;
		}

		virtual Auto<ActorDBMgr> GetDBMgr() { AssertNote(0, "This is not DBActorManager"); return Auto<ActorDBMgr>(); }

	public:
		FastHash<int, Auto<ActorFactory> >			mFactoryList;

		AutoActorMsgPtr										mSelfPtr;
		ANetNode													mNetNode;

		FastHash<AString, pActorMsgCall>			mOnMsgFunctionList;
		FastHash<AString, pActorNotifyMsgCall>	mOnNotifyMsgFunctionList;

	protected:
		ArrayList<AProcessComponect>				mProcessComponectList;	//Actor组件, 需要高速Process的列表
	};

	// 具有DB功能的ActorManager
	class ActorDBLib_Export  DBActorManager : public ActorManager
	{
	public:
		void Close()
		{
			if (mShareDBManager)
				mShareDBManager->Close();
		}

		virtual void Process() override
		{
			mShareDBManager->Process();
			
			ActorManager::Process();
		}

		virtual void LowProcess() override
		{
			mShareDBManager->LowProcess();
			ActorManager::LowProcess();
		}

	public:
		DBActorManager(const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
			: ActorManager(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
		{
			mShareDBManager = MEM_NEW ActorDBMgr(this);
		}
		~DBActorManager()
		{
			Close();
		}

		virtual Auto<ActorDBMgr> GetDBMgr() override { return mShareDBManager; }

	public:
		Auto<ActorDBMgr>									mShareDBManager;

	};
	//-------------------------------------------------------------------------

}

#endif //_INCLUDE_ACTORMANAGER_H_
