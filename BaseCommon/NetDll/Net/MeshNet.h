
#ifndef _INCLUDE_MESHNET_H_
#define _INCLUDE_MESHNET_H_

#pragma once

#include "IOCPClientSetNet.h"
#include "IOCPServerNet.h"
#include "MeshNetMsg.h"
#include "CoroutineTool.h"
#include "NetAddress.h"
#include "NetProcess.h"

enum EMeshNetMsgType
{
	eMeshMsg_RequestNodeInfo = PACKET_MAX + 1,

};

class MeshNet : public DefaultServerNet
{
	friend class MeshNetProcess;
	friend class MeshClientNetProcess;

public:
	class MeshConnectData : public AnyData
	{
	public:
		tNetConnect		*mpConnect = NULL; 
		UInt64				mNodeKey = 0;
	};
	typedef Hand<MeshConnectData> AConnectData;

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

public:
	MeshNet(size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM);

public:
	void ConnectNode(const char *szIp, int nPort, int overmilSecond)
	{
		CoroutineTool::Async(
			[=]()
		{
			AwaitConnectNode(szIp, nPort, overmilSecond);
		}
		);
	}

	virtual bool AwaitConnectNode(const char *szIp, int nPort, int overmilSecond)
	{
		HandConnect nodeConnect = mNodeClientNet->AwaitConnect(szIp, nPort, overmilSecond);
		if (nodeConnect)
		{
			NetAddress targetAddr(szIp, nPort);
			if (!mServerNodeList.exist((UInt64)targetAddr))
			{
				AConnectData d = MEM_NEW MeshConnectData();
				d->mpConnect = nodeConnect.getPtr();
				nodeConnect->SetUserData(d);
				mServerNodeList.insert(targetAddr, d);

				RQ_RequestMeshInfo req;
				req.mNodeAddr = mKey;
				AutoNice respData = mClientNetProcess->Await(nodeConnect.getPtr(), eMeshMsg_RequestNodeInfo, req, 6000);
				if (respData)
				{
					RS_MeshNodeInfo  info;
					info.Full(respData);

					for (int i = 0; i < info.mNodeList.size(); ++i)
					{
						NetAddress addr(info.mNodeList[i]);

						AConnectData exsitNode = mServerNodeList.find(addr);
						if (!exsitNode)
						{
							AwaitConnectNode(addr.Ip().c_str(), addr.GetPort(), 6000);
						}
					}

					return true;
				}
			}
		}
		return false;
	}

	virtual void StopNet() override
	{
		mbClose = true;
		DefaultServerNet::StopNet();
	}

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

protected:
	Hand<MeshClientNet>								mNodeClientNet;
	FastHash<UInt64, AConnectData>			mServerNodeList;

	Hand<tNetProcess>		mServerProcess;
	Hand<tNetProcess>		mClientNetProcess;
	
	UInt64			mKey;
	bool				mbClose = false;

public:
	int					mSafeCode = 0;
};



class MeshNetProcess : public tNetProcess
{
public:
	MeshNet*			GetNet(tNetConnect *pConnect)
	{
		return dynamic_cast<MeshNet*>(pConnect->GetNetHandle());
	}

	virtual void On(tNetConnect *pConnect, RQ_RequestMeshInfo &req, RS_MeshNodeInfo &info)
	{
			// 更新当前服务器Node列表
			Hand<MeshNet::MeshConnectData> d = MEM_NEW MeshNet::MeshConnectData();
			d->mpConnect = pConnect;
			d->mNodeKey = req.mNodeAddr;
			pConnect->SetUserData(d);
			MeshNet *pNet = GetNet(pConnect);

			// 返回所有列表
			//RS_MeshNodeInfo  info;

			for (auto it=pNet->mServerNodeList.begin(); it; ++it)
			{
				info.mNodeList.add(it.key());
			}
			//Auto< ResponseMsgPacket> resp = CreatePacket(PACKET_RESPONSE_MSG);
			//resp->mRequestID = req->mRequestID;
			//info.serialize(&resp->mData);
			//pConnect->Send(resp.getPtr(), false);
			//pNet->mServerNodeList.insert(req->mNodeAddr, d);
	}
};

class MeshClientNetProcess : public tNetProcess
{
public:
	MeshNet*			GetNet(tNetConnect *pConnect)
	{
		return (dynamic_cast<MeshNet::MeshClientNet*>(pConnect->GetNetHandle()))->mpOwnerNet;
	}


};


#endif
