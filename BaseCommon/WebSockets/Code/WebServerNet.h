#pragma once
#ifndef _INCLUDE_WEBSERVERNET_H_
#define _INCLUDE_WEBSERVERNET_H_


#include "BaseEventNet.h"
#include "WorkThread.h"

#if __LINUX__
#	define STATIC_WSS_NET
#else
#include <xstring>
#endif
#include <string_view>

#ifndef WssNet_Export
#ifdef STATIC_WSS_NET
#define WssNet_Export
#define WssNet_Export_H	
#else
#		ifdef WEBSOCKETS_EXPORTS
#			define WssNet_Export __declspec(dllexport)
#			define WssNet_Export_H	__declspec(dllexport)
#		else
#			define WssNet_Export __declspec(dllimport)
#			define WssNet_Export_H	
#		endif
#endif
#endif

//-------------------------------------------------------------------------
struct uv_loop_s;


namespace uWS
{
	enum WssNetInitState
	{
		eWssStateNoStart = 0,
		eWssStateStartSucceed = 1,
		eWssStateStartFail = 2,
		eWssStartIniting = 3,
	};
	//-------------------------------------------------------------------------
	template<bool, bool >
	struct WebSocket;

	template<bool>
	struct TemplatedApp;

	template<bool>
	class tWssConnect;

	//-------------------------------------------------------------------------
	class WebServerPtr : public AutoBase
	{
	public:
		BaseWebServer *mpServer;
	};
	typedef Auto< WebServerPtr> AutoWebServer;

	//-------------------------------------------------------------------------
	template<bool bUSE_SSL>
	class WssNet_Export tWssServerNet : public BaseWebServer
	{
		typedef uWS::WebSocket<bUSE_SSL, true>  WSS;

	public:
		virtual HandConnect CreateConnect();

	public:
		tWssServerNet();
		virtual ~tWssServerNet();

		virtual void OnForceFreeBefore() override { _stopNet(); }

	public:
		virtual void StartWeb(int port, const AString &keyPem, const AString &certPem, const AString &pass) override;
		virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) {  }

		// 启动二进制POST web, 兼容字符串GET
		virtual void StartBytesWeb(int port, const AString &keyPem, const AString &certPem, const AString &pass) override;
		virtual void OnResponseBytes(HandPacket requestMsg, DataBuffer &response, const AString &requestAddress) {  }

	public:
		virtual bool StartNet(const AString &wssServerIP, int serverPort, const AString &keyPemFile = "", const AString &pfxFileName = "", const AString &pfxPassword = "") override;


		virtual void StopNet(void) override {
			_stopNet();
			//mbStop = true;  tBaseEventNet::StopNet();
		}

		virtual void _stopNet(void);

		// 异步初始
		virtual bool AsyncStart(const AString &wssServerIP, int serverPort, const AString &keyPemFile = "", const AString &pfxFileName = "", const AString &pfxPassword = "") override;

	public:
		virtual void Process(void) override;
		virtual void LowProcess(int spaceTime) override;

		virtual Logic::tEventCenter* GetEventCenter()const override
		{
			return (Logic::tEventCenter*)mWebEventCenter.getPtr();
		}

		virtual void RegisterMsg(int eventIndex, Logic::AutoEventFactory factory) override;

		virtual const char* GetIp(void) const override { return mIP.c_str(); }
		virtual int		GetPort(void) const override { return mPort; }


		virtual AString GetRunInfo() override;

	public:
		// Wss部分
		void OnWsOpen(WSS *pWs);

		// Message
		void OnWsMessage(WSS *pWs, void *pReceiveData, DSIZE dataLength, int codeType);

		// Close
		void OnWsClose(WSS *pWs, int, const char *pMsgData, DSIZE dataLength);

	public:
		AutoWebServer				mSelfPtr;
		WssNetInitState					mStartState = eWssStateNoStart;
		AString mIP;
		int mPort;
		bool mbStop = false;

		AutoEventCenter				mWebEventCenter;
		HandConnect					mCommonConnect;  // 用于二进制POST消息处理
		EasyHash<UInt64, AutoData>		mReceiveDataBufferList;	 //  用于HTTP接收二进制时分段接收 (POST上传此http, 有可能不是一次接收成功, 必须使用些缓存接收完整

		TemplatedApp<bUSE_SSL> *mpWsApp;
		TemplatedApp<bUSE_SSL> *mpHttpApp;
		void										*mpHttpListen;

		uv_loop_s								*mpLoop;
		void										*mpListen;

	public:
		UInt64									mBeginTotalMilTime;
		UInt64									mSendMsgCount;
		UInt64									mSendSize;
		UInt64									mReceiveMsgCount;
		UInt64									mReceiveSize;
		uint										mSendSizeBySecond;
		uint										mReceiveSizeBySecond;
	};



	//-------------------------------------------------------------------------

	template<bool bUSE_SSL>
	class WssNet_Export tWssConnect : public BaseWebConnect
	{
		typedef uWS::WebSocket<bUSE_SSL, true>  WSS;

	public:
		WSS			*mpWebSocket;
		tNetHandle *mpNet;

		AString mIP;
		int mPort;

	public:
		tWssConnect()
			: mpNet(NULL)
			, mPort(0)
			, mpWebSocket(NULL)
		{

		}
		~tWssConnect()
		{
			DEBUG_LOG("Now free connect %s:%d", mIP.c_str(), mPort);
		}

		void Init(tNetHandle *pNet, const char *szID, int port);

		virtual AString GetRemoteAddr(int &port) { port = mPort; return  mIP; };

		virtual const char* GetIp() { return mIP.c_str(); }

	public:
		virtual bool Process() override
		{
			return true;
		}
		

		// NOTE: bNeedRemove = true 表示需要关闭C#层连接
		virtual void SetRemove(bool bNeedRemove)  override;

		virtual bool IsDisconnect() const override { return mpWebSocket == NULL; }

		virtual bool IsRemove() { return mpWebSocket == NULL; }

		virtual tNetHandle* GetNetHandle() override;

		virtual bool SendMsgData(DataStream *pSendMsgData, int frameType);
		virtual bool SendEvent(Logic::tEvent *pEvent) override;
		virtual bool Send(int packetID, const Packet  *msgPacket) override;
	};



	//typedef Hand<tWssConnect> HandWssConnect;

	//-------------------------------------------------------------------------
	class WssNet_Export WssWebServer : public tWssServerNet<true>
	{
	};

	class WssNet_Export WsWebServer : public tWssServerNet<false>
	{
	};

	class WssNet_Export WssConnect : public tWssConnect<true>
	{

	};

	class WssNet_Export WsConnect : public tWssConnect<false>
	{

	};
	//-------------------------------------------------------------------------
	class WssNet_Export CpuInfo
	{
	public:
		static float CpuProcess();
	};
	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_WEBSERVERNET_H_