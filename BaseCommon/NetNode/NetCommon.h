#pragma once
#ifndef _INCLUDE_NETCOMMON_H_
#define _INCLUDE_NETCOMMON_H_

#include "EventPacket.h"
#include "NetAddress.h"
#include "NetHead.h"

#define DEBUG_CLOUD_NET		0

#define GATE_HASH_KEY_MAX			1103				// ��������ϣKEY, NOTE: �����������ɴ��ڴ�ֵ

#define UNITID_DUMP(id)			id.dump().c_str()
#define UNITID_UINT64(id)			UnitID(id).dump().c_str()

namespace NetCloud
{
	typedef NetAddress	Address;

	//-------------------------------------------------------------------------*/
	// ���絥ԪID

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

		AString dump() const
		{
			AString info;
			info.Format("[t: %s, id: %s]", STRING(type), STRING(id));
			return info;
		}
	};

	//-------------------------------------------------------------------------*/
	// ���ӻ�����, ���Ӻ�, �˴˾Ϳ�ʼ���ͽڵ���Ϣ, ��ʼ����ʱ, ���뵽���������б�, �����ظ�����
	// һ���ڵ������ֵ������������, һ�������ӶԷ�, һ���ǶԷ�������
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
// �ڵ�֮�䴫���Ψһ����Ϣ������, �˰�������, �Ƿ��͵İ����������� 
// ����, ��ͨ���ڵ�Ĵ�����ִ��
	class Net_Export NodePacket : public Packet
	{
	public:
		NodePacket()
			: mTargetID()
			, mSenderID()
			, mSendCount(0)
			, mbBroadcast(false)
		{}

		virtual void InitData() override
		{
			mTargetID = 0;
			mSenderID = 0;
			mSendCount = 0;
			mbBroadcast = false;
#if DEBUG_CLOUD_NET
			mSendInfo.setNull();
#endif			
		}

	public:
		virtual UINT GetPacketSize() const { return _getInfoSize(); }

		virtual int _getInfoSize() const
		{
			return	sizeof(mSendCount) +sizeof(mbBroadcast) + sizeof(mSenderID) + sizeof(mTargetID) 
#if DEBUG_CLOUD_NET
				+ mSendInfo.length() + sizeof(StrLenType)
#endif
				;
		}


		virtual BOOL Read(DataStream& iStream, size_t packetSize)
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

			return TRUE;
		}

		virtual UINT		GetState() const { return 0; }
		virtual VOID		SetState(UINT stateData) {  }

		static int getStringSize(const AString &str) { return  str.length() + sizeof(StrLenType); }

		virtual void OnStartSend(tNetUnit *pSendUnit);

		virtual void OnBeforeSend(tNodeConnect *pNode);
		virtual void OnBeforeSend(tGateConnect *pNode);

		virtual bool OnBeforeRun(tNetUnit *pUnit);

	public:
		UnitID	mSenderID;
		UnitID	mTargetID;

		byte		mbBroadcast;
		byte		mSendCount;				// �����͹����ٴ�, ����3��, ����ת��
#if DEBUG_CLOUD_NET
		AString	mSendInfo;
#endif
	};
	//-------------------------------------------------------------------------*/
	//-------------------------------------------------------------------------*/
	// �ڵ�֮�䴫���Ψһ����Ϣ������, �˰�������, �Ƿ��͵İ�����������  PACKET_NODE
	// ����, ��ͨ���ڵ�Ĵ�����ִ��
	class TransferPacket : public NodePacket
	{
	public:
		TransferPacket()
			: mTargetPacketID(0)
		{}

		virtual void InitData() override 
		{
			NodePacket::InitData();
			mTargetPacketID = 0;
			mData.clear();
		}

	public:
		DataBuffer& GetData() { return mData; }

		virtual	PacketID_t	GetPacketID() const { return PACKET_TRANSFER; }
		virtual UINT GetPacketSize() const {	return _getInfoSize() + mData.dataSize(); }

		virtual int _getInfoSize() const
		{
			return	sizeof(mTargetPacketID) + NodePacket::_getInfoSize();
		}

		virtual BOOL Read(DataStream& iStream, size_t packetSize)
		{
			if (NodePacket::Read(iStream, packetSize) == FALSE)
				return FALSE;
			iStream.read(mTargetPacketID);
			size_t size = packetSize - _getInfoSize();

			mData.clear(false);
			if (mData.size() < (DSIZE)size)
				mData.resize(size);
			if (iStream.Read(mData.data(), (UINT)size) == size)
			{
				mData.setDataSize(size);
				return TRUE;
			}
			return FALSE;
		}
		virtual BOOL Write(DataStream& oStream)const
		{
			if (NodePacket::Write(oStream) == FALSE)
				return FALSE;
			oStream.write(mTargetPacketID);

			size_t size = mData.dataSize();

			const char *szData = ((DataStream*)&mData)->data();

			if (oStream.Write(szData, (UINT)size) == size)
				return TRUE;

			return FALSE;
		}

		virtual UINT		GetState() const { return 0; }
		virtual VOID		SetState(UINT stateData) {  }

	public:
		DataBuffer  mData;
		PacketID_t mTargetPacketID;
	};
	//-------------------------------------------------------------------------*/

}

#endif //_INCLUDE_NETCOMMON_H_