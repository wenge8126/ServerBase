#pragma once
#include "Component.h"
#include "NetHandle.h"

#include "ClientEvent.h"
#include "ServerEvent.h"
#include "Actor.h"

using namespace Logic;
using namespace NetCloud;
//-------------------------------------------------------------------------
// ʹ�����繦�ܵ����, ���������߼�������Լ̳д����ֱ��ʹ�����������
class NetWorkerComponent : public Component
{
public:
	virtual void _RegisterMsg(Logic::tEventCenter *pCenter) {}


public:
	AutoNet GetNet();
	//void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory);

public:
	void Start() override
	{
		if (!GetNet())
			StartUpdate(0.01);
	}

	void Update(float t) override
	{
		if (GetNet())
			StopUpdate();
	}

public:
	AutoNet	mNet;		// �������ú�, �ض�ʹ��Destory, �ſ��Խ��
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
	HandActor GetActor();

	Hand< NetWorkerComponent> GetNetWorker();
};


class ComponectRequestMsg : public Logic::tServerEvent
{
public:
	HandActor GetActor();

	Hand< NetWorkerComponent> GetNetWorker();
};
//-------------------------------------------------------------------------