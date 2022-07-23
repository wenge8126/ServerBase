#pragma once

#ifndef _INCLUDE_CLIENTACTOR_H_
#define _INCLUDE_CLIENTACTOR_H_

#include "Actor.h"
#include "ServerClientMsg.h"

//-------------------------------------------------------------------------
// 客户端连接Actor, 用于客户端与服务器内部沟通
// Actor 内绑定 ConnectPtr, Connect 设置数据为对应 的Actor
//-------------------------------------------------------------------------
class ActorDBLib_Export tClientActor : public Actor
{
public:
	tClientActor() {}
	~tClientActor()
	{
		if (mpClientConnect != NULL)
			mpClientConnect->SetUserData(AutoAny());
	}

	virtual void Init() override
	{
		
	}

	virtual void OnDisconnected() {}

public:
	AutoNet GetClientNet()
	{
		if (mpClientConnect!=NULL)
			return	mpClientConnect->GetNetHandle();
		
		return AutoNet();
	}

	// 这个只执行一次
	void RegisterMsg(ActorManager *pActorMgr) override
	{
		REG_ACTOR_MSG(tClientActor, SC_ActorRequestClientMsg, CS_ResponceServerActorMsg);
	}

	void On(SC_ActorRequestClientMsg &reqMsg, CS_ResponceServerActorMsg &clientResponse, UnitID sender, int);

public:
	tNetConnect		*mpClientConnect = NULL;
};

// 转发Actor 请求客户端Actor消息
class SC_ServerRequestClientMsg : public tRequestMsg
{
public:
	enum
	{
		PACKET_REQUEST_CLIENTACTOR = PACKET_MAX + 50,
	};

public:
	int mRequestID;
	UInt64 mClientActorID;
	AString mRequestMsgName;
	AutoData mRequestMsgData;

public:
	SC_ServerRequestClientMsg() { clear(false); };


	virtual  void Full(AutoNice scrData) override
	{
		clear(false);
		CheckGet(scrData, mClientActorID);
		CheckGet(scrData, mRequestID);
		mRequestMsgData = (DataStream*)scrData["mRequestMsgData"];
		CheckGet(scrData, mRequestMsgName);
	}

	virtual void ToData(AutoNice &destData) override
	{
		destData["mClientActorID"] = mClientActorID;
		destData["mRequestID"] = mRequestID;
		destData["mRequestMsgData"] = mRequestMsgData.getPtr();
		destData["mRequestMsgName"] = mRequestMsgName;
	}

	bool serialize(DataStream *destData) const override
	{
		destData->write(mRequestID);
		destData->write(mClientActorID);
		destData->writeString(mRequestMsgName);
		destData->writeData((DataStream*)mRequestMsgData.getPtr());
		return true;
	}

	void clear(bool bClearBuffer = false) override
	{
		mClientActorID = 0;
		mRequestID = 0;
		if (mRequestMsgData) mRequestMsgData.setNull();
		mRequestMsgName.setNull();
	}

	void copy(const tBaseMsg &otherMsg) override
	{
		AssertNote(0, "Can not use copy");
		//if (strcmp("SC_ServerRequestClientMsg", otherMsg.GetMsgName()) != 0) { LOG("%s is not SC_ServerRequestClientMsg", otherMsg.GetMsgName()); return; }; const SC_ServerRequestClientMsg &other = *(const SC_ServerRequestClientMsg*)(&otherMsg);
		//mClientActorID = other.mClientActorID;
		//mRequestID = other.mRequestID;
		//COPY_MSG_DATA(other.mRequestMsgData, mRequestMsgData);
		//mRequestMsgName = other.mRequestMsgName;
	}

	virtual const char* GetMsgName() const override { return "SC_ServerRequestClientMsg"; }

	virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

	virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

	AData get(const char *szMember) const
	{
		if (strcmp(szMember, "mClientActorID") == 0) { AData value; value = mClientActorID; return value; }
		if (strcmp(szMember, "mRequestID") == 0) { AData value; value = mRequestID; return value; }
		if (strcmp(szMember, "mRequestMsgName") == 0) { AData value; value = mRequestMsgName; return value; }
		return AData();
	}

	bool set(const char *szMember, AData value)
	{
		if (strcmp(szMember, "mClientActorID") == 0) { mClientActorID = value; return true; };
		if (strcmp(szMember, "mRequestID") == 0) { mRequestID = value; return true; };
		if (strcmp(szMember, "mRequestMsgName") == 0) { mRequestMsgName = value; return true; };
		LOG("No exist > %%s", szMember);  return false;
	}

	AData operator [] (const char *szMember) const
	{
		return get(szMember);
	}

	AData operator [] (const AString &member) const
	{
		return get(member.c_str());
	}

};

/* 透传消息
Hand<LoginData> d = mNetConnect->mAttachData;
if (d && d->mID > 0)
{
	AString msgName = get("MSG_NAME");
	int type = get("UNIT_TYPE");
	Int64 id = get("UNIT_ID");

	GetResponseEvent()["RESP"] = GetActor()->Await({ type, id }, msgName, GetData(), 6000, 0).getPtr();
}
*/

#endif //_INCLUDE_CLIENTACTOR_H_
