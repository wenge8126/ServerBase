#pragma once

#include "Component.h"

#define PING_CLIENT_OVERTIMESECOND		(60)

// 用于PlayerActor 评测对应的ClientActor, 确定客户端连接是否正常
class PingClientComponent : public Component
{
public:
	void OnPingClientOverTime()
	{
		ERROR_LOG("%s LoginClient ping over time, may be client disconnect", mpActor->GetID().dump().c_str());
	}

public:
	void Start() override
	{
		WaitTime(PING_CLIENT_OVERTIMESECOND);
	}

	bool _OnTimeOver() override
	{
		Hand<Component> self = GetSelf();
		CoroutineTool::AsyncCall([=]()
		{
			if (self && mpActor != NULL)
			{
				GL_RequestPingClient msg;
				LG_ResopnsePingClient resp;

				if (mpActor->Await({ Actor_LoginClient, mpActor->GetID().id }, msg, resp, PING_CLIENT_OVERTIMESECOND / 2))
				{
					WaitTime(PING_CLIENT_OVERTIMESECOND);
				}
				else
					OnPingClientOverTime();				
			}
		});

		return true;
	}
};
