#pragma once

#ifndef _INCLUDE_MESHNETNODE_H_
#define _INCLUDE_MESHNETNODE_H_

#include "NetNode.h"
#include "MeshedNodeNet.h"
#include "IOCPClientSetNet.h"
#include "MeshedNetNodeData.h"
#include "NetUnit.h"
#include "EventCenter.h"


namespace NetCloud
{
	//-------------------------------------------------------------------------
	// Ӧ�ö˽ڵ�, ͨ��MeshNet����
	class Net_Export MeshNetNode : public tNetNode
	{
	public:
		MeshNetNode(const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2);
		~MeshNetNode();

		virtual void SetSafeCode(int safeCode) override;

		virtual bool AwaitConnectGate(const Address &gateAddress) override;

	public:
		virtual void Init(const Address &gateAddress, CallBack<HandConnect> connectGateFinishCall = CallBack<HandConnect>()) override
		{
			mConnectGateCall = connectGateFinishCall;

			mNodeMeshNet->SetMainNode("127.0.0.1", 0); 
			mNodeMeshNet->StartNet("127.0.0.1", 0);

			mConnectGateNet->StartNet(gateAddress.Ip().c_str(), gateAddress.GetPort());
		}

		virtual void RegisterMsgPacket(Auto<PacketFactory> packetFactory) override;

		virtual Logic::tEventCenter* GetEventCenter() override
		{
			return mEventCenter.getPtr();
		}

		virtual void AppendUnit(tNetUnit *pUnit) override;

		// ֪ͨ����Ԫ��Ӧ��Gate
		void NotifyUnitInfoToGate(tNetUnit *pUnit, AGateConnect gate);

		virtual ANodeConnect FindNodeConnectByID(UnitID entityID) override
		{
			return mUnitConnectHash.find(entityID);
		}

		virtual AGateConnect FindGateByID(UnitID entityID) override
		{
			int key = (int)((UInt64)entityID % mGateCount);
			AGateConnect gate = mGateHash.find(key);
			if (!gate)
			{
				ERROR_LOG("No exist gate %d", key);
			}
			return gate;
		}

		virtual HandConnect FindGateConnect(int gateCode) override;

		virtual ANodeConnect FindNodeConnect(const Address &target);
		
		virtual tNetUnit* FindUnit(UnitID id) override
		{
			return mUnitListHash.find(id).getPtr();
		}

		virtual bool AwaitCheckExistUnit(UnitID id, int waitMilSecond) override;
		
		virtual AString Dump() override;

		virtual void RequestNodeInfo(AGateConnect targetGate, UnitID id) override
		{
			AutoEvent existReqeust = mRequestUnitNodeList.find(id);
			if (existReqeust && !existReqeust->getFinished())
				return;
			AutoEvent evt = targetGate->StartEvent("RequestNodeInfobyUnitID");
			evt["UNIT_ID"] = (UInt64)id;
			evt->DoEvent(false);
		}

		void OnReceiveNodeInfoForUnit(UnitID id, Address nodeAddress, Logic::tEvent *pEvt)
		{
			AutoEvent evt = mRequestUnitNodeList.find(id);
			if (evt.getPtr() == pEvt)
				mRequestUnitNodeList.erase(id);

			ANodeConnect node = FindNodeConnect(nodeAddress);
			if (node && node->IsOK())
			{
				mUnitConnectHash.insert(id, node);
			}
			else
			{
				bool bRe =	mNodeMeshNet->Connect(nodeAddress.Ip().c_str(), nodeAddress.GetPort(), 10000);
				DEBUG_LOG("Now connect node %s, result %s", nodeAddress.dump().c_str(), bRe ? "OK" : "FAIL");
			}
		}

		virtual HandPacket CreatePacket(PacketID_t packetID)
		{
			return mConnectGateNet->GetNetProtocol()->CreatePacket(packetID);
		}

		virtual void NotifyInvalidNode(ANodeConnect scrNode, UnitID id) override
		{
			if (scrNode)
			{
				AutoEvent evt = scrNode->StartEvent("NotifyInvalidNodeForUnit");
				evt["UNIT_ID"] = (UInt64)id;
				evt["NODE_ADDRESS"] = (UInt64)mNodeKey;
				evt->Send();
			}
		}

