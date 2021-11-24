#pragma once
#include "Component.h"
#include "NetHandle.h"

#include "ClientEvent.h"
#include "ServerEvent.h"
#include "Actor.h"

using namespace Logic;
using namespace NetCloud;
//-------------------------------------------------------------------------
// 使用网络功能的组件, 关于网络逻辑组件可以继承此组件直接使用网络服务功能
class NetWorkerComponent : public ProcessComponent
{
public:
	AutoNet GetNet();
	void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory);

public:
	AutoNet	mNet;		// 互相引用后, 必定使用Destory, 才可以解除
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