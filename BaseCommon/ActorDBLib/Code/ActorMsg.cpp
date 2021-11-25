#include "ActorMsg.h"
#include "Actor.h"
#include "ActorManager.h"
#include "Gate.h"

namespace NetCloud
{

	void NetCloud::ActorResponsePacket::Response(tResponseResultPacket *pResponse)
	{
		HandActor actor = mNetUnit;
		if (actor && actor->GetMgr() != NULL)
		{
			pActorMsgCall *pFun = actor->GetMgr()->mOnMsgFunctionList.findPtr(mMsgName);
			if (pFun != NULL)
			{
				ActorResponResultPacket *pResp = dynamic_cast<ActorResponResultPacket*>(pResponse);
				pResp->mSenderID = mSenderID;
				(*pFun)(actor.getPtr(), mRequestData.getPtr(), pResp);
			}
		}
	}

	void NetCloud::ActorRequestPacket::RegisterDBPacket(tNetProtocol *pProtocel)
	{
		pProtocel->RegisterNetPacket(MEM_NEW DefinePacketFactory<ActorRequestPacket, eMsgRequest>());
		pProtocel->RegisterNetPacket(MEM_NEW DefinePacketFactory<ActorResponsePacket, eMsgResponse>(eMsgResult));
		pProtocel->RegisterNetPacket(MEM_NEW DefinePacketFactory<ActorResponResultPacket, eMsgResult>());

		pProtocel->RegisterNetPacket(MEM_NEW DefinePacketFactory<ActorNotifyPacket, eMsgNotify>());
	}

	// Gate 中转 回复结果包, 目的不报出 不存在Unit
	class GateActorResponResultPacket : public ActorResponResultPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			return tCloudPacket::Execute(pConnect);
		}
	};
	class GateActorNotifyPacket : public ActorNotifyPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			return tCloudPacket::Execute(pConnect);
		}
	};

	void NetCloud::ActorRequestPacket::RegisterDBPacket(tGate *pNetGate)
	{
		pNetGate->RegisterMsgPacket(MEM_NEW DefinePacketFactory<ActorRequestPacket, eMsgRequest>());
		// NOTE: Gate 中转时必须
		pNetGate->RegisterMsgPacket(MEM_NEW DefinePacketFactory<GateTransferRequestPakcet, eMsgResponse>());
		pNetGate->RegisterMsgPacket(MEM_NEW DefinePacketFactory<GateActorResponResultPacket, eMsgResult>());
		pNetGate->RegisterMsgPacket(MEM_NEW DefinePacketFactory <GateActorNotifyPacket, eMsgNotify>());
	}

	void NetCloud::ActorRequestPacket::RegisterDBPacket(tNetNode *pNetNode)
	{
		pNetNode->RegisterMsgPacket(MEM_NEW DefinePacketFactory<ActorRequestPacket, eMsgRequest>());
		pNetNode->RegisterMsgPacket(MEM_NEW DefinePacketFactory<ActorResponsePacket, eMsgResponse>(eMsgResult));
		pNetNode->RegisterMsgPacket(MEM_NEW DefinePacketFactory<ActorResponResultPacket, eMsgResult>());
		pNetNode->RegisterMsgPacket(MEM_NEW DefinePacketFactory<ActorNotifyPacket, eMsgNotify>());
	}


	void ActorNotifyPacket::_AsyncDo()
	{
		HandActor actor = mNetUnit;
		if (actor && actor->GetMgr() != NULL)
		{
			pActorNotifyMsgCall *pFun = actor->GetMgr()->mOnNotifyMsgFunctionList.findPtr(mMsgName);
			if (pFun != NULL)
			{
				(*pFun)(actor.getPtr(), mMsgData.getPtr(), mSenderID);
			}
		}
	}

}