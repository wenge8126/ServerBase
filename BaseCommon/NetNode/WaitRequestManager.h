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
	// ÿ��NetNode�д���һ����ʵ��, ���ڵȴ�����ظ���Ϣ
	// ֻ�������õȴ�, �̳� ResponseResultPacket �İ�, �����ڴ˹����ڲ��ҵȴ���������Ϣ	
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
			// Ŀ��ID��1��ʼ
			mWaitRequestPacketHash.push_back(NULL);
			mTimeCenter = MEM_NEW TimerCenter();
		}

		~WaitRequestManager()
		{
			mTimeCenter._free();
		}

	public:
		// ��ΪPacket �ڻ���ʱ����ִ��InitData, ��InitData�б�֤ID�ͷ�, �ͷ�ʱ, ��������б����Ƴ�, ����, ָ��һֱ��Ч
		Array<Packet*>				mWaitRequestPacketHash;
		EasyStack<UINT>				mIDStack;

		Hand<TimerCenter>		mTimeCenter;
	};
	typedef Auto< WaitRequestManager> AWaitRequestManager;
	//-------------------------------------------------------------------------

}

#endif //_INLCUDE_WAITREQUESTMANAGER_H_