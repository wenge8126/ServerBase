#include "TcpComponent.h"



HandConnect TcpComponent::CreateConnect()
{
	return MEM_NEW ComponentConnect();
}

void TcpComponent::Start()
{
	mTcpNet = MEM_NEW ComponentNet(this, mMaxConnectCount, mMaxThreadCount);
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
