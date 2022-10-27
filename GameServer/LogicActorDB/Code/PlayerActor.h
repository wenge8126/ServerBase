#pragma once

#include "Actor.h"
//#include "ClientMsg.h"
#include "SCActor.h"

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

class PlayerActor : public SCActor
{
public:
	void On(RQ_PlayerBaseData &msg, RS_PlayerBaseData &resp, UnitID, int)
	{
		if (mData)
			resp.mPlayerData = mData.mRecord->ToNiceData();
	}

	virtual void RegisterMsg()
	{
		REG_ACTOR_MSG(PlayerActor, RQ_PlayerBaseData, RS_PlayerBaseData);
		SCActor::RegisterMsg();
	}

public:
	void Init()
	{
		mData = LoadRecord(TABLE_PLAYER, mID.id);
		if (mData)
			LOG("Player data :%s", mData.mRecord->dump().c_str())
		else
			ERROR_LOG("No exist player data %s", mID.dump().c_str());

		
	}


public:
	t_player		mData;
};


// 游戏服务区, 一个游戏区一个此对象
class GameServerActor : public NetCloud::DBActor
{
public:
	void On(RQ_CreatePlayerData &msg, RS_CreatePlayerData &resp, UnitID senderID, int)
	{
		t_player data = GetDBMgr()->GetTable(TABLE_PLAYER)->GrowthNewRecord();
		data.ACCOUNT() = msg.mAccount;
		resp.mPlayerID = data.wID();
		data.Save();
	}

	virtual void RegisterMsg()
	{
		REG_ACTOR_MSG(GameServerActor, RQ_CreatePlayerData, RS_CreatePlayerData);
	}
};
