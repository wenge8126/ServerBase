#pragma once

#ifndef _INCLUDEWSSWEBCOMPONENT_H_
#define _INCLUDEWSSWEBCOMPONENT_H_

#include "TcpComponent.h"
#include "WebServerNet.h"

//-------------------------------------------------------------------------
class  WssWebComponent : public SocketComponent
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
class ActorDBLib_Export_H WssWebNet : public uWS::tWssServerNet<bSSL>
{
public:
	virtual bool OnAddConnect(tNetConnect *pConnect)
	{
		mConnectList.add(HandConnect(pConnect));
		return mpComponent->OnConnected(pConnect);
	}

	virtual void OnCloseConnect(tNetConnect *pConnect) override
	{

	}

	virtual void LowProcess(int spaceTime) override
	{
		uWS::tWssServerNet<bSSL>::LowProcess(spaceTime);
		for (int i=mConnectList.size()-1; i>=0; --i)
		{
			Hand< BaseWebConnect> conn = mConnectList[i];
			if (conn)
				conn->ProcessPing();
			else
				mConnectList.removeAt(i);				
		}
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

	virtual void StopNet(void) override
	{
		for (int i = mConnectList.size() - 1; i >= 0; --i)
		{
			Hand< BaseWebConnect> conn = mConnectList[i];
			conn._free();
		}
		mConnectList.clear();
		uWS::tWssServerNet<bSSL>::StopNet();
	}

protected:
	SocketComponent		*mpComponent;

	ArrayList<HandConnect>	mConnectList;
};

//-------------------------------------------------------------------------

#endif //_INCLUDEWSSWEBCOMPONENT_H_