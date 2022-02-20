#include "MeshNet.h"
#include "EventProtocol.h"
#include "DefineNetFactory.h"

MeshNet::MeshNet(size_t maxConnectCount /*= NET_CONNECT_MAX*/, int threadNum /*= _IOCP_THREAD_NUM*/) : DefaultServerNet(maxConnectCount, threadNum)
{
	mNodeClientNet = MEM_NEW MeshClientNet(this);
	
	mServerProcess = MEM_NEW MeshNetProcess();
	mClientNetProcess = MEM_NEW MeshClientNetProcess();

	GetNetProtocol()->RegisterNetPacket(MEM_NEW DefaultNetReqeustFactory<eMeshMsg_RequestNodeInfo, RQ_RequestMeshInfo, RS_MeshNodeInfo, MeshNetProcess>(mServerProcess.getPtr()));
}
