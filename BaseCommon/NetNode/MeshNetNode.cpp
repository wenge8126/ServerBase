#include "MeshNetNode.h"
#include "MeshedNetNodeData.h"
#include "ServerEvent.h"

namespace NetCloud
{
	class NotifyInvalidNodeForUnitEvent : public Logic::tBaseNetEvent
	{
	public:
		virtual bool _DoEvent()
		{
			UInt64 id = get("UNIT_ID");
			UInt64 nodeAddress = get("NODE_ADDRESS");
			NetNodeConnectData *pNodeData = NULL;
			Hand< NodeRequestConnect> conn = mNetConnect;
			if (conn)
				pNodeData = conn->mNetNodeConnectData;
			else
			{
				Hand< NodeServerConnect> serverConn = mNetConnect;
				pNodeData = serverConn->mNetNodeConnectData;
			}
			if (pNodeData)
			{
				Hand<SimplifyMeshNet> net = pNodeData->mMeshedNet;
				if (net)
				{
					ANodeConnect node = net->mpMeshNetNode->mUnitConnectHash.find(id);
					if (node && (UInt64)node->GetAddress() == nodeAddress)
					{
						net->mpMeshNetNode->mUnitConnectHash.erase(id);
						NOTE_LOG("Remove unit %s from node %s", UnitID(id).dump().c_str(), node->GetAddress().dump().c_str());
					}
				}
				else
					ERROR_LOG("No is SimplifyMeshNet");
			}
			else
				ERROR_LOG("%s No found node connect data", GetEventName());
			return true;
		}
	};

