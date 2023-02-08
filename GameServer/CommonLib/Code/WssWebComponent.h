#pragma once

#ifndef _INCLUDEWSSWEBCOMPONENT_H_
#define _INCLUDEWSSWEBCOMPONENT_H_

#include "TcpComponent.h"
#include "WebServerNet.h"
#include "CommonLib.h"
#include "ServerToolMsg.h"
//-------------------------------------------------------------------------
class CommonLib_Export  WssWebComponent : public SocketComponent
{
public:
	virtual void RegisterMsg(int msgIndex, AutoEventFactory msgFactory);

	virtual void RegisterMsg(const AString &msgName, AutoEventFactory msgFactory) override
	{
		RegisterMsg(msgName.toInt(), msgFactory);
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

	// 使用WSS长连接接收HTTP HttpReqeustActorMsg请求消息, 主要是用于上传或下载大数据


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
// 响应 HttpReqeustActorMsg 消息请求事件
// 客户端请求时 将HttpReqeustActorMsg消息数据保存到 MSG_DATA, 回复数据中的["RESP_DATA"]即回复消息数据
class WssReqeustActorMsgEvent : public tClientEvent
{
	virtual void _AsyncDo() override
	{ 
		//NOTE_LOG("%s data > %s", GetEventName(), GetData().dump().c_str());

		if (!mNetConnect)
			return;

		tNetHandle *pNet = mNetConnect->GetNetHandle();
		if (pNet == NULL)
			return;

		Hand< WssWebComponent> comp = pNet->GetAttachData();
		if (comp)
		{
			AutoNice msgData = (tNiceData*)get("MSG_DATA");
			Auto<tBaseMsg> msg = MEM_NEW HttpReqeustActorMsg();
			if (msgData)
			{
				msg->Full(msgData);
				//msgData->seek(0);
				//if (!msg->Read(*msgData, msgData->dataSize(), mNetConnect.getPtr()))
				//	ERROR_LOG("Restor HttpReqeustActorMsg fail")
				if (comp->GetActor() != NULL)
				{				
					AutoNice respData = comp->GetActor()->ResponseBytesHttp(msg, mNetConnect->GetIp());
					if (respData)
					{
						tNiceData &d = *respData;
						GetResponseEvent()->GetData().append(d, true);
					}
					//NOTE_LOG("Resp: %s", GetResponseEvent()->GetData().dump().c_str());
				}
			}
			else
				ERROR_LOG("No MSG_DATA");			
		}
		else
			ERROR_LOG("WssWebComponent no exist");
		Finish();
	}
};

//-------------------------------------------------------------------------
#endif //_INCLUDEWSSWEBCOMPONENT_H_