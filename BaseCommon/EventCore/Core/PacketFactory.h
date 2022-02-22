


#ifndef __PACKETFACTORY_H__
#define __PACKETFACTORY_H__

#include "Packet.h"
#include "EasyStack.h"
#include "Auto.h"
#include "Hand.h"
#include "EventCore.h"

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

	virtual const char* GetPacketName() = 0;

public:
	virtual HandPacket	CreatePacket ()  = 0;
	virtual	void		destroyPacket(Packet* pPacket) = 0;

	virtual PacketID_t	GetPacketID() const = 0;

	virtual void SetReponsePacketID(int responseResultID) { mID = responseResultID; }
	virtual int GetResponsePacketID() const { return mID; }

	virtual void ProcessPacket(tNetConnect *pConnect, Packet *pPacket)
	{
		ERROR_LOG("No over write PacketFactory:ProcessPacket");
	}

public:
	Auto<PacketFactoryPtr> mOwnerPtr;
};

typedef Auto<PacketFactory> AutoPacketFactory;

//-------------------------------------------------------------------------*/
class BasePacket : public Packet
{
public:
	virtual BOOL		Read(DataStream& iStream, size_t packetSize, tNetConnect* ) override { return TRUE; }
	virtual BOOL		Write( DataStream& oStream ) const  override { return TRUE; }

	virtual UINT		Execute( tNetConnect* pConnect ) 
	{ 
		if (mFactoryPtr->mpFactory != NULL)
			mFactoryPtr->mpFactory->ProcessPacket(pConnect, this);
		return 0;  
	}

	virtual UINT		GetState() const { return 0; }
	virtual VOID		SetState(UINT stateData) { }
};
//-------------------------------------------------------------------------*/
#endif
