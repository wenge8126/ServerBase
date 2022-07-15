#pragma once

#ifndef _INCLUDE_CLIENTACTOR_H_
#define _INCLUDE_CLIENTACTOR_H_

//-------------------------------------------------------------------------
// �ͻ�������Actor, ���ڿͻ�����������ڲ���ͨ
// Actor �ڰ� ConnectPtr, Connect ��������Ϊ��Ӧ ��Actor
//-------------------------------------------------------------------------
class ClientActor : public Actor
{
public:
	ClientActor() {}
	~ClientActor()
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

	// ���ִֻ��һ��
	void RegisterMsg(ActorManager *pActorMgr) override
	{

	}

	

public:
	tNetConnect		*mpClientConnect = NULL;
};

/* ͸����Ϣ
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
