#include "WssServerNet.h"
#include "EventFactory.h"
#include <Windows.h>
//-------------------------------------------------------------------------
#using "WssNet.dll"

using namespace Fleck;
using namespace System::Runtime::InteropServices;

#define  CSSTR(csStr)       (const char*)(Marshal::StringToHGlobalAnsi(csStr)).ToPointer()
//-------------------------------------------------------------------------

Fleck::WebSocketConnection^  GetWssConnect(int connID)
{
	return Fleck::WssServerNet::GetWssConnect(connID);
}

void tWssConnect::SetRemove(bool bNeedRemove)
{
	if (bNeedRemove)
	{
		Fleck::WebSocketConnection^  conn = GetWssConnect(GetNetID());
		if (conn != nullptr)
			conn->Close();
	}
}

tNetHandle* tWssConnect::GetNetHandle()
{
	return &tWssServerNet::GetMe();
}

bool tWssConnect::SendMsgData(DataStream *pSendMsgData, int frameType)
{
	if (IsRemove())
		return false;
	Fleck::WebSocketConnection^  conn = GetWssConnect(GetNetID());
	if (conn != nullptr)
	{
		BaseLib::DataBuffer  ^sendBuffer = conn->mSendBuffer;
		if (pSendMsgData->dataSize() > sendBuffer->size())
			sendBuffer->_resize(pSendMsgData->dataSize());
		pin_ptr<unsigned char> ipi = &sendBuffer->mData[0];
		memcpy(ipi, pSendMsgData->data(), pSendMsgData->dataSize());
		conn->SendDataBuffer(pSendMsgData->dataSize());
		return true;
	}
	return false;
}

