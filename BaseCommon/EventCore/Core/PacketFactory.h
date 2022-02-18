


#ifndef __PACKETFACTORY_H__
#define __PACKETFACTORY_H__

#include "Packet.h"
#include "EasyStack.h"
#include "Auto.h"
#include "Hand.h"
#include "EventCore.h"

//-------------------------------------------------------------------------*/
class PacketFactoryPtr : public AutoBase
{
public:
	PacketFactoryPtr()
		: mpFactory(NULL){}
	PacketFactory *mpFactory;
};
//-------------------------------------------------------------------------*/
class tNetProtocol;
class EVENTCORE_EXPORTS_H PacketFactory : public AutoBase
{
public :
	PacketFactory()
	{
		mOwnerPtr = MEM_NEW PacketFactoryPtr();
		mOwnerPtr->mpFactory = this;
	}
	virtual ~PacketFactory (){  mOwnerPtr->mpFactory = NULL; }

public:
	virtual HandPacket	CreatePacket ()  = 0;
	virtual	void		destroyPacket(Packet* pPacket) = 0;

	virtual PacketID_t	GetPacketID() const = 0;

	virtual void SetReponsePacketID(int responseResultID) { mID = responseResultID; }
	virtual int GetResponsePacketID() const { return mID; }

public:
	Auto<PacketFactoryPtr> mOwnerPtr;
};

typedef Auto<PacketFactory> AutoPacketFactory;

//-------------------------------------------------------------------------*/
class BasePacket : public Packet
{
public:
	virtual	PacketID_t	GetPacketID( ) const { return mFactoryPtr->mpFactory->GetPacketID(); }

	virtual BOOL		Read(DataStream& iStream, size_t packetSize, tNetConnect* ) override { return TRUE; }
	virtual BOOL		Write( DataStream& oStream ) const  override { return TRUE; }

	virtual UINT		Execute( tNetConnect* pConnect ) { return 0;  }

	virtual UINT		GetState() const { return 0; }
	virtual VOID		SetState(UINT stateData) { }
};
//-------------------------------------------------------------------------*/
#endif
