#include "WebServerNet.h"
#include "WebProtocol.h"
#include "App.h"
#include "TimeManager.h"
#include "uv.h"
#include "libuv.h"
#include "CoroutineTool.h"
#include "Timer.h"
#include "NetAddress.h"

using namespace uWS;

/* We simply call the root header file "App.h", giving you uWS::App and uWS::SSLApp */

/* This is a simple WebSocket echo server example.
 * You may compile it with "WITH_OPENSSL=1 make" or with "make" */


//typedef TemplatedApp<true>::WebSocketBehavior  WssBehavior;
//typedef TemplatedApp<false>::WebSocketBehavior  WsBehavior;


struct ws_option : public us_socket_context_options_t {

	ws_option() {}
	ws_option(
		const char *szkey_file_name,
		const char *szcert_file_name,
		const char *szpassphrase
	)
	{
		key_file_name = szkey_file_name;
		cert_file_name = szcert_file_name;
		passphrase = szpassphrase;
		dh_params_file_name = nullptr;
		ca_file_name = nullptr;
	}
};


namespace uWS
{
	AString GetString(std::string_view str)
	{
		AString x;
		if (str.size()>0)
			x.set(str.data(), str.size());
		
		return x;
	}



	/* ws->getUserData returns one of these */
	struct PerSocketData {
		/* Fill with user data */
		HandConnect mConnect;
	};

	template<bool bUSE_SSL>
	HandConnect tWssServerNet<bUSE_SSL>::CreateConnect()
	{
		return HandConnect();
	}

	//-------------------------------------------------------------------------
	template<bool bUSE_SSL>
	tWssServerNet<bUSE_SSL>::tWssServerNet()
		: BaseWebServer()
		, mpWsApp(NULL)
		, mpLoop(NULL)
		, mpListen(NULL)
		, mpHttpApp(NULL)
		, mpHttpListen(NULL)
	{
		SetNetProtocol(MEM_NEW WebNetProtocal());
		mWebEventCenter = MEM_NEW WebNetEventCenter();
		mWebEventCenter->_SetNetTool(0, GetSelf());

		mIP = "Test ok";
	}

	template<bool bUSE_SSL>
	tWssServerNet<bUSE_SSL>::~tWssServerNet()
	{
		_stopNet();
		if (mpWsApp != NULL)
			delete mpWsApp;
		mpWsApp = NULL;
		if (mpHttpApp != NULL)
		{
			delete mpHttpApp;
			mpHttpApp = NULL;
		}
		mpLoop = NULL;
	}

	template<bool bUSE_SSL>
	class AsyncWebTool : public AutoBase
	{
	public:
		void AsyncResponse()
		{
			std::string_view response;
			mpNet->OnResponse(mRequestString, response, mbPost, mRequestAddress);
			
			mpHttpResonse->end(response);
		}

	public:
		tWssServerNet<bUSE_SSL>		*mpNet = NULL;
		HttpResponse<bUSE_SSL>			*mpHttpResonse = NULL;
		AString mRequestString;
		AString mRequestAddress;
		bool mbPost = false;
	};

