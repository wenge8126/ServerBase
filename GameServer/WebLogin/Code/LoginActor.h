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
// 客户端请求或回复的消息
// 服务器LogicActor 收到后, 使用异步等待 ClientActor 再次异步请求至目标Actor
// ClientActor 接收到回复后, LogicAtor 构建新的回复消息, 回复至客户端
// Client 再根据等待着的Actor 返回回复结果
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// 用于接受处理客户端连接, 并响应客户端请求服务器Actor消息
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

		// 同时支持多种网络模式
		Hand<WssWebComponent> wssNet = AddComponent("WssWebComponent");
		wssNet->mServerIp = "127.0.0.1";
		wssNet->mServerPort = 4002;

		//tcpNet->mSafeCode = 11;
		Hand<tBaseEventNet> net = tcpNet->GetNet();
		net->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eMsg_ClientRequestLogin, CS_RequestLogin, SC_ResponseLogin, LoginActor>(this));
		////net->SetNetProtocol();`
		 
		//AddComponent("LoginNetComponect");
	}



// 中转客户端请求服务器Actor消息
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