		virtual void Process() override
		{
			mConnectGateNet->Process();
			mNodeMeshNet->Process();
			if (mEventCenter)
				mEventCenter->ProcessEvent();
			tNetNode::Process();
		}
		virtual void LowProcess(int onceTime) override
		{
			mConnectGateNet->LowProcess(onceTime);
			mNodeMeshNet->LowProcess(onceTime);
		}

		void AppendGate(int gateID, HandConnect gateConnect, Address gateNodeAddress);

		// �㲥��Ϣ
		virtual void BroadcastMsg(NodePacket *pData) override;

		// ����㲥��Ϣ
		virtual void OnReceiveBroadcastMsg(NodePacket *pData) override;

		virtual HandConnect GetLocalConnect() override 
		{ 
			if (mConnectGateNet && mConnectGateNet->GetConnectList().size()>0)
				return mConnectGateNet->GetConnectList()[0];
			return HandConnect();
		}

		// gateCode ��ֵ, ��ʾ������Gate����
		virtual bool SendEventToGate(int gateCode, Logic::tEvent *pEvent) override;

		virtual void OnConnected(const char *szIP, int port){ DEBUG_LOG("Connected [%s:%d]", szIP, port); }
		virtual void OnConnectFinish(const char *szIP, int port) { DEBUG_LOG("Connect finish [%s:%d]", szIP, port); }
		virtual void OnDisconnect(const char *szIP, int port) { DEBUG_LOG("Disconnect [%s:%d]", szIP, port); }
		virtual void OnConnectFail(const char *szIP, int port) { DEBUG_LOG("Connect fail [%s:%d]", szIP, port); }

		virtual void OnJumpGateReadyFinish(UInt64 unitID) { LOG("%s gate ready jump finish", UNITID_UINT64(unitID)); }

	public:
		Hand<MeshedNodeNet>					mNodeMeshNet;		// Node ֮�������
		Hand<IOCPClientSetNet>					mConnectGateNet;		// IOCPClientSetNet ��������Gate
		Address												mNodeKey;
		EasyHash<int, AGateConnect>			mGateHash;
		EasyHash<UnitID, ANodeConnect>	mUnitConnectHash;
		int														mGateCount;
		EasyHash<UInt64, AutoEvent>			mRequestUnitNodeList;
		EasyHash<UnitID, AUnit>					mUnitListHash;
		AutoEventCenter									mEventCenter;
		CallBack<HandConnect>					mConnectGateCall;
		Hand< IOCPConnect>						mGlobalToGateConnect;		// ֻ��������������IP�˿���Ϣ
		bool														mbNeedTryReconnect;		// ��Ҫ��������(����ʧ�ܻ���ߺ�)

	public:
		//-------------------------------------------------------------------------
		class GateConnectPtr : public AutoBase
		{
		public:
			tGateConnect *mpGateConnect = NULL;
		};

		typedef Auto<GateConnectPtr> AGateConnectPtr;
		//-------------------------------------------------------------------------
		// ��������Gate�Ĺ���, һֱ����, ֱ�����ӳɹ�Ϊ��, ֮��Ͽ�, �ٴ�����
		class ClientGateConnect : public IOCPClientConnect
		{
		public:
			AGateConnectPtr		mGateConnectPtr;
			Address					mGateAddress;

		public:
			ClientGateConnect()
			{}

			virtual void OnDisconnect();

			void OnTime();
		};
		//-------------------------------------------------------------------------
		// ����ʧ��ʱ, �����ȴ���������
		class WaitReconnectTimer : public tTimer
		{
		public:
			virtual void onTime() override
			{
				if (mNet)
				{
					NOTE_LOG("Now reconnect [%s : %d] ...", mIP.c_str(), mPort);
					mNet->Connect(mIP.c_str(), mPort, 6000);
				}
				else
					NOTE_LOG("*** Net is NULL at reconnect [%s : %d] ...", mIP.c_str(), mPort);
			}

