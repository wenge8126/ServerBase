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

using namespace NetCloud;

class LoginActor : public NetCloud::Actor
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

		tcpNet->mSafeCode = 11;
		Hand<tBaseEventNet> net = tcpNet->GetNet();
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<30, NG_RequestGateInfo, GN_NotifyNodeInfo, LoginActor>(this));
		//net->SetNetProtocol();`

		AddComponent("LoginNetComponect");
	}

	void On(tNetConnect *, NG_RequestGateInfo &req, GN_NotifyNodeInfo &resp)
	{
		NOTE_LOG(req.dump().c_str());
		resp.mNodeKey = 8888;
		
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

