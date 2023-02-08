#include "WssWebComponent.h"
#include "CommonDefine.h"
#include "WebMsgEvent.h"

void WssWebComponent::RegisterMsg(int msgIndex, AutoEventFactory msgFactory)
{
	Hand< WebNetEventCenter> center = mWebNet->GetEventCenter();
	center->RegisterMsg(msgIndex, msgFactory);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void WssWebComponent::Start()
{
	if (mCertFile.length() > 0)
		mWebNet = MEM_NEW WssWebNet<true>(this);
	else
		mWebNet = MEM_NEW WssWebNet<false>(this);
	Hand<BaseWebServer> net = mWebNet;
	if (net->StartNet(mServerIp, mServerPort, mKeyFile, mCertFile, mPassword))
		NOTE_LOG("成功启动Wss 网络服务 : %s://%s:%d", mCertFile.length() > 0 ? "Wss" : "Ws", mServerIp.c_str(), mServerPort)
	else
		ERROR_LOG("启动失败Wss 网络服务 : %s://%s:%d", mCertFile.length() > 0 ? "Wss" : "Ws", mServerIp.c_str(), mServerPort);

	RegisterMsg(CL_RequestActorMsg, MEM_NEW Logic::EventFactory<WssReqeustActorMsgEvent>());

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
