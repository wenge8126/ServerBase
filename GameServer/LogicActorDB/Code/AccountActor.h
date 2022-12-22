#pragma once

#include "Actor.h"
#include "MsgData.h"
#include "t_account.h"

class DDCom : public NetCloud::Component
{
public:
	void On(RQ_CheckAndCreateAccount &msg, RS_CheckAndCreateAccount &resp, UnitID senderID)
	{
		resp.mbNew = true;
		resp.mResult = 88;
		resp.mID = 999;

		msg.mAccount = "pppp";

		GetActor()->SendMsg(msg, senderID);
	}

	virtual void RegisterMsg() override
	{
		//REG_COMP_MSG(DDCom, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount);
	}


	// 实例后, 加入到Actor之后即执行Awake
	virtual void Awake() { LOG("%s awake", Type()); }

	// 实例后, 在下次循环中执行一次Start()
	//virtual void Start() override { LOG("%s start", Type()); }

	virtual void AsyncStart() { LOG("%s async start", Type()); }

	// StartUpdate 开启后执行 Update, StopUpdate() 结束Update
	//virtual void Update(float onceTime) override { LOG("%s Update", Type()); }

	// 低速Update
	//virtual void LowUpdate() { LOG("%s Low update", Type()); }

	// 释放之前执行 OnDestory
	virtual void OnDestory() { LOG("%s on destory", Type()); }

	
};

class AccountActor : public NetCloud::Actor
{
public:
	virtual void Init() override
	{
		//AddComponent("DDCom");
	}

	void On(RQ_CheckAndCreateAccount &msg, RS_CheckAndCreateAccount &resp, UnitID senderID, int)
	{
		//t_account accountRe = mAccountList.find(msg.mAccount);
		//if (!accountRe)
		//{
		//	accountRe = LoadRecord(TABLE_ACCOUNT, msg.mAccount.c_str());
		//	if (accountRe)
		//		mAccountList.insert(msg.mAccount, accountRe);
		//}
		//resp.mbNew = false;
		//if (!accountRe)
		//{
		//	// 需要新建
		//	AutoTable accountTable = GetDBMgr()->GetTable(TABLE_ACCOUNT);
		//	if (!accountTable)
		//	{
		//		resp.mResult = eAccountCreateFail;
		//		return;
		//	}

		//	accountRe = accountTable->CreateRecord(msg.mAccount, true);
		//	if (!accountRe)
		//	{
		//		resp.mResult = eAccountCreateFail;
		//		return;
		//	}
		//	accountRe.PASSWORD() = msg.mPassword;
		//	accountRe.CREATE_TIME() = TimeManager::Now();
		//	resp.mbNew = true;
		//}
		//	
		//AutoNice serverData = accountRe.niceDBID_LIST(false);
		//if (!serverData)
		//	serverData = accountRe.niceDBID_LIST(true);

		//Int64 playerID = serverData[STRING(msg.mServerID)];

		//if (playerID<=0)
		//{
		//	// 创建Player数据
		//	RQ_CreatePlayerData req;
		//	req.mAccount = msg.mAccount;
		//	RS_CreatePlayerData resp;
		//	Await({ Actor_GameServer, msg.mServerID }, req, resp, 100000);
		//	auto responseMsg = &resp;
		//	if (responseMsg)
		//	{
		//		serverData[STRING(msg.mServerID)] = responseMsg->mPlayerID;
		//		accountRe.niceDBID_LIST(true);
		//		playerID = responseMsg->mPlayerID;
		//	}
		//	else
		//	{
		//		//resp.mResult = eCreateDataRecordFail;
		//		return;
		//	}
		//}

		//resp.mID = playerID;

		//accountRe.Save();

		//LOG("Account %s : \r\n%s", msg.mAccount.c_str(), accountRe.mRecord->dump().c_str());
	}

	virtual void RegisterMsg() 
	{	
		REG_ACTOR_MSG(AccountActor, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount);

		//REG_COMP_MSG(DDCom, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount);

		//pActorMgr->RegisterActorMsg("RQ_CheckAndCreateAccount", &Actor::OnMsg<AccountActor, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount>);

		//pActorMgr->RegisterActorMsg("RQ_CheckAndCreateAccount", &Actor::OnComponentMsg<DDCom, RQ_CheckAndCreateAccount, RS_CheckAndCreateAccount>);

	}

public:
	FastHash<AString, t_account>		mAccountList;
};

