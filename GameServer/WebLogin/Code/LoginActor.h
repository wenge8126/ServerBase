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

#include "ClientMsg.h"


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
		LoginNetActor::Init();
		AddComponent("TestNoSQLUserComponent");
		AddComponent("Test2Component");
		AddComponent("RecordNoSQLUserComponent");
		AddComponent("PlayerItemComp");
		//Hand<HttpComponect> comp = AddComponent("HttpComponect");
		//comp->mPort = 5000;

		Hand<TcpComponent> tcpNet = GetComponent("TcpComponent");
		tcpNet->mServerIp = "127.0.0.1";
		tcpNet->mServerPort = 4001;

		// ͬʱ֧�ֶ�������ģʽ
		Hand<WssWebComponent> wssNet = AddComponent("WssWebComponent");
		wssNet->mServerIp = "127.0.0.1";
		wssNet->mServerPort = 4002;

		//tcpNet->mSafeCode = 11;
		Hand<tBaseEventNet> net = tcpNet->GetNet();
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eMsg_ClientRequestLogin, CS_RequestLogin, SC_ResponseLogin, LoginActor>(this));
		////net->SetNetProtocol();`
		 
		//AddComponent("LoginNetComponect");
	}



// ��ת�ͻ������������Actor��Ϣ
void On(HandConnect connect, const CS_RequestLogin  &req, SC_ResponseLogin &resp)
{
	NOTE_LOG(req.dump().c_str());

	resp.error = 0;

}


	void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_COMPONENT( WssWebComponent);
		REG_COMPONENT( TestNoSQLUserComponent);
		REG_COMPONENT( Test2Component);
		REG_COMPONENT( RecordNoSQLUserComponent);
		REG_COMPONENT( PlayerItemComp);
		REG_COMPONENT( LoginNetComponect);
	}
};

