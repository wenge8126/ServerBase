#pragma once

#ifndef _INCLUDE_BASEPROTOCOL_H_
#define _INCLUDE_BASEPROTOCOL_H_

#include "NetHandle.h"
#include "NetHead.h"
#include "NetConfig.h"
#include "PacketFactory.h"
#include "EventProtocol.h"
#include "BaseMsg.h"
#include "CoroutineTool.h"
#include "Timer.h"

class Net_Export_H ResponseMsgPacket : public BasePacket
{


public:
	MSG_ID		mRequestID;
	DataBuffer	mData;
};

// 使用时间等待异步请求
class Net_Export AsyncProtocol : public EventNetProtocol
{
	class WaitResponse : public tTimer
	{
	public:
		void Init()
		{
			mResponsePacket.setNull();
			mWaitCoroID = 0;
			mRequestMsgID = 0;
		}

		virtual void onTime() override
		{
			RESUME(mWaitCoroID);
		}

	public:
		HandPacket mResponsePacket;
		CoroID mWaitCoroID;
		MSG_ID mRequestMsgID;
	};

	typedef Auto<WaitResponse> AWaitResponse;

public:
	AsyncProtocol()
	{
		RegisterNetPacket(MEM_NEW DefinePacketFactory<ResponseMsgPacket, PACKET_RESPONSE_MSG>());
	}

public:
	virtual bool ProcessReceivePacket(tNetConnect *pConnect, Packet *pPacket)
	{
		if (ProcessRequestPacket(pConnect, pPacket))
			return true;
		return EventNetProtocol::ProcessReceivePacket(pConnect, pPacket);
	}

	virtual bool ProcessRequestPacket(tNetConnect *pConnect, Packet *pPacket);

public:
	AutoNice Await(tNetConnect *pConnect, int msgID, tRequestMsg &req, int overMilSecond);

	AWaitResponse AllotEventID();

	void FreeServerEvent(WaitResponse *pWaitResponse);

	AWaitResponse FindWaitResponse(MSG_ID id);

protected:
	Array<AWaitResponse>			mEventList;
	EasyStack<UINT>				mIDStack;
};

class Net_Export BaseProtocol : public AsyncProtocol
{
	typedef Auto<tBaseMsg>(*pMsgCall)(tNetConnect *pConnect, AutoBase *pActor, tBaseMsg *pReqestMsgData);
	typedef void(*pNotifyMsgCall)(tNetConnect *pConnect, AutoBase *pActor, tBaseMsg *pReqestMsgData);

protected:
	FastHash<PacketID, pMsgCall>			mOnMsgFunctionList;
	FastHash<PacketID, pNotifyMsgCall>	mOnNotifyMsgFunctionList;

public:
	void RegisterMsg(PacketID id, pMsgCall  pFun)
	{
		mOnMsgFunctionList.insert(id, pFun);
	}

	void RegisterNotifyMsg(PacketID id, pNotifyMsgCall  pFun)
	{
		mOnNotifyMsgFunctionList.insert(id, pFun);
	}

	virtual bool ProcessReceivePacket(tNetConnect *pConnect, Packet *pPacket) override
	{
		{
			auto fun = mOnMsgFunctionList.findPtr(pPacket->GetPacketID());
			if (fun != NULL)
			{
				tBaseMsg *pMsg = dynamic_cast<tBaseMsg*>(pPacket);
				if (pMsg != NULL)
				{
					auto resp = (*fun)(pConnect, this, pMsg);
					if (!pConnect->Send(resp.getPtr(), false))
						ERROR_LOG("Send response fail %s", resp->GetMsgName());
				}
				else
					ERROR_LOG("%d is not tBaseMsg", pPacket->GetPacketID());
				return true;
			}
		}
		{
			auto fun = mOnNotifyMsgFunctionList.findPtr(pPacket->GetPacketID());
			if (fun != NULL)
			{
				tBaseMsg *pMsg = dynamic_cast<tBaseMsg*>(pPacket);
				if (pMsg != NULL)
				{
					(*fun)(pConnect, this, pMsg);			
				}
				else
					ERROR_LOG("%d is not tBaseMsg", pPacket->GetPacketID());
				return true;
			}
		}
		return EventNetProtocol::ProcessReceivePacket(pConnect, pPacket);
	}

public:
	template<typename T, typename ReqMsg, typename RespMsg>
	static Auto<tBaseMsg> OnMsg(tNetConnect *pConnect, BaseProtocol *pActor, tBaseMsg *pReqestMsgData)
	{
		ReqMsg *pMsg = dynamic_cast<ReqMsg*>(pReqestMsgData);
		if (pMsg==NULL)
		{
			ERROR_LOG("%s is not %s", pReqestMsgData->GetMsgName(), typeid(ReqMsg).name());
			return eSourceCodeError;
		}
		Auto<RespMsg> respMsg = MEM_NEW RespMsg();
		T *p = dynamic_cast<T*>(pActor);
		if (p == NULL)
		{
			ERROR_LOG("%s parent class is not Actor", typeid(T).name());
			return eSourceCodeError;
		}
		p->On(pConnect, *pMsg, *respMsg);
		

		return respMsg;
	}

	// 处理一般通知消息
	// 注册 pActorMgr->RegisterActorMsg(#RQ, &Actor::OnNotify<ActorClass, RQ>);
	template<typename T, typename Msg>
	static void OnNotify(tNetConnect *pConnect, BaseProtocol *pActor, tBaseMsg *pReqestMsgData)
	{
		Msg *pMsg = dynamic_cast<Msg*>(pReqestMsgData);
		if (pMsg == NULL)
		{
			ERROR_LOG("%s is not %s", pReqestMsgData->GetMsgName(), typeid(Msg).name());
			return eSourceCodeError;
		}
		T *p = dynamic_cast<T*>(pActor);
		if (p == NULL)
		{
			ERROR_LOG("%s parent class is not Actor", typeid(T).name());
			return eSourceCodeError;
		}
		p->Notify(pConnect, *pMsg);

	}
};

#endif //_INCLUDE_BASEPROTOCOL_H_