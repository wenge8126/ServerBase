#pragma once

#ifndef _INCLUDE_NETWORKERCOMPONENT_H
#define _INCLUDE_NETWORKERCOMPONENT_H

#include "Component.h"
#include "NetHandle.h"

#include "ClientEvent.h"
#include "ServerEvent.h"
#include "Actor.h"
#include "CommonLib.h"

using namespace Logic;
using namespace NetCloud;

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// 使用网络功能的组件, 关于网络逻辑组件可以继承此组件直接使用网络服务功能
class CommonLib_Export_H NetWorkerComponent : public Component
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


#endif //_INCLUDE_NETWORKERCOMPONENT_H