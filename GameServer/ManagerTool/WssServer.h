#pragma once

#include "WebServerNet.h"




class WssConnect : public uWS::tWssConnect<false>
{
public:
	virtual void SetNetID(int netID) { mNetID = netID; }
	virtual int GetNetID() const { return mNetID; }

	//virtual bool SendEvent(Logic::tEvent *pEvent) override { ERROR_LOG("No use SendEvent"); return false; }
	virtual bool Send(const Packet  *msgPacket, bool bEncrypt) override { ERROR_LOG("No use WebPlayer Send"); return false; }
	//virtual tNetHandle* GetNetHandle(void) = 0;

	virtual void OnSucceedSendEvent(Logic::tEvent *pEvt, Packet *p) { }

public:
	int			mNetID = 0;
	Int64		mDBID = 0;
};

// 手机通过WebSocket进行直连, 警告或错误DUMP时, 进行报警, 也可以设定定时查询运行状态
class ToolWssNet : public uWS::tWssServerNet<false>
{
	typedef uWS::tWssServerNet<false> Base;

public:
	virtual HandConnect CreateConnect() override
	{
		return MEM_NEW WssConnect();
	}

	bool SendToClient(Int64 playerID, Logic::tEvent *pEvent) override
	{
		HandConnect conn = Base::mConnectHash.find(playerID);
		if (conn)
			return conn->SendEvent(pEvent);

		DEBUG_LOG("%llu  player no exist, send fail >%s", (UInt64)playerID, pEvent->GetEventName());

		return false;
	}

	virtual void BroatcastMsg(Logic::tEvent *pMsg, Int64 excludeID) override
	{
		for (auto it = Base::mConnectHash.begin(); it; ++it)
		{
			Hand<WssConnect> conn = it.get();
			if (conn && !conn->IsRemove() && conn->mDBID != excludeID)
				conn->SendEvent(pMsg);
		}

	}

	virtual void InitClearAllPlayer() override
	{
		for (auto it = Base::mConnectHash.begin(); it; ++it)
		{
			Hand<BaseWebConnect> conn = it.get();
			if (conn)
				conn->SetRemove(true);
		}
		Base::mConnectHash.clear(false);
	}

};
