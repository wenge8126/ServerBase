#pragma once
#ifndef _INCULDE_BIGMSGPACKET_H_
#define _INCULDE_BIGMSGPACKET_H_

#include "CEvent.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "EventCenter.h"
#include "EventPacket.h"
#include "IOCPServerNet.h"
#include "IOCPConnect.h"
#include "NetHead.h"

using namespace Logic;

//-------------------------------------------------------------------------
// 接收方等待接收的事件
// 支持同时接收多个大包
// 保存至Player Connect 中的 EasyHash<EVENT_ID, AutoEvent>
// 覆盖实现在 tNetConnect> virtual void OnReceiveEvent(Logic::tEvent *pEvt, const Packet *pPacket){}
// pEvt==NULL , pPacket 不为空, 则为开始接收大消息事件, pEvt不为空, pPacket==NULL, 则表示结束接收, 移除处理
//-------------------------------------------------------------------------
class PartPacket;
class Net_Export WaitReceiveBigMsgEvent : public BaseEvent
{
public:
	static void InitRegister(tNetHandle *pNet);
	static bool SendBigMsg(Packet  *pPacket, tNetConnect *pTargetConnect, int onePartSize = 4 * 1024);
	static void OnConnectRevEvent(tNetConnect *pConnect, EasyHash<EVENT_ID, AutoEvent> &waitRevBigMsgList, Logic::tEvent *pEvent, const Packet *pPacket);

	bool _sendBigMsg(Packet  *pScrPacket, tNetConnect *pTargetConnect, int onePartSize = 4*1024);

	// 保存Player Connect 中的 EasyHash<EVENT_ID, AutoEvent>
	static AutoEvent InitReady(tNetConnect *pConn, PartPacket *pPartMsg);

	void OnRevPartDataMsg(tNetConnect *pConn, PartPacket *pPartMsg);

public:
	virtual bool _DoEvent() override
	{
		WaitTime(60 * 1000);		// 最长不能超过1分钟
		return true;
	}
	virtual bool _OnTimeOver() override
	{
		ERROR_LOG("Receive big msg %d over time", mBigMsgID);
		Finish();
		return true;
	}

	virtual void _OnFinish() override
	{
		// 从连接上移除
		mOwnerConnect->OnReceivePacket(NULL, this);
	}

public:
	virtual void SetData(AutoNice &scrData) { ERROR_LOG("No use WaitReceiveBigMsgEvent::SetData"); }
	virtual tNiceData& GetData(void) { static NiceData mData; return mData; }
	virtual const tNiceData& GetData(void) const { static NiceData mData; return mData; }

public:
	WaitReceiveBigMsgEvent()
		: mPartCount(0)
		, mAlreadyReceiveCode(0)
		, mBigMsgID(0)
		, mTotalDataSize(0)
		, mFlagID(0)
	{
		mRevData = MEM_NEW LoopDataStream(1024*1024*8, 1024*1024*128);
	}

	virtual void InitData() override
	{
		mFlagID = 0;
		mPartCount = 0;
		mAlreadyReceiveCode = 0;
		mBigMsgID = 0;
		mTotalDataSize = 0;
		mRevData->clear(false);
	}

public:
	EVENT_ID		mFlagID;
	PacketID_t mBigMsgID;
	int mTotalDataSize;
	byte	mPartCount;
	byte mAlreadyReceiveCode;
	AutoData mRevData;

	tNetConnect  *mOwnerConnect;
};

//-------------------------------------------------------------------------
// 大数据分包
class Net_Export PartPacket : public Packet
{
public:
	PartPacket() 
		: mPartCode(0)
		, mPacketFlag(0)
	{
		mData = MEM_NEW DataBuffer();
	}

public:
	virtual	PacketID_t	GetPacketID() const { return PACKET_PART_PACKET; }

	virtual BOOL Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect) override
	{
		CHECK_READ_STREAM(mPartCode);
		CHECK_READ_STREAM(mPacketFlag);

		if (mData->size() < (DSIZE)(packetSize - _GetInfoSize()))
			mData->resize((DSIZE)packetSize - _GetInfoSize());

		iStream._read(mData->data(), packetSize - _GetInfoSize());
		mData->setDataSize(packetSize - _GetInfoSize());
		return TRUE;
	}
	virtual BOOL Write(DataStream& oStream) const override
	{
		CHECK_WRITE_STREAM(mPartCode);
		CHECK_WRITE_STREAM(mPacketFlag);

		DataStream *p = (DataStream*)&mTempSendData;
		oStream._write(p->data(), p->dataSize());

		return TRUE;
	}

	virtual UINT		Execute(tNetConnect* pConnect)
	{
		pConnect->OnReceivePacket(this, NULL);
		return 0;
	}

	virtual size_t _GetInfoSize() const
	{
		static size_t s = sizeof(mPartCode) + sizeof(mPacketFlag);
		return s;
	}

	virtual UINT		GetState() const { return 0; }
	virtual VOID		SetState(UINT stateData) override { }

