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
// 使用网络功能的组件, 关于网络逻辑组件可以继承此组件直接使用网络服务功能
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
	//AutoNet	mNet;		// 互相引用后, 必定使用Destory, 才可以解除
	//FastHash<Int64, HandConnect> mConnectList;
};

//-------------------------------------------------------------------------
// 用到的相关消息事件
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