	//-------------------------------------------------------------------------
	// 用于接受连接
	class NodePacketForNodeServer : public TransferPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			 Hand<NodeServerConnect> conn = pConnect;
			 Hand< NodeServerNet> net = conn->GetNetHandle();
			Hand<SimplifyMeshNet> meshNet = net->mOwnerMeshNet;
			if (conn->mServerNetKey == 0)
				ERROR_LOG("Node connect server net key is 0");
			ANodeConnect node = meshNet->mpMeshNetNode->FindNodeConnect(conn->mServerNetKey);
			meshNet->mpMeshNetNode->OnReceive(node, this);
			return 0;
		}
	};
	// 用于连接端
	class NodePacketForConnectNode : public TransferPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			Hand<NodeRequestConnect> conn = pConnect;
			Hand<SimplifyMeshNet> meshNet = conn->GetNetHandle();
			ANodeConnect node = meshNet->mpMeshNetNode->FindNodeConnect(conn->GetTargetNetKey());
			meshNet->mpMeshNetNode->OnReceive(node, this);
			return 0;
		}
	};
	//-------------------------------------------------------------------------
	SimplifyMeshNet::SimplifyMeshNet(MeshNetNode *pNode, const char *szServerIp, int port, int nSafeCheck, int threadNum /*= 2*/) : MeshedNodeNet(szServerIp, port, nSafeCheck, threadNum)
		, mpMeshNetNode(pNode)
	{
		SetEventCenter(MEM_NEW Logic::EventCenter(), MEM_NEW Logic::EventCenter());

		mServerNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<NodePacketForNodeServer, PACKET_TRANSFER>);
		GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<NodePacketForConnectNode, PACKET_TRANSFER>);

	}

	void SimplifyMeshNet::SetEventCenter(AutoEventCenter serverCenter, AutoEventCenter connectCenter)
	{
		MeshedNodeNet::SetEventCenter(serverCenter, connectCenter);

		serverCenter->RegisterEvent("NotifyInvalidNodeForUnit", MEM_NEW Logic::EventFactory< NotifyInvalidNodeForUnitEvent>());
		connectCenter->RegisterEvent("NotifyInvalidNodeForUnit", MEM_NEW Logic::EventFactory< NotifyInvalidNodeForUnitEvent>());
	}

	void SimplifyMeshNet::OnReceiveResponse(NetNodeConnectData *nodeData, AutoEvent &requstEvt, AutoEvent &respEvt)
	{

		MeshedNodeNet::OnReceiveResponse(nodeData, requstEvt, respEvt);
		//增加到Node索引
		Address addr(nodeData->mNodeConnect->GetIp(), nodeData->mNodeConnect ->GetPort());
		Hand<MeshConnect> conn = mNodeConnectHash.find((UInt64)addr);
		if (!conn)
		{
			conn = MEM_NEW MeshConnect();
			conn->mAddress = addr;
			mNodeConnectHash.insert((UInt64)addr, conn);
			ANodeConnect x = mNodeConnectHash.find((UInt64)addr);
			DEBUG_LOG("=== Node %s Append other connect %s > %s:%d = %lld THIS result %s", mpMeshNetNode->mNodeKey.dump().c_str(), addr.dump().c_str(), GetIp(), GetPort(), (UInt64)addr, x ? "YES" : "NO");

			AString exsitNodeInfo;
			for (auto it = mNodeConnectHash.begin(); it; ++it)
			{
				exsitNodeInfo += Address(it.key()).dump();
				ANodeConnect node = *it;
				exsitNodeInfo += it.key();
				exsitNodeInfo += "\r\n";
			}
			DEBUG_LOG("%0x result %s,  Exist : \r\n%s", this, x ? "OK" : "NO", exsitNodeInfo.c_str());

		}
		conn->mLocalToOther = nodeData;
	}

	void SimplifyMeshNet::OnConnected(tNetConnect *pConnect)
	{
		MeshedNodeNet::OnConnected(pConnect);
		DumpAllConnect();
		//增加到Node索引
		Address addr(pConnect->GetIp(), pConnect->GetPort());
		Hand<MeshConnect> conn = mNodeConnectHash.find((UInt64)addr);
		if (!conn)
		{
			conn = MEM_NEW MeshConnect();
			conn->mAddress = addr;
			mNodeConnectHash.insert((UInt64)addr, conn);
			ANodeConnect x = mNodeConnectHash.find((UInt64)addr);
			DEBUG_LOG("=== Node %s Append other connect %s > %s:%d = %lld THIS result %s", mpMeshNetNode->mNodeKey.dump().c_str(), addr.dump().c_str(), GetIp(), GetPort(), (UInt64)addr, x?"YES":"NO");

			AString exsitNodeInfo;
			for (auto it = mNodeConnectHash.begin(); it; ++it)
			{
				exsitNodeInfo += Address(it.key()).dump();
				ANodeConnect node = *it;
				exsitNodeInfo += it.key();
				exsitNodeInfo += "\r\n";
			}
			DEBUG_LOG("%0x result %s,  Exist : \r\n%s", this, x ? "OK" : "NO", exsitNodeInfo.c_str());

		}
		Hand< NodeRequestConnect> reqeustConnect = pConnect;
		conn->mLocalToOther = reqeustConnect->mNetNodeConnectData;
	}

	bool SimplifyMeshNet::OnAddConnect(tNetConnect *pConnect)
	{
		AutoTable whiteListTable = TableManager::getSingleton().GetTable(CLOUD_WHITE_LIST_NAME);
		if (whiteListTable)
		{
			Hand<IOCPConnect> conn = pConnect;
			if (conn)
			{
				int port = 0;
				AString ip = conn->GetRemoteAddr(port);

				if (NetAddress::IsLan(ip.c_str()))
					return MeshedNodeNet::OnAddConnect(pConnect);

				ARecord re = whiteListTable->GetRecord(ip);
				if (re)
				{
					if (!re[1])
					{
						LOG("%s no allow connect gate, now remove", ip.c_str());
						conn->SetRemove(true);
						return false;
					}
				}
				else
				{
					LOG("%s no in gate white allow list, now remove", ip.c_str());
					conn->SetRemove(true);
					return false;
				}
			}
		}

		return MeshedNodeNet::OnAddConnect(pConnect);
	}

	void SimplifyMeshNet::OnOtherNodeRequestConnect(HandConnect connect, GSKEY otherServerNetKey)
	{
		Hand<NodeServerConnect> serverConnect = connect;
		serverConnect->mServerNetKey = otherServerNetKey;

		Hand<NetNodeConnectData> evt = mRequestConnectList.find(otherServerNetKey);
		if (evt)
		{
			if (evt->mNodeConnect && !evt->mNodeConnect->IsRemove())
			{
				DEBUG_LOG("已经存在连接 %s", NetAddress(otherServerNetKey).dump().c_str());
			}

			//evt->mResponseConnect = connect;
			serverConnect->mNetNodeConnectData = evt.getPtr();

		}
		Hand<MeshConnect> conn = mNodeConnectHash.find(otherServerNetKey);
		if (!conn)
		{
			conn = MEM_NEW MeshConnect();
			conn->mAddress = otherServerNetKey;
			mNodeConnectHash.insert(otherServerNetKey, conn);
			ANodeConnect x = mNodeConnectHash.find(otherServerNetKey);
			DEBUG_LOG(" *** Node %s Append other connect %s > %s:%d THIS %lld result %s", mpMeshNetNode->mNodeKey.dump().c_str(), Address(otherServerNetKey).dump().c_str(), GetIp(), GetPort(),  otherServerNetKey,  x ? "YES":"NO" );

			AString exsitNodeInfo;
			for (auto it = mNodeConnectHash.begin(); it; ++it)
			{
				exsitNodeInfo += Address(it.key()).dump();
				ANodeConnect node = *it;
				exsitNodeInfo += it.key();
				exsitNodeInfo += "\r\n";
			} 
			DEBUG_LOG("%0x result %s,  Exist : \r\n%s", this,  x ? "OK" : "NO", exsitNodeInfo.c_str());
		}
		conn->mOtherToLocal = connect;
	}


	MeshNetNode::MeshNetNode(const char *szServerIp, int port, int nSafeCheck, int threadNum /*= 2*/)
		: mNodeKey(szServerIp, port)
		, mGateCount(1)
		, mbNeedTryReconnect(true)
	{
		if (port <= 0)
			ERROR_LOG("%s Local node port is 0", szServerIp);
		mEventCenter = MEM_NEW Logic::EventCenter();
		mNodeMeshNet = MEM_NEW SimplifyMeshNet(this, szServerIp, port, nSafeCheck, threadNum);

		GateClientSetNet *p = MEM_NEW GateClientSetNet(this);
		p->SetSafeCode(nSafeCheck);
		mConnectGateNet = p;

		mGlobalToGateConnect = p->CreateConnect();
		mGlobalToGateConnect->Init(mConnectGateNet, 0, "None", 0);
	}

	MeshNetNode::~MeshNetNode()
	{
		for (auto it = mGateHash.begin(); it; ++it)
		{
			it.get()._free();
		}
		mGateHash.clear();
		mNodeMeshNet->StopNet();
		mConnectGateNet->StopNet();
		mNodeMeshNet._free();
		mConnectGateNet._free();
		mEventCenter._free();
	}

	void MeshNetNode::SetSafeCode(int safeCode)
	{
		mNodeMeshNet->SetSafeCode(safeCode);
		Hand< GateClientSetNet> net = mConnectGateNet;
		net->SetSafeCode(safeCode);
	}

	bool MeshNetNode::AwaitConnectGate(const Address &gateAddress)
	{
		if (CORO == 0)
		{
			ERROR_LOG("Must in coro function");
			return false;
		}
		class Waiter : public Base<Waiter>
		{
		public:
			void OnConnectFinish(HandConnect pConnect, bool bOK)
			{
				Hand< GateClientSetNet> net = pConnect->GetNetHandle();
				if (!net)
				{
					mbOK = false;
					if (!CoroutineTool::Resume(mAsyncCoro))
						ERROR_LOG("Resume coro %d fail, May be in coro function %d, Cannot resume in coro function", CORO);
					return;
				}
				//??? if (!bOK && pConnect != net->mpNetNode->mGlobalToGateConnect)
				//	return;

				NetAddress connAddr(pConnect->GetIp(), pConnect->GetPort());
				if (mGateAddress == connAddr)
				{ 
					mbOK = bOK;
					if (!CoroutineTool::Resume(mAsyncCoro))
						ERROR_LOG("Resume coro %d fail, May be in coro function %d, Cannot resume in coro function", CORO);
				}
			}

		public:
			unsigned int mAsyncCoro = 0;
			bool mbOK = false;
			Address mGateAddress;
		};

		Hand<Waiter> waiter = MEM_NEW Waiter();
		waiter->mAsyncCoro = CORO;
		waiter->mGateAddress = gateAddress;

		CallBack<HandConnect> tempCall = mConnectGateCall;

		Init(gateAddress, CallBack<HandConnect>(&Waiter::OnConnectFinish, waiter.getPtr()));

		CoroutineTool::yield();
		
		mConnectGateCall.cleanup();
		// 恢复原来的回调
		mConnectGateCall = tempCall;

		return waiter->mbOK;
	}

	void MeshNetNode::RegisterMsgPacket(Auto<PacketFactory> packetFactory)
	{
		mConnectGateNet->GetNetProtocol()->RegisterNetPacket(packetFactory);
		Hand<SimplifyMeshNet> net = mNodeMeshNet;
		net->RegisterMsgPacket(packetFactory);
	}

	void MeshNetNode::AppendUnit(tNetUnit *pUnit)
	{
		if (mUnitListHash.exist(pUnit->mID))
			ERROR_LOG("Already exist unit %s", pUnit->mID.dump().c_str());
		for (auto it=mUnitListHash.begin(); it; ++it)
		{
			if (it.get().getPtr() == pUnit)
			{
				NOTE_LOG("Already exist unit %s, now unit id %s", it.key().dump().c_str(), pUnit->mID.dump().c_str());
				it.erase();
				break;
			}
		}

		mUnitListHash.insert(pUnit->mID, pUnit);
		pUnit->mNode = this;
		pUnit->Ready();

		// 通知到对应的Gate
		int gateKey = (UInt64)pUnit->mID % mGateCount;
		AGateConnect gate = mGateHash.find(gateKey);
		if (gate)
		{
			NotifyUnitInfoToGate(pUnit, gate);
			//AutoEvent evt = gate->StartEvent("NG_NotifyAppendUnit");
			//evt["UNIT_ID"] = (UInt64)pUnit->mID;
			//evt["NODE_KEY"] = (UInt64)mNodeKey;
			//evt->Start();
		}
		else
			ERROR_LOG("Append unit no exist gate %d", gateKey);

	}

	void MeshNetNode::NotifyUnitInfoToGate(tNetUnit *pUnit, AGateConnect gate)
	{
		if (gate)
		{
			AutoEvent evt = gate->StartEvent("NG_NotifyAppendUnit");
			evt["UNIT_ID"] = (UInt64)pUnit->mID;
			evt["NODE_KEY"] = (UInt64)mNodeKey;
			evt->Start();
		}
		else
			ERROR_LOG("Append unit no exist gate");
	}

	NetCloud::ANodeConnect MeshNetNode::FindNodeConnect(const Address &target)
	{
		Hand<SimplifyMeshNet> net = mNodeMeshNet;
		auto nodeConn = net->mNodeConnectHash.find((UInt64)target);
#if DEVELOP_MODE
		AString exsitNodeInfo;
		for (auto it= net->mNodeConnectHash.begin(); it; ++it)
		{
			exsitNodeInfo += Address(it.key()).dump();
			ANodeConnect node = *it;
			exsitNodeInfo += it.key();
			exsitNodeInfo += "\r\n";
		}
		DEBUG_LOG("&&& %0x Need find node connect %s, result %s,  Exist : \r\n%s", net.getPtr(), target.dump().c_str(), nodeConn?"OK":"NO", exsitNodeInfo.c_str());
#endif
		return nodeConn;
	}
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class GateConnectAtNode : public tGateConnect
	{
	public:
		virtual void SendToGate(UnitID targetAddress, Packet *pPacket)
		{
			mGateConnect->Send(pPacket, false);
		}

		virtual AutoEvent StartEvent(const char *eventMsgName)
		{
			if (mGateConnect)
				return mGateConnect->StartEvent(eventMsgName);
			return AutoEvent();
		}

		virtual Address GetAddress() const { return mGateAddress; }

	public:
		GateConnectAtNode(int gateID, HandConnect gateConnect, Address gateNodeAddress)
		{
			mGateID = gateID;
			mGateNodeAddress = gateNodeAddress;
			mGateConnect = gateConnect;
			Hand<IOCPConnect> conn = mGateConnect;
			int port = 0;
			AString ip = conn->GetRemoteAddr(port);
			mGateAddress = Address(ip.c_str(), port);
		}
		~GateConnectAtNode()
		{
			if (mGateConnectPtr)
			{
				mGateConnectPtr->mpGateConnect = NULL;
			}
		}

	public:
		HandConnect	mGateConnect;
		Address			mGateAddress;
		Address			mGateNodeAddress;
		int						mGateID;
		MeshNetNode::AGateConnectPtr mGateConnectPtr;
	};
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	void MeshNetNode::ClientGateConnect::OnDisconnect()
	{
		Hand< GateClientSetNet> net = GetNetHandle();
		if (net)
		{
			net->mpNetNode->OnDisconnect(GetIp(), GetPort());
		}

		if (mGateConnectPtr && mGateConnectPtr->mpGateConnect != NULL && net)
		{				
			Hand<GateConnectAtNode> node = mGateConnectPtr->mpGateConnect;
			Hand<GateConnectAtNode> gateNode = net->mpNetNode->mGateHash.find(node->mGateID);
			if (gateNode==node)
			{
				mGateAddress = node ->mGateAddress;
				net->mpNetNode->mGateHash.erase(node->mGateID);
				if (net->mpNetNode->mbNeedTryReconnect)
				{
					NOTE_LOG("Gate node %s ->%s disconnect, Try reconnect after 3 Sec ...", node->mGateNodeAddress.dump().c_str(), node->mGateAddress.dump().c_str());
					TimerCenter::WaitTime(3000, AsyncFunction(&ClientGateConnect::OnTime, this));
				}
			}
		}
	}

	void MeshNetNode::ClientGateConnect::OnTime()
	{
		Hand< IOCPClientSetNet> net = GetNetHandle();
		if (net)
		{
			NOTE_LOG("Begin reconnect %s ... ", mGateAddress.dump().c_str());
			net->Connect(mGateAddress.Ip().c_str(), mGateAddress.GetPort(), 6000);
		}
		else
			ERROR_LOG("Reconnect fail %s, net is NULL", mGateAddress.dump().c_str());
	}

	//-------------------------------------------------------------------------
	void MeshNetNode::AppendGate(int gateID, HandConnect gateConnect, Address gateNodeAddress)
	{
		Hand< GateConnectAtNode> gate = MEM_NEW GateConnectAtNode(gateID, gateConnect, gateNodeAddress);
		gate->mGateConnectPtr = MEM_NEW GateConnectPtr();
		gate->mGateConnectPtr->mpGateConnect = gate.getPtr();

		Hand< ClientGateConnect> conn = gateConnect;
		conn->mGateConnectPtr = gate->mGateConnectPtr;

		mGateHash.insert(gateID, gate);

		// 将当前存在的对应gate的单元通知到此Gate
		for (auto it=mUnitListHash.begin(); it; )
		{
			if (!it.get())
				it.erase();
			else
			{
				if (((UInt64)it.key() % mGateCount) == gateID)
					NotifyUnitInfoToGate(it.get().getPtr(), gate);
				++it;
			}
		}
	}

	void MeshNetNode::BroadcastMsg(NodePacket *pData)
	{
		// 分发到自己节点上的所有单元
		OnReceiveBroadcastMsg(pData);

		// 广播到所有Gate上
		for (auto it = mGateHash.begin(); it; ++it)
		{
			AGateConnect gateConn = it.get();
			if (gateConn)
				gateConn->SendToGate(pData->mTargetID, pData);
		}
	}

	void MeshNetNode::OnReceiveBroadcastMsg(NodePacket *pData)
	{
		// 全部广播
		if (pData->mbBroadcast == eBroadcastAll)
		{
			for (auto it = mUnitListHash.begin(); it; ++it)
			{
				AUnit unit = it.get();
				if (unit)
					unit->OnReceiveProcess(pData);
			}
		}
		else if (pData->mbBroadcast == eBroadcastByType)
		{
			// 广播到指定类型
			for (auto it = mUnitListHash.begin(); it; ++it)
			{
				AUnit unit = it.get();
				if (unit && unit->GetID().type == pData->mTargetID.type)
					unit->OnReceiveProcess(pData);
			}
		}
		else
			ERROR_LOG("No use broadcast type %d", pData->mbBroadcast);
	}

	bool MeshNetNode::SendEventToGate(int gateCode, Logic::tEvent *pEvent)
	{
		if (gateCode < 0)
		{
			bool bRe = false;
			for (auto it = mGateHash.begin(); it; ++it)
			{
				Hand<GateConnectAtNode> conn = it.get();
				if (conn && conn->mGateConnect)
					if (conn->mGateConnect->SendEvent(pEvent))
						bRe = true;
			}
			return bRe;
		}
		else
		{
			Hand<GateConnectAtNode> conn = mGateHash.find(gateCode);
			if (conn && conn->mGateConnect)
				return conn->mGateConnect->SendEvent(pEvent);

		}
		return false;
	}

	HandConnect MeshNetNode::FindGateConnect(int gateCode)
	{
		Hand< GateConnectAtNode> conn = mGateHash.find(gateCode);
		if (conn)
			return conn->mGateConnect;

		return HandConnect();
	}

	class RS_NotifyNodeKeyRequestGateInfo : public Logic::tServerEvent
	{
	public:
		virtual void _OnResp(AutoEvent &respEvent)
		{
			if (!mNetConnect)
			{
				ERROR_LOG("Connect is NULL");
				return;
			}
			Hand< MeshNetNode::GateClientSetNet>gateClientNet = mNetConnect->GetNetHandle();
			AssertEx(gateClientNet, "Must is MeshNetNode::GateClientSetNet");

			int gateID = respEvent["GATE_ID"];
			UInt64 gateNodeAddress = respEvent["GATE_NODE_ADDRESS"];
			gateClientNet->mpNetNode->mGateCount = respEvent["GATE_COUNT"];
			gateClientNet->mpNetNode->AppendGate(gateID, mNetConnect, gateNodeAddress);
			gateClientNet->mpNetNode->OnConnectFinish(mNetConnect->GetIp(), mNetConnect->GetPort());
			gateClientNet->mpNetNode->mConnectGateCall(mNetConnect, true);

			AutoData gateList = (DataStream*)respEvent["GATE_LIST_DATA"];
			if (gateList)
			{
				gateList->seek(0);
				UInt64 ipKey = 0;
				while (gateList->read(ipKey) && ipKey > 0)
				{
					Address address = ipKey;
					bool bExist = false;
					for (auto it = gateClientNet->mpNetNode->mGateHash.begin(); it; ++it)
					{
						if ((*it)->GetAddress() == address)
						{
							bExist = true;
							break;
						}
					}
					if (!bExist)
					{
						gateClientNet->Connect(address.Ip().c_str(), address.GetPort(), 10000);
						NOTE_LOG(">>> Connect gate [%s]", address.dump().c_str());
					}
				}
			}
		}

		virtual void _OnFinish() override
		{

		}

		virtual bool _OnTimeOver() override
		{
			// 如果连接到不是Gate的网络会执行到这里
			if (!mNetConnect)
			{
				ERROR_LOG("Connect is NULL");
				return true;
			}
			Hand< MeshNetNode::GateClientSetNet>gateClientNet = mNetConnect->GetNetHandle();
			AssertEx(gateClientNet, "Must is MeshNetNode::GateClientSetNet");
			if (gateClientNet->mpNetNode!=NULL)
				gateClientNet->mpNetNode->mConnectGateCall(mNetConnect, false);

			return Logic::tServerEvent::_OnTimeOver();
		}

		virtual void InitData() override
		{
			Logic::tServerEvent::InitData();
		}
	};

	class RS_RequestNodeInfobyUnitID : public Logic::tServerEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			WaitTime(10);
			return true;
		}
		virtual void _OnResp(AutoEvent &respEvent)
		{
			UInt64 id = get("UNIT_ID");
			UInt64 addr = respEvent["NODE_ADDRESS"];
			if (addr > 0)
			{
				if (!mNetConnect)
				{
					ERROR_LOG("Connect is NULL");
					return;
				}
				Hand< MeshNetNode::GateClientSetNet>gateClientNet = mNetConnect->GetNetHandle();
				AssertEx(gateClientNet, "Must is MeshNetNode::GateClientSetNet");
				gateClientNet->mpNetNode->OnReceiveNodeInfoForUnit(id, addr, this);
			}
			Finish();
		}
	};

	class GN_NotifyGateUpdateInfo : public Logic::tBaseNetEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			Hand< MeshNetNode::GateClientSetNet>gateClientNet = mNetConnect->GetNetHandle();
			AssertEx(gateClientNet, "Must is MeshNetNode::GateClientSetNet");
			AutoData gateList = (DataStream*)get("GATE_LIST_DATA");
			if (gateList)
			{
				gateList->seek(0);
				UInt64 ipKey = 0;
				while (gateList->read(ipKey) && ipKey > 0)
				{
					Address address = ipKey;
					bool bExist = false;
					for (auto it = gateClientNet->mpNetNode->mGateHash.begin(); it; ++it)
					{
						if ((*it)->GetAddress() == address)
						{
							bExist = true;
							break;
						}
					}
					if (!bExist)
					{
						gateClientNet->Connect(address.Ip().c_str(), address.GetPort(), 10000);
						NOTE_LOG(">>> Connect gate [%s]", address.dump().c_str());
					}
				}
			}
			return true;
		}
	};

	class GN_NodePacket : public TransferPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			Hand< MeshNetNode::GateClientSetNet> net = pConnect->GetNetHandle();
				net->mpNetNode->OnReceiveGate( this);
			return 0; 
		}
	};
	//-------------------------------------------------------------------------
	class G2N_NotifyInvalidNodeForUnitEvent : public Logic::tBaseNetEvent
	{
	public:
		virtual bool _DoEvent()
		{
			UInt64 id = get("UNIT_ID");
			UInt64 nodeAddress = get("NODE_ADDRESS");
			
			Hand<MeshNetNode::GateClientSetNet> net = mNetConnect->GetNetHandle();
			if (!net)
			{
				ERROR_LOG("Connect Net is not GateClientSetNet");
				return true;
			}

			ANodeConnect node = net->mpNetNode->mUnitConnectHash.find(id);
			if (node && (UInt64)node->GetAddress() == nodeAddress)
			{
				net->mpNetNode->mUnitConnectHash.erase(id);
				NOTE_LOG("Remove unit %s from node %s", UnitID(id).dump().c_str(), node->GetAddress().dump().c_str());
			}

			return true;
		}
	};
	//-------------------------------------------------------------------------
	class NG_NotifyBeginJumpUnit : public Logic::tServerEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			WaitTime(10);
			return true;
		}
		virtual void _OnResp(AutoEvent &respEvent)
		{
			if (!mNetConnect)
			{
				ERROR_LOG("Connect is NULL");
				return;
			}
			Hand< MeshNetNode::GateClientSetNet>gateClientNet = mNetConnect->GetNetHandle();
			AssertEx(gateClientNet, "Must is MeshNetNode::GateClientSetNet");
			if (gateClientNet && gateClientNet->mpNetNode!=NULL)
				gateClientNet->mpNetNode->OnJumpGateReadyFinish(get("UNIT_ID"));
		}
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	MeshNetNode::GateClientSetNet::GateClientSetNet(MeshNetNode *pNode)
		: mpNetNode(pNode)
		, mSafeCode(0)
	{
		GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<GN_NodePacket, PACKET_TRANSFER>());
		mEventCenter = MEM_NEW Logic::EventCenter();

		mEventCenter->RegisterEvent("NotifyNodeKeyRequestGateInfo", MEM_NEW Logic::EventFactory<RS_NotifyNodeKeyRequestGateInfo>());
		mEventCenter->RegisterEvent("RequestNodeInfobyUnitID", MEM_NEW Logic::EventFactory<RS_RequestNodeInfobyUnitID>());
		REGISTER_EVENT(mEventCenter, GN_NotifyGateUpdateInfo);
		REGISTER_MSG_EVENT(mEventCenter, "NG_NotifyAppendUnit");
		REGISTER_EVENT(mEventCenter, NG_NotifyBeginJumpUnit);
		EX_REGISTER_EVENT(mEventCenter, "NotifyInvalidNodeForUnitEvent", G2N_NotifyInvalidNodeForUnitEvent);
		EX_REGISTER_EVENT(mEventCenter, "NG_CheckExistUnit", Logic::tAsyncRequestEvent);
	}


	bool MeshNetNode::AwaitCheckExistUnit(UnitID id, int waitMilSecond)
	{
		if (mUnitListHash.exist(id))
			return true;

		// 异步请求
		AGateConnect gate = FindGateByID(id);
		if (gate)
		{
			Hand<Logic::tAsyncRequestEvent> evt = gate->StartEvent("NG_CheckExistUnit");
			if (evt)
			{
				evt["UNIT_ID"] = (UInt64)id;
				AutoEvent resp = evt->AwaitRequest(waitMilSecond);
				if (resp)
					return resp["EXIST"];
				ERROR_LOG("NG_CheckExistUnit request fail");
				return false;
			}
			ERROR_LOG("No exist NG_NotifyBeginJumpUnit ");
		}
		else
			ERROR_LOG("No exist Gate %lld type %d ", id.id, id.type);
		return false;
	}

	AString MeshNetNode::Dump()
	{
		AString info;
		info.Format("\r\n---------------Node : %s--------------------\r\nConnect gate count : %d\r\n", mNodeKey.dump().c_str(), mGateHash.size());
		for (auto it = mGateHash.begin(); it; ++it)
		{
			Hand<GateConnectAtNode> gate = *it;
			AString gateInfo;
			gateInfo.Format("Gate %d > %s = Mesh %s \r\n", it.key(), gate->GetAddress().dump().c_str(), gate->mGateNodeAddress.dump().c_str());
			info += gateInfo;
		}
		info += "Exist node connect : ";
		Hand< SimplifyMeshNet> net = mNodeMeshNet;
		info += net->mNodeConnectHash.size();
		info += "\r\n";
		for (auto it = net->mNodeConnectHash.begin(); it; ++it)
		{
			info += (*it)->GetAddress().dump();
			info += "\r\n";
		}
		info += "-----------------------node info end -----------------------\r\n";
		return info;
	}



}