bool tWssConnect::SendEvent(Logic::tEvent *pEvent)
{
	mSendMsgBuffer.clear(false);
	bool b = pEvent->GetEventCenter()->SerializeMsg(pEvent->GetSelf(), &mSendMsgBuffer);
	if (b)
		return SendMsgData(&mSendMsgBuffer, WS_BINARY_FRAME);
	return false;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
tWssServerNet    *tWssServerNet::msCWssServerNet = NULL;
//-------------------------------------------------------------------------
namespace _WssTool
{
	AString GetIPInfo(Fleck::WebSocketConnection ^conn)
	{
		AString str;
		str.Format("[%s:%d]", CSSTR(conn->Socket->RemoteIpAddress), conn->Socket->RemotePort);
		return str;

	}

	void OnAcceptConnect(Fleck::WebSocketConnection ^conn);

	void OnProcessMsg(Fleck::WebSocketConnection ^conn, int connectID, System::String ^msg)
	{
		NOTE_LOG("Thread %u >OnProcessMsg %s >%s", ::GetCurrentThreadId(), GetIPInfo(conn).c_str(), CSSTR(msg));
	}

	void OnProcessRevData(Fleck::WebSocketConnection ^conn, int connectID, cli::array<byte, 1> ^revData, int dataSize);

	void OnConnectClose(Fleck::WebSocketConnection ^conn, int connectID)
	{
		NOTE_LOG("Thread %u >OnConnectClose %s", ::GetCurrentThreadId(), GetIPInfo(conn).c_str());
		tWssServerNet::GetMe().mConnectMapLock.lock();
		HandWssConnect &player = tWssServerNet::GetMe().mWssPlayerMap.Find(connectID);
		if (player)
			player->SetRemove(false);
		tWssServerNet::GetMe().mConnectMapLock.unlock();

	}

	void OnConnectError(Fleck::WebSocketConnection ^conn, int connectID, System::String  ^err)
	{
		ERROR_LOG("Thread %u >OnConnectError %s >%s, now close", ::GetCurrentThreadId(), GetIPInfo(conn).c_str(), CSSTR(err));
		HandWssConnect &player = tWssServerNet::GetMe().mWssPlayerMap.Find(connectID);
		if (player)
			player->SetRemove(false);
		conn->Close();
	}

}
//-------------------------------------------------------------------------
tWssServerNet::tWssServerNet() : tBaseEventNet()
{
	AssertNote(msCWssServerNet == NULL, "当前已经存在 CWssServerNet");
	msCWssServerNet = this;

	SetNetProtocol(MEM_NEW WebNetProtocal());
	mWebEventCenter = MEM_NEW WebNetEventCenter();
	mWebEventCenter->_SetNetTool(0, GetSelf());


}

bool tWssServerNet::StartNet(const AString &wssServerIP, int serverPort, bool bWss, const AString pfxFileName /*= ""*/, const AString pfxPassword /*= ""*/)
{
	_freeWssNet();
	if (bWss && (pfxFileName.length() <= 0 || pfxPassword.length() <= 0))
	{
		ERROR_LOG("WSS must need pfxFile and password");
		return false;
	}
	mIP = wssServerIP;
	mPort = serverPort;
	AString url;
	//Marshal::StringToHGlobalAnsi(url).ToPointer();
	url.Format("%s://%s:%d", bWss ? "wss" : "ws", wssServerIP.c_str(), serverPort);
	Fleck::WssServerNet ^mWssNet = Fleck::WssServerNet::CreateWssNet(gcnew System::String(url.c_str()));
	if (bWss)
	{
		if (!mWssNet->SetSSLPfxFile(gcnew System::String(pfxFileName.c_str()), gcnew System::String(pfxPassword.c_str())))
		{
			ERROR_LOG("Init set [wss://%s:%d] ssl pfx file SetSSLPfxFile(%s, %s) fail", wssServerIP.c_str(), serverPort, pfxFileName.c_str(), pfxPassword.c_str());
			return false;
		}
	}
	mWssNet->mOnAcceptConnect = gcnew Fleck::OnAcceptConnect(_WssTool::OnAcceptConnect);
	mWssNet->mProcessMsg = gcnew Fleck::ProcessMsg(_WssTool::OnProcessMsg);
	mWssNet->mReceiveFun = gcnew Fleck::ProcessReceive(_WssTool::OnProcessRevData);
	mWssNet->mOnConnectClose = gcnew Fleck::OnConnectClose(_WssTool::OnConnectClose);
	mWssNet->mOnConnectError = gcnew Fleck::OnConnectError(_WssTool::OnConnectError);
	mWssNet->Start();
	bool bResult = true;
	NOTE_LOG("Thread %u init wss net %s %s", ::GetCurrentThreadId(), url.c_str(), bResult ? "succeed" : "fail");
	return true;
}
namespace _WssTool
{
	void OnAcceptConnect(Fleck::WebSocketConnection ^conn)
	{
		//AString ipInfo = GetIPInfo(conn);
		//printf("Thread %u >OnAcceptConnect %s\r\n", ::GetCurrentThreadId(), ipInfo.c_str());

		tWssServerNet::GetMe().mConnectMapLock.lock();
		if (conn->mID >= 0 || tWssServerNet::GetMe().mWssPlayerMap.Find(conn->mID))
		{
			tWssServerNet::GetMe().mConnectMapLock.unlock();
			return;
		}
		auto &playerList = tWssServerNet::GetMe().mWssPlayerMap;
		bool bAppend = false;
		int count = playerList.size();
		for (int i = 0; i < playerList.size(); ++i)
		{
			HandWssConnect p = playerList[i];
			if (!p)
			{
				conn->mID = i;
				bAppend = true;
			}
		}
		if (!bAppend)
		{
			conn->mID = count;
		}
		HandWssConnect player = tWssServerNet::GetMe().CreateConnect();
		player->Init(&tWssServerNet::GetMe(), CSSTR(conn->Socket->RemoteIpAddress), conn->Socket->RemotePort);
		playerList.Append(conn->mID, player);
		tWssServerNet::GetMe().mConnectMapLock.unlock();
		printf("*** Accept connect %d: %s\r\n", conn->mID, GetIPInfo(conn).c_str());
		player->SetNetID(conn->mID);
		player->OnConnected();
	}

	void OnProcessRevData(Fleck::WebSocketConnection ^conn, int connectID, cli::array<byte, 1> ^revData, int dataSize)
	{
		//printf("Thread %u >OnProcessRevData %d: %s, rev %d\r\n", ::GetCurrentThreadId(), connectID, GetIPInfo(conn).c_str(), dataSize);
		tWssServerNet::GetMe().mConnectMapLock.lock();
		HandWssConnect &player = tWssServerNet::GetMe().mWssPlayerMap.Find(connectID);
		tWssServerNet::GetMe().mConnectMapLock.unlock();

		if (revData->Length <= 0)
			return;

		if (player && !player->IsRemove())
		{
			pin_ptr<unsigned char> ipi = &revData[0];
			if (dataSize > 0xFFFF - WSS_MSG_HEAD_SIZE || dataSize >= player->mWssReceiveData.size() - WSS_MSG_HEAD_SIZE)
			{
				ERROR_LOG("Wss msg %d size too long, %d(with msg head +3) more then 0xFFFF or max buffer length %d", (int)*ipi, dataSize + WSS_MSG_HEAD_SIZE, player->mWssReceiveData.size());
				return;
			}
			ushort length = (ushort)(dataSize + sizeof(ushort));
			player->mRevDataLock.lock();
			player->mWssReceiveData.SendData((char*)&length, sizeof(ushort));
			player->mWssReceiveData.SendData((char*)ipi, dataSize);
			player->mRevDataLock.unlock();
		}
		else
		{
			WARN_LOG("===No find player %d, now list count %d, now try append", connectID, (int)tWssServerNet::GetMe().mWssPlayerMap.size());
			OnAcceptConnect(conn);
			tWssServerNet::GetMe().mConnectMapLock.lock();
			HandWssConnect &player2 = tWssServerNet::GetMe().mWssPlayerMap.Find(connectID);
			tWssServerNet::GetMe().mConnectMapLock.unlock();
			if (player2)
				OnProcessRevData(conn, connectID, revData, dataSize);
			else
				ERROR_LOG("Try append player fail >%d : %s", conn->mID, GetIPInfo(conn).c_str());
		}
	}
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------