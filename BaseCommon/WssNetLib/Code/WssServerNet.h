#pragma once

#ifndef _INCLUDE_WSSSERVERNETLIB_H_
#define _INCLUDE_WSSSERVERNETLIB_H_

#pragma once
#ifndef _INCLUDE_WSSERVERNET_H_
#define _INCLUDE_WSSERVERNET_H_
#include "Hand.h"
#include "NetHandle.h"

#include "WebGameNet.h"
#include "BaseEventNet.h"
#include "Lock.h"

#include "WssNetHead.h"


#define  WSS_MSG_HEAD_SIZE		3		// (ushort)size + 1 msg ID
//-------------------------------------------------------------------------
class WssNet_Export tWssConnect : public tNetConnect
{
public:
	tNetHandle *mpNet;
	ThreadLoopData mWssReceiveData;
	DataBuffer mSendMsgBuffer;
	CsLock      mRevDataLock;
	AString mIP;
	int mPort;

public:
	tWssConnect()
		: mWssReceiveData(16 * 1024)	// big packet > 8192
		, mpNet(NULL)
		, mPort(0)
	{

	}
	~tWssConnect()
	{
		DEBUG_LOG("Now free connect %s:%d", mIP.c_str(), mPort);
	}

	void Init(tNetHandle *pNet, const char *szID, int port)
	{
		mpNet = pNet;
		mIP = szID;
		mPort = port;
	}

	virtual AString GetRemoteAddr(int &port) { port = mPort; return  mIP; };

	virtual void ProcessMsg(LoopData *revFrameData) = 0;

public:
	virtual bool Process() override
	{
		if (mWssReceiveData.mWaitSendBuffer.dataSize() > 0)
		{
			if (mRevDataLock.trylock())
			{
				mWssReceiveData.ProcessSendData();
				mRevDataLock.unlock();
			}
		}
		ProcessMsg(&mWssReceiveData);
		return true;
	}

	// NOTE: bNeedRemove = true 表示需要关闭C#层连接
	virtual void SetRemove(bool bNeedRemove) override;

	virtual tNetHandle* GetNetHandle() override;

	virtual bool SendMsgData(DataStream *pSendMsgData, int frameType);
	virtual bool SendEvent(Logic::tEvent *pEvent) override;
};
typedef Hand<tWssConnect> HandWssConnect;
//-------------------------------------------------------------------------

class WssNet_Export tWssServerNet : public tBaseEventNet
{
public:
	virtual HandConnect CreateConnect() = 0;

public:
	AString mIP;
	int mPort;
	ArrayMap<HandWssConnect>    mWssPlayerMap;
	Hand<WebNetEventCenter> mWebEventCenter;
	

	CsLock              mConnectMapLock;

protected:
	static tWssServerNet    *msCWssServerNet;

public:
	tWssServerNet();

	~tWssServerNet()
	{
		_freeWssNet();
		AssertNote(msCWssServerNet != NULL, "应该存在 CWssServerNet");
		msCWssServerNet = NULL;
	}

	static tWssServerNet& GetMe() { AssertNote(msCWssServerNet != NULL, "Must init CWssServerNet"); return *msCWssServerNet; }

	bool StartNet(const AString &wssServerIP, int serverPort, bool bWss, const AString pfxFileName = "", const AString pfxPassword = "");

public:
	virtual void Process(void) override
	{
		mConnectMapLock.lock();
		for (int i = 0; i < mWssPlayerMap.size(); ++i)
		{
			HandWssConnect player = mWssPlayerMap[i];
			if (player)
			{
				if (player->IsRemove())
				{
					mWssPlayerMap.remove(i);
				}
				else
					player->Process();
			}
		}
		mConnectMapLock.unlock();
		mWebEventCenter->ProcessEvent();
	}

	virtual Logic::tEventCenter* GetEventCenter()const override
	{
		return (Logic::tEventCenter*)mWebEventCenter.getPtr();
	}

	virtual void RegisterMsg(int eventIndex, Logic::AutoEventFactory factory)
	{
		mWebEventCenter->RegisterMsg(eventIndex, factory);
	}

	virtual const char* GetIp(void) const { return mIP.c_str(); }
	virtual int		GetPort(void) const { return mPort; }

protected:
	void _freeWssNet()
	{
		//if (mWssNet != nullptr)
		//{
		//    delete mWssNet;
		//    delete mWssNet->mOnAcceptConnect;
		//    delete mWssNet->mOnConnectClose;
		//    delete mWssNet->mOnConnectError;
		//    delete mWssNet->mProcessMsg;
		//    delete mWssNet->mReceiveFun;
		//    mWssNet = nullptr;
		//}
	}

};

#endif //_INCLUDE_WSSERVERNET_H_



#endif //_INCLUDE_WSSSERVERNETLIB_H_