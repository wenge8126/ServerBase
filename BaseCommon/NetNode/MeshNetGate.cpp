#include "MeshNetGate.h"
#include "EventCenter.h"

namespace NetCloud
{

	//-------------------------------------------------------------------------*/
	// 接收处理结点连接请求, 通知IP key, 如果需要其他所有节点信息, 则回复
	class SR_NotifyNodeKeyRequestGateInfo : public Logic::tClientEvent
	{
	public:
		virtual bool _DoEvent()
		{
			if (!mNetConnect)
			{
				ERROR_LOG("%s Net connect is null", GetEventName());
				return false;
			}
			Hand<MeshGate::ViaductNodeServerNet> nodeServer = mNetConnect->GetNetHandle();
			AssertEx(nodeServer, "Must is MeshGate::ViaductNodeServerNet");
			Address key = (UInt64)get("NODE_KEY");
			nodeServer->mpMeshGate->OnNodeConnected(key, mNetConnect, this);
			Finish();
			return true;
		}
	};

	class SR_RequestNodeInfobyUnitID : public Logic::tClientEvent
	{
	public:
		virtual bool _DoEvent()
		{
			if (!mNetConnect)
			{
				ERROR_LOG("%s Connect is NULL", GetEventName());
				return false;
			}
			UInt64 id = get("UNIT_ID");

			Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
			ANodeConnect node = net->mpMeshGate->mUnitConnectHash.find(id);
			if (node)
			{
				GetResponseEvent()["NODE_ADDRESS"] = (UInt64)node->GetAddress();

			}
			else
				GetResponseEvent()["ERROR"] = "No found";
			Finish();
			return true;
		}
	};

	void MeshGate::RegisterMsgPacket(Auto<PacketFactory> packetFactory)
	{
		mNodeServerNet->GetNetProtocol()->RegisterNetPacket(packetFactory);
		Hand<GateMeshNet> net = mGateNet;
		net->RegisterMsgPacket(packetFactory);
	}

	AString MeshGate::Dump()
	{
		AString gateInfo;
		gateInfo.Format("\r\n---------------- Gate count : %d ----------------------", GetGateCount());		
		gateInfo += mGateNet->DumpAllConnect(false);
		gateInfo += "Node :\r\n";
		for (auto it = mNodeHash.begin(); it; ++it)
		{
			gateInfo += it.key().dump() + "\r\n";
		}
		gateInfo += "----------------------gate info end-----------------------\r\n";
		return gateInfo;
	}

	NetCloud::ANodeConnect MeshGate::FindNodeConnect(UnitID target)
	{
#if !DEVELOP_MODE			
		ANodeConnect conn = mUnitConnectHash.find(target);
		if (conn)
		{
			if (conn->IsOK())
				return conn;
			mUnitConnectHash.erase(target);
		}
		return ANodeConnect();
#else
		ANodeConnect conn = mUnitConnectHash.find(target);
		if (conn)
		{
			if (conn->IsOK())
				return conn;
			mUnitConnectHash.erase(target);
			conn.setNull();
		}
		{
			NOTE_LOG("Now find node for %s", target.dump().c_str());
			NOTE_LOG("---------- Exist ------------------------------");
			for (auto it = mUnitConnectHash.begin(); it; ++it)
			{
				auto connect = it.get();
				UnitID  id;
				id.value = it.key();
				NOTE_LOG("key %s address > %s", id.dump().c_str(), connect->GetAddress().dump().c_str());
			}
			NOTE_LOG("============End ==========");
		}
		return ANodeConnect();
#endif
	}

	void MeshGate::BroadCastEvent(Logic::tEvent *pEvent, bool bToGate)
	{
		for (auto it = mNodeHash.begin(); it; ++it)
		{
			Hand< ViaductNodeConnect> conn = it.get();
			if (conn && conn->IsOK())
				conn->mNetConnect->SendEvent(pEvent);
		}
		if (!bToGate)
			return;
		// 同时还需要广播到其他Gate
		Hand< GateMeshNet> net = mGateNet;
		for (auto it = net->mGateConnectHash.begin(); it; ++it)
		{
			HandConnect gateConnect = *it;
			if (gateConnect)
				gateConnect->SendEvent(pEvent);
		}
	}

