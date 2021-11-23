#pragma once

#ifndef _INCLUDE_ACTOR_H_
#define _INCLUDE_ACTOR_H_

#include "BigDataEvent.h"
#include "ActorMsg.h"
#include "BaseMsg.h"
#include "ActorMsg.h"
#include "ArrayList.h"
#include "DBTableManager.h"
#include "Component.h"
#include "EasyMap.h"

#define ACTOR_UPDATE_DATA_SECOND		(10)

#define REG_ACTOR_MSG(ActorClass, RQ, RS)		pActorMgr->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
#define REG_COMP_MSG(ComponentClass, RQ, RS)		pActorMgr->RegisterActorMsg(#RQ, &Actor::OnComponentMsg<ComponentClass, RQ, RS>);

namespace NetCloud
{
	class ActorManager;
	class ActorMgrPtr : public AutoBase
	{
	public:
		ActorManager		*mpMgr;
	};

	typedef Auto<ActorMgrPtr>	AutoActorMsgPtr;

	class ActorDBLib_Export ActorFactory : public AutoBase
	{
	public:
		virtual AUnit NewActor() = 0;
		//Call pActorMsr->RegisterActorMsg(msgName, fun)
		virtual void RegisterMsg(ActorManager *pActorMgr) = 0;

		virtual AUnit _NewActor();

		void SetType(int actorType) { mActorType = actorType; }
		virtual int GetType() const { return mActorType; }

	public:
		AutoActorMsgPtr		mMgr;
		int								mActorType = 0;
	};

	class ActorDBLib_Export Actor : public BigMsgUnit
	{
		friend class Component;
		friend class ProcessComponect;
		friend class ActorFactory;

	public:
		virtual void Init() {}

		AComponent AddComponent(const AString &compName);

		bool RemoveComponent(const AString &compName);

		AComponent GetComponent(const AString &compName)
		{
			return mComponentList.find(compName);
		}

	public:
		template<typename RespMsg>
		Auto<RespMsg> Await(tBaseMsg &reqestMsg, UnitID targetID,  int waitMilSecond)
		{
			Auto< ActorRequestPacket> requestPak = GetMgr()->mNetNode->CreatePacket(eMsgRequest);
			requestPak->mNetUnit = this;
			Auto<ActorResponResultPacket> resultPak =  requestPak->Await(reqestMsg, targetID, waitMilSecond);
			if (resultPak)
			{
				Auto<RespMsg> msg = MEM_NEW RespMsg();
				resultPak->mResultData->seek(0);
				if (!msg->restore(resultPak->mResultData.getPtr()))
				{
					ERROR_LOG("Restore %s fail", msg->GetMsgName());
				}
				return msg;
			}
			return Auto<RespMsg>();
		}

		// 处理消息函数
		//template<typename ReqMsg, typename RespMsg>
		//virtual void On(ReqMsg &reqest, RespMsg &resp) = 0;		

		template<typename T, typename ReqMsg, typename RespMsg>
		static int OnMsg(Actor *pActor, DataStream *pReqestMsgData, ActorResponResultPacket *pResponse)
		{			
			ReqMsg pMsg;			
			pReqestMsgData->seek(0);
			if (!pMsg.restore(pReqestMsgData))
			{
				ERROR_LOG("%s restore fail", pMsg.GetMsgName());
				return eSourceCodeError;
			}
			RespMsg respMsg;
			T *p = dynamic_cast<T*>(pActor);
			if (p == NULL)
			{
				ERROR_LOG("%s parent class is not Actor", typeid(T).name());
				return eSourceCodeError;
			}
			p->On(pMsg, respMsg, pResponse->mSenderID);
			pResponse->mResultType = eNoneError;
			//LOG("Respons msg %s:\r\n%s", respMsg.GetMsgName(), respMsg.dump().c_str());
			pResponse->mResultData->clear();
			respMsg.serialize(pResponse->mResultData.getPtr());

			return pResponse->mResultType;
		}

