#include "TcpComponent.h"



HandConnect TcpComponent::CreateConnect()
{
	return MEM_NEW TcpComponentConnect();
}

void TcpComponent::Awake()
{
	mTcpNet = MEM_NEW TcpComponentNet(this, mMaxConnectCount, mMaxThreadCount);
}

void TcpComponent::Start()
{
	
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

void TcpComponent::OnDestory()
{
	//Hand<NetWorkerComponent> comp = GetNetWorkerComponent();
	//if (comp)
	//{
	//	FastHash<Int64, HandConnect> temp;
	//	temp.swap(comp->mConnectList);
	//	for (auto it = temp.begin(); it; ++it)
	//	{
	//		it.get()._free();
	//	}
	//}
	mTcpNet->StopNet();
	mTcpNet._free();
}

bool SocketComponent::OnConnected(HandConnect connect)
{
	LOG("Connected [%s:%d]", connect->GetIp(), connect->GetPort());
	Hand<NetWorkerComponent> comp = GetNetWorkerComponent();
	if (comp)
		return comp->OnConnected(connect);

	return true;
}
