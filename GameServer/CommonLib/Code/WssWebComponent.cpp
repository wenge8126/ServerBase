#include "WssWebComponent.h"


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void WssWebComponent::Start()
{
	if (mCertFile.length() > 0)
		mWebNet = MEM_NEW WssWebNet<true>(this);
	else
		mWebNet = MEM_NEW WssWebNet<false>(this);
	Hand<BaseWebServer> net = mWebNet;
	net->StartNet(mServerIp, mServerPort, mKeyFile, mCertFile, mPassword);
}

void WssWebComponent::LowUpdate()
{
	mWebNet->LowProcess(1);
	//Hand<NetWorkerComponent> comp = GetNetWorkerComponent();
	//if (comp)
	//{
	//	for (auto it = comp->mConnectList.begin(); it; ++it)
	//	{
	//		Hand< BaseWebConnect> connect = it.get();
	//		connect->ProcessPing();
	//	}
	//}
}

void WssWebComponent::OnDestory()
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
	mWebNet->StopNet();
	mWebNet._free();
}
