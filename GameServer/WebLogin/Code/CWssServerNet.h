#pragma once
#ifndef _INCLUDE_CWSSERVERNET_H_
#define _INCLUDE_CWSSERVERNET_H_
#include "Hand.h"
#include "WebGameNet.h"
#include "BaseEventNet.h"
#include "Lock.h"

#	include "WebServerNet.h"

#define  WSS_MSG_HEAD_SIZE		3		// (ushort)size + 1 msg ID
//-------------------------------------------------------------------------

template<bool bSSL>
class WebPlayer : public uWS::tWssConnect<bSSL>
{
	typedef uWS::tWssConnect<bSSL> Base;

public:
	virtual void SetNetID(int netID) {}
	virtual int GetNetID() const { return 0; }

	virtual bool Send(const Packet  *msgPacket, bool bEncrypt)
	{
		ERROR_LOG("Can not use WebPlayer::Send");
		return false;
	}

	virtual void OnSucceedSendEvent(Logic::tEvent *pEvent, Packet *p) {}
};
//-------------------------------------------------------------------------

class WebLoginThread;

template<bool bSSL>
class CWssServerNet : public uWS::tWssServerNet<bSSL>
{
	typedef uWS::tWssServerNet<bSSL> Base;

public:
	WebLoginThread *mpLoginThread;

public:
	CWssServerNet(WebLoginThread *pThread);

public:
	virtual HandConnect CreateConnect() override
	{
		return MEM_NEW WebPlayer<bSSL>();
	}

public:
	virtual bool SendToClient(Int64 playerID, Logic::tEvent *pEvent) override;

	virtual void BroatcastMsg(Logic::tEvent *pMsg, Int64 excludeID) override;

	// 只清除Player列表, 连接不强制断开, 客户端在任何请求回复为未登陆状态时, 则重新登陆
	virtual void InitClearAllPlayer() override;

	virtual void OnResponse(const AString &requestData, std::string_view &response, bool bPost, const AString &requestAddress) override;
	

};

//-------------------------------------------------------------------------
template<bool bSSL>
class HttpsWebNet : public CWssServerNet<bSSL>
{
public:
	HttpsWebNet(WebLoginThread *pThread)
		: CWssServerNet<bSSL>(pThread)
	{}

public:
	virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) override;
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_WSSERVERNET_H_
