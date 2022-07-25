#pragma once

#ifndef _INCLUDE_HTTPCOMPONECT_H_
#define _INCLUDE_HTTPCOMPONECT_H_

#include "Component.h"
#include "WebServerNet.h"
#include "CommonLib.h"
//-------------------------------------------------------------------------
// Https ���, �ɼ̳�ʵ����Ҫ�Ĺ���
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

	virtual void OnDestory() override
	{
		mHttpNet._free();
	}

public:
	AutoNet		mHttpNet;
	AString			mIP;
	int				mPort;
	AString			mCertFile;		//  pem֤���ļ�
	AString			mKeyFile;			//  ssl Key ֤��
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
	virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) override
	{
		mpHttps->OnResponse(requestData, response, bPost, requestAddress);
	}

protected:
	HttpComponect		*mpHttps;
};

//-------------------------------------------------------------------------


#endif //_INCLUDE_HTTPCOMPONECT_H_