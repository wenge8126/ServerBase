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
#include "ClientMsg.h"

using namespace NetCloud;

//-------------------------------------------------------------------------
// �ͻ��������ظ�����Ϣ
// ������LogicActor �յ���, ʹ���첽�ȴ� ClientActor �ٴ��첽������Ŀ��Actor
// ClientActor ���յ��ظ���, LogicAtor �����µĻظ���Ϣ, �ظ����ͻ���
// Client �ٸ��ݵȴ��ŵ�Actor ���ػظ����
//-------------------------------------------------------------------------
enum CS_MSG_ID
{
	eMsg_ClientRequestServer = 101,
	eMsg_ServerRequestClient = 102,
};

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
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW AsyncProcessReqeustMsgFactory<eMsg_ClientRequestServer, CS_ClientRequest, SC_ResponseMsg, LoginActor>(this));
		//net->SetNetProtocol();`

		AddComponent("LoginNetComponect");
	}

	void OnAsyncRequest(HandConnect connect, const CS_ClientRequest  &req, SC_ResponseMsg &resp)
	{
		NOTE_LOG(req.dump().c_str());
		//resp.mNodeKey = 8888;
		
		Hand<ClientActor> client = connect->GetUserData();
		if (client)
		{
			AutoNice msgData = req.mActorMsg;
			AutoNice respData = client->Await(UnitID(Actor_DBWorker, 1), req.mRequestMsgName, *msgData, 10000, 0);
			resp.mResponseData = respData;
			if (respData)
				NOTE_LOG("Actor response : \r\n%s", respData->dump().c_str());
		}
		else
			ERROR_LOG("No exist client actor");

		//CoroutineTool::AsyncCall([=]()
		//{
		//	// 3�����һ����������
		//	tTimer::AWaitTime(3000);
		//	NG_RequestGateInfo req;
		//	GN_NotifyNodeInfo resp;
		//	req.mNodeKey = 66667777;
		//	if (tNetProcess::Await(pConnect, 41, req, resp, 6000))
		//	{
		//		NOTE_LOG("=====");
		//		NOTE_LOG(resp.dump().c_str());
		//	}
		//	else
		//	{
		//		ERROR_LOG("Test request fail");
		//	}
		//});

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