	// 解析出请求端的地址
	AString GetRequestAddress(const std::string_view &address)
	{
		if (address.size() == 4)
		{
			static thread_local char buf[32];			
			unsigned char *b = (unsigned char *)address.data();
			sprintf(buf, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
			return buf;
		}
		else if (address.size() == 16)
		{
			static thread_local char buf[32];
			unsigned char *b = (unsigned char *)address.data();
			sprintf(buf, "%u.%u.%u.%u", b[12], b[13], b[14], b[15]);
			return buf;
		}
		return "Error Address";
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::StartWeb(int port, const AString &keyPem, const AString &certPem, const AString &pass)
	{
		if (mpHttpApp != NULL)
		{
			if (mpHttpListen != NULL)
				us_listen_socket_close(bUSE_SSL ? 1 : 0, (us_listen_socket_t*)mpHttpListen);
			mpHttpListen = NULL;
			delete mpHttpApp;
		}

		if (bUSE_SSL)
		{
			mpHttpApp = new TemplatedApp<bUSE_SSL>(
				ws_option(
					/* there are example certificates in uwebsockets.js repo */
					keyPem.c_str(),
					certPem.c_str(),
					pass.c_str()
				)
				);
		}
		else
			mpHttpApp = new TemplatedApp<bUSE_SSL>();

		mpHttpApp->get("/*", [=](auto *res, auto *req) {			

			// SB设计 这里处理回复时, 未提供获取POST数据, 执行此函数后, 才会调用onData
			// 而在此函数执行后, 如果未回复, 则断言错误, 只有提供onAborted处理才可以
			res->onAborted([]() {});
			res->writeHeader("Access-Control-Allow-Origin", "*");

			//NiceData requestParam;
			//requestParam["PARAM"] = GetString(req->getQuery());
			//for (auto it = req->begin(); it != req->end(); ++it)
			//{
			//	std::pair<std::string_view, std::string_view> str = *it;

			//	requestParam[GetString(str.first)] = GetString(str.second);
			//}

			Auto<AsyncWebTool<bUSE_SSL>> tool = MEM_NEW AsyncWebTool<bUSE_SSL>();
			tool->mpNet = this;
			tool->mpHttpResonse = res;
			tool->mRequestString = GetString(req->getQuery());
			tool->mbPost = false;
			tool->mRequestAddress = GetRequestAddress( res->getRemoteAddress() );

			ASYNCAUTO(&AsyncWebTool<bUSE_SSL>::AsyncResponse, tool.getPtr());

			//AString response;
			//OnResponse(GetString(req->getQuery()), response, false);

			//res->end(response.c_str());

		});

		mpHttpApp->post("/*", [=](auto *res, auto *req) {
			// SB设计 这里处理回复时, 未提供获取POST数据, 执行此函数后, 才会调用onData
			// 而在此函数执行后, 如果未回复, 则断言错误, 只有提供onAborted处理才可以
			res->onAborted([]() {});
			res->writeHeader("Access-Control-Allow-Origin", "*");

			res->onData(
				[=](std::string_view data, bool b)
			{
				AString value = GetString(data);
				if (value.length() <= 0)
				{
					// POST 疼蛋的 expect : 100 - continue 处理
					//	当POST发送较大参数时, 
					// 服务器会返回 100 Continue 表示接受数据
					//	客户端再继续发送参数, 再次回调onData
					// 先发 100 - continue(也会触发onData此时完成回复后, 接收处理参数后onData的回复无效, 造成超时BUG),
					if (req->getHeader("expect") !="100-continue")
						res->end("None");
					return;
				}

				DEBUG_LOG("Request > %s", value.c_str());
				//AString response;
				//OnResponse(value, response, true);

				//res->end(response.c_str());

				Auto<AsyncWebTool<bUSE_SSL>> tool = MEM_NEW AsyncWebTool<bUSE_SSL>();
				tool->mpNet = this;
				tool->mpHttpResonse = res;
				tool->mRequestString = value;
				tool->mbPost = true;
				tool->mRequestAddress = GetRequestAddress(res->getRemoteAddress());

				ASYNCAUTO(&AsyncWebTool<bUSE_SSL>::AsyncResponse, tool.getPtr());
			}
			);
		
			// 这里一旦回复, 之后再次end 也没有作用了
			//res->end(NULL);		

			//NiceData requestParam;
			//requestParam["PARAM"] = GetString(req->getUrl());
			//for (auto it = req->begin(); it != req->end(); ++it)
			//{
			//	std::pair<std::string_view, std::string_view> str = *it;

			//	requestParam[GetString(str.first)] = GetString(str.second);
			//}

		});
		mpHttpApp->listen(port, [=](auto *token) {
			if (token) {
				NOTE_LOG("%s listening on port > %d", bUSE_SSL ? "Https":"Http", port);
			}
			else
			{
				ERROR_LOG("%s start listen port %d fail", bUSE_SSL ? "Https" : "Http", port);
				if (bUSE_SSL)
					LOG("Check key file : Key %s, Cert %s, Pass %s", keyPem.c_str(), certPem.c_str(), pass.c_str());
			}
			mpHttpListen = token;
		});

		if (mpLoop == NULL)
		{
			us_loop_t *pLoop = (us_loop_t*)Loop::get();
			us_loop_integrate(pLoop);

			mpLoop = pLoop->uv_loop;
		}
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::RegisterMsg(int eventIndex, Logic::AutoEventFactory factory)
	{
		Hand< WebNetEventCenter> webCenter = mWebEventCenter;
		webCenter->RegisterMsg(eventIndex, factory);
	}

	template<bool bUSE_SSL>
	bool uWS::tWssServerNet<bUSE_SSL>::AsyncStart(const AString &wssServerIP, int serverPort, const AString &keyPemFile /*= ""*/, const AString &pfxFileName /*= ""*/, const AString &pfxPassword /*= ""*/)
	{
		if (!StartNet(wssServerIP, serverPort, keyPemFile, pfxFileName, pfxPassword))
			return false;

		while (mStartState == eWssStartIniting)
		{
			tTimer::AWaitTime(1000);

		}

		return mStartState == eWssStateStartSucceed;
	}

	template<bool bUSE_SSL>
	bool tWssServerNet<bUSE_SSL>::StartNet(const AString &wssServerIP, int serverPort, const AString &keyPemFile, const AString &pfxFileName, const AString &pfxPassword)
	{
		mStartState = eWssStartIniting;
		mIP = wssServerIP;
		mPort = serverPort;

		mBeginTotalMilTime = TimeManager::NowTick();

		typename uWS::TemplatedApp<bUSE_SSL>::WebSocketBehavior  wsWsBehavior;
		wsWsBehavior.maxPayloadLength = 100 * 1024 * 1024;
		wsWsBehavior.maxBackpressure = 200 * 1024 * 1024;
		//tWssServerNet *pNet = this;
		wsWsBehavior.open = [=](WSS *pWs)
		{
			//printf("%s == Ws open,  at thread  %u,  ip = %s\r\n", pNet->mIP.c_str(), GetCurrentThreadId(), pWs->getRemoteAddressAsText().data());
			OnWsOpen(pWs);
		};
		wsWsBehavior.message = [=](WSS *pWs, std::string_view  msg, uWS::OpCode type)
		{
			//printf("%s == Rev msg,  at thread  %u , rev %d, code = %d\r\n", pNet->mIP.c_str(), GetCurrentThreadId(), (int)msg.size(), type);
			OnWsMessage(pWs, (void*)msg.data(), (DSIZE)msg.size(), type);
		};
		wsWsBehavior.close = [=](WSS *pWs, int code, std::string_view  msg)
		{
			//printf("%s == Close,  at thread  %u , code %d, msg = %s\r\n", pNet->mIP.c_str(), GetCurrentThreadId(), (int)msg.size(), msg.data());
			OnWsClose(pWs, code, msg.data(), (DSIZE)msg.size());
		};

		if (mpWsApp != NULL)
		{
			if (mpListen != NULL)
				us_listen_socket_close(bUSE_SSL ? 1 : 0, (us_listen_socket_t*)mpListen);
			mpListen = NULL;
			delete mpWsApp;
		}

		if (bUSE_SSL)
		{
			mpWsApp = new TemplatedApp<bUSE_SSL>(
				ws_option(
					/* there are example certificates in uwebsockets.js repo */
					keyPemFile.c_str(),
					pfxFileName.c_str(),
					pfxPassword.c_str()
				)
				);
		}
		else
			mpWsApp = new TemplatedApp<bUSE_SSL>();

		TemplatedApp<bUSE_SSL> &&app = (TemplatedApp<bUSE_SSL> &&)*mpWsApp;
		app.ws<PerSocketData>("/*", (typename TemplatedApp<bUSE_SSL>::WebSocketBehavior&&)wsWsBehavior);
		app.listen(mPort, [=](auto *token) {
			if (token) {
				std::cout << "Wss listening on port " << serverPort << std::endl;
				mStartState = eWssStateStartSucceed;
			}
			else
				mStartState = eWssStateStartFail;
			
			mpListen = token;
		});

		//app.run();
		if (mpLoop == NULL)
		{
			us_loop_t *pLoop = (us_loop_t*)Loop::get();
			us_loop_integrate(pLoop);

			mpLoop = pLoop->uv_loop;
		}
		return true;
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::_stopNet(void)
	{
		if (mpWsApp != NULL)
		{
			// 必须关闭监听, 否则关闭时会出现assert 存在未清空的handle
			if (mpListen != NULL)
				us_listen_socket_close(bUSE_SSL ? 1 : 0, (us_listen_socket_t*)mpListen);
			mpListen = NULL;

			for (int i = 0; i < 10; ++i)
			{
				Process();
				TimeManager::Sleep(10);
			}
			delete mpWsApp;
		}
		mpWsApp = NULL;

		if (mpHttpApp != NULL)
		{
			// 必须关闭监听, 否则关闭时会出现assert 存在未清空的handle
			if (mpHttpListen != NULL)
				us_listen_socket_close(bUSE_SSL ? 1 : 0, (us_listen_socket_t*)mpHttpListen);
			mpHttpListen = NULL;
			delete mpHttpApp;
			for (int i = 0; i < 10; ++i)
			{
				Process();
				TimeManager::Sleep(10);
			}
		}
		mpHttpApp = NULL;

		mpLoop = NULL;
		try {
			Loop::get()->free();
		}
		catch (std::exception e)
		{
			ERROR_LOG("Free uv loop > %s", e.what());
		}
		
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::Process(void)
	{
		if (mbStop)
		{
			mbStop = false;
			_stopNet();
		}

		if (mpLoop != NULL)
			uv_run(mpLoop, UV_RUN_NOWAIT);
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::LowProcess(int spaceTime)
	{
		if (TimeManager::NowTick() - mBeginTotalMilTime>=1000)
		{
			mSendSizeBySecond = 0;
			mReceiveSizeBySecond = 0;
			mBeginTotalMilTime = TimeManager::NowTick();
		}
	}

	//void tWssServerNet::WssThreadRun()
	//{
	//	AString url;

	//	tWssServerNet *pNet = this;

	//	WsBehavior  wsWsBehavior;

	//	wsWsBehavior.open = [=](WSS *pWs)
	//	{
	//		printf("%s == Ws open,  at thread  %u,  ip = %s\r\n", pNet->mIP.c_str(), GetCurrentThreadId(), pWs->getRemoteAddressAsText().data());
	//		OnWsOpen(pWs);
	//	};

	//	wsWsBehavior.message = [=](WSS *pWs, std::string_view  msg, uWS::OpCode type)
	//	{
	//		printf("%s == Rev msg,  at thread  %u , rev %d, code = %d\r\n", pNet->mIP.c_str(), GetCurrentThreadId(), (int)msg.size(), type);
	//		OnWsMessage(pWs, (void*)msg.data(), (DSIZE)msg.size(), type);
	//	};
	//	wsWsBehavior.close = [=](WSS *pWs, int code, std::string_view  msg)
	//	{
	//		printf("%s == Close,  at thread  %u , code %d, msg = %s\r\n", pNet->mIP.c_str(), GetCurrentThreadId(), (int)msg.size(), msg.data());
	//		OnWsClose(pWs, code, msg.data(), (DSIZE)msg.size());
	//	};

	//	/* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
	//	 * You may swap to using uWS:App() if you don't need SSL */
	//	 //uWS::App(
	//	 //	//my_option(
	//	 //	//	/* There are example certificates in uWebSockets.js repo */
	//	 //	//	"e:/key.pem",
	//	 //	//	"e:/cert.pem",
	//	 //	//	"1234"
	//	 //	//)
	//	 //).ws<PerSocketData>("/*", (WsBehavior&&)wsWsBehavior

	//	 //	).listen(mPort, [=](auto *token) {
	//	 //	if (token) {
	//	 //		std::cout << "Listening on port " << pNet->mPort << std::endl;
	//	 //	}
	//	 //}).run();

	//	TemplatedApp<false> &&app = App();
	//	app.ws< PerSocketData>("/*", (WsBehavior&&)wsWsBehavior);
	//	app.listen(mPort, [=](auto *token) {
	//		if (token) {
	//			std::cout << "Listening on port " << pNet->mPort << std::endl;
	//		}
	//	});

	//	app.run();
	//	//us_loop_t *pLoop = (us_loop_t*)Loop::get();
	//	//us_loop_integrate(pLoop);

	//	//while (true)
	//	//{
	//	//	uv_run((uv_loop_t*)pLoop->uv_loop, UV_RUN_NOWAIT);
	//	//	TimeManager::Sleep(1);
	//	//}
	//	//bool bResult = true;
	//	//NOTE_LOG("Thread %u init wss net %s %s", GetCurrentThreadId(), url.c_str(), bResult ? "succeed" : "fail");

	//}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::OnWsOpen(WSS *pWs)
	{
		Hand<tWssConnect<bUSE_SSL>>  conn = CreateConnect();
		conn->Init(this, pWs->getRemoteAddressAsText().data(), 0);
		conn->mpWebSocket = pWs;
		((PerSocketData*)pWs->getUserData())->mConnect = conn;
		OnAddConnect(conn.getPtr());
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::OnWsMessage(WSS *pWs, void *pReceiveData, DSIZE dataLength, int codeType)
	{
		TempDataBuffer	revData(pReceiveData, dataLength);

		mReceiveMsgCount++;
		mReceiveSizeBySecond += dataLength;
		mReceiveSize += dataLength;

		AutoEvent evt = GetEventCenter()->RestoreMsg(&revData);
		if (evt)
		{
			//NOTE_LOG("Rev msg event >%s", evt->GetEventName());
			//NOTE_LOG("%s", evt->GetData().dump().c_str());
			HandConnect conn = ((PerSocketData*)pWs->getUserData())->mConnect;
			if (conn)
			{
				conn->OnPing(NULL);
				evt->_OnBindNet(conn.getPtr());
			}
			// 异步执行
			evt->AsyncDo();
			//evt->DoEvent(true);
		}
		else
		{

			HandConnect conn = ((PerSocketData*)pWs->getUserData())->mConnect;
			if (conn)
			{
				ERROR_LOG("Restore msg event [?] fail >?, then now remove connect %s", conn->GetIp());
				conn->SetRemove(true);
			}
			else
				ERROR_LOG("Restore msg event [?] fail >?, then now remove connect");
		}
	}

	template<bool bUSE_SSL>
	void tWssServerNet<bUSE_SSL>::OnWsClose(WSS *pWs, int, const char *pMsgData, DSIZE dataLength)
	{
		HandConnect conn = ((PerSocketData*)pWs->getUserData())->mConnect;
		conn->OnDisconnect();
		OnCloseConnect(conn.getPtr());

		conn._free();
	}

	template<bool bUSE_SSL>
	AString uWS::tWssServerNet<bUSE_SSL>::GetRunInfo()
	{
		AString info;
		info.Format("Count: %d, Send: %.2fK/S T %.2fM %llu, Rev: %.2fK/S T %.2fM %llu"
			, 1
			, mSendSizeBySecond/1024.0f
			, mSendSize/1024/1024.0f
			, mSendMsgCount
			, mReceiveSizeBySecond/1024.0f
			, mReceiveSize/1024/1024.0f
			, mReceiveMsgCount
		);

		return info;
	}

	//-------------------------------------------------------------------------
	template<bool bUSE_SSL>
	void uWS::tWssConnect<bUSE_SSL>::Init(tNetHandle *pNet, const char *szID, int port)
	{
		mpNet = pNet;
		mIP = NetAddress::IPv6ToIPv4(szID);		
		mPort = port;
	}

	template<bool bUSE_SSL>
	void tWssConnect<bUSE_SSL>::SetRemove(bool bNeedRemove)
	{
		if (bNeedRemove)
		{
			if (mpWebSocket != NULL)
				mpWebSocket->close();
			mpWebSocket = NULL;
		}
	}

	template<bool bUSE_SSL>
	tNetHandle* tWssConnect<bUSE_SSL>::GetNetHandle()
	{
		return mpNet;
	}

	template<bool bUSE_SSL>
	bool tWssConnect<bUSE_SSL>::SendMsgData(DataStream *pSendMsgData, int frameType)
	{
		if (IsRemove())
			return false;

		std::string_view message(pSendMsgData->data(), pSendMsgData->dataSize());

		mpWebSocket->send(message, frameType == WS_BINARY_FRAME ? uWS::OpCode::BINARY : uWS::OpCode::TEXT);
		Hand< tWssServerNet< bUSE_SSL> > net = GetNetHandle();
		net->mSendMsgCount++;
		net->mSendSizeBySecond += pSendMsgData->dataSize();
		net->mSendSize += pSendMsgData->dataSize();

		return false;
	}

	template<bool bUSE_SSL>
	bool tWssConnect<bUSE_SSL>::SendEvent(Logic::tEvent *pEvent)
	{
		thread_local DataBuffer  sTempBuffer(16 * 1024);
		sTempBuffer.clear(false);
		AutoEvent evt = pEvent->GetSelf();
		bool b = pEvent->GetEventCenter()->SerializeMsg(evt, &sTempBuffer);
		if (b)
			return SendMsgData(&sTempBuffer, WS_BINARY_FRAME);
		return false;
	}

	template<bool bUSE_SSL>
	bool uWS::tWssConnect<bUSE_SSL>::Send(int packetID, const Packet *msgPacket)
	{
		static thread_local DataBuffer  sTempBuffer(16 * 1024);
		sTempBuffer.clear(false);
		msgPacket->Write(sTempBuffer);

		return SendMsgData(&sTempBuffer, WS_BINARY_FRAME);
	}
	//-------------------------------------------------------------------------

	float CpuInfo::CpuProcess()
	{
		uv_cpu_info_t *cpu_infos_ptr = NULL;
		int cpu_count = 0;
		uv_cpu_info(&cpu_infos_ptr, &cpu_count);
		if (cpu_infos_ptr != NULL)
		{
			UInt64 total = 0;			
			UInt64 speed = 0;
			double all = 0;
			for (int i = 0; i < cpu_count; ++i)
			{
				speed += cpu_infos_ptr[i].cpu_times.idle;
				total += cpu_infos_ptr[i].cpu_times.idle;
				total += cpu_infos_ptr[i].cpu_times.irq;
				total += cpu_infos_ptr[i].cpu_times.nice;
				total += cpu_infos_ptr[i].cpu_times.sys;
				total += cpu_infos_ptr[i].cpu_times.user;
				UInt64 x = cpu_infos_ptr[i].cpu_times.sys + cpu_infos_ptr[i].cpu_times.user + cpu_infos_ptr[i].cpu_times.idle;
				double r = (cpu_infos_ptr[i].cpu_times.sys + cpu_infos_ptr[i].cpu_times.user) / (double)x * 100;
				printf("cpu %d  %llu / %llu = %.2f\r\n", i, cpu_infos_ptr[i].cpu_times.sys + cpu_infos_ptr[i].cpu_times.user, x
					, r);
				all += r;
			}
			uv_free_cpu_info(cpu_infos_ptr, cpu_count);
			return (float)all;
		}
		return 0;
	}

}