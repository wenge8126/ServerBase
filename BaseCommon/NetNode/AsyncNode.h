#ifndef _INCLUDE_ASYNCNODE_H_
#define _INCLUDE_ASYNCNODE_H_

#pragma once
#include "NetProcess.h"
#include "NetUnit.h"
#include "MeshNet.h"
#include "FastHash.h"

class Net_Export  AsyncNode : public tNetProcess
{
public:
	AsyncNode();

public:
	virtual void StartNode(const char *szNodeIP, int nPort, int nSafeCheck)
	{
		mNodeNet->mSafeCode = nSafeCheck;
		mNodeNet->StartNet(szNodeIP, nPort);
	}

	virtual void CloseNode();

	virtual void Process()
	{
		mNodeNet->Process();
	}

	virtual void LowProcess(int spaceTime)
	{
		mNodeNet->LowProcess(spaceTime);
	}

	void ConnectGate(const char *szGateIP, int nPort, int overMilSecond);

	bool AwaitConnectGate(const char *szGateIP, int nPort, int overMilSecond);

	bool AppendUnit(AUnit unit);

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
			if (gate->mpConnect->Send(packetID, p))
				return true;
			ERROR_LOG("Send MS_AppendUnit %s fail", p->Dump().c_str());
		}
		else
			ERROR_LOG("No exist unit %s gate %d", p->Dump().c_str(), p->mTargetID.Hash(mGateCount));

		return false;
	}

	MeshNet::AConnectData GetGate(UnitID id)
	{
		int code = id.Hash(mGateCount);
		auto gate = mGateList.Find(code);
		return gate;
	}

	Hand<MeshNet> GetNet() const { return mNodeNet; }

public:
	// 
	void On(tNetConnect *pConnect, GN_NotifyNodeInfo &msg);

	// 节点之前相互请求, 缓存Unit信息
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

#if DEBUG_CLOUD_NET
		AString info;
		info.Format(" >>> %d : N %s ", ++pPacket->mSendCount, NetAddress(mNodeNet->mKey).dump().c_str());
		pPacket->mSendInfo += info;
		NOTE_LOG("%u :  %s", pPacket->mRequestID, pPacket->mSendInfo.c_str());
#endif

		auto existUnit = mUnitList.find(pPacket->mTargetID);
		if (existUnit)
		{
			existUnit->OnReceiveProcess(pPacket);
		}
		else
		{
			BroadcastUnitNoExist(pPacket->mTargetID);
			// 尝试中再次发送给目标Gate
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
	ArrayList<AUnit>	mWaitAppendList;
	LoopDataStream	mTempLoopData;
};


#endif //_INCLUDE_ASYNCNODE_H_
