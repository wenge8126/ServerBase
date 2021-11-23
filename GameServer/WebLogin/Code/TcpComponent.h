#pragma once

#ifndef _INCLUDE_TCPCOMPONECT_H_
#define _INCLUDE_TCPCOMPONECT_H_

#include "Component.h"
#include "IOCPServerNet.h"
#include "IOCPConnect.h"

class TcpComponent : public NetCloud::ProcessComponent
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

private:
};

class ComponentNet : public DefaultServerNet
{
public:
	ComponentNet(TcpComponent *pComp, int nMaxCount, int nMaxThreadCount)
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

public:
	TcpComponent		*mpComponent;
};

class ComponentConnect : public IOCPServerConnect
{
public:
	TcpComponent* GetComponent()
	{
		Hand<ComponentNet> net = GetNetHandle();
		return net->mpComponent;
	}
};

#endif //_INCLUDE_TCPCOMPONECT_H_
