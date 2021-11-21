#pragma once

#ifndef _INCLUDE_MESHNETGATE_H_
#define _INCLUDE_MESHNETGATE_H_

#include "Gate.h"
#include "MeshedNetNodeData.h"
#include "ClientEvent.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
	// Gate节点
		分布固定数量方式, 如有停止的Gate, 及时启动补充
		哈希值 = UnitID / Gate数量
		KEY和Node的KEY 都是 Mesh node key, 即 Gate 或 Node 创建参数地址
		NOTE: Gate Server Net 与 Node 网络校验码
	//-------------------------------------------------------------------------*/
	class Net_Export MeshGate : public tGate
	{
		friend class _NodifyNodeAddressResponse;
	public:
		// 参数: const char *szCloudGateIp, int nCloudGatePort 当前Gate节点的地址, 单元节点连接的GateServer地址
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
		// 参数: const Address &nodeServerAddress  单元节点连接的GateServer地址
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

			// 通知Unit所有Node无效, 移除索引
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
		
		// 广播到本Gate内的所有Node, NOTE: 在Node中进行已经广播到其他 Gate, 且包括自己节点内的其他单元
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
		Hand<MeshedNodeNet>		mGateNet;			// 所有Gate MeshNet
		AutoNet								mNodeServerNet; // 接收Node连接的网络
		EasyHash<Address, ANodeConnect>			mNodeHash;
		EasyHash<UInt64, ANodeConnect>				mUnitConnectHash;
		Address								mGateKey;			// 接收Node的服务地址
		int											mGateID;				// Gate序号, 用来哈希
		int											mFixedGateCount;

	public:
		//-------------------------------------------------------------------------*/
		// Node 连接
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
		// 接受节点连接的服务
		// 节点连接成功后, 即发送节点地址Key, 然后增加到节点索引
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
				//??? 还需要处理GateMesh节点之间的消息包
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

			// 作为被连接者，接收到连接者连接后的请求事件, 主要告诉连接者当前整个节点群的信息
			virtual void OnResponseNode(GSKEY otherPartyGSKey, AutoEvent &requstEvt, AutoEvent &respEvt) {
				MeshedNodeNet::OnResponseNode(otherPartyGSKey, requstEvt, respEvt);
				respEvt["INFO"] = NodeInfo(); 
				respEvt["GATE_ID"] = mpMeshGate->mGateID;
				respEvt["GATE_KEY"] = (UInt64)mpMeshGate->mGateKey;
			}

			// 对方回复数据
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