#pragma once
#ifndef _INCLUDE_NETCOMMON_H_
#define _INCLUDE_NETCOMMON_H_

#include "EventPacket.h"
#include "NetAddress.h"
#include "NetHead.h"

#define DEBUG_CLOUD_NET			1

#define GATE_HASH_KEY_MAX			1103				// 网关最大哈希KEY, NOTE: 网关数量不可大于此值

#define UNITID_DUMP(id)			id.dump().c_str()
#define UNITID_UINT64(id)			UnitID(id).dump().c_str()

//-------------------------------------------------------------------------
enum EMeshNetMsgType
{
	eMeshMsg_RequestNodeInfo = PACKET_MAX + 1,
	eMeshMsg_BroadcastNodeClose,
	eMeshMsg_Max,
};

enum GateMsgType
{
	eN2G_RequestGateInfo = eMeshMsg_Max + 1,
	eN2G_AppendUnit,
	eG2N_NotifyNodeInfo,
	eN2N_RequestUnitList,
	eNGN_TransferMsg,
	eN2G_NotifyNodeClose,
	eN2N_BroadcastUnitNoExist,
};
//-------------------------------------------------------------------------

namespace NetCloud
{
	typedef NetAddress	Address;

	//-------------------------------------------------------------------------*/
	// 网络单元ID

	struct UnitID
	{
		union
		{
			struct {
				Int64				id		: 48;
				Int64				type	: 16;
			};
			UInt64			value;
		};

		UnitID() :type(0), id(0) {}
		UnitID(int ntype, Int64 uid) :type(ntype), id(uid) {}
		UnitID(UInt64 uValue) { *((UInt64*)this) = uValue; }
		operator UInt64 () const { return *((UInt64*)this); }
		UnitID & operator = (UInt64 uValue) { *((UInt64*)this) = uValue; return *this; }
#if __LINUX__
		//operator size_t () const { return (size_t)(return *((UInt64*)this)); }
#endif
		bool operator == (const UnitID &other) const { return *((UInt64*)this) == (UInt64)other; }

		int Hash(int nCount)
		{
			return (int)(value % nCount);
		}

		AString dump() const
		{
			AString info;
			info.Format("[t: %s, id: %s]", STRING(type), STRING(id));
			return info;
		}
	};

	//-------------------------------------------------------------------------*/
	// 连接或被连接, 连接后, 彼此就开始发送节点信息, 开始连接时, 加入到正在连接列表, 避免重复连接
	// 一个节点的索引值中有两个连接, 一个是连接对方, 一个是对方的连接
	class tNodeConnect : public Base<tNodeConnect>
	{
	public:
		virtual bool SendToNode(UnitID targetAddress, Packet *pPacket) = 0;
		virtual AutoEvent StartEvent(const char *eventMsgName) = 0;
		virtual Address GetAddress() const = 0;

		virtual bool IsOK() = 0;

	public:
		virtual ~tNodeConnect()
		{

		}
	};
	typedef Hand<tNodeConnect> ANodeConnect;

	//-------------------------------------------------------------------------*/
	class tGateConnect : public Base<tGateConnect>
	{
	public:
		virtual void SendToGate(UnitID targetAddress, Packet *pPacket) = 0;
		virtual AutoEvent StartEvent(const char *eventMsgName) = 0;
		virtual Address GetAddress() const = 0;
	};
	typedef Hand<tGateConnect> AGateConnect;

	class tNetUnit;
	//-------------------------------------------------------------------------*/
// 节点之间传输的唯一的消息包类型, 此包的数据, 是发送的包的序列数据 
// 处理, 是通过节点的处理函数执行
	class Net_Export NodePacket : public Packet
	{
	public:
		NodePacket()
			: mTargetID()
			, mSenderID()
			, mSendCount(0)
			, mbBroadcast(false)
			, mTargetPacketID(0)
			, mRequestID(0)
			, mMsgType(0)
		{}

