#pragma once

#include "NetWorkerComponent.h"
#include "MsgData.h"
#include "CommonDefine.h"

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
		RS_CheckAndCreateAccount resp;
		bool bRe = GetActor()->Await({ Actor_Account, 1 }, rq,  resp, 600000);

		if (bRe && mNetConnect && resp.mID > 0)
		{
			LoginData *p = MEM_NEW LoginData();
			p->mID = resp.mID;
			mNetConnect->mAttachData = p;
		}
		else
			ERROR_LOG("Account check fail");
		AutoNice d = MEM_NEW NiceData();
		resp.ToData(d);
		 GetResponseEvent()["RESP"] = d.getPtr();

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

			GetResponseEvent()["RESP"] = GetActor()->Await({ type, id }, msgName, GetData(), 6000, 0).getPtr();
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