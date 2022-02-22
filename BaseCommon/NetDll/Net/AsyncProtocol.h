
#ifndef _INCLUDE_ASYNCPROTOCOL_H_
#define _INCLUDE_ASYNCPROTOCOL_H_

#pragma once

#include "NetHandle.h"
#include "NetHead.h"
#include "NetConfig.h"
#include "PacketFactory.h"
#include "EventProtocol.h"
#include "BaseMsg.h"
#include "CoroutineTool.h"
#include "Timer.h"

//-------------------------------------------------------------------------
class Net_Export_H ResponseMsgPacket : public BasePacket
{
public:
	virtual PacketID_t GetPacketID() const  {
		return PACKET_RESPONSE_MSG;
	}
	virtual UINT Execute(tNetConnect* pConnect) override;

public:
	virtual BOOL		Read(DataStream& iStream, size_t packetSize, tNetConnect*) override 
	{
		if (!iStream.read(mRequestID))
			return FALSE;
		if (!iStream.readData(&mData))
			return FALSE;
		return TRUE; 
	}
	virtual BOOL		Write(DataStream& oStream) const  override 
	{ 
		oStream.write(mRequestID);
		oStream.writeData((DataStream*)&mData, mData.dataSize());
		return TRUE; 
	}

public:
	MSG_ID		mRequestID;
	DataBuffer	mData;
};
//-------------------------------------------------------------------------
class WaitResponse : public tTimer
{
public:
	void Init()
	{
		mResponsePacket.setNull();
		mWaitCoroID = 0;
		mRequestMsgID = -1;
	}

	virtual void onTime() override
	{
		if (mWaitCoroID > 0)
		{
			RESUME(mWaitCoroID);
			mWaitCoroID = 0;
		}
	}

public:
	HandPacket mResponsePacket;
	CoroID mWaitCoroID = 0;
	MSG_ID mRequestMsgID = -1;
};

typedef Auto<WaitResponse> AWaitResponse;
//-------------------------------------------------------------------------
// 使用时间等待异步请求
class Net_Export AsyncProtocol : public EventNetProtocol
{

public:
	AsyncProtocol();

public:
	AWaitResponse AllotWaitID();

	void FreeWaitID(WaitResponse *pWaitResponse);

	AWaitResponse FindWaitResponse(MSG_ID id);

protected:
	Array<AWaitResponse>			mEventList;
	EasyStack<UINT>				mIDStack;
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_ASYNCPROTOCOL_H_