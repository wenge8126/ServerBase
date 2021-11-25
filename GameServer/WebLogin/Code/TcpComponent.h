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
	AString mServerIp;
	int		mServerPort = 0;

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

	bool OnConnected(HandConnect connect);
	void OnDisconnect(HandConnect connect)
	{
		LOG("Disconnected [%s:%d]", connect->GetIp(), connect->GetPort());
		Hand<NetWorkerComponent> comp = GetNetWorkerComponent();
		if (comp)
			comp->OnDisconnect(connect);
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

	int		mSafeCode = 0;

public:
	virtual HandConnect CreateConnect();

	virtual void RegisterMsg(Logic::tEventCenter *pCenter) {}

	virtual void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory) override
	{
		if (mTcpNet)
			mTcpNet->GetEventCenter()->RegisterEvent(msgName, msgFactory);
		else
			ERROR_LOG("Now no exist tcp net");
	}

	virtual AutoNet GetNet() override
	{
		return mTcpNet;
	}

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

	void OnDestory() override;

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

	virtual int GetSafeCode() override
	{
		return mpComponent->mSafeCode;
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

	virtual AutoAny GetUserData() override { return mUserData; }
	virtual void SetUserData(AutoAny userData) override { mUserData = userData; }

	virtual ConnectPtr GetPtr() { return ConnectPtr(); }

public:
	TcpComponentConnect()
	{
		mSelfPtr = MEM_NEW _ConnectPtr();
		mSelfPtr->mpConnect = this;
	}
	~TcpComponentConnect()
	{
		mSelfPtr->mpConnect = NULL;
	}

protected:
	AutoAny		mUserData;
	ConnectPtr	mSelfPtr;
};
//-------------------------------------------------------------------------
#endif //_INCLUDE_TCPCOMPONECT_H_
