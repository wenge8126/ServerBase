#pragma once

#ifndef _INCLUDE_NODEMSGEVENT_H_
#define _INCLUDE_NODEMSGEVENT_H_

#include "ServerEvent.h"
#include "ClientEvent.h"
#include "CoroutineTool.h"
#include "AsyncRequestPacket.h"
//-------------------------------------------------------------------------
// 节点业务之间的事件消息
//-------------------------------------------------------------------------
namespace NetCloud
{
	class tMsgEvent : public Logic::CEvent
	{
	public:
		virtual bool _OnEvent(void *pData, const type_info &dataType) override
		{
			if (typeid(tNetUnit) == dataType)
			{
				mUnit = (tNetUnit*)pData;
				return true;
			}
			else if (typeid(UnitID) == dataType)
			{
				mTargetID = *(UnitID*)pData;
				return true;
			}
			ERROR_LOG("type error");
			return false;
		}

		virtual bool Send(int nType = 0, int nTarget = 0) override
		{
			if (mUnit)
			{
				if (mUnit->SendEvent(mTargetID, this))
					return true;

				ERROR_LOG("%s Send fail", GetEventName());
			}
			else
				ERROR_LOG("No exist unit for Send %s", GetEventName());
			return false;
		}

	public:
		AUnit mUnit;
		UnitID	mTargetID;
	};

	class tRequestEvent : public Logic::tServerEvent
	{
	public:
		virtual bool _OnEvent(void *pData, const type_info &dataType) override
		{
			if (typeid(tNetUnit) == dataType)
			{
				mUnit = (tNetUnit*)pData;
				return true;
			}
			else if (typeid(UnitID) == dataType)
			{
				mTargetID = *(UnitID*)pData;
				return true;
			}
			ERROR_LOG("type error");
			return false;
		}

		virtual bool Send(int nType = 0, int nTarget = 0) override
		{
			if (mUnit)
			{
				if (mUnit->SendEvent(mTargetID, this, mBroadcastMode))
					return true;

				ERROR_LOG("%s Send fail", GetEventName());
			}
			else
				ERROR_LOG("No exist unit for Send %s", GetEventName());
			return false;
		}

	public:
		virtual void InitData() override
		{
			tServerEvent::InitData();
			mUnit.setNull();
			mTargetID = UnitID( 0, 0 );
			mBroadcastMode = eBroadcastNone;
		}

	public:
		AUnit mUnit;
		UnitID	mTargetID;
		BROADCAST_MODE mBroadcastMode = eBroadcastNone;
	};

	//-------------------------------------------------------------------------
	// 直接在协程内异步请求
	class AsyncRequestEvent : public tRequestEvent
	{
	public:
		// broadcastMode 使用广播到类型, 可以在不知道对方ID的情况下使用
		AutoEvent AwaitReqeust(AUnit unit, UnitID requestTargetID, float waitSecond, BROADCAST_MODE broadcastMode = eBroadcastNone)
		{
			mWaitCoroID = CORO;
			if (mWaitCoroID == 0)
			{
				ERROR_LOG("tRequestEvent :: Await in coroutine function");
				return AutoEvent();
			}
			mUnit = unit;
			mTargetID = requestTargetID;
			mBroadcastMode = broadcastMode;
			DoEvent(true);
			WaitTime(waitSecond);
			YIELD;
			return mResponse;
		}

		virtual void _OnResp(AutoEvent &respEvent)
		{
			mResponse = respEvent;
			RESUME(mWaitCoroID);
		}

		virtual bool DoTimeOver() override
		{
			WARN_LOG("RequestPacket over time > %s", GetEventName());
			mResponse.setNull();
			RESUME(mWaitCoroID);
			return true;
		}

		virtual void InitData()
		{
			tRequestEvent::InitData();
			mResponse.setNull();
			mWaitCoroID = 0;
		}

		AsyncRequestEvent()
			: mWaitCoroID(0) {}

	protected:
		CoroID	mWaitCoroID;
		AutoEvent mResponse;
	};

	typedef Hand<AsyncRequestEvent> HAsyncRequestEvent;

	//-------------------------------------------------------------------------
	class tResponseEvent : public Logic::tClientEvent
	{
	public:
		virtual bool _OnEvent(void *pData, const type_info &dataType) override
		{
			if (typeid(tNetUnit) == dataType)
			{
				mUnit = (tNetUnit*)pData;
				return true;
			}
			else if (typeid(UnitID) == dataType)
			{
				mSenderID = *(UnitID*)pData;
				return true;
			}
			ERROR_LOG("type error");
			return false;
		}

		virtual void SendResponce(AutoEvent hRespEvent) override
		{
			if (!mUnit->SendEvent(mSenderID, hRespEvent.getPtr()))
				ERROR_LOG("%s 发送回复失败", GetEventName());
		}

	public:
		AUnit mUnit;
		UnitID	mSenderID;
	};
	//-------------------------------------------------------------------------
	// 异步执行处理
	class AsyncResponse : public tResponseEvent
	{
	public:
		virtual void AsyncDo() = 0;

		virtual bool _DoEvent() override
		{
			ASYNC(&AsyncResponse::AsyncDo, this);
			return true;
		}

	};
	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_NODEMSGEVENT_H_