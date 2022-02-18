
#ifndef _INCLUDE_DEFINEPACKETFACTORY_H_
#define _INCLUDE_DEFINEPACKETFACTORY_H_

#include "PacketFactory.h"
#include "NetConfig.h"


#	define _IOCP_LOCK_PACKET_FACTORY

//-------------------------------------------------------------------------*/
class Net_Export_H tPacketFactory : public PacketFactory
{
public:
	tPacketFactory(int responseResultID = 0)
	{
		SetReponsePacketID(responseResultID);
	}

	virtual Packet *_createPacket() = 0;

public:
    virtual HandPacket	CreatePacket ()  
    {
        _IOCP_LOCK_PACKET_FACTORY
            if (mFreeList.empty())
            {
                //static int i = 0;
                //PRINT("*++ [%d]\n", ++i);
                Packet *p = _createPacket();
                p->mFactoryPtr = mOwnerPtr;
                return p;
            }

            return mFreeList.pop();
    }
    
    virtual	void		destroyPacket(Packet* pPacket)
    {        
            //static int i = 0;
            //PRINT("@-- [%d]\n", ++i);
            pPacket->InitData();
        mFreeList.push(pPacket);
    }

public:
    ~tPacketFactory()
    {        
            while (!mFreeList.empty())
            {
                Packet *p = mFreeList.pop();
                delete p;
            }
    }

public:
    EasyStack<Packet*>	mFreeList;

};

template<typename T, int typeID>
class Net_Export_H DefinePacketFactory : public tPacketFactory
{
public:
	DefinePacketFactory(int responseResultID = 0)
		: tPacketFactory(responseResultID)
	{
	}

public:
	virtual Packet *_createPacket() { return MEM_NEW T(); }
	virtual PacketID_t	GetPacketID()const { return typeID; }


};

#endif //_INCLUDE_DEFINEPACKETFACTORY_H_