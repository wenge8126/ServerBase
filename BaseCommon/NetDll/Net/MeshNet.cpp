#include "MeshNet.h"
#include "EventProtocol.h"
#include "DefineMsgFactory.h"
//-------------------------------------------------------------------------
class MeshClientNet : public IOCPClientSetNet
{
public:
	MeshClientNet(MeshNet *pOwnerNet, size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM)
		: IOCPClientSetNet(maxConnectCount, threadNum)
		, mpOwnerNet(pOwnerNet)
	{
	}

public:
	virtual Logic::tEventCenter* GetEventCenter(void) const override { return NULL; }

	virtual int GetSafeCode() override { return mpOwnerNet->GetSafeCode(); }

	class NodeConnect : public IOCPConnect
	{
	public:
		~NodeConnect()
		{
			mAttachData._free();
		}


	};

	virtual HandConnect CreateConnect() override
	{
		HandConnect conn = MEM_NEW NodeConnect();

		return conn;
	}
	virtual void OnConnected(tNetConnect *pConnect) override
	{

	}

	virtual void OnConnectFail(const char *szIp, int nPort) override
	{

	}

	virtual void OnCloseConnect(tNetConnect *pConnect) override
	{
		if (!mpOwnerNet->mbClose)
		{
			CoroutineTool::AsyncCall(
				[=]()
			{
				tTimer::AWaitTime(100);
				while (!mpOwnerNet->mbClose)
				{
					HandConnect nodeConnect = AwaitConnect(pConnect->GetIp(), pConnect->GetPort(), 3000);
					if (nodeConnect)
						break;
				}
			}
			);
		}
	}

public:
	MeshNet *mpOwnerNet;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MeshNet::MeshNet(int nCode, size_t maxConnectCount /*= NET_CONNECT_MAX*/, int threadNum /*= _IOCP_THREAD_NUM*/) 
	: IOCPServerNet(maxConnectCount, threadNum)
	, mCode(nCode)
{
	mNodeClientNet = MEM_NEW MeshClientNet(this);
	
	mServerProcess = MEM_NEW MeshNetProcess();
	mClientNetProcess = MEM_NEW MeshClientNetProcess();

	GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultReqeustMsgFactory<eMeshMsg_RequestNodeInfo, RQ_RequestMeshInfo, RS_MeshNodeInfo, MeshNetProcess>(mServerProcess.getPtr()));
}
//-------------------------------------------------------------------------
HandConnect MeshNet::AwaitConnectNode(const char *szIp, int nPort, int overmilSecond)
{
	HandConnect nodeConnect = mNodeClientNet->AwaitConnect(szIp, nPort, overmilSecond);
	if (nodeConnect)
	{
		RQ_RequestMeshInfo req;
		req.mNodeAddr = mKey;
		req.mNodeCode = mCode;
		AutoNice respData = mClientNetProcess->Await(nodeConnect.getPtr(), eMeshMsg_RequestNodeInfo, req, 6000);
		if (respData)
		{
			RS_MeshNodeInfo  info;
			info.Full(respData);

			NetAddress targetAddr(szIp, nPort);
			if (!mServerNodeList.exist((UInt64)targetAddr))
			{
				AConnectData d = MEM_NEW MeshConnectData();
				d->mpConnect = nodeConnect.getPtr();
				d->mNodeKey = targetAddr;
				d->mNodeCode = info.mTargetNodeCode;
				nodeConnect->SetUserData(d);
				mServerNodeList.insert(targetAddr, d);
				OnConnectNode(d);
			}

			for (int i = 0; i < info.mNodeList.size(); ++i)
			{
				NetAddress addr(info.mNodeList[i]);

				AConnectData exsitNode = mServerNodeList.find(addr);
				if (!exsitNode)
				{
					AwaitConnectNode(addr.Ip().c_str(), addr.GetPort(), 6000);
				}
			}
			Dump();
			return nodeConnect;
		}

	}
	return HandConnect();
}

void MeshNet::Dump()
{
	NOTE_LOG("-------------------------------------------------------");
	for (auto it = mServerNodeList.begin(); it; ++it)
	{
		AConnectData d = it.get();
		if (d)
		{
			NetAddress addr(d->mNodeKey);
			Hand< MeshServerConnect> conn = d->mpConnect;
			if (conn)
			{
				NOTE_LOG("THIS [%llu : %s:%d] <<< [%llu : %s:%d]", mKey, mIp.c_str(), mPort, it.get(), addr.Ip().c_str(), addr.GetPort());
			}
			else if (d->mpConnect != NULL)
			{
				NOTE_LOG("THIS [%llu : %s:%d] >>> [%llu : %s:%d]", mKey, mIp.c_str(), mPort, it.get(), addr.Ip().c_str(), addr.GetPort());
			}
		}
	}
	NOTE_LOG("-------------------------------------------------------");
}

//-------------------------------------------------------------------------
void MeshNetProcess::On(tNetConnect *pConnect, RQ_RequestMeshInfo &req, RS_MeshNodeInfo &info)
{
	// 更新当前服务器Node列表
	Hand<MeshNet::MeshConnectData> d = MEM_NEW MeshNet::MeshConnectData();
	d->mpConnect = pConnect;
	d->mNodeKey = req.mNodeAddr;
	d->mNodeCode = req.mNodeCode;
	pConnect->SetUserData(d);
	MeshNet *pNet = GetNet(pConnect);
	pNet->mServerNodeList.insert(req.mNodeAddr, d);
	pNet->Dump();
	pNet->OnConnectNode(d);
	// 返回所有列表
	//RS_MeshNodeInfo  info;

	for (auto it = pNet->mServerNodeList.begin(); it; ++it)
	{
		info.mNodeList.add(it.key());
	}
	//Auto< ResponseMsgPacket> resp = CreatePacket(PACKET_RESPONSE_MSG);
	//resp->mRequestID = req->mRequestID;
	//info.serialize(&resp->mData);
	//pConnect->Send(resp.getPtr(), false);
	//pNet->mServerNodeList.insert(req->mNodeAddr, d);
}
//-------------------------------------------------------------------------

MeshNet* MeshClientNetProcess::GetNet(tNetConnect *pConnect)
{
	return (dynamic_cast<MeshClientNet*>(pConnect->GetNetHandle()))->mpOwnerNet;
}
