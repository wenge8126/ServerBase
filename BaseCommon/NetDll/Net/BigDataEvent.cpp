#include "BigDataEvent.h"



void NetCloud::BigMsgUnit::Ready()
{
	REGISTER_REQUEST_EVENT(GetEventCenter(), Cloud_SendDataPartEvent);
	REGISTER_REQUEST_EVENT(GetEventCenter(), Cloud_RevDataPartEvent);
	GetEventCenter()->RegisterEvent("_R_Cloud_SendDataPartEvent", MEM_NEW EventFactory<Cloud_RevDataPartEvent>());
	GetEventCenter()->RegisterEvent("_R_Cloud_RevDataPartEvent", MEM_NEW EventFactory<Cloud_SendDataPartEvent>());

	REGISTER_EVENT(GetEventCenter(), WaitReceiveBigMsgData);
}
