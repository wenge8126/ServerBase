#pragma once
#ifndef _INLCUDE_UNITCOMPONENT_H_
#define _INLCUDE_UNITCOMPONENT_H_

#include "NetUnit.h"
#include "UnitNetProcess.h"

namespace NetCloud
{
	class Net_Export NetUnit : public tNetUnit
	{
	public:
		virtual AutoEvent StartEvent(const char *szEventName, UnitID targetID)
		{
			Logic::tEventCenter *center = GetEventCenter();
			if (center != NULL)
			{
				AutoEvent evt = center->StartEvent(szEventName);
				if (evt)
				{
					evt->_OnEvent(this, typeid(tNetUnit));
					evt->_OnEvent(&targetID, typeid(UnitID));
				}
				else
					ERROR_LOG("Event %s no register", szEventName);
				return evt;
			}
			ERROR_LOG("Create event %s fail, Event center is not exist");
			return AutoEvent();
		}

		virtual bool OnReceiveProcess(NodePacket *pNodePacket) override;
	};

	//-------------------------------------------------------------------------*/
	// 网络终端应用, 直接组合此组件, 然后使用Init关联处理消息事件回调即可
	//-------------------------------------------------------------------------*/
	class Net_Export UnitComponent : public NetUnit
	{
	public:
		UnitComponent(void *pParent) : mpParent(pParent) {}

		void Init(NetProcess receiveProcess, NetProcess readyCall) { mReceiveCall = receiveProcess; mReadyCall = readyCall; }

		void *GetParent() { return mpParent; }

		virtual void Ready() override 
		{
			mReadyCall(this, NULL);
		}

		virtual bool OnReceiveProcess(NodePacket *pNodePacket) override
		{
			if (!NetUnit::OnReceiveProcess(pNodePacket))
				mReceiveCall(this, pNodePacket);
			return true;
		}

	protected:
		NetProcess			mReceiveCall;
		NetProcess			mReadyCall;
		void						*mpParent;
	};
	//-------------------------------------------------------------------------*/
}
#endif //_INLCUDE_UNITCOMPONENT_H_