			Hand<IOCPClientSetNet> mNet;
			AString mIP;
			int mPort = 0;
		};
		//-------------------------------------------------------------------------*/
		// ����Gate
		class GateClientSetNet : public IOCPClientSetNet
		{
		public:
			HandConnect CreateConnect() override
			{
				return MEM_NEW ClientGateConnect();
			}

			virtual void OnConnected(tNetConnect *pConnect) override
			{
				if (mpNetNode->mNodeKey.GetPort() <= 0)
				{
					ERROR_LOG("%s Local node port is 0, can not connect to other ndoe", mpNetNode->mNodeKey.dump().c_str());
					return;
				}

				AutoEvent evt = pConnect->StartEvent("NotifyNodeKeyRequestGateInfo");
				evt["NEED_ALL_GATE"] = 1;
				evt["NODE_KEY"] = (UInt64)mpNetNode->mNodeKey;

				evt->WaitTime(10);
				evt->DoEvent(false);
				mpNetNode->OnConnected(pConnect->GetIp(), pConnect->GetPort());
			}

			virtual void OnConnectFail(const char *szIp, int nPort)
			{
				mpNetNode->OnConnectFail(szIp, nPort);
				mpNetNode->mGlobalToGateConnect->Init(this, 0, szIp, nPort);
				mpNetNode->mConnectGateCall(mpNetNode->mGlobalToGateConnect, false);
				if (!mpNetNode->mbNeedTryReconnect)
					return;
				NOTE_LOG("Fail connect [%s : %d], try reconnect after 3 Sec ...", szIp, nPort);
				Auto<WaitReconnectTimer> t = MEM_NEW WaitReconnectTimer();
				t->mNet = GetSelf();
				t->mIP = szIp;
				t->mPort = nPort;
				t->Wait(3000);
			}

			virtual void SetNetProtocol(Auto<tNetProtocol> netProtocol) override
			{
				IOCPClientSetNet::SetNetProtocol(netProtocol);
			}

			virtual Logic::tEventCenter* GetEventCenter() const override
			{
				return ((GateClientSetNet*)this)->mEventCenter.getPtr();
			}

			virtual void SetSafeCode(int safeCode) { mSafeCode = safeCode; }
			virtual int GetSafeCode() override { return mSafeCode; }

			virtual void Process() override
			{
				IOCPClientSetNet::Process();
				if (mEventCenter)
					mEventCenter->ProcessEvent();
			}

			virtual void OnReceivePacket(Packet *p) override 
			{ 
				NodePacket *pNode = dynamic_cast<NodePacket*>(p);
				if (pNode != NULL)
					mpNetNode->OnReceive(ANodeConnect(), pNode);
				else
					ERROR_LOG("Packet %s is not NodePacket", p->GetMsgName());
			}

		public:
			GateClientSetNet(MeshNetNode *pNode);
			~GateClientSetNet()
			{
				mEventCenter._free();
			}

		public:
			AutoEventCenter		mEventCenter;
			MeshNetNode			*mpNetNode;
			int mSafeCode;
		};
	};
	
	class TestConnect : public NodeRequestConnect
	{
	public:
		virtual bool Process() override
		{
			return NodeRequestConnect::Process();
		}
	};
	//-------------------------------------------------------------------------
	// ����Ӧ�ö˵�MeshNet �ص��ǶԷ��Ӻ�, ���������������ӶԷ�, �ȷ�����Ҫʱ���������ӻ����ӲŽ�������
	// ����ɢ�ṹ, NODE�����ֻ����һ������
	//-------------------------------------------------------------------------
	class SimplifyMeshNet : public MeshedNodeNet
	{
	public:
		SimplifyMeshNet(MeshNetNode *pNode, const char *szServerIp, int port, int nSafeCheck, int threadNum = 2);
		~SimplifyMeshNet()
		{
			AutoEventCenter center = mServerNet->GetEventCenter();
			center._free();
			mEventCenter._free();
		}

		virtual void SetEventCenter(AutoEventCenter serverCenter, AutoEventCenter connectCenter) override;

		virtual const char* NodeInfo() override { return "NODE SimplifyMeshNet"; }

