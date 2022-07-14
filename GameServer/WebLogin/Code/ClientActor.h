#pragma once

#ifndef _INCLUDE_CLIENTACTOR_H_
#define _INCLUDE_CLIENTACTOR_H_

//-------------------------------------------------------------------------
// 客户端连接Actor, 用于客户端与服务器内部沟通
// Actor 内绑定 ConnectPtr, Connect 设置数据为对应 的Actor
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

	virtual void OnDisconnected() {}

public:
	tNetConnect		*mpClientConnect = NULL;
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
