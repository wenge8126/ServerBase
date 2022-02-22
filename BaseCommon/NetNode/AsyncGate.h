
#ifndef _INCLUDE_ASYNCGATE_H_
#define _INCLUDE_ASYNCGATE_H_

#pragma once

#include "MeshNet.h"
#include "DefineMsgFactory.h"
#include "NetCommon.h"
#include "FastHash.h"

enum GateMsgType
{
	eN2G_RequestGateInfo = eMeshMsg_Max+1,
	eN2G_AppendUnit,
	eG2N_NotifyNodeInfo,
	eN2N_RequestUnitList,
	eNGN_TransferMsg,
	eN2G_NotifyNodeClose,
	eN2N_BroadcastUnitNoExist,
};

class AsyncGate : public tNetProcess
{
public:
	AsyncGate(size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM);

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


	// 接收节点请求获取Gate信息
	void On(tNetConnect *pConnect, NG_RequestGateInfo  &req, GN_ResponseGateListInfo &info)
	{		
		MeshNet::AConnectData nodeData = MEM_NEW MeshNet::MeshConnectData();
		nodeData->mNodeKey = req.mNodeKey;
		pConnect->SetUserData(nodeData);

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
	}

	void On(tNetConnect *pConnect, NG_AppendUnit &req)
	{
		mUnitList.insert(req.mUintID, pConnect->GetUserData());
		NOTE_LOG("Append unit %s", UnitID(req.mUintID).dump().c_str());
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

public:
	int mGateCount  = 1;
	Hand<MeshNet>		mGateNet;
	FastHash<UInt64, MeshNet::AConnectData> mUnitList;
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


