
#ifndef _INCLUDE_ASYNCGATE_H_
#define _INCLUDE_ASYNCGATE_H_

#pragma once

#include "MeshNet.h"
#include "DefineMsgFactory.h"
#include "NetCommon.h"
#include "FastHash.h"


class Net_Export AsyncGate : public tNetProcess
{
public:
	AsyncGate(size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM);
	~AsyncGate()
	{
		mGateNet._free();
	}

public:
	void StartGate(int gateCount, int gateCode, const char* szGateIP, int gatePort, const char *szMainIP, int mainPort, int nSafeCode)
	{
		mGateNet->mCode = gateCode;
		mGateCount = gateCount;
		mGateNet->mSafeCode = nSafeCode;
		mGateNet->StartNet(szGateIP, gatePort);

		if (strcmp(szGateIP, szMainIP) != 0 || gatePort != mainPort)
			mGateNet->ConnectNode(szMainIP, mainPort, 6000);
	}

	void CloseGate()
	{
		mUnitList.clear();
		mNodeList.clear();
		mGateNet->StopNet();
	}

	virtual void Process()
	{
		mGateNet->Process();
	}

	virtual void LowProcess(int spaceTime)
	{
		mGateNet->LowProcess(spaceTime);
	}

	// 接收节点请求获取Gate信息
	void On(tNetConnect *pConnect, NG_RequestGateInfo  &req, GN_ResponseGateListInfo &info)
	{		
		MeshNet::AConnectData nodeData = MEM_NEW MeshNet::MeshConnectData();
		nodeData->mNodeKey = req.mNodeKey;
		nodeData->mpConnect = pConnect;
		pConnect->SetUserData(nodeData);

		mNodeList.insert(req.mNodeKey, nodeData);

		info.mGateCode = mGateNet->mCode;
		info.mGateCount = mGateCount;

		auto &gateList = mGateNet->GetMeshNodeList();
		info.mGateList.resize(gateList.size() + 1);
		for (auto it=gateList.begin(); it; ++it)
		{
			auto gateData = it.get();
			if (gateData)
				info.mGateList[gateData->mNodeCode] = gateData->mNodeKey;
		}

		DumpNode();
	}

	void On(tNetConnect *pConnect, NG_AppendUnit &req)
	{
		mUnitList.insert((UInt64)req.mUintID, pConnect->GetUserData());
		NOTE_LOG("Append unit %s", UnitID(req.mUintID).dump().c_str());
		DumpUnit();
	}

	void On(tNetConnect *pConnect, NG_NotifyNodeClose &msg)
	{
		MeshNet::AConnectData nodeData = pConnect->GetUserData();
		{
			for (auto it = mUnitList.begin(); it; )
			{
				if (it.get() == nodeData)
					it.erase();
				else
					++it;
			}
		}
		pConnect->SetUserData(AutoAny());
		pConnect->SetRemove(true);
	}

	virtual bool ProcessPacket(tNetConnect* pConnect, Packet *pPak) override
	{
		TransferPacket *pPacket = dynamic_cast<TransferPacket*>(pPak);
#if DEBUG_CLOUD_NET
		AString info;
		info.Format(" >>> %d : G %s ", ++pPacket->mSendCount, NetAddress(mGateNet->mKey).dump().c_str());
		pPacket->mSendInfo += info;
		NOTE_LOG("%u :  %s", pPacket->mRequestID, pPacket->mSendInfo.c_str());
#endif
		auto nodeData = mUnitList.find(pPacket->mTargetID);
		if (nodeData)
		{
			nodeData->mpConnect->Send(eNGN_TransferMsg, pPacket);

			GN_NotifyNodeInfo nodeInfo;
			nodeInfo.mNodeKey = nodeData->mNodeKey; 
			pConnect->Send(eG2N_NotifyNodeInfo, &nodeInfo);
		}
		else
			ERROR_LOG("No exist node %s", pPacket->mTargetID.dump().c_str());
		return false;
	}

	void DumpNode()
	{
		NOTE_LOG("Gate %s\r\n---------------- node ----------------", NetAddress(mGateNet->mKey).dump().c_str());
		for (auto it=mNodeList.begin(); it; ++it)
		{
			auto nodeData = it.get();
			NOTE_LOG("Node : %s", NetAddress(it.key()).dump().c_str())
		}
		NOTE_LOG("\r\n--------------------------------------");
	}

	void DumpUnit()
	{
		NOTE_LOG("Gate %s\r\n---------------- unit ----------------", NetAddress(mGateNet->mKey).dump().c_str());
		for (auto it = mUnitList.begin(); it; ++it)
		{
			auto nodeData = it.get();
			if (nodeData)
				NOTE_LOG("unit %s > node : %s ", UnitID(it.key()).dump().c_str(), NetAddress(nodeData->mNodeKey).dump().c_str());
		}
		NOTE_LOG("\r\n--------------------------------------");
	}

public:
	int mGateCount  = 1;
	Hand<MeshNet>		mGateNet;
	FastHash<UInt64, MeshNet::AConnectData> mUnitList;
	FastHash<UInt64, MeshNet::AConnectData> mNodeList;
};
//-------------------------------------------------------------------------
class GateTransferPacketFactory : public tPacketFactory
{
public:
	GateTransferPacketFactory(tNetProcess *pGate)
	{
		mGate = pGate->mAutoThis;
	}

public:
	virtual void ProcessPacket(tNetConnect *pConnect, Packet *pPacket) override
	{
		mGate->mpProcess->ProcessPacket(pConnect, pPacket);
	}

	virtual Packet *_createPacket() { return MEM_NEW TransferPacket(); }
	virtual PacketID_t	GetPacketID()const override { return eNGN_TransferMsg; }

	virtual const char* GetPacketName() { return  "GateTransferPacketFactory"; }

public:
	AProcess mGate;
};
//-------------------------------------------------------------------------

#endif //_INCLUDE_ASYNCGATE_H_


