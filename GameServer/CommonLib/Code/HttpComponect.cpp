#include "HttpComponect.h"
#include "Actor.h"
#include "ServerClientMsg.h"

void HttpComponect::Start()
{
	if (mCertFile.length() > 0)
		mHttpNet = MEM_NEW CompHttpsWebNet<true>(this);
	else
		mHttpNet = MEM_NEW CompHttpsWebNet<false>(this);
	Hand<BaseWebServer> net = mHttpNet;
	net->StartBytesWeb(mPort, mKeyFile, mCertFile, mPassword);

	net->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<HttpReqeustActorMsg, eMsg_ClientRequestServer>());
	
}

void HttpComponect::OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress)
{
	GREEN_LOG("Now receive http request <%s>: %s", requestAddress.c_str(), requestData.c_str());
	if (mpActor!=NULL)
		mpActor->ResponseHttp(requestData, response, bPost, requestAddress);
	
}

void HttpComponect::OnResponseBytes(HandPacket requestMsg, DataBuffer &response, const AString &requestAddress)
{
	if (mpActor != NULL)
		mpActor->ResponseBytesHttp(requestMsg, response, requestAddress);
}
