#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"
#include "TcpComponent.h"
#include "WssWebComponent.h"
#include "NoSQLComponent.h"

#include "TestNoSQLUserComponent.h"
#include "Test2Component.h"
#include "ItemDataComponent.h"
#include "LoginNetComponect.h"
#include "DefineMsgFactory.h"
#include "ServerClientMsg.h"
#include "LoginNetActor.h"

using namespace NetCloud;

//-------------------------------------------------------------------------
// �ͻ��������ظ�����Ϣ
// ������LogicActor �յ���, ʹ���첽�ȴ� ClientActor �ٴ��첽������Ŀ��Actor
// ClientActor ���յ��ظ���, LogicAtor �����µĻظ���Ϣ, �ظ����ͻ���
// Client �ٸ��ݵȴ��ŵ�Actor ���ػظ����
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// ���ڽ��ܴ���ͻ�������, ����Ӧ�ͻ������������Actor��Ϣ
//-------------------------------------------------------------------------
class LoginActor : public LoginNetActor
{
public:
	WebLoginThread* GetLoginThread()
	{
		Auto<LoginActorManager> mgr = GetMgr();
		return mgr->mpThread;
	}

	virtual void Init() override
	{
		AddComponent("TestNoSQLUserComponent");
		AddComponent("Test2Component");
		AddComponent("RecordNoSQLUserComponent");
		AddComponent("PlayerItemComp");
		Hand<HttpComponect> comp = AddComponent("HttpComponect");
		comp->mPort = 5000;

		Hand<TcpComponent> tcpNet = AddComponent("TcpComponent");
		tcpNet->mServerIp = "127.0.0.1";
		tcpNet->mServerPort = 4001;

		// ͬʱ֧�ֶ�������ģʽ
		Hand<WssWebComponent> wssNet = AddComponent("WssWebComponent");
		wssNet->mServerIp = "127.0.0.1";
		wssNet->mServerPort = 4002;

		tcpNet->mSafeCode = 11;
		Hand<tBaseEventNet> net = tcpNet->GetNet();
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW AsyncProcessReqeustMsgFactory<eMsg_ClientRequestServer, CS_ClientRequest, SC_ResponseMsg, LoginActor>(this));
		//net->SetNetProtocol();`

		AddComponent("LoginNetComponect");
	}



	void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_COMPONENT(pActorMgr, WssWebComponent);
		REG_COMPONENT(pActorMgr, TestNoSQLUserComponent);
		REG_COMPONENT(pActorMgr, Test2Component);
		REG_COMPONENT(pActorMgr, RecordNoSQLUserComponent);
		REG_COMPONENT(pActorMgr, PlayerItemComp);
		REG_COMPONENT(pActorMgr, LoginNetComponect);
	}
};

