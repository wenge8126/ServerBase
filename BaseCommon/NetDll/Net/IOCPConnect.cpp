
#include "IOCPConnect.h"
#include "IOCPCommon.h"
#include "EventProtocol.h"
#include "SocketAPI.h"
#include "TimeManager.h"
#include "EventData.h"
#include "NetAddress.h"

#if _DEBUG_NET_
#   include "IOCPServerNet.h"
#endif

#if __LINUX__
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#   include "IOCPServerNet.h"


// Linux 实现 
void IOCPConnect::_SendTo()
{	
	if (mSendLoopData.dataSize() > 0)
	{
		IOCPBaseNet *pNet = dynamic_cast<IOCPBaseNet*>(GetNetHandle());
		if (pNet == NULL)
			return;

		//int len = mSendLoopData.dataSize();
		//if (pNet->mTempRevBuffer.size() < len)
		//	pNet->mTempRevBuffer.resize(len);
		//if (!mSendLoopData.peek(pNet->mTempRevBuffer.data(), len))
		//	ERROR_LOG("No read data %d", len);

		//size_t wLen = write(mSocketID, pNet->mTempRevBuffer.data(), len);
		//if (wLen <= 0)
		//	return;
		//if (wLen == (size_t)-1)
		//	ERROR_LOG(">>> Send error %d", errno)
		//else
		//{
		//	mSendLoopData.skip(wLen);
		//	DEBUG_LOG("~~~~ Send data > %u", wLen);
		//}

		do {
			int len = 0;
			char *szSendData = mSendLoopData.GetDataPtr(len);			
			if (len <= 0)
				break;
			size_t wLen = write(mSocketID, szSendData, len);
			if (wLen == 0)
				return;
			else if (wLen == (size_t)-1)
			{
				ERROR_LOG(">>> Send error %d", errno);
				return;
			}
			else
			{
				mSendLoopData.skip(wLen);
				//DEBUG_LOG("~~~~ Send data > %u", wLen);
			}
		}while(mSendLoopData.dataSize() > 0);
	}
}

void IOCPConnect::_PostReceive()
{
}

bool IOCPConnect::Process()
{
	if (mNeedRemove)
	{		 
#if		_DEBUG_NET_
		if (TimeManager::Now() - mRemoveTime > _DEBUG_REMOVE_OVER_TIME)
		{
			WARN_LOG("连接移除状态超过 [%d] S, 应及时清理", _DEBUG_REMOVE_OVER_TIME);
			mRemoveTime = TimeManager::Now();
		}
#endif
		return false;
	}
	else
	{
		bool bThriggerSend = false;
		if (mSendLoopData.ProcessSendData())
			bThriggerSend = true;

		if (mReceiveLoopData.dataSize() > 0)		
			_ProcessReceiveData();		
		// 这个是技巧性的设计, 如果Net线程端还有待传数据, 则直接触发发送过程, 然后再发送事件后, 进行继续转移接收的待转移数据
		if (mReceiveLoopData.mWaitSendBuffer.dataSize() > 0)
			bThriggerSend = true;

		if (bThriggerSend)
			TriggerThreadSend();
	}
	return true;
}

void  ZeroMemory(void *p, int size)
{
	memset(p, 0, size);
}

#else
#include <WS2tcpip.h>

