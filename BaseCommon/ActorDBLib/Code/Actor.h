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

/*/-------------------------------------------------------------------------
Actor
1 异步请求消息RPC
2 通知消息, 支持广播(全部, 按类型)
3 支持异步DB, 结合ShareSQLUpdate 可以实现安全落地
4 在节点中跳转
//-------------------------------------------------------------------------*/
#define ACTOR_UPDATE_DATA_SECOND		(10)

// 注册Actor
#define REG_ACTOR(pActorMgr, actorType, ActorClass) pActorMgr->RegisterActor(actorType, MEM_NEW DefineActorFactory<ActorClass>());

// 注册组件
#define REG_COMPONENT(pActorMgr, ComponentClass) pActorMgr->RegisterComponect(#ComponentClass, MEM_NEW Logic::EventFactory<ComponentClass>());

// 注册Actor消息处理
#define REG_ACTOR_MSG(ActorClass, RQ, RS)		pActorMgr->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
#define REG_NOTIFY_MSG(ActorClass, MSG)		pActorMgr->RegisterNotifyMsg(#MSG, &Actor::OnNotify<ActorClass, MSG>);
// 注册组件消息处理
#define REG_COMP_MSG(ComponentClass, RQ, RS)		pActorMgr->RegisterActorMsg(#RQ, &Actor::OnComponentMsg<ComponentClass, RQ, RS>);
#define REG_COMP_NOTIFY(ComponentClass, MSG)		pActorMgr->RegisterNotifyMsg(#MSG, &Actor::OnComponentNotify<ComponentClass, MSG>);
//-------------------------------------------------------------------------
namespace NetCloud
{
	class ActorManager;
	class ActorMgrPtr : public AutoBase
	{
	public:
		ActorManager		*mpMgr;
	};

	typedef Auto<ActorMgrPtr>	AutoActorMsgPtr;
	//-------------------------------------------------------------------------
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

	//-------------------------------------------------------------------------
	class ActorDBLib_Export Actor : public BigMsgUnit
	{
		friend class Component;
		friend class ProcessComponent;
		friend class ActorFactory;

	public:
		virtual void Init() {}

		AComponent AddComponent(const AString &compName);

		bool RemoveComponent(const AString &compName);

		AComponent GetComponent(const AString &compName)
		{
			return mComponentList.find(compName);
		}

		template<typename T>
		Hand<T> GetComponent()
		{
			for (int i=0; i<mComponentList.size(); ++i)
			{
				Hand<T> comp = mComponentList.get(i);
				if (comp)
					return comp;
			}
			return Hand<T>();
		}

	public:
		// RPC request msg
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
	
		AutoNice Await(const AString &requestMsgName, tNiceData &reqestMsg, UnitID targetID, int waitMilSecond);

		bool SendMsg(tBaseMsg &msg, UnitID targetID, BROADCAST_MODE eMode = eBroadcastNone)
		{
			return SendMsg(msg.GetMsgName(), msg, targetID, eMode);
		}

		bool SendMsg(const AString &msgName, tBaseMsg &msg, UnitID targetID, BROADCAST_MODE eMode = eBroadcastNone);

		// 处理RPC消息函数
		//template<typename ReqMsg, typename RespMsg>
		//virtual void On(ReqMsg &reqest, RespMsg &resp) = 0;		
		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
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

		// 处理一般通知消息
		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnNotify<ActorClass, RQ>);
		template<typename T, typename Msg>
		static void OnNotify(Actor *pActor, DataStream *pMsgData, UnitID senderID)
		{
			Msg pMsg;
			pMsgData->seek(0);
			if (!pMsg.restore(pMsgData))
			{
				ERROR_LOG("%s restore fail", pMsg.GetMsgName());
				return;
			}
			T *p = dynamic_cast<T*>(pActor);
			if (p == NULL)
			{
				ERROR_LOG("%s parent class is not Actor", typeid(T).name());
				return;
			}
			p->Notify(pMsg, senderID);			
		}

		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnComponentNotify<ComponentClass, RQ>);
		template<typename T, typename Msg>
		static void OnComponentNotify(Actor *pActor, DataStream *pMsgData, UnitID senderID)
		{
			Msg pMsg;
			pMsgData->seek(0);
			if (!pMsg.restore(pMsgData))
			{
				ERROR_LOG("%s restore fail", pMsg.GetMsgName());
				return;
			}
			Hand<T> comp = pActor->GetComponent<T>();
			if (!comp)
			{
				ERROR_LOG("%s parent class is not Componect %s", typeid(*pActor).name(), typeid(T).name());
				return;
			}
			comp->Notify(pMsg, senderID);
		}

		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnComponentMsg<ComponentClass, RQ, RS>);
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

			Hand<T> comp = pActor->GetComponent<T>();		
			if (!comp)
			{
				ERROR_LOG("%s parent class is not Componect %s", typeid(*pActor).name(), typeid(T).name());
				return eSourceCodeError;
			}
			comp->On(pMsg, respMsg, pResponse->mSenderID);
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

		virtual AutoDBManager GetDBMgr()
		{
			AssertNote(0, "Must inherit DBActor");
			return NULL;
		}

		virtual void LowProcess()
		{
			for (int i = 0; i < mComponentList.size(); ++i)
			{
				AComponent comp = mComponentList.get(i);
				if (comp)
					comp->LowUpdate();
			}
		}

		virtual ARecord LoadRecord(const char *szTableName, const char *szKey) { AssertNote(0, "Must inherit DBActor"); return ARecord(); }
		virtual ARecord LoadRecord(const char *szTableName, Int64 nKey) { AssertNote(0, "Must inherit DBActor"); return ARecord(); }

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

	//-------------------------------------------------------------------------
	class ActorDBLib_Export DBActor : public Actor
	{
	public:
		ARecord LoadRecord(const char *szTableName, const char *szKey);
		ARecord LoadRecord(const char *szTableName, Int64 nKey);

		virtual AutoDBManager GetDBMgr() override;

	public:
		DBActor() {}
		~DBActor()
		{

		}

		virtual void LowProcess() override
		{
			Actor::LowProcess();
			if (TimeManager::Now() - mLastUpdateDataTimeSec >= ACTOR_UPDATE_DATA_SECOND)
			{
				mLastUpdateDataTimeSec = TimeManager::Now();
				for (int i = 0; i < mDataRecordList.size(); ++i)
				{
					mDataRecordList[i]->SaveUpdate();
				}
			}
		}

	protected:
		ArrayList<ARecord> mDataRecordList;

	};
	//-------------------------------------------------------------------------
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