	void MeshGate::OnNodeConnected(Address nodeKey, HandConnect nodeConnect, Logic::tClientEvent *pReqeust)
	{
		pReqeust->GetResponseEvent()["GATE_ID"] = mGateID;
		pReqeust->GetResponseEvent()["GATE_NODE_ADDRESS"] = (UInt64)Address(mGateNet->GetIp(), mGateNet->GetPort());
		Hand<GateMeshNet> gateNet = mGateNet;
		pReqeust->GetResponseEvent()["GATE_COUNT"] = gateNet->mpMeshGate->GetGateCount();
		if (pReqeust->get("NEED_ALL_GATE"))
		{
			Hand<GateMeshNet> gate = mGateNet;
			AutoData gateListData = MEM_NEW DataBuffer();
			for (auto it = gate->mGateConnectHash.begin(); it; ++it)
			{
				Hand< MeshGateConnect> gateConnect = *it;
				gateListData->write((UInt64)gateConnect->mGateKey);
			}
			pReqeust->GetResponseEvent()["GATE_LIST_DATA"] = gateListData.getPtr();
		}
		Hand< ViaductNodeConnect> node = MEM_NEW  ViaductNodeConnect();
		node->mNetConnect = nodeConnect;
		node->mAddress = nodeKey;
		mNodeHash.insert(nodeKey, node);
	}

	// Node 创建或增加应用单元时, 发送单元ID信息, 追加到索引中
	class NG_NotifyAppendUnit : public Logic::tBaseNetEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			if (!mNetConnect)
			{
				ERROR_LOG("%s Connect is NULL", GetEventName());
				return false;
			}
			UInt64 id = get("UNIT_ID");
			UInt64 nodekey = get("NODE_KEY");
			Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
			ANodeConnect conn = net->mpMeshGate->mNodeHash.find(nodekey);
			if (conn)
			{
				auto existConn = net->mpMeshGate->mUnitConnectHash.find(id);
				if (existConn)
				{
					UnitID existID(id);
					ERROR_LOG("Now exist node connect of unit %s > %s, now append %s connect %s", existID.dump().c_str(), existConn->GetAddress().dump().c_str(), UnitID(id).dump().c_str(), conn->GetAddress().dump().c_str());
					// 如果存在的连接不一至, 则广播移除
					if (existConn != conn)
					{
						AutoEvent evt = net->GetEventCenter()->StartDefaultEvent("NotifyInvalidNodeForUnitEvent");
						evt["UNIT_ID"] = id;
						evt["NODE_ADDRESS"] = (UInt64)existConn->GetAddress();
						net->mpMeshGate->BroadCastEvent(evt.getPtr());
					}
				}
				net->mpMeshGate->mUnitConnectHash.insert(id, conn);
				NOTE_LOG("**==** Gate %d Append unit {%lld:%lld} for node  >%s", net->mpMeshGate ->mGateID, UnitID(id).type, UnitID(id).id, conn->GetAddress().dump().c_str());
				net->mpMeshGate->OnJumpFinish(id);
			}
			else
				ERROR_LOG("**--** Gate %d No exist node %s, {%lld:%lld}", net->mpMeshGate->mGateID, Address(nodekey).dump().c_str(), UnitID(id).type, UnitID(id).id);
			return true;
		}
	};


	class NG_NotifyBeginJumpUnit : public Logic::tClientEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			if (!mNetConnect)
			{
				ERROR_LOG("%s Connect is NULL", GetEventName());
				return false;
			}
			UInt64 id = get("UNIT_ID");
			//UInt64 nodekey = get("NODE_KEY");
			Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
			net->mpMeshGate->OnReqeustJumpNode(id);
			Finish();
			return true;
		}
	};

	//-------------------------------------------------------------------------
	class NG_CheckExistUnit : public Logic::tClientEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			if (!mNetConnect)
			{
				ERROR_LOG("%s Connect is NULL", GetEventName());
				return false;
			}
			UInt64 id = get("UNIT_ID");
			//UInt64 nodekey = get("NODE_KEY");
			Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
			GetResponseEvent()["EXIST"] =  net->mpMeshGate->mUnitConnectHash.exist(id);
			Finish();
			return true;
		}
	};

	//-------------------------------------------------------------------------

	class NG_NodePacket : public TransferPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			Hand< MeshGate::ViaductNodeServerNet> net = pConnect->GetNetHandle();
			 
			net->mpMeshGate->OnReceive(this);
			return 0;
		}
	};

	MeshGate::ViaductNodeServerNet::ViaductNodeServerNet()
		: mSafeCode(0)
	{
		GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<NG_NodePacket, PACKET_TRANSFER>);
		mEventCenter = MEM_NEW Logic::EventCenter(); 
		mEventCenter->RegisterEvent("NotifyNodeKeyRequestGateInfo", MEM_NEW Logic::EventFactory< SR_NotifyNodeKeyRequestGateInfo>());
		mEventCenter->RegisterEvent("RequestNodeInfobyUnitID", MEM_NEW Logic::EventFactory< SR_RequestNodeInfobyUnitID>());
		mEventCenter->RegisterEvent("GN_NotifyGateUpdateInfo", MEM_NEW Logic::EventFactory<Logic::tBaseNetEvent>());
		REGISTER_EVENT(mEventCenter, NG_NotifyAppendUnit);
		REGISTER_EVENT(mEventCenter, NG_NotifyBeginJumpUnit);
		REGISTER_EVENT(mEventCenter, NG_CheckExistUnit);
	}

	bool MeshGate::ViaductNodeServerNet::OnAddConnect(tNetConnect *pConnect)
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
					return IOCPServerNet::OnAddConnect(pConnect);

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

		return IOCPServerNet::OnAddConnect(pConnect);
	}

	//-------------------------------------------------------------------------
