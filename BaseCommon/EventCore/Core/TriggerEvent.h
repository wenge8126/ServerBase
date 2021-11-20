#pragma once

#ifndef _INCLUDE_TRIGGEREVENT_H_
#define _INCLUDE_TRIGGEREVENT_H_

#include "CEvent.h"
#include "EventCenter.h"

class TriggerEvent : public Logic::CEvent
{
public:
	virtual bool DoEvent(bool bImmediately) override
	{
		bool b = CEvent::DoEvent(bImmediately);

		if (bImmediately)
		{
			GetEventCenter()->DispatchEvent(this);
		}
		return b;
	}
};




#endif //_INCLUDE_TRIGGEREVENT_H_