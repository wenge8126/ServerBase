#pragma once

#ifndef _INCLUDE_RESPONSEPACKET_H_
#define _INCLUDE_RESPONSEPACKET_H_

#include "RequestPacket.h"


namespace NetCloud
{
	//-------------------------------------------------------------------------
	// 
	class Net_Export tResponsePacket : public tCloudPacket
	{
	public:
		virtual void Response(tResponseResultPacket *pResponse) = 0;

		virtual PacketID_t GetResultPacketID() { return (PacketID_t)mFactoryPtr->mpFactory->GetResponsePacketID(); }

	public:
		virtual UINT		Execute(tNetConnect* pUnit) override;

		virtual void InitData() override
		{
			mID = 0;
		}

	public:
		virtual UINT		GetState() const { return 0; }
		virtual VOID		SetState(UINT stateData) {}
	};
	//-------------------------------------------------------------------------*/

	//-------------------------------------------------------------------------
	// 回复包回复结果的消息包
	class Net_Export tResponseResultPacket : public tCloudPacket
	{
	public:		
		virtual UINT		Execute(tNetConnect* pConnect) override;
	};

	template<typename T, int resultID>
	class DefineResponseResultFactory : public tPacketFactory
	{
	public:
		virtual Packet *_createPacket() { return MEM_NEW T(); }
		virtual PacketID_t	GetPacketID()const { return resultID; }
		virtual void SetPacketID(PacketID_t id) override { AssertEx(0, "Can not use"); }
	};

	//-------------------------------------------------------------------------
	class CommonResponseResult : public tResponseResultPacket
	{
	public:
		AutoNice			mResultData;
	};
	//-------------------------------------------------------------------------*/
	// 结果数据很大时, 可以发送此分包
	class Net_Export ResponseResultPartPacket : public tCloudPacket
	{
	public:
		DataBuffer	mData;
		short			mPartCount;
		short			mPartIndex;
		DSIZE			mTotalSize;

	public:
		ResponseResultPartPacket()
		{
			InitData();
		}

	public:
		virtual UINT		Execute(tNetConnect* pConnect) override;

		virtual int _getInfoSize() const
		{
			return	sizeof(short)*2+mData.dataSize() + tCloudPacket::_getInfoSize();
		}

		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			tCloudPacket::Read(iStream, packetSize);

			iStream.read(mPartCount);
			iStream.read(mPartIndex);
			mData.clear(false);
			return iStream.readData(&mData);
		}
		virtual BOOL Write(DataStream& oStream)const
		{
			tCloudPacket::Write(oStream);
			oStream.write(mPartCount);
			oStream.write(mPartIndex);
			return oStream.writeData((DataStream*)&mData);
		}

		virtual void InitData() override
		{
			tCloudPacket::InitData();
			mPartCount = 0;
			mPartIndex = 0;
			mTotalSize = 0;
			mData.clear(false);
		}
	};
	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_RESPONSEPACKET_H_
