#pragma once
#ifndef _INCLUDE_REQEUSTPACKET_H_
#define _INCLUDE_REQEUSTPACKET_H_

#include "Timer.h"
#include "Packet.h"
#include "NetUnit.h"
#include "NetHead.h"
#include "Timer.h"
#include "PacketFactory.h"
#include "NetCommon.h"
#include "NetHandle.h"

namespace NetCloud
{

	//-------------------------------------------------------------------------*/
	class tCloudPacket : public NodePacket
	{
	public:
		virtual PacketID_t GetPacketID() const  { if (mFactoryPtr->mpFactory == NULL) return 0; return  mFactoryPtr->mpFactory->GetPacketID(); }
		virtual UINT		Execute(tNetConnect* pConnect) override 
		{ 
			if (pConnect == NULL) 
				return 0;  
			pConnect->GetNetHandle()->OnReceivePacket(this); 
			return 0; 
		}
		virtual void		OnTime() { WARN_LOG("RequestPacket over time"); }
		virtual bool		SendPacket(UnitID target, Packet *pPacket)
		{
			if (mNetUnit)
				return mNetUnit->SendPacket(target, pPacket);
			ERROR_LOG("Packet %s send fail, No exist net base", pPacket->GetMsgName());
			return false;
		}
		virtual HandPacket CreatePacket(PacketID_t packetID)
		{
			if (mNetUnit)
				return mNetUnit->CreatePacket(packetID);
			ERROR_LOG("Create packet %d fail, No exist net base", packetID);
			return HandPacket();
		}

		virtual bool  OnBeforeRun(tNetUnit *pUnit) override
		{
			NodePacket::OnBeforeRun(pUnit);
			mNetUnit = pUnit;
			return true;
		}

		EVENT_ID GetEventID() const { return (EVENT_ID)mID; }
		void SetEventID(EVENT_ID id) { mID = (int)id; }

	public:
		virtual BOOL Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect) override
		{
			iStream.read(mID);

			return NodePacket::Read(iStream, packetSize, pConnect);
		}
		virtual BOOL Write(DataStream& oStream)const
		{
			oStream.write(mID);
			return NodePacket::Write(oStream);
		}

		virtual void InitData() override
		{
			NodePacket::InitData();
			mNetUnit.setNull();
		}

	public:
		Hand<tNetUnit>			mNetUnit;
	};
	//-------------------------------------------------------------------------*/
	// NOTE: 注册此消息时, 注册给工厂的ID, 只用来创建此消息, 
	// 与GetPacketID不同, GetPacketID是接收方接收消息的ID
	class tResponsePacket;
	class Net_Export tRequestPacket : public tCloudPacket
	{
	public:
		virtual	PacketID_t	GetPacketID() const = 0;
		virtual void OnResponse(HandPacket responseResult) = 0;

		virtual void OnResponsePartData(HandPacket partDataPacket, tNetConnect *pConnect)
		{
			ERROR_LOG("%d no write receive part data packet");
		}

	public:
		bool Request(UnitID targetID, int overTimeSecond)
		{
			AllotEventIDAndWait(overTimeSecond);
			return SendPacket(targetID, this);
		}

		// 从等待的消息包中查找
		virtual void AllotEventIDAndWait(int overTimeMilSecond);
		void FreeID();

		bool IsFinish() const { return mbFinish; }
		void SetFinish(bool bFinish) { mbFinish = bFinish; }

	public:
		virtual UINT		Execute(tNetConnect* pUnit) override
		{
			ERROR_LOG("Request %d packet can not Execute");
			return 0;
		}

		virtual void InitData() override 
		{
			FreeID(); 
			if (mID != 0)
				ERROR_LOG("%s Free event id %d fail", GetMsgName(), mID);
			mbFinish = false;			
			tCloudPacket::InitData();
		}

	public:
		tRequestPacket()
			: mbFinish(false)
		{

		}
		~tRequestPacket()
		{
			if (mID != 0)
				FreeID();
		}

		virtual UINT		GetState() const { return 0; }
		virtual VOID		SetState(UINT stateData) {}

	protected:
		bool mbFinish;		
		AWaitRequestManager mWaitRequestManager;
	};
	//-------------------------------------------------------------------------*/
	template<typename T, int typeID>
	class DefineRequestFactory : public tPacketFactory
	{
	public:
		DefineRequestFactory()
		{

		}

	public:
		virtual Packet *_createPacket() { return MEM_NEW T(); }
		virtual PacketID_t	GetPacketID()const { return typeID; }
	};

}

#endif //_INCLUDE_REQEUSTPACKET_H_
