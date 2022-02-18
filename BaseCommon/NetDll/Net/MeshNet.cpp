#include "MeshNet.h"



MeshNet::MeshNet(size_t maxConnectCount /*= NET_CONNECT_MAX*/, int threadNum /*= _IOCP_THREAD_NUM*/) : DefaultServerNet(maxConnectCount, threadNum)
{
	SetNetProtocol(MEM_NEW MeshNetProtocol());
	mNodeClientNet = MEM_NEW MeshClientNet(this);
	mNodeClientNet->SetNetProtocol(MEM_NEW MeshClientNetProtocol());
}
