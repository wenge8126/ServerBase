#pragma once

#ifndef _INCLUDE_HTTPCOMPONECT_H_
#define _INCLUDE_HTTPCOMPONECT_H_

#include "Component.h"
#include "WebServerNet.h"

//-------------------------------------------------------------------------
// Https 组件, 可继承实现需要的功能
class HttpComponect : public NetCloud::ProcessComponect
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

	virtual void OnResponse(const AString &requestData, std::string_view &response, bool bPost, const AString &requestAddress)
	{
		GREEN_LOG("Now receive http request <%s>: %s", requestAddress.c_str(), requestData.c_str());
		response = "ok";
	}

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

template<bool bSSL>
class CompHttpsWebNet : public uWS::tWssServerNet<bSSL>
{
public:
	CompHttpsWebNet(HttpComponect *pHttps)
		: mpHttps(pHttps)
	{}

public:
	virtual void OnResponse(const AString &requestData, std::string_view &response, bool bPost, const AString &requestAddress) override
	{
		mpHttps->OnResponse(requestData, response, bPost, requestAddress);
	}

protected:
	HttpComponect		*mpHttps;
};

//-------------------------------------------------------------------------


#endif //_INCLUDE_HTTPCOMPONECT_H_
