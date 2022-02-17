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
		AutoNice d = MEM_NEW NiceData();
		resp->ToData(d);
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

			GetResponseEvent()["RESP"] = GetActor()->Await(msgName, GetData(), { type, id }, 6000).getPtr();
		}
		Finish();
	}

protected:
private:
};

// ���ڴ�����½��Ϣ���

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
		// �����ȴ���½�¼�, ��ʱ��δ��½, ������
		return NetWorkerComponent::OnConnected(connect);
	}
};