// NOTE:  仅在网络线程中执行
void IOCPConnect::_SendTo()
{
	if (mSocketID <= 0 || mNeedRemove)
		return;

	if (mSendOverlapped->mbWaiting)
	{
		if (TimeManager::Now() - mLastSendTime > _IOCP_SEND_OVERTIME)
		{
			WARN_LOG("[%d][%s:%d]连接发送超时>[%d] Sec, 强制关闭连接", GetNetID(), GetIp(), GetPort(), _IOCP_SEND_OVERTIME);
			SetRemove(true);
			return;
		}
		return;
	}

	UINT len = mSendLoopData.dataSize();
	if (len <= 0)
	{
		return;
	}

	mSendOverlapped->mbWaiting = true;
	mLastSendTime = TimeManager::Now();
	// 创建一个发送完成事件
	SendOverlapped* pSendOverlapped = mSendOverlapped;

	int dataSize = 0;
	//if (len > pSendOverlapped->WsaBuf.len)
	//	len = pSendOverlapped->WsaBuf.len;
	//mSendLoopData._read(pSendOverlapped->WsaBuf.buf, len);
	//dataSize = len;

	pSendOverlapped->WsaBuf.buf = mSendLoopData.GetDataPtr(dataSize); // pSendOverlapped->buffer;
	AssertNote(dataSize > 0, "Must has send data");
	pSendOverlapped->WsaBuf.len = dataSize;

	// 发送事件
	DWORD SendBytes = 0;
	int result = WSASend(mSocketID, &pSendOverlapped->WsaBuf, 1, &SendBytes, 0,
		(LPOVERLAPPED)pSendOverlapped, NULL);
	if (result == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		if (iError != ERROR_IO_PENDING)
		{
			if (iError == 10054)
			{
				// WSAECONNRESET(10054) 远程客户端强制关闭连接
				DEBUG_LOG("[%d] [%s:%d] Other part connect close", iError, GetIp(), GetPort());
			}
			else
				ERROR_LOG("IOCPServer::SendData - WSASend() 发生了如下错误：   %d\n", iError);
			// 异常错误事件
			//OnError(nClient, iError);
			// 关闭SOCKET
			mSendOverlapped->mbWaiting = false;
			SetRemove(true);

			return;
		}
	}
}

// NOTE:  仅在网络线程中执行
void IOCPConnect::_PostReceive()
{
	if (mNeedRemove || mSocketID <= 0)
		return;

	// 在完成端口线程内, 不会再设置为true, 目的是防止, 投递时, 被移除
	//if (mRecvOverlapped->mbWaiting)
	//{
	//	ERROR_LOG("[%s:%d]Now wait receive state, May be code error", GetIp(), GetPort());
	//	return;
	//}

	DWORD RecvBytes = 0;
	DWORD Flags = 0;
	RecvOverlapped* pRecvOverlapped = mRecvOverlapped;

	mRecvOverlapped->mbWaiting = true;

	int result = WSARecv(mSocketID, &pRecvOverlapped->WsaBuf, 1, &RecvBytes, &Flags,
		(LPOVERLAPPED)pRecvOverlapped, NULL);
	if (result == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		if (iError != ERROR_IO_PENDING)
		{
			if (mNeedRemove)
				return;
			mRecvOverlapped->mbWaiting = false;

			// 异常错误
			ERROR_LOG("NET ERROR: [%d], [%u]", GetNetID(), iError);
			SetRemove(true);
			return;
		}
	}
}
#endif	// __LINUX__

int IOCPConnect::GetLocalPort()
{
	std::string ip;
	unsigned short port = 0;

	struct sockaddr_storage sa;  
	socklen_t salen = sizeof(sa);

	if (::getsockname((SOCKET)mSocketID, (struct sockaddr*)&sa, &salen) == -1) {  
		ip = "?";  
		port = 0;  
		return port;  
	}  

	if (sa.ss_family == AF_INET) {  
		struct sockaddr_in *s = (struct sockaddr_in*)&sa;  
		ip = ::inet_ntoa(s->sin_addr);  
		port = ::ntohs(s->sin_port);  
		return port;  
	}  
	return 0;  	
}

AString IOCPConnect::GetLocalIP()
{
	std::string ip;
	unsigned short port = 0;

	struct sockaddr_storage sa;
	socklen_t salen = sizeof(sa);

	if (::getsockname((SOCKET)mSocketID, (struct sockaddr*)&sa, &salen) == -1) {
		ip = "?";
		port = 0;
		return port;
	}

	if (sa.ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in*)&sa;
		return ::inet_ntoa(s->sin_addr);		
	}
	return "Unkwon";
}

