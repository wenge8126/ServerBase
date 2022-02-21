#ifndef _INCLUDE_ASYNCNODE_H_
#define _INCLUDE_ASYNCNODE_H_

#pragma once
#include "NetProcess.h"
#include "NetUnit.h"
#include "MeshNet.h"

class AsyncNode : public tNetProcess
{
public:
	AsyncNode();

public:
	virtual void StartNode(const char *szNodeIP, int nPort)
	{
		mNodeNet->StartNet(szNodeIP, nPort);
	}

	void ConnectGate(const char *szGateIP, int nPort);

	void AppendUnit(AUnit unit)
	{
		auto gate = GetGate(unit->GetID());
		if (gate)
		{
			MS_AppendUnit msg;
			msg.mUintID = unit->GetID();
			if (gate->mpConnect->Send(&msg, false))
				mUnitList.insert(unit->GetID(), unit);
			else
				ERROR_LOG("Send MS_AppendUnit %s fail", unit->GetID().dump().c_str());
		}
		else
			ERROR_LOG("No exist unit %s gate %d", unit->GetID().dump().c_str(), unit->GetID().Hash(mGateCount));
	}

	bool Send(NodePacket *p)
	{
		auto nodeData = mUnitNodeIndex.find(p->mTargetID);
		if (nodeData)
		{
			if (nodeData->mpConnect->Send(p, false))
				return true;
		}
		auto gate = GetGate(p->mTargetID);
		if (gate)
		{
			if (!gate->mpConnect->Send(p, false))
				ERROR_LOG("Send MS_AppendUnit %s fail", p->Dump().c_str());
		}
		else
			ERROR_LOG("No exist unit %s gate %d", p->Dump().c_str(), p->mTargetID.Hash(mGateCount));
	}

	MeshNet::AConnectData GetGate(UnitID id)
	{
		int code = id.Hash(mGateCount);
		auto gate = mGateList.Find(code);
		return gate;
	}

public:
	void On(tNetConnect *pConnect, MS_NotifyNodeInfo &msg);

	void On(tNetConnect *pConnect, RQ_RequestUnitListInfo &msg, RS_UnitInfoList &resp)
	{
		//???
	}

public:
	int mGateCount = 1;
	Hand<MeshNet>		mNodeNet;
	ArrayMap<MeshNet::AConnectData>	mGateList;

	FastHash<UInt64, AUnit>		mUnitList;
	FastHash<UInt64, MeshNet::AConnectData> mUnitNodeIndex;
};


#endif //_INCLUDE_ASYNCNODE_H_
