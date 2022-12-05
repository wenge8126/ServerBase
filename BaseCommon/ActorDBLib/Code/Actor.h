#pragma once

#ifndef _INCLUDE_ACTOR_H_
#define _INCLUDE_ACTOR_H_

//#include "BigDataEvent.h"
//#include "ActorMsg.h"
#include "BaseMsg.h"
//#include "ActorMsg.h"
#include "ArrayList.h"
#include "DBTableManager.h"
#include "Component.h"
#include "EasyMap.h"
#include "AsyncNode.h"
#include "AsyncProtocol.h"

/*/-------------------------------------------------------------------------
Actor
1 异步请求消息RPC
2 通知消息, 支持广播(全部, 按类型)
3 支持异步DB, 结合ShareSQLUpdate 可以实现安全落地
4 在节点中跳转
特别注意:
	1 相同的消息可以注册到不同类型的Actor
	2 不同组件不可共用想同的消息, 会出现覆盖, 其中一个组件的消息得不到执行, 可直接 继承消息定义一个新的消息类型使用到不同的组件
//-------------------------------------------------------------------------*/
#define ACTOR_UPDATE_DATA_SECOND		(10)

// 注册Actor
#define REG_ACTOR(pActorMgr, actorType, ActorClass) pActorMgr->RegisterActor(actorType, MEM_NEW DefineActorFactory<ActorClass>());

// 注册组件
#define REG_COMPONENT(ComponentClass) GetMgr()->RegisterComponect(#ComponentClass, MEM_NEW Logic::EventFactory<ComponentClass>());

// 注册Actor消息处理
#define REG_ACTOR_MSG(ActorClass, RQ, RS)		mActorFactory->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
#define REG_NOTIFY_MSG(ActorClass, MSG)		mActorFactory->RegisterNotifyMsg(#MSG, &Actor::OnNotify<ActorClass, MSG>);
// 注册组件消息处理
#define REG_COMP_MSG(ComponentClass, RQ, RS)		GetMgr()->RegisterActorComMsg(#RQ, &Actor::OnComponentMsg<ComponentClass, RQ, RS>);
#define REG_COMP_NOTIFY(ComponentClass, MSG)		GetMgr()->RegisterComNotifyMsg(#MSG, &Actor::OnComponentNotify<ComponentClass, MSG>);
//-------------------------------------------------------------------------
enum ActorMsgType
{
	eActorMsg_Notify = 0,
	eActorMsg_Reqeust = 1,
	eActorMsg_response = 2,
};


class AsyncNode;

namespace NetCloud
{
	class ActorManager;
	class ActorMgrPtr : public AutoBase
	{
	public:
		ActorManager		*mpMgr;
	};

	typedef Auto<ActorMgrPtr>	AutoActorMsgPtr;

	typedef int(*pActorMsgCall)(Actor*, DataStream*, TransferPacket*, int);
	typedef void(*pActorNotifyMsgCall)(Actor*, DataStream*, UnitID, int);

	//-------------------------------------------------------------------------
	class ActorDBLib_Export ActorFactory : public AutoBase
	{
	public:
		virtual AUnit NewActor() = 0;
		//Call pActorMsr->RegisterActorMsg(msgName, fun)
		virtual void RegisterMsg() = 0;

		virtual AUnit _NewActor();

		void SetType(int actorType) { mActorType = actorType; }
		virtual int GetType() const { return mActorType; }

		void RegisterActorMsg(const AString &msgName, pActorMsgCall  pFun)
		{
			if (mOnMsgFunctionList.exist(msgName))
				WARN_LOG("Aleardy exist actor request msg [%s]", msgName.c_str());
			mOnMsgFunctionList.insert(msgName, pFun);
		}

		void RegisterNotifyMsg(const AString &notifyMsgName, pActorNotifyMsgCall  pFun)
		{
			if (mOnNotifyMsgFunctionList.exist(notifyMsgName))
				WARN_LOG("Aleardy exist actor notify msg [%s]", notifyMsgName.c_str());
			mOnNotifyMsgFunctionList.insert(notifyMsgName, pFun);
		}

	public:
		AutoActorMsgPtr		mMgr;
		int								mActorType = 0;

		FastHash<AString, pActorMsgCall>			mOnMsgFunctionList;
		FastHash<AString, pActorNotifyMsgCall>	mOnNotifyMsgFunctionList;
	};

	//-------------------------------------------------------------------------
	class ActorDBLib_Export Actor : public tNetUnit
	{
		friend class Component;
		friend class ProcessComponent;
		friend class ActorFactory;

		typedef tNetUnit Base;

	public:
		virtual void Init() {}