AString IOCPConnect::GetRemoteAddr(int &port)
{
#if __WINDOWS__
	struct sockaddr addr;
	struct sockaddr_in* addr_v4;
	socklen_t addr_len = sizeof(addr);
	//获取remote ip and port
	ZeroMemory(&addr, sizeof(addr));
	if (0 == getpeername(mSocketID, &addr, &addr_len))
	{
		if (addr.sa_family == AF_INET)
		{
			addr_v4 = (sockaddr_in*)&addr;
			port = ntohs(addr_v4->sin_port);
			return NetAddress::Num2IP(addr_v4->sin_addr.S_un.S_addr);			
		}
	}
#endif
	return AString();
}

bool IOCPConnect::SendEvent( Logic::tEvent *sendEvent )
{
	if (sendEvent!=NULL)
	{
		return EventNetProtocol::SendEvent(this, sendEvent);
	}

	return false;
}

bool IOCPConnect::SendPacket(const Packet *msgPacket, bool bEncrypt)
{
	if (mNeedRemove)
		return false;

	tNetHandle *p = GetNetHandle();
	if (p!=NULL)
	{
		msgPacket->SetNeedEncrypt(bEncrypt);

		bool re = p->GetNetProtocol()->WritePacket(msgPacket, &mSendLoopData.mWaitSendBuffer);
		if (!re)
		{
			AString err;
			err.Format("%d [%s:%d] write packet fail to send [%d] size %u, then remove", GetNetID(), GetIp(), GetPort(), msgPacket->GetPacketID(), msgPacket->GetPacketSize());
			ERROR_LOG(err.c_str());
			SetRemove(true);
		}

		TrySend();
		
		return true;
	}
	return false;
}

void IOCPConnect::OnPing(Packet *pingPacket)
{
	mLastReceivePingTime = TimeManager::NowTick();

//#if _DEBUG_NET_ && DEVELOP_MODE
	//int port = 0;
	//AString ip = GetRemoteAddr(port);
	//NOTE_LOG(" * Get heart beat :Remote [%s:%d] >  [%s:%d]", ip.c_str(), port, GetLocalIP().c_str(), GetLocalPort());
//	DEBUG_LOG("* [%s:%d] -> [%s:%d] Rev ping %llu", ip.c_str(), port, GetIp(), GetLocalPort(), mLastReceivePingTime);
//#endif
}

void IOCPConnect::OnReceivePingOverTime()
{
	int port = 0;
	AString ip = GetRemoteAddr(port);
#if DEVELOP_MODE
	NOTE_LOG("WARN: [%s:%d] -> [%s:%d] Ping over time %llu", ip.c_str(), port, GetIp(), GetLocalPort(), TimeManager::NowTick()-mLastReceivePingTime);
#else // !_DEBUG_NET_
	NOTE_LOG("WARN: [%s:%d] -> [%s:%d] Ping over time %llu, then now remove", ip.c_str(), port, GetIp(), GetLocalPort(), TimeManager::NowTick() - mLastReceivePingTime);
	SetRemove(true);
#endif
}

void IOCPConnect::ProcessPing()
{
	if (TimeManager::NowTick()-mLastSendPingTime>OnceSendPingSecond()*1000)
	{
		mLastSendPingTime = TimeManager::NowTick();
		
		// 发送PING
		HandPacket pingPacket = GetNetHandle()->GetNetProtocol()->CreatePacket(eNotifyHeartBeat);

		Send(pingPacket.getPtr(), false);
		//int port = 0;
		//AString ip = GetRemoteAddr(port);
		//NOTE_LOG(" = Send heart beat :  [%s:%d] > remote [%s:%d] ", GetLocalIP().c_str(), GetLocalPort(), ip.c_str(), port);
	}
	if (TimeManager::NowTick()-mLastReceivePingTime>ReceivePingOverSecond()*1000)
	{		
		// 接收PING超时
		OnReceivePingOverTime();
		mLastReceivePingTime = TimeManager::NowTick();
	}
}
#if __WINDOWS__
bool IOCPConnect::Process()
{
	if (mNeedRemove)
	{
		if (!mRecvOverlapped->mbWaiting && !mSendOverlapped->mbWaiting)
		{
			// 如果返回false, 会进行释放
				return false;
		}
#if		_DEBUG_NET_
		else if (TimeManager::Now()-mRemoveTime>_DEBUG_REMOVE_OVER_TIME)
		{
			WARN_LOG("连接移除状态超过 [%d] S, 应及时清理", _DEBUG_REMOVE_OVER_TIME);
			mRemoveTime = TimeManager::Now();
		}
#endif
	}
	else
	{
		if (mReceiveLoopData.dataSize()>0)
			_ProcessReceiveData();

		if (mSendLoopData.ProcessSendData())
			TriggerThreadSend();
	}
	return true;
}
#endif
void IOCPConnect::Log( const char *szInfo, ... )
{
#if DEVELOP_MODE && __WINDOWS__
	va_list va;
	va_start(va, szInfo);
	TableTool::DebugLog(va, szInfo);
#endif
}