		//virtual bool StartNet(const char *szIP, int port)  override
		//{
		//	return IOCPBaseNet::StartNet(szIP, port);
		//}

		void RegisterMsgPacket(Auto<PacketFactory> packetFactory)
		{
			GetNetProtocol()->RegisterNetPacket(packetFactory);
			mServerNet->GetNetProtocol()->RegisterNetPacket(packetFactory);
		}

		virtual HandConnect CreateConnect() override
		{
			return MEM_NEW TestConnect();
		}

		// ���������ӶԷ�Node���ɹ���, ֪ͨ�Է��¼�����֮ǰ����Я����������
		//virtual void OnRequestNode(NetNodeConnectData *nodeData, AutoEvent &requestEvt) { requestEvt["NODE_KEY"] = mpMeshNetNode->mNodeKey; }

		//// ��Ϊ�������ߣ����յ����������Ӻ�������¼�, ��Ҫ���������ߵ�ǰ�����ڵ�Ⱥ����Ϣ
		//virtual void OnResponseNode(GSKEY otherPartyGSKey, AutoEvent &requstEvt, AutoEvent &respEvt);

		// �Է��ظ�����
		virtual void OnReceiveResponse(NetNodeConnectData *nodeData, AutoEvent &requstEvt, AutoEvent &respEvt);

		virtual void OnConnected(tNetConnect *pConnect) override;

		virtual bool OnAddConnect(tNetConnect *pConnect) override;

		virtual void OnReceivePacket(Packet *p) override 
		{ 
			NodePacket *pNodePak = dynamic_cast<NodePacket*>(p);
			if (pNodePak == NULL)
				ERROR_LOG("Packet %s is not NodePacket", p->GetMsgName())
			else
			mpMeshNetNode->OnReceive(ANodeConnect(), pNodePak);
		}

		class MeshConnect : public tNodeConnect
		{
			friend class SimplifyMeshNet;

		public:
			virtual bool SendToNode(UnitID targetAddress, Packet *pPacket) override
			{
				if (mOtherToLocal)
				{
					if (mOtherToLocal->Send(pPacket->GetFactory()->GetPacketID(), pPacket))
						return true;
					else
						mOtherToLocal.setNull();
				}

				if (mLocalToOther && mLocalToOther->mNodeConnect)
				{
					if (mLocalToOther->mNodeConnect->Send(pPacket->GetFactory()->GetPacketID(), pPacket))
						return true;
					mLocalToOther->mNodeConnect.setNull();
					mLocalToOther.setNull();
				}

				//else
				//{
				//	ERROR_LOG("Node mesh connect invable %s", mAddress.dump().c_str());
				//	return false;
				//}
				DEBUG_LOG("> Fail send to node %s, packet %s", mAddress.dump().c_str(), pPacket->GetMsgName());
				return false;
			}

			virtual AutoEvent StartEvent(const char *eventMsgName)
			{				
				if (mOtherToLocal)
					return mOtherToLocal->StartEvent(eventMsgName);
				else
					if (mLocalToOther && mLocalToOther->mNodeConnect)
					return mLocalToOther->mNodeConnect->StartEvent(eventMsgName);
				
				ERROR_LOG("Connect is NULL, create event fail  %s", eventMsgName);
				return AutoEvent();
			}
			virtual Address GetAddress() const
			{
				return mAddress;
			}

			virtual bool IsOK() override { return mOtherToLocal || (mLocalToOther&&mLocalToOther->mNodeConnect); }
			
		public:
			Address mAddress;

		protected:
			HandConnect mOtherToLocal;
			Hand<NetNodeConnectData> mLocalToOther;
		};
	public:
		virtual void OnOtherNodeRequestConnect(HandConnect connect, GSKEY otherServerNetKey) override;
		virtual void StopNet() override
		{
			mNodeConnectHash.clear();
			MeshedNodeNet::StopNet();
		}

	public:
		EasyHash<UInt64, ANodeConnect>	mNodeConnectHash;
		MeshNetNode		*mpMeshNetNode;
	};
	//-------------------------------------------------------------------------
}
#endif //_INCLUDE_MESHNETNODE_H_

