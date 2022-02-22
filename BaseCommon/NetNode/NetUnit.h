#pragma once

#ifndef _INCLDUE_NETUNIT_H_
#define _INCLDUE_NETUNIT_H_

#include "AnyData.h"
#include "NetCommon.h"

#include "NetHead.h"

#include "NetProcess.h"

namespace Logic
{
	class tEventCenter;
	class tEvent;
}

enum BROADCAST_MODE
{
	eBroadcastNone = 0,
	eBroadcastAll = 1,
	eBroadcastByType = 2,
};

namespace NetCloud
{

	//-------------------------------------------------------------------------*/
	// Ӧ�ö���, ����EntityNet ����ֻͨ��ID�Ϳ��Է��͵��Է���Ϣ
	// mID �����봴����Gate����, ��ϣ���Եõ�Gate
	// �����˶����͵�����IDʱ, ������ؽڵ��ﲻ������ֱ�ӷ�����Ӧ��Gate, Ȼ������Gate�������������
	//-------------------------------------------------------------------------
	class Net_Export tNetUnit : public AnyData
	{
		friend class tGate;

	public:
		virtual bool OnReceiveProcess(NodePacket *pNodePacket) = 0;

		void SetID(int type, UInt64 id);
		virtual void Ready() {}

		virtual Logic::tEventCenter* GetEventCenter()
		{
			//if (mNode)
			//	return mEventCenter.getPtr();
			return NULL;
		}

		AutoEvent StartEvent(const char *szEventName);


	public:
		// ���͵�Ψһ�ӿ� NOTE: ���͵���Ϣ��, ������ DataPacket
		virtual bool Send(UnitID targetID, Packet *p, BROADCAST_MODE bBroadcast = eBroadcastNone);

		// ���ڻ�������ظ��¼��ɶ��巢�������¼��Ĺ������� SaveEventName SaveEventNameIndex , ���͵��¼�����׺��_R
		virtual bool SendEvent(UnitID targetID, Logic::tEvent *pEvent, BROADCAST_MODE bBroadcast = eBroadcastNone);

		// ���� �̳��� tMsgEvent �� tRequestEvent ����Ϣ�¼�
		virtual bool StartSend(UnitID targetID, Logic::tEvent *pEvent);

		bool SendPacket(UnitID targetID, Packet *pNodePacket, BROADCAST_MODE bBroadcast = eBroadcastNone);

		virtual void StartTransferNode();

		virtual HandPacket CreatePacket(PacketID_t packetID)
		{
			//if (mNode)
			//	return mNode->CreatePacket(packetID);
			ERROR_LOG("Childen class need override CreatePacket"); return HandPacket(); 
		}

		Auto<TransferPacket> CreateNodePacket();

	public:
		UnitID GetID() { return mID; }
		void SetID(UnitID id) { mID = id; }

		virtual void SetBeginJump() { mbJumping = true; }
		virtual bool IsJumping() { return mbJumping; }

	protected:
		virtual bool SendTo(NodePacket *p);

	public:
		tNetUnit()
		{

		}

	public:
		AProcess		mNode;
		//ANodeConnect				mNodeConnect;
		
		UnitID	mID;
		bool mbJumping;
	};
	typedef Hand<tNetUnit> AUnit;

}

#endif //_INCLDUE_NETUNIT_H_