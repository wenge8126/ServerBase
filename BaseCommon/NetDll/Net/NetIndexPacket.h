
#ifndef _INCLUDE_NETINDEXPACKET_H_
#define _INCLUDE_NETINDEXPACKET_H_

#include "EventPacket.h"
#include "PacketFactory.h"
//-------------------------------------------------------------------------*/
// 通知消息索引，优化使用网络消息ID反序列消息
class NotifyEventIndexPacket : public DataPacket
{
public:
	NotifyEventIndexPacket()
	{
		SetData(MEM_NEW DataBuffer(512), 0);
	}

	virtual	PacketID_t	GetPacketID( ) const { return  eNotifyNetEventID; } 

	virtual UINT		Execute( tNetConnect* pConnect ) 
	{
		pConnect->GetNetHandle()->GetEventCenter()->UpdateNetMsgIndex(mData);
		pConnect->GetNetHandle()->OnUpdateNetMsgIndex();
		return 0;
	}

	// 此消息数据不进行加密
	virtual void SetNeedEncrypt(bool bNeed) const override { mStateData.close(ePacketStateEncrypt); }
};
class IndexPacketFactory : public PacketFactory
{
public:
	virtual HandPacket	CreatePacket () override { return mIndexPacket; }
	virtual PacketID_t	GetPacketID ()const override { return eNotifyNetEventID; } 
	virtual	void		destroyPacket(Packet* pPacket) override {}

	virtual const char* GetPacketName() override {
		return "IndexPacketFactory";
	}

public:
	HandPacket	mIndexPacket;

public:
	IndexPacketFactory()
	{
		mIndexPacket = MEM_NEW NotifyEventIndexPacket();
	}
};
//-------------------------------------------------------------------------*/
// 心跳包，由服务器隔10秒统一发送给客户端，客户端只更新最后接收包时间
// NOTE: 目前服务器拒绝接收此消息
class HeartBeatPacket : public Packet
{
public:
	virtual	PacketID_t	GetPacketID( ) const { return  eNotifyHeartBeat; }

	virtual BOOL		Read(DataStream& iStream, size_t packetSize, tNetConnect* ) override { return TRUE; }
	virtual BOOL		Write( DataStream& oStream ) const { return TRUE; }

	virtual UINT		Execute( tNetConnect* pConnect ) 
    { 
//#ifdef __SERVER__
//        WARN_LOG("No thing, now server can not this packet"); 
//#endif
//		NOTE_LOG("接收到一个心跳包");
        pConnect->OnPing(this); 
        return 0; 
    }

	virtual UINT		GetState() const { return 0; }
	virtual VOID		SetState(UINT stateData) { }
};
class HeartBeatPacketFactory : public PacketFactory
{
public:
	virtual HandPacket	CreatePacket () override  { return mHeartPacket; }
	virtual PacketID_t	GetPacketID ()const override { return eNotifyHeartBeat; } 
	virtual	void		destroyPacket(Packet* pPacket) override {}

	virtual const char* GetPacketName() override {
		return "HeartBeatPacketFactory";
	}

public:
	HandPacket	mHeartPacket;

public:
	HeartBeatPacketFactory()
	{
		mHeartPacket = MEM_NEW HeartBeatPacket();
	}
};
//-------------------------------------------------------------------------*/

#endif //_INCLUDE_NETINDEXPACKET_H_