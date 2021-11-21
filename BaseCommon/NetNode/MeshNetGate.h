#pragma once

#ifndef _INCLUDE_MESHNETGATE_H_
#define _INCLUDE_MESHNETGATE_H_

#include "Gate.h"
#include "MeshedNetNodeData.h"
#include "ClientEvent.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
	// Gate�ڵ�
		�ֲ��̶�������ʽ, ����ֹͣ��Gate, ��ʱ��������
		��ϣֵ = UnitID / Gate����
		KEY��Node��KEY ���� Mesh node key, �� Gate �� Node ����������ַ
		NOTE: Gate Server Net �� Node ����У����
	//-------------------------------------------------------------------------*/
	class Net_Export MeshGate : public tGate
	{
		friend class _NodifyNodeAddressResponse;
	public:
		// ����: const char *szCloudGateIp, int nCloudGatePort ��ǰGate�ڵ�ĵ�ַ, ��Ԫ�ڵ����ӵ�GateServer��ַ
		MeshGate(int fixedGateCount, int gateID, const char *szCloudGateIp, int nCloudGatePort, int nGateMeshNetSafeCheck, int threadNum = 2)
			: mGateKey(szCloudGateIp, nCloudGatePort)
			, mGateID(gateID)
			, mFixedGateCount(fixedGateCount)
		{
			mGateNet = MEM_NEW GateMeshNet(this, szCloudGateIp, nCloudGatePort, nGateMeshNetSafeCheck, threadNum);
			ViaductNodeServerNet *p = MEM_NEW ViaductNodeServerNet();		
			p->SetSafeCode(nGateMeshNetSafeCheck);
			mNodeServerNet = p;
			p->mpMeshGate = this;
		}
		~MeshGate()
		{
			Close();
			mNodeServerNet._free();
			mGateNet._free();
		}

		virtual void Close() override
		{
			mNodeServerNet->StopNet();
			mGateNet->StopNet();
		}
		// ����: const Address &nodeServerAddress  ��Ԫ�ڵ����ӵ�GateServer��ַ
		virtual void Init(const Address &nodeServerAddress, const Address &mainGateAddress)
		{
			mGateKey = nodeServerAddress;
			mNodeServerNet->StartNet(nodeServerAddress.Ip().c_str(), nodeServerAddress.GetPort());

			mGateNet->SetMainNode(mainGateAddress.Ip().c_str(), mainGateAddress.GetPort());
			mGateNet->StartNet(mainGateAddress.Ip().c_str(), mainGateAddress.GetPort());
		}

		virtual Logic::tEventCenter* GetEventCenter() override
		{
			return mNodeServerNet->GetEventCenter();
		}

		virtual void RegisterMsgPacket(Auto<PacketFactory> packetFactory) override;

		virtual AString Dump() override;
		
		virtual void Process() override
		{
			mGateNet->Process();
			mNodeServerNet->Process();
		}
		virtual void LowProcess(int milSec) override
		{
			mGateNet->LowProcess(milSec);
			mNodeServerNet->LowProcess(milSec);
		}

		virtual void OnReqeustJumpNode(UnitID id) override
		{			
			ANodeConnect node = FindNodeConnect(id);
			tGate::OnReqeustJumpNode(id);
			mUnitConnectHash.erase(id.value);

			// ֪ͨUnit����Node��Ч, �Ƴ�����
			if (node)
			{
				AutoEvent evt = mNodeServerNet->GetEventCenter()->StartDefaultEvent("NotifyInvalidNodeForUnitEvent");
				evt["UNIT_ID"] = id;
				evt["NODE_ADDRESS"] = (UInt64)node->GetAddress();
				BroadCastEvent(evt.getPtr());
			}
		}

	public:
		virtual AGateConnect FindGateConnect(const Address &targetAddress) { return AGateConnect(); }
		//virtual UInt64 CreateNewNetID() { return 0; }

		virtual AGateConnect FindGateConnect(UnitID target) override
		{
			int gateKey = (UInt64)target % GetGateCount();
			Hand< GateMeshNet> net = mGateNet;
			return net->mGateConnectHash.find(gateKey);
		}

		virtual ANodeConnect FindNodeConnect(UnitID target) override;
		
		// �㲥����Gate�ڵ�����Node, NOTE: ��Node�н����Ѿ��㲥������ Gate, �Ұ����Լ��ڵ��ڵ�������Ԫ
		virtual void BroadCastMsg(NodePacket *pData) override
		{
			ANodeConnect sendConn = FindNodeConnect(pData->mSenderID);

			for (auto it= mNodeHash.begin(); it; ++it)
			{
				ANodeConnect conn = it.get();
				if (conn && conn->IsOK() && conn!=sendConn)
					conn->SendToNode(pData->mTargetID, pData);
			}			
		}

		virtual void BroadCastEvent(Logic::tEvent *pEvent, bool bToGate = true);

	public:
		void OnNodeConnected(Address nodeKey, HandConnect nodeConnect, Logic::tClientEvent *pReqeust);

		int GetGateCount() const { return mFixedGateCount; }

		int GetNowGateConnect() { return mGateNet->GetNetNodeList().size(); }

		int GetCloudNodeCount() const { return mNodeHash.size(); }

		int TotalUnitCount() const { return mUnitConnectHash.size(); }

	public:
		Hand<MeshedNodeNet>		mGateNet;			// ����Gate MeshNet
		AutoNet								mNodeServerNet; // ����Node���ӵ�����
		EasyHash<Address, ANodeConnect>			mNodeHash;
		EasyHash<UInt64, ANodeConnect>				mUnitConnectHash;
		Address								mGateKey;			// ����Node�ķ����ַ
		int											mGateID;				// Gate���, ������ϣ
		int											mFixedGateCount;

	public:
		//-------------------------------------------------------------------------*/
		// Node ����
		class ViaductNodeConnect : public tNodeConnect
		{
		public:
			virtual bool SendToNode(UnitID targetAddress, Packet *pPacket) override;
			
			virtual AutoEvent StartEvent(const char *eventMsgName) override
			{
				if (mNetConnect)
					return mNetConnect->StartEvent(eventMsgName);
				return AutoEvent(); 
			}
			virtual Address GetAddress() const override { return mAddress; }

			virtual bool IsOK() override { return mNetConnect; }

		public:
			HandConnect				mNetConnect;
			Address						mAddress;
		};

		class ToGateConnect : public IOCPServerConnect
		{
		public:
			virtual void OnConnectSafeCodeError(const char *szIp, int port, int safeCode) override
			{
				ERROR_LOG(" [%s : %d] connect gate server safe code error [%d], now net code [%d]", szIp, port, safeCode, GetNetHandle()->GetSafeCode());
				TableTool::red();
				PRINT("ERROR: [%s : %d] connect gate server safe code error [%d], now net code [%d]\r\n", szIp, port, safeCode, GetNetHandle()->GetSafeCode());
				TableTool::white();
			}
		};

		//-------------------------------------------------------------------------*/
		// ���ܽڵ����ӵķ���
		// �ڵ����ӳɹ���, �����ͽڵ��ַKey, Ȼ�����ӵ��ڵ�����
		class ViaductNodeServerNet : public IOCPServerNet
		{
			friend class MeshGate;
		public:			
			ViaductNodeServerNet();
			~ViaductNodeServerNet()
			{
				mEventCenter._free();
			}

			virtual HandConnect CreateConnect() override { return MEM_NEW ToGateConnect(); }

			virtual Logic::tEventCenter* GetEventCenter(void) const { return ((ViaductNodeServerNet*)this)->mEventCenter.getPtr(); }

			virtual void SetSafeCode(int safeCode) { mSafeCode = safeCode; }
			virtual int GetSafeCode() override { return mSafeCode; }

			virtual void Process() override
			{
				IOCPServerNet::Process();
				mEventCenter->ProcessEvent();
			}

			virtual bool OnAddConnect(tNetConnect *pConnect) override;

			virtual void OnCloseConnect(tNetConnect *pConnect) override
			{
				IOCPServerNet::OnCloseConnect(pConnect);
				HandConnect conn = pConnect;
				conn._free();
			}

			virtual void OnReceivePacket(Packet *pPacket) override
			{
				//??? ����Ҫ����GateMesh�ڵ�֮�����Ϣ��
				NodePacket *pNodePak = dynamic_cast<NodePacket*>(pPacket);
				if (pNodePak != NULL)
					mpMeshGate->OnReceive(pNodePak);
				else
					ERROR_LOG("xxxxxxxxxx %d can not receive", pPacket->GetPacketID());
			}

		public:
			MeshGate		*mpMeshGate;
			AutoEventCenter	mEventCenter;

			int				mSafeCode;
		};

		//-------------------------------------------------------------------------*/
		class MeshGateConnect : public tGateConnect
		{
			friend class GateMeshNet;
		public:
			virtual void SendToGate(UnitID targetAddress, Packet *pPacket) { }
			
			virtual AutoEvent StartEvent(const char *eventMsgName) 
			{ 
				if (mNodeConnect && mNodeConnect->mNodeConnect)
					return mNodeConnect->mNodeConnect->StartEvent(eventMsgName);
				return AutoEvent();
			}

			virtual Address GetAddress() const { return mGateKey; }

		public:
			Hand<NetNodeConnectData>	mNodeConnect;
			Address mGateKey;
		};
		//-------------------------------------------------------------------------*/
		class GateMeshNet : public MeshedNodeNet
		{
		public:
			GateMeshNet(MeshGate *pGate, const char *szServerIp, int port, int nSafeCheck, int threadNum = 2);
			~GateMeshNet()
			{
				AutoEventCenter center = mServerNet->GetEventCenter();
				center._free();
				mEventCenter._free();
			}

			virtual const char* NodeInfo() override { return "GATE"; }

			void RegisterMsgPacket(Auto<PacketFactory> packetFactory)
			{
				GetNetProtocol()->RegisterNetPacket(packetFactory);
				mServerNet->GetNetProtocol()->RegisterNetPacket(packetFactory);
			}

			virtual void RegisterNodeEvent(AutoEventCenter serverCenter, AutoEventCenter connectCenter) override;

		public:
			//virtual void OnRequestNode(NetNodeConnectData *nodeData, AutoEvent &requestEvt) {  }

			// ��Ϊ�������ߣ����յ����������Ӻ�������¼�, ��Ҫ���������ߵ�ǰ�����ڵ�Ⱥ����Ϣ
			virtual void OnResponseNode(GSKEY otherPartyGSKey, AutoEvent &requstEvt, AutoEvent &respEvt) {
				MeshedNodeNet::OnResponseNode(otherPartyGSKey, requstEvt, respEvt);
				respEvt["INFO"] = NodeInfo(); 
				respEvt["GATE_ID"] = mpMeshGate->mGateID;
				respEvt["GATE_KEY"] = (UInt64)mpMeshGate->mGateKey;
			}

			// �Է��ظ�����
			virtual void OnReceiveResponse(NetNodeConnectData *nodeData, AutoEvent &requstEvt, AutoEvent &respEvt);

			virtual void StopNet() override
			{
				mGateConnectHash.clear();
				MeshedNodeNet::StopNet();
			}

			virtual void OnReceivePacket(Packet *p) override 
			{ 
				NodePacket *pNodePak = dynamic_cast<NodePacket*>(p);
				if ( pNodePak== NULL)
					ERROR_LOG("Packet %d is not NodePacket", p->GetPacketID())
				else
					mpMeshGate->OnReceive(pNodePak); 
			}

		public:
			EasyHash <int, Hand<tGateConnect>>		mGateConnectHash;
			MeshGate			*mpMeshGate;
		};
		//-------------------------------------------------------------------------*/
	};

}

#endif //_INCLUDE_MESHNETGATE_H_