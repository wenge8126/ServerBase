#pragma once

#include "NetWorkerComponent.h"
#include "MsgData.h"

using namespace NetCloud;



class LoginData : public AnyData
{
public:
	Int64			mID;

};

class CheckAccountMsg : public ComponectResponseMsg
{
	virtual void Async() override 
	{
		RQ_CheckAndCreateAccount rq;
		rq.mAccount = get("ACCOUNT");
		rq.mPassword = get("PASSWORD");
		rq.mServerID = 1;
		Auto< RS_CheckAndCreateAccount> resp = GetActor()->Await<RS_CheckAndCreateAccount>(rq, { Actor_Account, 1 }, 6000);

		if (resp && mNetConnect && resp->mID > 0)
		{
			LoginData *p = MEM_NEW LoginData();
			p->mID = resp->mID;
			mNetConnect->mAttachData = p;
		}
		else
			ERROR_LOG("Account check fail");

		 GetResponseEvent()["RESP"] = (tNiceData*)resp.getPtr();

		Finish();
	}
};

class CS_ActorMsg : public ComponectResponseMsg
{
public:


	void Async()
	{
		if (!mNetConnect)
		{
			Finish();
			return;
		}

		Hand<LoginData> d = mNetConnect->mAttachData;
		if (d && d->mID > 0)
		{
			AString msgName = get("MSG_NAME");
			int type = get("UNIT_TYPE");
			Int64 id = get("UNIT_ID");

			GetResponseEvent()["RESP"] = GetActor()->Await(msgName, GetData(), { type, id }, 6000).getPtr();
		}
		Finish();
	}

protected:
private:
};

// 用于处理登陆消息组件

class LoginNetComponect : public NetWorkerComponent
{
	virtual void _RegisterMsg(Logic::tEventCenter *pCenter) override
	{
		REGISTER_EVENT(pCenter, CheckAccountMsg);
		REGISTER_EVENT(pCenter, CS_ActorMsg);
		//		pCenter->RegisterMsg(200, MEM_NEW Logic::EventFactory<CS_ActorMsg>());
	}

	virtual bool OnConnected(HandConnect connect)
	{
		// 启动等待登陆事件, 长时间未登陆, 则清理
		return NetWorkerComponent::OnConnected(connect);
	}
};