		AComponent AddComponent(const AString &compName);

		bool RemoveComponent(const AString &compName);

		AComponent GetComponent(const AString &compName)
		{
			return mComponentList.find(compName);
		}

		AComponent GetComponent(int nCompNameIndex)
		{
			for (int i = 0; i < mComponentList.size(); ++i)
			{
				AComponent comp = mComponentList.get(i);
				if (comp && comp->GetEventFactory()->GetNameIndex()==nCompNameIndex)
					return comp;
			}
			return AComponent();
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
		bool Await(UnitID targetID, tBaseMsg &reqestMsg, RespMsg &respMsg,  int waitMilSecond)
		{
			if (CORO == 0)
			{
				ERROR_LOG("Await reqeust must in coro");
				return AutoNice();
			}

			AsyncNode *pNetNode = GetNetNode();
			if (pNetNode == NULL)
			{
				ERROR_LOG("Actor %s Node is NULL, May be not append node", GetID().dump().c_str());
				return AutoNice();
			}

			Auto<AsyncProtocol> protocol = pNetNode->mNodeNet->GetNetProtocol();

			Auto<TransferPacket> tranPak = protocol->CreatePacket(eNGN_TransferMsg);
			tranPak->mData.clear(false);
			// 在数据开关写入请求消息名称
			tranPak->mData.write(reqestMsg.GetAttachValue());
			tranPak->mData.writeString(reqestMsg.GetMsgName());
			reqestMsg.serialize(&tranPak->mData);
			AutoNice respNice = Await(tranPak, targetID, waitMilSecond);
			if (respNice)
			{
				respMsg.Full(respNice);
				return true;
			}
			return false;
		}
	
	
		AutoNice Await(UnitID targetID, const AString &requestMsgName, tNiceData &reqestMsg, int waitMilSecond, int nCompIndex)
		{
			if (CORO == 0)
			{
				ERROR_LOG("AwaitConnect must in coro");
				return AutoNice();
			}

			AsyncNode *pNetNode = GetNetNode();
			if (pNetNode == NULL)
			{
				ERROR_LOG("Actor %s Node is NULL, May be not append node", GetID().dump().c_str());
				return AutoNice();
			}

			Auto< AsyncProtocol> protocol = pNetNode->mNodeNet->GetNetProtocol();		

			Auto<TransferPacket> tranPak = protocol->CreatePacket(eNGN_TransferMsg);
			tranPak->mData.clear(false);
			// 在数据开关写入请求消息名称
			tranPak->mData.write(nCompIndex);
			tranPak->mData.writeString(requestMsgName);
			reqestMsg.serialize(&tranPak->mData);
			return Await(tranPak, targetID, waitMilSecond);
		}

		AutoNice Await(UnitID targetID, const AString &requestMsgName, DataStream *pRequestMsgData, int waitMilSecond, int nCompIndex)
		{
			if (CORO == 0)
			{
				ERROR_LOG("AwaitConnect must in coro");
				return AutoNice();
			}

			AsyncNode *pNetNode = GetNetNode();
			if (pNetNode == NULL)
			{
				ERROR_LOG("Actor %s Node is NULL, May be not append node", GetID().dump().c_str());
				return AutoNice();
			}

			Auto< AsyncProtocol> protocol = pNetNode->mNodeNet->GetNetProtocol();

			Auto<TransferPacket> tranPak = protocol->CreatePacket(eNGN_TransferMsg);
			tranPak->mData.clear(false);
			// 在数据开关写入请求消息名称
			tranPak->mData.write(nCompIndex);
			tranPak->mData.writeString(requestMsgName);
			tranPak->mData._write(pRequestMsgData->data(), pRequestMsgData->dataSize());
			
			return Await(tranPak, targetID, waitMilSecond);
		}

		AutoNice Await(Auto<TransferPacket> tranPak, UnitID targetID, int overMilSecond);

		bool SendMsg(tBaseMsg &msg, UnitID targetID, BROADCAST_MODE eMode = eBroadcastNone)
		{
			return SendMsg(msg.GetMsgName(), msg, targetID, eMode);
		}

		bool SendMsg(const AString &msgName, tBaseMsg &msg, UnitID targetID, BROADCAST_MODE eMode = eBroadcastNone);

		// 处理RPC消息函数
		//template<typename ReqMsg, typename RespMsg>
		//virtual void On(ReqMsg &reqest, RespMsg &resp) = 0;		
		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnMsg<ActorClass, RQ, RS>);
		// 已经在协程内处理
		template<typename T, typename ReqMsg, typename RespMsg>
		static int OnMsg(Actor *pActor, DataStream *pReqestMsgData, TransferPacket *pResponse, int nCompIndex)
		{			
			ReqMsg pMsg;			
			//pReqestMsgData->seek(0);
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
			p->On(pMsg, respMsg, pResponse->mSenderID, nCompIndex);
			//pResponse->mResultType = eNoneError;
			//LOG("Respons msg %s:\r\n%s", respMsg.GetMsgName(), respMsg.dump().c_str());
			pResponse->mData.clear();
			respMsg.serialize(&pResponse->mData);

			return eNoneError;
		}

