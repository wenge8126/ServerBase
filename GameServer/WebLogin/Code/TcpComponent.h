#pragma once

#ifndef _INCLUDE_TCPCOMPONECT_H_
#define _INCLUDE_TCPCOMPONECT_H_

#include "Component.h"
#include "IOCPServerNet.h"
#include "IOCPConnect.h"
#include "NetWorkerComponent.h"

//-------------------------------------------------------------------------
//网络界面组件, 继承实现网络功能 (Tcp WebSocket kcp ...)
class SocketComponent : public NetCloud::ProcessComponent
{
public:
	// 这是组件主动注册
	virtual void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory) = 0;
	virtual AutoNet GetNet() = 0;

public:
	AComponent GetNetWorkerComponent()
	{
		if (!mNetLogicComponent)
			mNetLogicComponent = mpActor->GetComponent<NetWorkerComponent>();

		return mNetLogicComponent;
	}


protected:
	AComponent	mNetLogicComponent;
};
//-------------------------------------------------------------------------


class TcpComponent : public SocketComponent
{
public:
	int		mMaxConnectCount = 16;
	int		mMaxThreadCount = 2;
	AString mServerIp;
	int		mServerPort = 0;

public:
	virtual bool OnConnected(HandConnect connect)
	{
		LOG("Connected [%s:%d]", connect->GetIp(), connect->GetPort());
		return true;
	}

	virtual void OnDisconnect(HandConnect connect)
	{
		LOG("Disconnected [%s:%d]", connect->GetIp(), connect->GetPort());
	}

	virtual HandConnect CreateConnect();

	virtual void RegisterMsg(Logic::tEventCenter *pCenter) {}

public:
	void Awake() override
	{
	}

	void Start() override;

	void Process() override
	{
		mTcpNet->Process();
	}

	void LowUpdate() override
	{
		mTcpNet->LowProcess(1);
	}

	void OnDestory() override
	{
		mTcpNet._free();
	}

protected:
	AutoNet		mTcpNet;

};

class TcpComponentNet : public DefaultServerNet
{
public:
	TcpComponentNet(TcpComponent *pComp, int nMaxCount, int nMaxThreadCount)
		: DefaultServerNet(nMaxCount, nMaxThreadCount)
		, mpComponent(pComp)
	{}

	HandConnect CreateConnect()
	{
		return mpComponent->CreateConnect();
	}

	virtual bool OnAddConnect(tNetConnect *pConnect) override
	{
		return mpComponent->OnConnected(pConnect);
	}
	virtual void OnCloseConnect(tNetConnect *pConnect) override
	{
		mpComponent->OnDisconnect(pConnect);
	}

	virtual AutoAny GetAttachData() override
	{
		AutoAny comp = mpComponent;
		return comp;
	}

public:
	TcpComponent		*mpComponent;
};

class TcpComponentConnect : public IOCPServerConnect
{
public:
	TcpComponent* GetComponent()
	{
		Hand<TcpComponentNet> net = GetNetHandle();
		return net->mpComponent;
	}
};
//-------------------------------------------------------------------------
#endif //_INCLUDE_TCPCOMPONECT_H_
