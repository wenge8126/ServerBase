#pragma once

#include "TcpComponent.h"
#include "WebServerNet.h"

//-------------------------------------------------------------------------
class WssWebComponent : public SocketComponent
{
public:
	virtual void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory)
	{
		mWebNet->GetEventCenter()->RegisterEvent(msgName, msgFactory);
	}

	virtual AutoNet GetNet() { return mWebNet; }

public:
	virtual void Start() override;

	void Process() override
	{
		mWebNet->Process();
	}

	void LowUpdate() override;

	void OnDestory();

public:
	AutoNet		mWebNet;
	AString			mCertFile;		//  pem证书文件
	AString			mKeyFile;			//  ssl Key 证书
	AString			mPassword;
};

//-------------------------------------------------------------------------
template<bool bSSL>
class WssConnect : public uWS::tWssConnect<bSSL>
{
public:
	virtual void ProcessPing()
	{

	}

public:
	virtual void SetNetID(int netID) {}
	virtual int GetNetID() const { return 0; }

	
};
//-------------------------------------------------------------------------

template<bool bSSL>
class WssWebNet : public uWS::tWssServerNet<bSSL>
{
public:
	virtual bool OnAddConnect(tNetConnect *pConnect)
	{
		return mpComponent->OnConnected(pConnect);
	}

	virtual HandConnect CreateConnect() override
	{

		return MEM_NEW WssConnect<bSSL>();
		
	}

	virtual AutoAny GetAttachData() override
	{
		AutoAny comp = mpComponent;
		return comp;
	}

public:
	WssWebNet(SocketComponent *pComp)
		: mpComponent(pComp)
	{}

protected:
	SocketComponent		*mpComponent;
};

//-------------------------------------------------------------------------