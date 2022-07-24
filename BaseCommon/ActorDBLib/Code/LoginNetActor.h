#pragma once

#ifndef _INCLUDE_LOGINNETACTOR_H_
#define _INCLUDE_LOGINNETACTOR_H_

#include "ActorManager.h"

#include "TcpComponent.h"

#include "NoSQLComponent.h"


#include "DefineMsgFactory.h"
#include "ServerClientMsg.h"

using namespace NetCloud;

//-------------------------------------------------------------------------
// �ͻ��������ظ�����Ϣ
// ������LogicActor �յ���, ʹ���첽�ȴ� ClientActor �ٴ��첽������Ŀ��Actor
// ClientActor ���յ��ظ���, LogicAtor �����µĻظ���Ϣ, �ظ����ͻ���
// Client �ٸ��ݵȴ��ŵ�Actor ���ػظ����
//-------------------------------------------------------------------------


// ���ڴ����½��Ϣ���

class ActorDBLib_Export_H LoginNetComponect : public NetWorkerComponent
{
	virtual void _RegisterMsg(Logic::tEventCenter *pCenter) override
	{
		//REGISTER_EVENT(pCenter, CheckAccountMsg);
		//REGISTER_EVENT(pCenter, CS_ActorMsg);
		//		pCenter->RegisterMsg(200, MEM_NEW Logic::EventFactory<CS_ActorMsg>());
	}

	virtual bool OnConnected(HandConnect connect)
	{
		CreateClientActor(connect, 1);
		// �����ȴ���½�¼�, ��ʱ��δ��½, ������
		return NetWorkerComponent::OnConnected(connect);
	}

	virtual void OnDisconnect(HandConnect connect) override
	{
		Hand<ClientActor> actor = connect->GetUserData();
		if (actor)
		{
			actor->OnDisconnected();
			actor->mpClientConnect = NULL;
			connect->SetUserData(AutoAny());
		}
	}

	void CreateClientActor(HandConnect connect, Int64 id)
	{
		Hand<ClientActor> actor = mpActor->GetMgr()->CreateActor(Actor_Client, id);
		connect->SetUserData(actor);
		actor->mpClientConnect = connect.getPtr();
	}
};
//-------------------------------------------------------------------------
// ���ڽ��ܴ���ͻ�������, ����Ӧ�ͻ������������Actor��Ϣ
//-------------------------------------------------------------------------
class ActorDBLib_Export_H LoginNetActor : public NetCloud::Actor
{
public:
	WebLoginThread* GetLoginThread()
	{
		Auto<LoginActorManager> mgr = GetMgr();
		return mgr->mpThread;
	}

	virtual void Init() override
	{		
		Hand<TcpComponent> tcpNet = AddComponent("TcpComponent");
		tcpNet->mServerIp = "127.0.0.1";
		tcpNet->mServerPort = 4001;

		tcpNet->mSafeCode = 11;
		Hand<tBaseEventNet> net = tcpNet->GetNet();
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW AsyncProcessReqeustMsgFactory<eMsg_ClientRequestServer, CS_ClientRequest, SC_ResponseMsg, LoginNetActor>(this));
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultMsgFactory<eMsg_ServerClientNotify, SCS_NotifyMsg, LoginNetActor>(this));

		//net->SetNetProtocol();`

		AddComponent("LoginNetComponect");
	}

	// ��ת�ͻ������������Actor��Ϣ
	void OnAsyncRequest(HandConnect connect, const CS_ClientRequest  &req, SC_ResponseMsg &resp)
	{
		//NOTE_LOG(req.dump().c_str());

		Hand<ClientActor> client = connect->GetUserData();
		if (client)
		{
			AutoData msgData = req.mRequestMsgData;
			AutoNice respData = client->Await(req.mTargetActorID, req.mRequestMsgName, msgData.getPtr(), 10000, 0);
			resp.mResponseData = respData;
			if (respData)
				NOTE_LOG("Actor response : \r\n%s", respData->dump().c_str());
		}
		else
			ERROR_LOG("No exist client actor");

	}

	// ��ת�ͻ��˷��������Actor ֪ͨ��Ϣ
	void On(HandConnect connect, SCS_NotifyMsg &notifyMsg)
	{
		Hand<ClientActor> client = connect->GetUserData();
		if (client)
		{			
			if (!client->SendMsg(notifyMsg, notifyMsg.mActorID))
				ERROR_LOG("Send to actor notify msg fail");
		}
		else
			ERROR_LOG("No exist client actor");
	}

	void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_COMPONENT(pActorMgr, TcpComponent);
		REG_COMPONENT(pActorMgr, HttpComponect);
		REG_COMPONENT(pActorMgr, WssWebComponent);
		REG_COMPONENT(pActorMgr, LoginNetComponect);
	}
};

#endif //_INCLUDE_LOGINNETACTOR_H_