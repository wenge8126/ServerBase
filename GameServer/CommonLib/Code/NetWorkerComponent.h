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
// ʹ�����繦�ܵ����, ���������߼�������Լ̳д����ֱ��ʹ�����������
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
	//AutoNet	mNet;		// �������ú�, �ض�ʹ��Destory, �ſ��Խ��
	//FastHash<Int64, HandConnect> mConnectList;
};

//-------------------------------------------------------------------------


#endif //_INCLUDE_NETWORKERCOMPONENT_H