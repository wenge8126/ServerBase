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
	// 应用对象, 其他EntityNet 可以只通过ID就可以发送到对方消息
	// mID 由申请创建的Gate创建, 哈希可以得到Gate
	// 其他此对象发送到其他ID时, 如果本地节点里不存在则直接发到对应的Gate, 然后连接Gate并索引具体对象
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
		// 发送的唯一接口 NOTE: 发送的消息包, 必须是 DataPacket
		virtual bool Send(UnitID targetID, Packet *p, BROADCAST_MODE bBroadcast = eBroadcastNone);

		// 对于互等请求回复事件可定义发送请求事件的工厂覆盖 SaveEventName SaveEventNameIndex , 发送的事件名后缀加_R
		virtual bool SendEvent(UnitID targetID, Logic::tEvent *pEvent, BROADCAST_MODE bBroadcast = eBroadcastNone);

		// 发送 继承于 tMsgEvent 或 tRequestEvent 的消息事件
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