void IOCPConnect::_Free()
{
#if DEVELOP_MODE
		if (mReceiveLoopData.dataSize()>0)
			WARN_LOG("移除连接时，接收缓存尚存在 [%u] 数据未处理", mReceiveLoopData.dataSize());
#endif
    // NOTE: 因为使用了线程接收, 且使用了接收缓存的空间, 在返回之前, 不可清理
	//mReceiveData.CleanUp();
    if (mSocketID!=0)
	{		
		if (SocketAPI::closesocket_ex(mSocketID)==FALSE)
			//if (closesocket(mSocketID) == SOCKET_ERROR) 
		{   
#if __LINUX__
			Log("Close socket error >[%llu]", mSocketID);
#else
			Log("Close socket error >[%u]", GetLastError());				
#endif
		}				
		mSocketID = 0;
	}	
}

IOCPConnect::IOCPConnect(int revBufferLen, int sendBufferLen) 
	: mSocketID(0)
	, mNeedRemove(false)
	, mPort(0)
	, mLastSendTime(0)
#if		_DEBUG_NET_
	, mRemoveTime(0)
    , mReceiveSize(0)
    , mProcessSize(0)
	, mSendSize(0)
#endif
	, mIocpHandle(NULL)
	, mReceiveLoopData(revBufferLen>0?revBufferLen:DEFAULTSOCKETINPUTBUFFERSIZE, revBufferLen > 0 ? revBufferLen : DEFAULTSOCKETINPUTBUFFERSIZE, (revBufferLen > 0 ? revBufferLen : DEFAULTSOCKETINPUTBUFFERSIZE)*4 )
	, mSendLoopData(sendBufferLen>0?sendBufferLen:DEFAULTSOCKETINPUTBUFFERSIZE, sendBufferLen > 0 ? sendBufferLen : DEFAULTSOCKETINPUTBUFFERSIZE, (sendBufferLen > 0 ? sendBufferLen : DEFAULTSOCKETINPUTBUFFERSIZE)*4 )
{
#if __WINDOWS__
	mSendOverlapped = new SendOverlapped();
	mRecvOverlapped = new RecvOverlapped();
	mForSendNotifyOverlapped = new BaseOverlapped();
	mForSendNotifyOverlapped->IoMode = IoNotifySend;
#else
	mNetThreadID = -1;
#endif
}

IOCPConnect::~IOCPConnect()
{
	//if (mSocketID!=0)
	//{
	//	shutdown(mSocketID, SD_BOTH);
	//	mSocketID = 0;
	//}
#if __LINUX__
	//???
#else
	if (mSocketID != 0 || mRecvOverlapped->mbWaiting || mSendOverlapped->mbWaiting)
	{
		ERROR_LOG("Socket close 逻辑顺序错误, 关闭时, socket 还未关闭");
		_Free();
	}

	delete mSendOverlapped;
	delete mRecvOverlapped;
	delete mForSendNotifyOverlapped;
	mSendOverlapped = NULL;
	mRecvOverlapped = NULL;
	mForSendNotifyOverlapped = NULL;
#endif
	// 只需要关闭Socket即可，如果关闭Iocp Handle, 则整个IOCP都无效，其它连接也都不能正常工作
	//if (mIocpHandle!=NULL)
	//	CloseHandle((HANDLE)mIocpHandle);
	mIocpHandle = NULL;
}

