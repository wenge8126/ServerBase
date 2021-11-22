
#include "CWssServerNet.h"
#include "WebMsg.h"
#include "EventFactory.h"
//#include "BigMsgEvent.h"
#include "CommonDefine.h"
#include "WebLoginThread.h"

template<bool bSSL>
CWssServerNet<bSSL>::CWssServerNet(WebLoginThread *pThread) : mpLoginThread(pThread)
{
	WebMsg::Register(this);
	Base::RegisterMsg(WEB_MSG_RESPONSE_MSG, MEM_NEW ResponseEventFactory());
	//Base::RegisterMsg(WEB_SEND_BIG_MSG, MEM_NEW Logic::EventFactory<WEB_SendDataPartEvent>());
	//Base::RegisterMsg(WEB_REV_BIG_MSG, MEM_NEW Logic::EventFactory<WEB_RevDataPartEvent>());
	//REGISTER_EVENT(Base::GetEventCenter(), WEB_SendDataPartEvent);
}

template<bool bSSL>
bool CWssServerNet<bSSL>::SendToClient(Int64 playerID, Logic::tEvent *pEvent)
{

	HandConnect conn = Base::mConnectHash.find(playerID);
	if (conn)
		return conn->SendEvent(pEvent);
	DEBUG_LOG("%llu  player no exist, send fail >%s", (UInt64)playerID, pEvent->GetEventName());

	return false;
}

template<bool bSSL>
void CWssServerNet<bSSL>::BroatcastMsg(Logic::tEvent *pMsg, Int64 excludeID)
{

	for (auto it = Base::mConnectHash.begin(); it; ++it)
	{
		Hand<WebPlayer<bSSL>> conn = it.get();
		if (conn && !conn->IsRemove() && conn->mDBID!=excludeID)
			conn->SendEvent(pMsg);
	}

}

template<bool bSSL>
void CWssServerNet<bSSL>::InitClearAllPlayer()
{
	for (auto it = Base::mConnectHash.begin(); it; ++it)
	{
		Hand<BaseWebConnect> conn = it.get();
		if (conn)
			conn->mDBID = 0;
	}
	Base::mConnectHash.clear(false);
}

//#include "CloudDBCall.h"

extern void Analysis(NiceData &msg, const AString &requestData);


template<bool bSSL>
void CWssServerNet<bSSL>::OnResponse(const AString &requestData, std::string_view &response, bool bPost, const AString &requestAddress)
{
	//NiceData msg;
	//NiceData res;

	//Analysis(msg, requestData);
	//NOTE_LOG("==========================================");
	//NOTE_LOG("==========================================");
	//NOTE_LOG("========================================== %s", requestData.c_str());
	//if (bPost)
	//{
	//	//PostRecharge(msg, res);
	//}
	//else
	//{
	//	int nResult = 0;
	//	mpLoginThread->mDBUnit->AwaitRequestDB("t_cmd", msg["server_id"], "read_cmd_table", &msg, 10000, &nResult);
	//	res['code'] = nResult == 0 ? 1 : 0;
	//}
	//response = res.ToJSON();
}



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
template<bool bSSL>
void HttpsWebNet<bSSL>::OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress)
{
	//NOTE_LOG("Http request >%s", requestData.c_str());
	// 获取服务器列表
	if (requestData != "ReqeustServerList")
	{ 
		response = "{'result':'None'}";
		return;
	}

	response = CWssServerNet<bSSL>::mpLoginThread->mServerListJson;
	
}

//-------------------------------------------------------------------------