		// 处理一般通知消息
		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnNotify<ActorClass, RQ>);
		template<typename T, typename Msg>
		static void OnNotify(Actor *pActor, DataStream *pMsgData, UnitID senderID, int nCompIndex)
		{
			Msg pMsg;
			//pMsgData->seek(0);
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
			p->Notify(pMsg, senderID, nCompIndex);			
		}

		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnComponentNotify<ComponentClass, RQ>);
		template<typename T, typename Msg>
		static void OnComponentNotify(Actor *pActor, DataStream *pMsgData, UnitID senderID, int nCompIndex)
		{
			Msg pMsg;
			//pMsgData->seek(0);
			if (!pMsg.restore(pMsgData))
			{
				ERROR_LOG("%s restore fail", pMsg.GetMsgName());
				return;
			}
			Hand<T> comp;
			if (nCompIndex == 0)
				comp = pActor->GetComponent<T>();
			else
			{
				comp = pActor->GetComponent(nCompIndex);
				if (!comp)
					comp = pActor->GetComponent<T>();
			}
			if (!comp)
			{
				ERROR_LOG("%s parent class is not Componect %s", typeid(*pActor).name(), typeid(T).name());
				return;
			}
			comp->Notify(pMsg, senderID, nCompIndex);
		}

		// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnComponentMsg<ComponentClass, RQ, RS>);
		template<typename T, typename ReqMsg, typename RespMsg>
		static int OnComponentMsg(Actor *pActor, DataStream *pReqestMsgData, TransferPacket *pResponse, int nCompIndex)
		{
			ReqMsg pMsg;
			//!!! 新的协议数据开头为消息名称,后面即为消息数据 pReqestMsgData->seek(0);
			if (!pMsg.restore(pReqestMsgData))
			{
				ERROR_LOG("%s restore fail", pMsg.GetMsgName());
				return eSourceCodeError;
			}
			RespMsg respMsg;

			Hand<T> comp;
			if (nCompIndex == 0)
				comp = pActor->GetComponent<T>();
			else
			{
				comp = pActor->GetComponent(nCompIndex);
				if (!comp)
					comp = pActor->GetComponent<T>();
			}
			if (!comp)
			{
				ERROR_LOG("%s parent class is not Componect %s", typeid(*pActor).name(), typeid(T).name());
				return eSourceCodeError;
			}
			comp->On(pMsg, respMsg, pResponse->mSenderID, nCompIndex);
			//pResponse->mResultType = eNoneError;
			//LOG("Respons msg %s:\r\n%s", respMsg.GetMsgName(), respMsg.dump().c_str());
			pResponse->mData.clear();
			respMsg.serialize(&pResponse->mData);

			return eNoneError;
		}

		virtual void ResponseHttp(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) { response = "Nothing"; NOTE_LOG("No code for ResponseHttp"); }

		virtual void ResponseBytesHttp(HandPacket requestMsg, DataBuffer &response, const AString &requestAddress) { NOTE_LOG("No code for ResponseBytesHttp, packet : %d", requestMsg->GetFactory()->GetPacketID()); }

		virtual bool OnReceiveProcess(NodePacket *pNodePacket) override;

		virtual void RegisterMsg() {}

	public:
		ActorManager* GetMgr()
		{
			if (mActorFactory)
				return mActorFactory->mMgr->mpMgr;
			return NULL;
		}

		virtual AutoDBManager GetDBMgr();

		AsyncNode* GetNetNode();

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

		virtual Logic::tEventCenter* GetEventCenter() override;

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

			Base::Release();
		}

		virtual void Destory()
		{
			GetEventCenter()->WaitTime(0, EventCallBack(&Actor::WaitDestory, this));
		}

		bool WaitDestory(Logic::tEvent *pEvent);

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
		virtual ARecord LoadRecord(const char *szTableName, const char *szKey) override;
		virtual ARecord LoadRecord(const char *szTableName, Int64 nKey) override;

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
	virtual void RegisterMsg() override
	{
		HandActor actor = _NewActor();
		actor->RegisterMsg();
	}
};

#endif //_INCLUDE_ACTOR_H_
