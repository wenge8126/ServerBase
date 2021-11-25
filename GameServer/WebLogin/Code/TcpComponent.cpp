#include "TcpComponent.h"



HandConnect TcpComponent::CreateConnect()
{
	return MEM_NEW TcpComponentConnect();
}

void TcpComponent::Start()
{
	mTcpNet = MEM_NEW TcpComponentNet(this, mMaxConnectCount, mMaxThreadCount);
	if (mServerIp.empty() || mServerPort <= 0)
	{
		ERROR_LOG("Error ip or port [%s : %d]", mServerIp.c_str(), mServerPort);
		return;
	}

	RegisterMsg(mTcpNet->GetEventCenter());

	if (!mTcpNet->StartNet(mServerIp.c_str(), mServerPort))
	{
		ERROR_LOG("Server start fail : [%s:%d]", mServerIp.c_str(), mServerPort);
	}
}

bool SocketComponent::OnConnected(HandConnect connect)
{
	LOG("Connected [%s:%d]", connect->GetIp(), connect->GetPort());
	Hand<NetWorkerComponent> comp = GetNetWorkerComponent();
	if (comp)
		return comp->OnConnected(connect);

	return true;
}