// 用于接受连接
	class NodePacketForGateMeshServer : public TransferPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			Hand<NodeServerConnect> conn = pConnect;
			Hand< NodeServerNet> net = conn->GetNetHandle();
			Hand<MeshGate::GateMeshNet> meshNet = net->mOwnerMeshNet;
			//ANodeConnect node = meshNet->mpMeshNetNode->FindNodeConnect(conn->mServerNetKey);
			meshNet->mpMeshGate->OnReceive(this);
			return 0;
		}
	};
	// 用于连接端
	class NodePacketForGateConnectMesh : public TransferPacket
	{
	public:
		virtual UINT Execute(tNetConnect* pConnect) override
		{
			Hand<NodeRequestConnect> conn = pConnect;
			Hand<MeshGate::GateMeshNet> meshNet = conn->GetNetHandle();
			//ANodeConnect node = meshNet->mpMeshNetNode->FindNodeConnect(conn->GetTargetNetKey());
			meshNet->mpMeshGate->OnReceive(this);
			return 0;
		}
	};
	class NotifyInvalidNodeForUnitEvent : public Logic::tBaseNetEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			if (!mNetConnect)
				return true;
			Hand<MeshGate::GateMeshNet> net = mNetConnect->GetNetHandle();
			if (net && net->mpMeshGate!=NULL)
				net->mpMeshGate->BroadCastEvent(this, false);
			return true;
		}
	};
	//-------------------------------------------------------------------------
	MeshGate::GateMeshNet::GateMeshNet(MeshGate *pGate, const char *szServerIp, int port, int nSafeCheck, int threadNum /*= 2*/) 
		: MeshedNodeNet(szServerIp, port, nSafeCheck, threadNum)
		, mpMeshGate(pGate)
	{
		SetEventCenter(MEM_NEW Logic::EventCenter(), MEM_NEW Logic::EventCenter());

		mServerNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<NodePacketForGateMeshServer, PACKET_TRANSFER>);
		GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<NodePacketForGateConnectMesh, PACKET_TRANSFER>);
	}

	void MeshGate::GateMeshNet::RegisterNodeEvent(AutoEventCenter serverCenter, AutoEventCenter connectCenter)
	{
		REGISTER_EVENT(serverCenter, NotifyInvalidNodeForUnitEvent);
		REGISTER_EVENT(connectCenter, NotifyInvalidNodeForUnitEvent);
	}

	void MeshGate::GateMeshNet::OnReceiveResponse(NetNodeConnectData *nodeData, AutoEvent &requstEvt, AutoEvent &respEvt)
	{
		MeshedNodeNet::OnReceiveResponse(nodeData, requstEvt, respEvt);
		int gateID = respEvt["GATE_ID"];

		Hand<MeshGateConnect> conn = mGateConnectHash.find(gateID);
		if (!conn)
		{
			conn = MEM_NEW MeshGateConnect();
			mGateConnectHash.insert(gateID, conn);

		}
		conn->mNodeConnect = nodeData->mNodeConnect;
		conn->mGateKey = (UInt64)respEvt["GATE_KEY"];

		// 通知已经存在的节点当前所有GateInfo
		AutoEvent notifyEvt = mpMeshGate->mNodeServerNet->GetEventCenter()->StartEvent("GN_NotifyGateUpdateInfo"); 
		
		AutoData gateListData = MEM_NEW DataBuffer();
		for (auto it = mGateConnectHash.begin(); it; ++it)
		{
			Hand< MeshGateConnect> gateConnect = *it;
			gateListData->write((UInt64)gateConnect->mGateKey);
		}
		notifyEvt["GATE_LIST_DATA"] = gateListData.getPtr();

		for (auto it = mpMeshGate->mNodeHash.begin(); it; ++it)
		{
			Hand< ViaductNodeConnect> nodeConn = *it;
			if (nodeConn && nodeConn->mNetConnect)
				nodeConn->mNetConnect->SendEvent(notifyEvt.getPtr());
		}
	}

	bool MeshGate::ViaductNodeConnect::SendToNode(UnitID targetAddress, Packet *pPacket)
	{
		if (mNetConnect)
		{
			if (mNetConnect->Send(pPacket->GetFactory()->GetPacketID(), pPacket))
				return true;
			else
				mNetConnect.setNull();
		}
		else
			ERROR_LOG("%s  Now node net connect is NULL", mAddress.dump().c_str());
		return false;
	}

}

