#pragma once

#include "Actor.h"
#include "MsgData.h"

class ToolActor : public NetCloud::Actor
{
public:
	virtual void Init() override
	{
		AddComponent("HttpReqeustComponent");
	}
};


