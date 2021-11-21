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
	�ֲ�ʽ����ԭ��
	ģ��: Gate��Ⱥ �� ҵ��Node��Ⱥ Э�������ת��Ϣ��ֱ������
	1 ҵ��Node ��������һ��Gate�ڵ��, ���յ�����Gate��Ⱥ����������Gate��Ϣ, Ȼ��ȫ������
	2 ҵ�����絥Ԫ����ʱ, ���һ��Gate�ڵ�, ��Gate���� Ψһ�� ID, ��������Gate��
	3 ����Ŀ��IDҵ��Ԫʱ, ���ұ��������ڵ�, ��, �����Ӧ��Gate, ��֮��������Gate����Ŀ�����ڵĽڵ���Ϣ
	4 Gate�ظ��ڵ���Ϣ��, ���Ӳ�����
	5 ��Ԫ��תʱ, ������˵�Ԫ����Ϣ����������Gate, �����ת��, ȫ������Ԫ
	6 ҵ��ڵ�����ӵ�һ�������ڵ�Ԫ����Ϣ, ����Ŀ��������Gate, ��֪ͨԴ�ڵ�Ŀ���ַ�Ѿ���Ч, Դ�ڵ��������Ч��ַ����

	
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

		// ֪ͨ, ��ǰ�ڵ��Ӧ��Entity��Ч(�Ѿ����ڵ�ǰ�ڵ�)
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
		// ��Э���ڷ��͵��Լ�����Ϣ, ���д���, �ص����ߴ���, ���ڳɹ����� Resume(������Э���ڴ���)
		Array<HandPacket>	mWaitRunPacket;												
	};

	typedef Hand<tNetNode> ANetNode;
	
}

#endif //_INCLUDE_NETNODE_H_