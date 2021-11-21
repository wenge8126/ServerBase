#pragma once

#ifndef _INLCUDE_WAITREQUESTMANAGER_H_
#define _INLCUDE_WAITREQUESTMANAGER_H_

#include "Timer.h"
#include "EventCore.h"
#include "EasyStack.h"

class Packet;
class tNetConnect;

namespace NetCloud
{
	class tRequestPacket;
	class tResponseResultPacket;
	class tCloudPacket;

	//-------------------------------------------------------------------------
	// 每个NetNode中创建一个此实例, 用于等待处理回复消息
	// 只用来放置等待, 继承 ResponseResultPacket 的包, 都会在此工厂内查找等待的请求消息	
	//-------------------------------------------------------------------------
	class WaitRequestManager : public AutoBase
	{
	public:
		void AllotEventIDAndWait(tRequestPacket *serverEvent, int waitMilSecond);
		void FreeServerEvent(tRequestPacket *serverEvent);
		void OnResponseResult(tResponseResultPacket *pResultPacket);
		void OnResponseResultPartData(tCloudPacket *pResultPacket, tNetConnect* pConnect);

		Packet* FindServerEvent(EVENT_ID id);

		void Process() { mTimeCenter->Process(); }

		void Close() { mTimeCenter->ClearAll(); }

	public:
		WaitRequestManager()
		{
			// 目的ID从1开始
			mWaitRequestPacketHash.push_back(NULL);
			mTimeCenter = MEM_NEW TimerCenter();
		}

		~WaitRequestManager()
		{
			mTimeCenter._free();
		}

	public:
		// 因为Packet 在回收时必须执行InitData, 在InitData中保证ID释放, 释放时, 将从这个列表中移除, 所以, 指针一直有效
		Array<Packet*>				mWaitRequestPacketHash;
		EasyStack<UINT>				mIDStack;

		Hand<TimerCenter>		mTimeCenter;
	};
	typedef Auto< WaitRequestManager> AWaitRequestManager;
	//-------------------------------------------------------------------------

}

#endif //_INLCUDE_WAITREQUESTMANAGER_H_