		template<typename T, typename ReqMsg, typename RespMsg>
		static int OnComponentMsg(Actor *pActor, DataStream *pReqestMsgData, ActorResponResultPacket *pResponse)
		{
			ReqMsg pMsg;
			pReqestMsgData->seek(0);
			if (!pMsg.restore(pReqestMsgData))
			{
				ERROR_LOG("%s restore fail", pMsg.GetMsgName());
				return eSourceCodeError;
			}
			RespMsg respMsg;

			AComponent comp;
			for (int i = 0; i < pActor->mComponentList.size(); ++i)
			{
				AComponent c = pActor->mComponentList.get(i);
				const char *p = typeid(T).name();
				const char *p2 = typeid(*c.getPtr()).name();
				if ( strcmp(typeid(T).name(), typeid(*c.getPtr()).name())==0)
				{
					comp = c;
					break;
				}
			}
			if (!comp)
			{
				ERROR_LOG("No exist componect <%s>", typeid(T).name());
				return 0;
			}

			T *p = dynamic_cast<T*>(comp.getPtr());
			if (p == NULL)
			{
				ERROR_LOG("%s parent class is not Componect", typeid(T).name());
				return eSourceCodeError;
			}
			p->On(pMsg, respMsg, pResponse->mSenderID);
			pResponse->mResultType = eNoneError;
			//LOG("Respons msg %s:\r\n%s", respMsg.GetMsgName(), respMsg.dump().c_str());
			pResponse->mResultData->clear();
			respMsg.serialize(pResponse->mResultData.getPtr());

			return pResponse->mResultType;
		}

		virtual bool OnReceiveProcess(NodePacket *pNodePacket) override
		{
			return pNodePacket->Execute(NULL) == 0;			
			//return BigMsgUnit::OnReceiveProcess(pNodePacket);
		}

		virtual void RegisterMsg(ActorManager *pActorMgr) {}

	public:
		ActorManager* GetMgr()
		{
			if (mActorFactory)
				return mActorFactory->mMgr->mpMgr;
			return NULL;
		}

		AutoDBManager GetDBMgr();

		virtual void LowProcess()
		{
			for (int i = 0; i < mComponentList.size(); ++i)
			{
				AComponent comp = mComponentList.get(i);
				if (comp)
					comp->LowUpdate();
			}
			if (TimeManager::Now() - mLastUpdateDataTimeSec >= ACTOR_UPDATE_DATA_SECOND)
			{
				mLastUpdateDataTimeSec = TimeManager::Now();
				for (int i=0; i<mDataRecordList.size(); ++i)
				{
					mDataRecordList[i]->SaveUpdate();
				}
			}
		}

		ARecord LoadRecord(const char *szTableName, const char *szKey);

		ARecord LoadRecord(const char *szTableName, Int64 nKey);

	public:
		Actor() {}
		~Actor()
		{

		}

		virtual void Release() override
		{
			if (!mComponentList.empty())
			{
				int lastPos = mComponentList.size()-1;
				AComponent comp = mComponentList.get(lastPos);
				mComponentList._remove(lastPos);
				comp._free();
			}

			BigMsgUnit::Release();
		}

	protected:
		Auto<ActorFactory>	mActorFactory;
		ArrayList<ARecord> mDataRecordList;
		UInt64						mLastUpdateDataTimeSec = 0;

		EasyMap<AString, AComponent> mComponentList;
		
	};

	typedef Hand<Actor>		HandActor;
}

template<typename T>
class DefineActorFactory : public ActorFactory
{
public:
	virtual AUnit NewActor() { return MEM_NEW T(); }
	//Call pActorMsr->RegisterActorMsg(msgName, fun)
	virtual void RegisterMsg(ActorManager *pActorMgr) override
	{
		HandActor actor = NewActor();
		actor->RegisterMsg(pActorMgr);
	}
};

#endif //_INCLUDE_ACTOR_H_
