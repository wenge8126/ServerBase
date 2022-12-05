#pragma once

#ifndef _INCLUDE_HTTPCOMPONECT_H_
#define _INCLUDE_HTTPCOMPONECT_H_

#include "Component.h"
#include "WebServerNet.h"
#include "CommonLib.h"
#include "AsyncProtocol.h"
//-------------------------------------------------------------------------
// Https 组件, 可继承实现需要的功能
class CommonLib_Export  HttpComponect : public NetCloud::ProcessComponent
{
public:
	virtual void Awake() override
	{
		
	}
	virtual void Start() override;

	virtual void Process() override
	{
		mHttpNet->Process();
	}

	virtual void LowUpdate() override
	{
		if (mHttpNet)
			mHttpNet->LowProcess(1000);
	}

	virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress);


	virtual void OnResponseBytes(HandPacket requestMsg, DataBuffer &response, const AString &requestAddress);

	virtual void OnDestory() override
	{
		mHttpNet._free();
	}

public:
	AutoNet		mHttpNet;
	AString			mIP;
	int				mPort;
	AString			mCertFile;		//  pem证书文件
	AString			mKeyFile;			//  ssl Key 证书
	AString			mPassword;
};
//-------------------------------------------------------------------------
template<bool bSSL>
class HttpCommonConnect : public uWS::tWssConnect<bSSL>
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
class CompHttpsWebNet : public uWS::tWssServerNet<bSSL>
{
public:
	CompHttpsWebNet(HttpComponect *pHttps)
		: mpHttps(pHttps)
	{
		
	}

public:
	virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) override
	{
		mpHttps->OnResponse(requestData, response, bPost, requestAddress);
	}

	virtual void OnResponseBytes(HandPacket requestMsg, DataBuffer &response, const AString &requestAddress) override
	{
		mpHttps->OnResponseBytes(requestMsg, response, requestAddress);
	}

	virtual HandConnect CreateConnect() override
	{
		uWS::tWssServerNet<bSSL>::SetNetProtocol(MEM_NEW AsyncProtocol());
		HttpCommonConnect<bSSL> *p = MEM_NEW HttpCommonConnect<bSSL>();
		p->Init(this, "127.0.0.1", 0);
		return p;
	}

protected:
	HttpComponect		*mpHttps;
};

//-------------------------------------------------------------------------


#endif //_INCLUDE_HTTPCOMPONECT_H_
