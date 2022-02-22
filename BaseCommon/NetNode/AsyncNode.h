#ifndef _INCLUDE_ASYNCNODE_H_
#define _INCLUDE_ASYNCNODE_H_

#pragma once
#include "NetProcess.h"
#include "NetUnit.h"
#include "MeshNet.h"
#include "FastHash.h"

class AsyncNode : public tNetProcess
{
public:
	AsyncNode();

public:
	virtual void StartNode(const char *szNodeIP, int nPort)
	{
		mNodeNet->StartNet(szNodeIP, nPort);
	}

	virtual void CloseNode();

	void ConnectGate(const char *szGateIP, int nPort);

	void AppendUnit(AUnit unit);

	void RemoveUnit(AUnit unit)
	{
		AUnit existUnit = mUnitList.find(unit->GetID());
		if (existUnit == unit)
		{
			mUnitList.erase(unit->GetID());
			BroadcastUnitNoExist(unit->GetID());
		}
	}

	void BroadcastUnitNoExist(UnitID id);

	bool Send(int packetID, NodePacket *p)
	{
		auto nodeData = mUnitNodeIndex.find(p->mTargetID);
		if (nodeData)
		{
			if (nodeData->mpConnect->Send(packetID, p))
				return true;
		}
		auto gate = GetGate(p->mTargetID);
		if (gate)
		{
			if (!gate->mpConnect->Send(packetID, p))
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
	// 
	void On(tNetConnect *pConnect, GN_NotifyNodeInfo &msg);

	// �ڵ�֮ǰ�໥����, ����Unit��Ϣ
	void On(tNetConnect *pConnect, NN_RequestUnitListInfo  &msg, NN_ResponseUnitInfoList &resp)
	{
		resp.mUnitList.resize(mUnitList.size());
		int i = 0;
		for (auto it=mUnitList.begin(); it; ++it)
		{
			if (it.get())
				resp.mUnitList[i++] = it.key();
		}
	}

	void On(tNetConnect *pConnect, NN_BroadcastUnitNoExist &msg)
	{
		MeshNet::AConnectData d = pConnect->GetUserData();
		if (d)
		{
			auto existUnit = mUnitNodeIndex.find(msg.mNoExistUnitID);
			if (existUnit == d)
			{
				mUnitNodeIndex.erase(msg.mNoExistUnitID);
				NOTE_LOG("Remove unit %s", UnitID(msg.mNoExistUnitID).dump().c_str());
			}
		}
	}


	virtual bool ProcessPacket(tNetConnect* pConnect, Packet *pPak) override
	{
		TransferPacket *pPacket = dynamic_cast<TransferPacket*>(pPak);
		auto existUnit = mUnitList.find(pPacket->mTargetID);
		if (existUnit)
		{
			existUnit->OnReceiveProcess(pPacket);
		}
		else
		{
			BroadcastUnitNoExist(pPacket->mTargetID);
			// �������ٴη��͸�Ŀ��Gate
			auto gate = GetGate(pPacket->mTargetID);
			if (gate)
			{
				if (gate->mpConnect != pConnect && gate->IsValid())
				{
					if (!gate->mpConnect->Send(pPacket->GetFactory()->GetPacketID(), pPacket))
						ERROR_LOG("Send %s fail to gate %d, target %s", pPacket->GetMsgName(), gate->mNodeCode, pPacket->mTargetID.dump().c_str());
				}
				else
					ERROR_LOG("Target gate is now from connect, then stop send %s", pPacket->GetMsgName());
			}
			else
				ERROR_LOG("No exist unit %s gate %d", pPacket->mTargetID.dump().c_str(), pPacket->mTargetID.Hash(mGateCount));
		}
		return true;
	}

public:
	int mGateCount = 1;
	Hand<MeshNet>		mNodeNet;
	ArrayMap<MeshNet::AConnectData>	mGateList;

	FastHash<UInt64, AUnit>		mUnitList;
	FastHash<UInt64, MeshNet::AConnectData> mUnitNodeIndex;
};


#endif //_INCLUDE_ASYNCNODE_H_