		virtual void InitData() override
		{
			mTargetID = 0;
			mSenderID = 0;
			mSendCount = 0;
			mbBroadcast = false;
			mTargetPacketID = 0;
			mRequestID = 0;
			mMsgType = 0;
#if DEBUG_CLOUD_NET
			mSendInfo.setNull();
#endif			
		}

	public:
		virtual BOOL Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect) override
		{
#if DEBUG_CLOUD_NET
			iStream.readString(mSendInfo);
#endif
			iStream.read(mSendCount);
			iStream.read(mbBroadcast);
			UInt64 idValue = 0;
			iStream.read(idValue);
			mSenderID = idValue;
			iStream.read(idValue);
			mTargetID = idValue;
			iStream.read(mMsgType);
			iStream.read(mTargetPacketID);
			iStream.read(mRequestID);

			return TRUE;
		}
		virtual BOOL Write(DataStream& oStream)const
		{
#if DEBUG_CLOUD_NET
			oStream.writeString(mSendInfo);
#endif
			oStream.write(mSendCount);
			oStream.write(mbBroadcast);
			oStream.write((UInt64)mSenderID);
			oStream.write((UInt64)mTargetID);
			oStream.write(mMsgType);
			oStream.write(mTargetPacketID);
			oStream.write(mRequestID);

			return TRUE;
		}

		virtual UINT		GetState() const { return 0; }
		virtual VOID		SetState(UINT stateData) {  }

		static int getStringSize(const AString &str) { return  str.length() + sizeof(StrLenType); }

		virtual void OnStartSend(tNetUnit *pSendUnit);

		virtual void OnBeforeSend(tNodeConnect *pNode);
		virtual void OnBeforeSend(tGateConnect *pNode);

		virtual bool OnBeforeRun(tNetUnit *pUnit);

		AString Dump()
		{ 
			AString info;
			info.Format("%s > %s", mSenderID.dump().c_str(), mTargetID.dump().c_str());
#if DEBUG_CLOUD_NET
			info += " : ";
			info += mSendInfo;
#endif
			return info;
		}

	public:
		UnitID	mSenderID;
		UnitID	mTargetID;

		byte		mbBroadcast;
		byte		mSendCount;				// 被发送过多少次, 超过3次, 不再转发
		byte		mMsgType;
		PacketID_t		mTargetPacketID;
		MSG_ID		mRequestID;
#if DEBUG_CLOUD_NET
		AString	mSendInfo;
#endif
	};
	//-------------------------------------------------------------------------*/
	//-------------------------------------------------------------------------*/
	// 节点之间传输的唯一的消息包类型, 此包的数据, 是发送的包的序列数据  PACKET_NODE
	// 处理, 是通过节点的处理函数执行
	class TransferPacket : public NodePacket
	{
	public:
		TransferPacket()
		{}

		virtual void InitData() override 
		{
			NodePacket::InitData();
			mData.clear();
		}

	public:
		DataBuffer& GetData() { return mData; }

		//virtual UINT Execute(tNetConnect* pConnect) override { ERROR_LOG("TransferPacket execute"); return 0; }
		virtual UINT		Execute(tNetConnect* pConnect)
		{
			if (mFactoryPtr->mpFactory != NULL)
				mFactoryPtr->mpFactory->ProcessPacket(pConnect, this);
			else
				ERROR_LOG("TransferPacket execute");
			return 0;
		}

		virtual BOOL Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect) override
		{
			if (NodePacket::Read(iStream, packetSize, pConnect) == FALSE)
				return FALSE;			

			mData.clear(false);
			if (iStream.readData(&mData))
			{
				return TRUE;
			}
			return FALSE;
		}
		virtual BOOL Write(DataStream& oStream)const
		{
			if (NodePacket::Write(oStream) == FALSE)
				return FALSE;			

			if (oStream.writeData((DataStream*)&mData) )
				return TRUE;

			return FALSE;
		}

		virtual UINT		GetState() const { return 0; }
		virtual VOID		SetState(UINT stateData) {  }

	public:
		DataBuffer  mData;
		
	};
	//-------------------------------------------------------------------------*/

}

#endif //_INCLUDE_NETCOMMON_H_