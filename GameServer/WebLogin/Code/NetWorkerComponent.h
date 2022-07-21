#pragma once
#include "Component.h"
#include "NetHandle.h"

#include "ClientEvent.h"
#include "ServerEvent.h"
#include "Actor.h"

using namespace Logic;
using namespace NetCloud;

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// ʹ�����繦�ܵ����, ���������߼�������Լ̳д����ֱ��ʹ�����������
class NetWorkerComponent : public Component
{
public:
	virtual void _RegisterMsg(Logic::tEventCenter *pCenter) {}

	virtual bool OnConnected(HandConnect connect) 
	{
		//connect->SetUserData(this);
		return true;
	}
	virtual void OnDisconnect(HandConnect connect) {}

public:
	//AutoNet GetNet();
	////void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory);
	//HandConnect FindConnect(Int64 id)
	//{
	//	return mConnectList.find(id);
	//}

public:
	//void Start() override
	//{
	//	if (!GetNet())
	//		StartUpdate(0.01f);
	//}

	//void Update(float t) override
	//{
	//	if (GetNet())
	//		StopUpdate();
	//}

public:
	//AutoNet	mNet;		// �������ú�, �ض�ʹ��Destory, �ſ��Խ��
	//FastHash<Int64, HandConnect> mConnectList;
};

//-------------------------------------------------------------------------
// �õ��������Ϣ�¼�
class ComponectNetMsg : public Logic::tBaseNetEvent
{
public:
	NetCloud::HandActor GetActor();

	Hand< NetWorkerComponent> GetNetWorker();
};

class ComponectResponseMsg : public Logic::tClientEvent
{
public:
	virtual void Async() {}
		
public:
	HandActor GetActor();

	Hand< NetWorkerComponent> GetNetWorker();


	virtual bool _DoEvent() override
	{
		ASYNC(&ComponectResponseMsg::Async, this);
		return true;
	}
};


class ComponectRequestMsg : public Logic::tServerEvent
{
public:
	HandActor GetActor();

	Hand< NetWorkerComponent> GetNetWorker();
};
//-------------------------------------------------------------------------