#pragma once
#ifndef _INCLUDE_NETNODE_H_
#define _INCLUDE_NETNODE_H_

#include "Hand.h"
#include "EasyHash.h"
#include "NetAddress.h"

#include "NetCommon.h"
#include "Packet.h"
#include "WaitRequestManager.h"
#include "CallBack.h"
/*/-------------------------------------------------------------------------
	分布式网络原型
	模型: Gate集群 与 业务Node集群 协调完成中转消息的直发功能
	1 业务Node 连接其中一个Gate节点后, 即收到所有Gate集群的其他所有Gate信息, 然后全部连接
	2 业务网络单元创建时, 随机一个Gate节点, 由Gate分配 唯一的 ID, 并索引在Gate上
	3 发向目标ID业务单元时, 查找本地索引节点, 无, 则发向对应的Gate, 且之后立即向Gate请求目标所在的节点信息
	4 Gate回复节点信息后, 连接并索引
	5 单元跳转时, 将发向此单元的消息缓存在所属Gate, 完成跳转后, 全部发向单元
	6 业务节点如果接到一个不存在单元的消息, 则发送目标所属的Gate, 并通知源节点目标地址已经无效, 源节点清理掉无效地址索引

	
//------------------------------------------------------------------------*/
namespace Logic
{
	class tEventCenter;
}
class Packet;
//-------------------------------------------------------------------------	
namespace NetCloud
{
	class tNetUnit;

	class Net_Export tNetNode : public Base<tNetNode>
	{
	public:
		virtual bool AwaitConnectGate(const Address &gateAddress) = 0;
		virtual void Init(const Address &gateAddress, CallBack<HandConnect> connectGateFinish = CallBack<HandConnect>()) = 0;
		virtual void AppendUnit( tNetUnit*) = 0;
		virtual ANodeConnect FindNodeConnectByID(UnitID entityID) = 0;
		virtual AGateConnect FindGateByID(UnitID entityID) = 0;
		virtual HandConnect GetLocalConnect() = 0;
		virtual HandConnect FindGateConnect(int gateCode) = 0;
		virtual ANodeConnect FindNodeConnect(const Address &target) = 0;
		virtual tNetUnit* FindUnit(UnitID id) = 0;		
		virtual bool AwaitCheckExistUnit(UnitID id, int waitMilSecond) = 0;
		
		virtual void RequestNodeInfo(AGateConnect targetGate, UnitID id) = 0;
		virtual HandPacket CreatePacket(PacketID_t packetID) = 0;
		virtual Logic::tEventCenter* GetEventCenter() = 0;

		virtual void Process();
		virtual void LowProcess(int onceTime) = 0;
		virtual AString Dump() = 0;

		// 通知, 当前节点对应的Entity无效(已经不在当前节点)
		virtual void NotifyInvalidNode(ANodeConnect scrNode, UnitID) = 0;

		virtual void RegisterMsgPacket(Auto<PacketFactory>) = 0;

		virtual void BroadcastMsg(NodePacket *pData) = 0;

		virtual void OnReceiveBroadcastMsg(NodePacket *pData) = 0;

		virtual void SetSafeCode(int safeCode) = 0;

		virtual HandPacket CreateNodePacket() { return CreatePacket(PACKET_TRANSFER); }

		virtual bool SendToGate(UnitID targetID, Packet *p);
		virtual bool SendToTarget(NodePacket *pData);

		virtual void OnReceive(ANodeConnect scrNodeConnect, NodePacket *pData);
		virtual void OnReceiveGate(NodePacket *pData);				

		AWaitRequestManager	 GetWaitReqeustMgr() { return	mWaitRequestManager; }

		virtual void RunLocalPacket(NodePacket *pData);

		virtual bool SendEventToGate(int gateCode, Logic::tEvent *pEvent) { return false; }


	public:
		tNetNode();
		virtual ~tNetNode();
		friend class tNetUnit;
		friend class DBUnit;

		struct TableDBKeyHashInfo
		{
			int mTableUnitType;
			bool mbStringKey;
			ArrayMap<int>	mKeyHashByDBCodeMap;
			RandTool<int>		mRandDBList;
		};

		AutoNice				mDBResultData;
	protected:
		LoopDataStream	mTempLoopData;
		EasyHash<AString, TableDBKeyHashInfo>	mTableTypeInfoHash;
		AWaitRequestManager			mWaitRequestManager;
		// 在协程内发送到自己的消息, 进行处理, 回到主线处理, 用于成功触发 Resume(不能在协程内触发)
		Array<HandPacket>	mWaitRunPacket;												
	};

	typedef Hand<tNetNode> ANetNode;
	
}

#endif //_INCLUDE_NETNODE_H_