#pragma once
#ifndef _INCLUDE_ACTORMSG_H_
#define _INCLUDE_ACTORMSG_H_

#include "BaseMsg.h"
#include "NetConfig.h"
#include "AsyncRequestPacket.h"
#include "NetHead.h"
#include "DefinePacketFactory.h"
#include "ActorDBLib.h"

class tBaseMsg;

namespace NetCloud
{
	enum ActorMsgType
	{
		eMsgRequest = PACKET_MAX+1,
		eMsgResponse,
		eMsgResult,
		eMsgNotify,
	};

	class Actor;
	class ActorResponResultPacket;

	typedef int (*pActorMsgCall)(Actor*, DataStream*, ActorResponResultPacket*);
	typedef void(*pActorNotifyMsgCall)(Actor*, DataStream*, UnitID);

	class ActorDBLib_Export ActorRequestPacket :  public AsyncRequestPacket
	{
	public:
		static void RegisterDBPacket(tNetProtocol *pProtocel);

		static void RegisterDBPacket(tNetNode *pNetNode);

		static void RegisterDBPacket(tGate *pNetGate);

	public:
		ActorRequestPacket()
		{
			mRequestData = MEM_NEW DataBuffer();
		}

		void SetReqeustMsgData(tBaseMsg &msg)
		{
			mMsgName = msg.GetMsgName();
			mRequestData->clear(false);
			if (!msg.serialize(mRequestData.getPtr()))
				ERROR_LOG("Msg seralize fail : \r\n%s", msg.dump().c_str())
			//else
			//	LOG("Msg send data :\r\n%s", msg.dump().c_str());
		}

		Auto<tResponseResultPacket> Await(tBaseMsg &requestMsg, UnitID targetID, int overTimeSecond)
		{
			SetReqeustMsgData(requestMsg);
			return AwaitRequest(targetID, overTimeSecond);
		}

		Auto<tResponseResultPacket> Await(const AString &requestMsgMame, const  tNiceData &requestMsg, UnitID targetID, int overTimeSecond)
		{
			mMsgName = requestMsgMame;
			mRequestData->clear(false);
			if (!requestMsg.serialize(mRequestData.getPtr()))
				ERROR_LOG("Msg seralize fail : \r\n%s", requestMsg.dump().c_str())
			return AwaitRequest(targetID, overTimeSecond);
		}

		virtual	PacketID_t	GetPacketID() const override { return eMsgResponse; }

	public:
		virtual UINT GetPacketSize() const { return AsyncRequestPacket::GetPacketSize() + mRequestData->dataSize() + sizeof(DSIZE) + sizeof(StrLenType)+mMsgName.length(); }
		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			ERROR_LOG("Not can send ActorRequestPacket msg, so not call Read");
			AssertEx(0, "Not can send ActorRequestPacket msg, so not call Read");
			return FALSE;
		}

		virtual BOOL Write(DataStream& oStream)const
		{
			oStream.write(mMsgName);
			oStream.writeData((DataStream*)mRequestData.getPtr(), mRequestData->dataSize());
			return AsyncRequestPacket::Write(oStream);
		}

		virtual void InitData() override
		{
			AsyncRequestPacket::InitData();
			mMsgName.setNull();
			mRequestData->clear(false);
		}

