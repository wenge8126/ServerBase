
#ifndef _NODENETEVENT_H_
#define _NODENETEVENT_H_

#include "NetHead.h"
#include "ServerEvent.h"
#include "ClientEvent.h"
#include "NetHandle.h"

//using namespace Logic;

class NodeRequestConnect;
class MeshedNodeNet;
//-------------------------------------------------------------------------
class Net_Export tNodeRequestEvent : public Logic::tServerEvent
{
public:
	NodeRequestConnect* GetNodeConnect();

	MeshedNodeNet* GetMeshedNodeNet();

};


class Net_Export tNodeResponseEvent : public Logic::tClientEvent
{
public:
	MeshedNodeNet* GetMeshedNodeNet();
};

//-------------------------------------------------------------------------


#endif //_NODENETEVENT_H_