#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"

#include "DefineMsgFactory.h"

#include "SCActor.h"
#include "ServerMsg.h"
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
class PlayerActor : public SCActor
{
public:
	GameThread* GetLoginThread()
	{
		Auto<GameActorManager> mgr = GetMgr();
		return mgr->mpThread;
	}

	virtual void Init() override
	{
		SCActor::Init();
	
		NOTE_LOG("Player %s init", GetID().dump().c_str());

	}

	template<typename RespMsg>
	bool RequestClient(UnitID clientActorID, tBaseMsg &requestMsg, RespMsg &responseMsg, int overMilSecond)
	{
		return AwaitClient(GetID().id, clientActorID, requestMsg, responseMsg, overMilSecond);
	}

public:
	void On(CG_RequestPlayerData &request, GC_ResponsePlayerData &response, UnitID sender, int)
	{
		// 分配后, 请求GameServerActor 创建PlayerActor
		response.mName = "yyyywwww";

		response.mData = MEM_NEW NiceData();
		response.mData["INFO"] = "OOOOOOOOOKKKKKKKKK";
		response.mData["LEVE"] = 7;

		//response.error = eErrorCodeNone;

		CoroutineTool::AsyncCall([&]()
		{
			tTimer::AWaitTime(3000);
			GC_RequestClientState requestMsg;
			requestMsg.mValue = 999555;
			CG_ResponseClientState responseMsg;
			if (RequestClient({ Actor_Client, GetID().id }, requestMsg, responseMsg, 10000))
				NOTE_LOG("TTTTTTT Client response : %s", responseMsg.dump().c_str())
			else
			{
				ERROR_LOG("Reqeust client fail");
			}
		});
	}


	void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_ACTOR_MSG(PlayerActor, CG_RequestPlayerData, GC_ResponsePlayerData);

	}
};

