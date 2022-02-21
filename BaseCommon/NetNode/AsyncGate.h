
#ifndef _INCLUDE_ASYNCGATE_H_
#define _INCLUDE_ASYNCGATE_H_

#pragma once

#include "MeshNet.h"
#include "DefineMsgFactory.h"
#include "NetCommon.h"

enum GateMsgType
{
	eGateMsg_RequestGateInfo = eMeshMsg_Max+1,
	eGateMsg_AppendUnit,
	eGateMsg_NotifyNodeInfo,
	eNodeMsg_UnitList,
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
	void On(tNetConnect *pConnect, RQ_RequestGateInfo &req, RS_GateListInfo &info)
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
			auto nodeData = it.get();
			info.mGateList[nodeData->mNodeCode] =  nodeData->mNodeKey;
		}
	}

	void On(tNetConnect *pConnect, MS_AppendUnit &req)
	{
		mUnitList.insert(req.mUintID, pConnect->GetUserData());
	}

	virtual bool ProcessPacket(tNetConnect* pConnect, TransferPacket *pPacket)
	{
		auto nodeData = mUnitList.find(pPacket->mTargetID);
		if (nodeData)
		{
			nodeData->mpConnect->Send(pPacket, false);

			MS_NotifyNodeInfo nodeInfo;
			nodeInfo.mNodeKey = nodeData->mNodeKey; //??? eGateMsg_NotifyNodeInfo
			pConnect->Send(&nodeInfo, false);
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



#endif //_INCLUDE_ASYNCGATE_H_


