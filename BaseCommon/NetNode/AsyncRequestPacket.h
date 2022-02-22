#pragma once
#ifndef _INCLUDE_ASYNCREQUESTPACKET_H_
#define _INCLUDE_ASYNCREQUESTPACKET_H_

#include "RequestPacket.h"
#include "ResponsePacket.h"
#include "CoroutineTool.h"

namespace NetCloud
{

	class AsyncRequestPacket : public tRequestPacket
	{
	public:
		// 此功能需要在异步函数内调用, 原因是此函数为异步阻塞执行
		Auto<tResponseResultPacket> AwaitRequest(UnitID targetID, int overTimeSecond)
		{
			mCoroutine = CORO;
			if (mCoroutine == 0)
			{
				ERROR_LOG("Must AwaitRequest in coroutione function");
				return Auto<tResponseResultPacket>();
			}
			AllotEventIDAndWait(overTimeSecond);
			if (SendPacket(targetID, this))
			{
				CoroutineTool::yield();
				return mResponseResult;
			}
			return Auto<tResponseResultPacket>();
		}

		virtual void OnResponse(HandPacket responseResult) override
		{
			mResponseResult = responseResult;
			CoroutineTool::Resume(mCoroutine);
		}

		virtual void	OnTime() override
		{
			WARN_LOG("RequestPacket over time > %s", GetMsgName());
			CoroutineTool::Resume(mCoroutine);
		}

		virtual void InitData() override
		{
			tRequestPacket::InitData();
			mResponseResult.setNull();
			mCoroutine = 0;
		}

	public:
		AsyncRequestPacket()
			: mCoroutine(0) {}

	protected:
		HandPacket mResponseResult;
		unsigned int		mCoroutine;
	};

}
//-------------------------------------------------------------------------

#endif  //_INCLUDE_ASYNCREQUESTPACKET_H_