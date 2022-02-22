//#include "stdafx.h"

#include "PacketFactory.h"

#include "Packet.h"

const char* Packet::GetMsgName() const
{
	if (mFactoryPtr && mFactoryPtr->mpFactory != NULL)
		return mFactoryPtr->mpFactory->GetPacketName();
	return "";
}

void Packet::Release()
{
	if (mFactoryPtr && mFactoryPtr->mpFactory!=NULL) 
		mFactoryPtr->mpFactory->destroyPacket(this); 
	else 
		delete this;
}
