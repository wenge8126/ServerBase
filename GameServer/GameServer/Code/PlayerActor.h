#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"

#include "DefineMsgFactory.h"

#include "SCActor.h"
#include "ServerMsg.h"
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
		// �����, ����GameServerActor ����PlayerActor
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