public:
	byte			mPartCode;
	EVENT_ID			mPacketFlag;

	AutoData		mData;
	TempDataBuffer	mTempSendData;
};
//-------------------------------------------------------------------------
// 第一个包
class Net_Export FirstPartPacket : public PartPacket
{
public:
	FirstPartPacket()
		: mTotalDataSize(0)
		, mPartCount(0)
		, mBigMsgID(0)
	{
		
	}

	virtual void InitData() override 
	{ 
		PartPacket::InitData(); 
		mTotalDataSize = 0; 
		mPartCount = 0; 
		mBigMsgID = 0;
	}

public:
	virtual UINT		Execute(tNetConnect* pConnect)
	{
		WaitReceiveBigMsgEvent::InitReady(pConnect, this);
		return 0;
	}

	virtual	PacketID_t	GetPacketID() const { return FIRST_PART_PACKET; }

	virtual BOOL Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect) override
	{
		CHECK_READ_STREAM(mPartCount);
		CHECK_READ_STREAM(mTotalDataSize);
		CHECK_READ_STREAM(mBigMsgID);
		
		return PartPacket::Read(iStream, packetSize, pConnect);
	}
	virtual BOOL Write(DataStream& oStream) const override
	{
		CHECK_WRITE_STREAM(mPartCount);
		CHECK_WRITE_STREAM(mTotalDataSize);
		CHECK_WRITE_STREAM(mBigMsgID);

		return PartPacket::Write(oStream);
	}

	virtual size_t _GetInfoSize() const override
	{
		static size_t s = PartPacket ::_GetInfoSize() + sizeof(mPartCount) + sizeof(mTotalDataSize)+sizeof(mBigMsgID);
		return s;
	}
public:
	byte			mPartCount;
	int			mTotalDataSize;
	PacketID_t			mBigMsgID;
};

//-------------------------------------------------------------------------
// 具有可自动分包功能的连接示例
//-------------------------------------------------------------------------
template<int MsgSendMaxLength>
class BigMsgConnect : public IOCPConnect
{
public:
	BigMsgConnect(int revBufferSize, int sendBufferSize)
		: IOCPConnect(revBufferSize, sendBufferSize)
	{

	}
	~BigMsgConnect()
	{
		for (auto it=mWaitReceiveBigMsgList.begin(); it.have(); it.next())
		{
			AutoEvent evt = it.get();
			evt._free();
		}
	}

public:
	virtual void OnReceivePacket(Packet *msgPacket, Logic::tEvent *pEvent) override
	{
		if (msgPacket!=NULL)
			WaitReceiveBigMsgEvent::OnConnectRevEvent(this, mWaitReceiveBigMsgList, pEvent, msgPacket);
		else
		{
			Hand<WaitReceiveBigMsgEvent> evt = pEvent;
			if (evt)
			{
				mWaitReceiveBigMsgList.erase(evt->mFlagID);
				evt._free();
			}
		}
	}

	virtual bool Send(const Packet  *pMsgPacket, bool bEncrypt) override
	{
		if (pMsgPacket->IsBigPacket())
			return WaitReceiveBigMsgEvent::SendBigMsg((Packet*)pMsgPacket, this, MsgSendMaxLength);
		else
			return IOCPConnect::SendPacket(pMsgPacket, bEncrypt);
	}

protected:
	EasyHash<EVENT_ID, AutoEvent>		mWaitReceiveBigMsgList;
};
//-------------------------------------------------------------------------
template<int MsgSendMaxLength>
class BigMsgServerConnect : public IOCPServerConnect
{
public:
	BigMsgServerConnect(int revBufferSize, int sendBufferSize)
		: IOCPServerConnect(revBufferSize, sendBufferSize)
	{

	}
	~BigMsgServerConnect()
	{
		for (auto it = mWaitReceiveBigMsgList.begin(); it.have(); it.next())
		{
			AutoEvent evt = it.get();
			evt._free();
		}
	}

public:
	virtual void OnReceivePacket(Packet *msgPacket, Logic::tEvent *pEvent) override
	{
		if (msgPacket!=NULL)
			WaitReceiveBigMsgEvent::OnConnectRevEvent(this, mWaitReceiveBigMsgList, pEvent, msgPacket);
		else
		{
			Hand<WaitReceiveBigMsgEvent> evt = pEvent;
			if (evt)
			{
				mWaitReceiveBigMsgList.erase(evt->mFlagID);
				evt._free();
			}
		}
	}

	virtual bool Send(const Packet  *pMsgPacket, bool bEncrypt) override
	{
		if (pMsgPacket->IsBigPacket())
			return WaitReceiveBigMsgEvent::SendBigMsg((Packet*)pMsgPacket, this, MsgSendMaxLength);
		else
			return IOCPConnect::SendPacket(pMsgPacket, bEncrypt);
	}

protected:
	EasyHash<EVENT_ID, AutoEvent>		mWaitReceiveBigMsgList;
};
//-------------------------------------------------------------------------


#endif //_INCULDE_BIGMSGPACKET_H_