bool IOCPConnect::PingOpenPort(const char *szIp, int nPort)
{
#if __LINUX__
	SOCKET tempSocket = SocketAPI::socket_ex(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (tempSocket == INVALID_SOCKET)
	{
		DEBUG_LOG("NET ERROR: Client socket create fail");
		return false;
	}
#else
	SOCKET tempSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (tempSocket == INVALID_SOCKET)
	{
		DEBUG_LOG("NET ERROR: IOCP client socket create fail > [%u]", GetLastError());
		return false;
	}
#endif
	AString connectIP;
	if (NetAddress::IsIP(szIp))
	{
		connectIP = szIp;
	}
	else
	{
		connectIP = NetAddress::AddressToIP(szIp);
		if (connectIP.empty())
			return false;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(connectIP.c_str());
	addr.sin_port = htons(nPort);

	SocketAPI::setsocketnonblocking_ex(tempSocket, FALSE);
	bool bOpen = (SocketAPI::connect_ex(tempSocket, (const struct sockaddr *)&addr, sizeof(addr)) == TRUE);

	SocketAPI::closesocket_ex(tempSocket);

	return bOpen;
}

void IOCPConnect::_ProcessReceiveData()
{
	UInt64 now = TimeManager::NowTick();
	do
	{
		HandPacket packet = mNet->GetNetProtocol()->ReadPacket(this, &mReceiveLoopData);
		if (packet)
		{
			try{
				packet->Execute(this);		
                if (IsRemove())
                    return;

				// NOTE: 一个循环中处理太多, 阻塞太多时间
				if (TimeManager::NowTick()-now>1)
					return; 
			}
			catch (std::exception &e)
			{
				ERROR_LOG("Net message execute error >%s", e.what());

			}
			catch (...)
			{
				ERROR_LOG("Net message [%d] execute error", packet->GetPacketID());
			}
		}
		else
			break;
	} while (mReceiveLoopData.dataSize() > 0);
}


void IOCPConnect::_Start()
{
	mLastReceivePingTime = TimeManager::NowTick();
	mLastSendPingTime = TimeManager::NowTick();

	//mSendOverlapped->mbWaiting = false;
	_SendTo();

	//mRecvOverlapped->mbWaiting = false;
	_PostReceive();
}

void IOCPConnect::TrySend()
{
	mSendLoopData.ProcessSendData();
	TriggerThreadSend();
}

void IOCPConnect::TriggerThreadSend()
{
#if __WINDOWS__
	PostQueuedCompletionStatus((HANDLE)mIocpHandle, 0, (ULONG_PTR)this, mForSendNotifyOverlapped);
#else __LINUX__
	IOCPBaseNet *pNet = dynamic_cast<IOCPBaseNet*>(GetNetHandle());
	if (pNet == NULL)
		return;
	IocpOrEpollProcessNetThread *pNetThread = pNet->mCompletionThreadList.find(mNetThreadID);
	AssertEx(pNetThread != NULL, "Net thread %d must exist", mNetThreadID);

	epoll_event &ev = pNet->mTempEpollEvent;
	ev.data.fd = mSocketID;
	//设置用于注测的读操作事件
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	if (epoll_ctl((int)(size_t)pNetThread->m_hCompletionPort, EPOLL_CTL_MOD, mSocketID, &ev)<0)
		ERROR_LOG("Add to epoll fail >%d", errno);
#endif
}

void IOCPConnect::SetRemove(bool bNeedRemove)
{
	mNeedRemove = bNeedRemove;  
	_Free(); 
#if _DEBUG_NET_
	mRemoveTime = TimeManager::Now();
#endif
}
//-------------------------------------------------------------------------*/
//-------------------------------------------------------------------------*/
void IOCPServerConnect::OnReceivePacket(Packet *pPacket)
{
	pPacket->Execute(this);
}

//-------------------------------------------------------------------------

void IOCPServerConnect::_ProcessReceiveData()
{
    if (!mbReceiveData)
	{		
		if (mReceiveLoopData.dataSize()<=0)
		{
			return;
		}
		DEBUG_LOG("Check safe ...");
		// NOTE: 默认验证码为0时, 不进行安全码验证
		// 只有验证OK后才执行OnConnected()
		if (mNet->GetSafeCode()==0)
		{
			mbReceiveData = true;
			mNet->OnAddConnect(this);
			OnConnected();
		}
		else
		{	
			int safeCode;
			if ( mReceiveLoopData._read((CHAR*)&safeCode, sizeof(int)) )
			{
				if (safeCode==mNet->GetSafeCode())
				{
					DEBUG_LOG("[%s:%d] Check safe code succeed %d >[%s:%d]", GetNetHandle()->GetIp(), GetNetHandle()->GetPort(), safeCode, GetIp(), GetPort());
					mbReceiveData = true;
					mNet->OnAddConnect(this);
					OnConnected();
					return;
				}
			}
#if DEVELOP_MODE
			ERROR_LOG("WARN: [%s : %d] Connect safe check fail, then now remove  [S:%d!=C%d]", GetIp(), GetPort(), mNet->GetSafeCode(), safeCode);
#else
			DEBUG_LOG("WARN: [%s : %d] Connect safe check fail, then now remove  [S:%d!=C%d]", GetIp(), GetPort(), mNet->GetSafeCode(), safeCode);
#endif
			OnConnectSafeCodeError(GetIp(), GetPort(), safeCode);
			SetRemove(true);
		}
	}
	else
	{
		// NOTE: 如果每个包都加锁，可能会在执行事件的同时会继续接收数据，这样其他连接得到执行的机会就会减少
		// 所以一次处理接收，只需要加锁一次，效率也相对高些
		int count = 0;
		UInt64 nowTick = TimeManager::NowTick();

		while (true) //count++<_ONCE_EXECUTE_MSG_COUNT) // NOTE: 使用数量控制时, 出现接收缓存增长到最大后断开连接
		{
			HandPacket packet = mNet->GetNetProtocol()->ReadPacket(this, &mReceiveLoopData);

			if (packet)
			{
				//if (++count>MAX_PROCESS_PACKET_NUM)
				//{
				//	NOTE_LOG("[%s:%d] 处理接收包数量超时最大数量，移除处理", GetIp(), GetPort());
				//	SetRemove(true);
				//	break;
				//}
#if _DEBUG_NET_
				Hand<IOCPBaseNet> net = GetNetHandle();
				++net->mRevPacketCount;
#endif
				try{
					OnReceivePacket(packet.getPtr());			
                    if (IsRemove())
                        return;
				}
				catch (std::exception &e)
				{
					ERROR_LOG("Net message execute error >%s", e.what());
				}
				catch (...)
				{
					ERROR_LOG("Net message [%d] execute error", packet->GetPacketID());
				}
			}
			else
				break;

			if (TimeManager::NowTick()-nowTick>1)
			{
				//int port = 0;
				//AString ip = GetRemoteAddr(port);
				//DEBUG_LOG("[%s:%d] receive packet process time %llu more then 10, may be need remove connect", ip.c_str(), port, TimeManager::NowTick()-nowTick);
				//OnProcessLongTime();
				
				break;
			}
		}
//#endif

	}
}

bool IOCPServerConnect::Process()
{	
	bool re = IOCPConnect::Process();

	if (!mbReceiveData && re)
	{
		if (TimeManager::Now() - mStartTime > mNet->GetSafeCheckOverTime())
		{
			ERROR_LOG("NET ERROR: [%s: %d] connect by  [%s:%d] Over time for connect wait safe check or reveice data, then remove", GetNetHandle()->GetIp(), GetNetHandle()->GetPort(), GetIp(), GetPort());
			mbReceiveData = true;
			Close();
		}
	}

	return re;
}

//-------------------------------------------------------------------------