	public:
		AutoData		mRequestData;
		AString			mMsgName;
	};
	//-------------------------------------------------------------------------
	// 用于Gate中转包时RequestPacket
	class GateTransferRequestPakcet : public tCloudPacket
	{
	public:
		virtual UINT GetPacketSize() const { return tCloudPacket::GetPacketSize() + mRequestData->dataSize() + sizeof(DSIZE) + sizeof(StrLenType) + mMsgName.length(); }
		virtual BOOL Write(DataStream& oStream)const
		{
			oStream.write(mMsgName);
			oStream.writeData((DataStream*)mRequestData.getPtr(), mRequestData->dataSize());
			return tCloudPacket::Write(oStream);
		}
		
		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			iStream.readString(mMsgName);
			mRequestData->clear(false);
			if (iStream.readData(mRequestData.getPtr()))
				return tCloudPacket::Read(iStream, packetSize - mRequestData->dataSize() - sizeof(DSIZE) - sizeof(StrLenType) - mMsgName.length());
			return FALSE;
		}

		virtual void InitData() override
		{
			tCloudPacket::InitData();
			mMsgName.setNull();
			mRequestData->clear(false);
		}
	public:
		GateTransferRequestPakcet()
		{
			mRequestData = MEM_NEW DataBuffer();
		}
	public:
		AutoData		mRequestData;
		AString			mMsgName;
	};

	//-------------------------------------------------------------------------
	class ActorDBLib_Export ActorResponsePacket : public tResponsePacket
	{
	public:
		virtual void Response(tResponseResultPacket *pResponse) override;

		// 转化为异步执行
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			tCloudPacket::Execute(pConnect);
			ASYNCAUTO(&ActorResponsePacket::AsyncExecute, this);
			return 0;
		}

		void AsyncExecute()
		{
			if (mFactoryPtr->mpFactory == NULL)
				return;
			HandPacket p = CreatePacket(mFactoryPtr->mpFactory->GetResponsePacketID());
			Auto<tResponseResultPacket> respPacket = p;
			if (respPacket)
			{
				Response(respPacket.getPtr());
				respPacket->SetEventID(GetEventID());

				SendPacket(mSenderID, respPacket.getPtr());
			}
			else
				ERROR_LOG("xxxxxxxx %s %d is not ResponseResultPacket", mMsgName.c_str(), mFactoryPtr->mpFactory->GetResponsePacketID());
		}

	public:
		virtual UINT GetPacketSize() const override 
		{ 
			ERROR_LOG("Not can send ActorResponsePacket msg, so not call GetPacketSize");
			AssertEx(0, "Not can send ActorResponsePacket msg, so not call GetPacketSize");
			return 0; 
		}
		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			iStream.readString(mMsgName);
			mRequestData->clear(false);
			if (iStream.readData(mRequestData.getPtr()))
				return tResponsePacket::Read(iStream, packetSize - mRequestData->dataSize() - sizeof(DSIZE)-sizeof(StrLenType)-mMsgName.length());
			return FALSE;
		}

		virtual BOOL Write(DataStream& oStream) const override
		{
			ERROR_LOG("Not can send ActorResponsePacket msg, so not call Write");
			AssertEx(0, "Not can send ActorResponsePacket msg, so not call Write");
			return FALSE;
		}

		virtual void InitData() override
		{
			tResponsePacket::InitData();
			mMsgName.setNull();
			mRequestData->clear(false);
		}

	public:
		ActorResponsePacket()
		{
			mRequestData = MEM_NEW DataBuffer();
		}

	public:
		AString			mMsgName;
		AutoData		mRequestData;
	};
	//-------------------------------------------------------------------------
	class ActorResponResultPacket : public tResponseResultPacket
	{
	public:
		virtual PacketID_t GetPacketID() const override { return  eMsgResult; }

	public:
		virtual UINT GetPacketSize() const { return tResponseResultPacket::GetPacketSize() + mResultData->dataSize() + sizeof(DSIZE) + sizeof(mResultType); }
		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			iStream.read(mResultType);
			mResultData->clear(false);
			if (iStream.readData(mResultData.getPtr()))
				return tResponseResultPacket::Read(iStream, packetSize - mResultData->dataSize() - sizeof(DSIZE)-sizeof(mResultType));
			return FALSE;
		}

		virtual BOOL Write(DataStream& oStream)const
		{
			oStream.write(mResultType);
			oStream.writeData((DataStream*)mResultData.getPtr(), mResultData->dataSize());
			return tResponseResultPacket::Write(oStream);
		}

	public:
		ActorResponResultPacket()
		{
			mResultData = MEM_NEW DataBuffer();
		}

		virtual void InitData() override
		{
			tResponseResultPacket::InitData();
			mResultData->clear(false);
			mResultType = eNoneError;
		}

	public:
		AutoData				mResultData;
		int						mResultType = eNoneError;
	};

	//-------------------------------------------------------------------------
	// 一般消息
	class ActorNotifyPacket : public tCloudPacket
	{
	public:
		bool SendMsg(AUnit localUnit, UnitID targetID, const  tBaseMsg &msg)
		{
			return SendMsg(localUnit, targetID, msg.GetMsgName(), msg);
		}
		bool SendMsg(AUnit localUnit, UnitID targetID, const AString &requestMsgMame, const  tNiceData &msg)
		{
			mNetUnit = localUnit;
			mMsgName = requestMsgMame;
			mMsgData->clear(false);
			if (!msg.serialize(mMsgData.getPtr()))
				ERROR_LOG("Msg seralize fail : \r\n%s", msg.dump().c_str())

			return SendPacket(targetID, this);
		}

		virtual bool		SendPacket(UnitID target, Packet *pPacket) override
		{
			if (mNetUnit)
				return mNetUnit->SendPacket(target, pPacket, (BROADCAST_MODE)mNotifyType);
			ERROR_LOG("Packet %d send fail, No exist net base", pPacket->GetPacketID());
			return false;
		}

		void _AsyncDo();

	public:
		virtual PacketID_t GetPacketID() const override { return  eMsgNotify; }

		virtual UINT		Execute(tNetConnect* pConnect) override
		{
			tCloudPacket::Execute(pConnect);
			ASYNCAUTO(&ActorNotifyPacket::_AsyncDo, this);
			return 0;
		}

	public:
		virtual UINT GetPacketSize() const { return tCloudPacket::GetPacketSize() + mMsgData->dataSize() + sizeof(DSIZE) + sizeof(mNotifyType) + sizeof(StrLenType) + mMsgName.length(); }
		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			iStream.read(mNotifyType);
			iStream.readString(mMsgName);
			mMsgData->clear(false);
			if (iStream.readData(mMsgData.getPtr()))
				return tCloudPacket::Read(iStream, packetSize - mMsgData->dataSize() - sizeof(DSIZE) - sizeof(mNotifyType) -  sizeof(StrLenType) - mMsgName.length());
			return FALSE;
		}

		virtual BOOL Write(DataStream& oStream)const
		{
			oStream.write(mNotifyType);
			oStream.writeString(mMsgName);
			oStream.writeData((DataStream*)mMsgData.getPtr(), mMsgData->dataSize());
			return tCloudPacket::Write(oStream);
		}

	public:
		ActorNotifyPacket()
		{
			mMsgData = MEM_NEW DataBuffer();
		}

		virtual void InitData() override
		{
			tCloudPacket::InitData();
			mMsgData->clear(false);
			mMsgName.setNull();
			mNotifyType = eNoneError;
		}

	public:
		AutoData				mMsgData;
		AString					mMsgName;
		int						mNotifyType = eNoneError;
	};
}

#endif //_INCLUDE_ACTORMSG_H_