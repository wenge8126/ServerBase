#pragma once
#ifndef _INLCUDE_GATE_H_
#define _INLCUDE_GATE_H_

#include "NetCommon.h"
#include "NetUnit.h"

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// GateNode֮��ʹ��MeshNet ����
	// NOTE: ֻ������ת��Ϣ���ṩNode��Ϣ, �����ڴ������д���ҵ��
	//-------------------------------------------------------------------------
	class tGate : public Base<tGate>
	{
	public:		
		// gateServerAddress : ���ܽڵ����ӵĵ�ַ, (����������ڵ��ַ), mainGateAddress : ��GateMesh��ַ
		virtual void Init(const Address &gateServerAddress, const Address &mainGateAddress) = 0;
		
		virtual void Process() = 0;
		virtual void LowProcess(int milSec) = 0;
		virtual AString Dump() = 0;

		virtual AGateConnect FindGateConnect(UnitID target) = 0;
		virtual ANodeConnect FindNodeConnect(UnitID target) = 0;

		virtual void BroadCastMsg(NodePacket *pData) = 0;
		//virtual UInt64 CreateNewNetID(int type) = 0;

		virtual Logic::tEventCenter* GetEventCenter() = 0;
		virtual void RegisterMsgPacket(Auto<PacketFactory> packetFactory) = 0;

		virtual void Close() {}

		virtual bool SendToNode(NodePacket *p)
		{
			ANodeConnect nodeConn = FindNodeConnect(p->mTargetID);
			if (nodeConn)
			{
				p->OnBeforeSend(nodeConn.getPtr());
				nodeConn->SendToNode(p->mTargetID, p);
				return true;
			}
			return false;
		}

		// ������, Node��ֱ�����е�GateNode
		virtual void SendToGate(NodePacket *pData)
		{
			// ����ID�ҵ����ڵ�GATE, ������ȥ
			AGateConnect gateConn = FindGateConnect(pData->mTargetID);
			if (gateConn)
			{
				pData->OnBeforeSend(gateConn.getPtr());
				gateConn->SendToGate(pData->mTargetID, pData);
			}
			else
				DEBUG_LOG("No exist gate %d : %lld", pData->mTargetID.type, pData->mTargetID.id);
		}

		virtual void OnReceive(NodePacket *pData)
		{		
			// ID==0, �㲥��Ϣ
			if (pData->mbBroadcast)
			{
				BroadCastMsg(pData);
				return;
			}

			// ���������ת��, ��������ʱ����, ���Ȼ�����Ϣ
			EasyList<HandPacket> *p = mJumpMsgCacheHash.findPtr(pData->mTargetID);
			if (p != NULL)
			{
				p->push_back(pData);
				return;
			}

			ANodeConnect nodeConn = FindNodeConnect(pData->mTargetID);
			if (nodeConn)
			{				
				if (pData->mSendCount > 10)
				{
					ERROR_LOG("%d Append send times > 10, then give up", pData->GetPacketID());
					return;
				}
				pData->OnBeforeSend(nodeConn.getPtr());
				nodeConn->SendToNode(pData->mTargetID, pData);
			}
			else
				SendToGate(pData);
		}

		// ��ת���߼�
		virtual void OnReqeustJumpNode(UnitID id)
		{
			DEBUG_LOG("====== %lld type %d begin jump", id.id, id.type);					

			if (!mJumpMsgCacheHash.exist(id))
				mJumpMsgCacheHash.insert(id, MsgDataList());
		}
		virtual void OnJumpFinish(UnitID id)
		{
			MsgDataList *pList = mJumpMsgCacheHash.findPtr(id);
			if (pList != NULL)
			{
				DEBUG_LOG("<<<<<< %lld type %d finish jump, now send %d msg", id.id, id.type, pList->size());
				for (auto it = pList->begin(); it; ++it)
				{
					Auto<NodePacket> d = *it;
					if (d)
						SendToNode(d.getPtr());
				}
				mJumpMsgCacheHash.erase(id);
			}
		}

	protected:
		typedef EasyList<HandPacket> MsgDataList;
		EasyHash<UnitID, MsgDataList>		mJumpMsgCacheHash;
		EasyHash<UnitID, ANodeConnect>					mEntityList;
	};
	typedef Hand<tGate> AGate;
}


#endif //_INLCUDE_GATE_H_
