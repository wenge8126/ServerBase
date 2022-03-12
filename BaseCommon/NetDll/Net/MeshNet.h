
#ifndef _INCLUDE_MESHNET_H_
#define _INCLUDE_MESHNET_H_

#pragma once

#include "IOCPClientSetNet.h"
#include "IOCPServerNet.h"
#include "MeshNetMsg.h"
#include "CoroutineTool.h"
#include "NetAddress.h"
#include "NetProcess.h"
#include "Timer.h"


class Net_Export MeshNet : public IOCPServerNet
{
	friend class MeshNetProcess;
	friend class MeshClientNet;
	friend class AsyncNode;
	friend class AsyncGate;

public:
	//-------------------------------------------------------------------------
	class MeshConnectData : public AnyData
	{
	public:
		bool IsValid() { return mpConnect != NULL && !mpConnect->IsDisconnect() && !mbIsClose; }

	public:
		tNetConnect		*mpConnect = NULL; 
		UInt64					mNodeKey = 0;
		int						mNodeCode;
		bool						mbIsClose = false;
	};
	typedef Hand<MeshConnectData> AConnectData;
	//-------------------------------------------------------------------------

public:
	MeshNet(int nCode, size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM);
	~MeshNet()
	{
		mNodeClientNet._free();
	}

public:
	virtual void OnConnectNode(AConnectData nodeData){ }
	virtual void OnNodeClose(AConnectData closeNode) {}

	void ConnectNode(const char *szIp, int nPort, int overmilSecond)
	{
		CoroutineTool::Async(
			[=]()
		{
			AwaitConnectNode(szIp, nPort, overmilSecond);
		}
		);
	}

	virtual HandConnect AwaitConnectNode(const char *szIp, int nPort, int overmilSecond);

	virtual bool StartNet(const char *szIP, int nPort) override
	{
		NetAddress addr(szIP, nPort);
		mKey = addr;

		return IOCPServerNet::StartNet(szIP, nPort);
	}

	virtual void StopNet() override
	{
		mbClose = true;
		IOCPServerNet::StopNet();
		mNodeClientNet->StopNet();
		mServerNodeList.clear();
	}

	virtual void Process() override
	{
		IOCPServerNet::Process();
		mNodeClientNet->Process();
	}

	virtual void LowProcess(int spaceTime) override 
	{
		IOCPServerNet::LowProcess(spaceTime);
		mNodeClientNet->LowProcess(spaceTime);
	}

	virtual void _OnConnectStart(tNetConnect *pConnect) override {}

	class MeshServerConnect : public IOCPServerConnect
	{
	public:
		~MeshServerConnect()
		{
			mAttachData._free();
		}
	};

	virtual HandConnect CreateConnect() override
	{
		return MEM_NEW MeshServerConnect();
	}

	virtual int GetSafeCode() override { return mSafeCode; }

	virtual Logic::tEventCenter* GetEventCenter(void) const { return NULL; }

	void Dump();

	FastHash<UInt64, AConnectData> & GetMeshNodeList() { return mServerNodeList; }

	UInt64 GetKey() const { return mKey; }

protected:
	Hand<IOCPClientSetNet>								mNodeClientNet;
	FastHash<UInt64, AConnectData>					mServerNodeList;

	Hand<tNetProcess>		mNetProcess;
	
	UInt64			mKey;
	bool				mbClose = false;

public:
	int					mSafeCode = 0;
	int					mCode = 0;
};

//-------------------------------------------------------------------------

class Net_Export MeshNetProcess : public tNetProcess
{
public:
	virtual MeshNet*	GetNet()
	{
		return mpNet;
	}

	virtual void On(tNetConnect *pConnect, RQ_RequestMeshInfo &req, RS_MeshNodeInfo &info);

	virtual void On(tNetConnect *pConnect, MS_BroadcastNodeClose &msg)
	{
		MeshNet::AConnectData connData = pConnect->GetUserData();
		if (connData)
		{
			connData->mbIsClose = true;
			auto existData = mpNet->mServerNodeList.find(connData->mNodeKey);
			if (connData == existData)
			{
				mpNet->OnNodeClose(existData);
				mpNet->mServerNodeList.erase(connData->mNodeCode);
			}
			else if (!existData && mpNet->mServerNodeList.exist(connData->mNodeKey))
				mpNet->mServerNodeList.erase(connData->mNodeCode);
		}
	}

public:
	MeshNetProcess(MeshNet *pNet)
		: mpNet(pNet) {}

public:
	MeshNet *mpNet;
};
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

#endif
