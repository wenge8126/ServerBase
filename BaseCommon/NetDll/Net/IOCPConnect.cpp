
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


// Linux ʵ�� 
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
			WARN_LOG("�����Ƴ�״̬���� [%d] S, Ӧ��ʱ����", _DEBUG_REMOVE_OVER_TIME);
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
		// ����Ǽ����Ե����, ���Net�̶߳˻��д�������, ��ֱ�Ӵ������͹���, Ȼ���ٷ����¼���, ���м���ת�ƽ��յĴ�ת������
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

// NOTE:  ���������߳���ִ��
void IOCPConnect::_SendTo()
{
	if (mSocketID <= 0 || mNeedRemove)
		return;

	if (mSendOverlapped->mbWaiting)
	{
		if (TimeManager::Now() - mLastSendTime > _IOCP_SEND_OVERTIME)
		{
			WARN_LOG("[%d][%s:%d]���ӷ��ͳ�ʱ>[%d] Sec, ǿ�ƹر�����", GetNetID(), GetIp(), GetPort(), _IOCP_SEND_OVERTIME);
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
	// ����һ����������¼�
	SendOverlapped* pSendOverlapped = mSendOverlapped;

	int dataSize = 0;
	//if (len > pSendOverlapped->WsaBuf.len)
	//	len = pSendOverlapped->WsaBuf.len;
	//mSendLoopData._read(pSendOverlapped->WsaBuf.buf, len);
	//dataSize = len;

	pSendOverlapped->WsaBuf.buf = mSendLoopData.GetDataPtr(dataSize); // pSendOverlapped->buffer;
	AssertNote(dataSize > 0, "Must has send data");
	pSendOverlapped->WsaBuf.len = dataSize;

	// �����¼�
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
				// WSAECONNRESET(10054) Զ�̿ͻ���ǿ�ƹر�����
				DEBUG_LOG("[%d] [%s:%d] Other part connect close", iError, GetIp(), GetPort());
			}
			else
				ERROR_LOG("IOCPServer::SendData - WSASend() ���������´���   %d\n", iError);
			// �쳣�����¼�
			//OnError(nClient, iError);
			// �ر�SOCKET
			mSendOverlapped->mbWaiting = false;
			SetRemove(true);

			return;
		}
	}
}

// NOTE:  ���������߳���ִ��
void IOCPConnect::_PostReceive()
{
	if (mNeedRemove || mSocketID <= 0)
		return;

	// ����ɶ˿��߳���, ����������Ϊtrue, Ŀ���Ƿ�ֹ, Ͷ��ʱ, ���Ƴ�
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

			// �쳣����
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
	//��ȡremote ip and port
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
		
		// ����PING
		HandPacket pingPacket = GetNetHandle()->GetNetProtocol()->CreatePacket(eNotifyHeartBeat);

		Send(pingPacket.getPtr(), false);
		//int port = 0;
		//AString ip = GetRemoteAddr(port);
		//NOTE_LOG(" = Send heart beat :  [%s:%d] > remote [%s:%d] ", GetLocalIP().c_str(), GetLocalPort(), ip.c_str(), port);
	}
	if (TimeManager::NowTick()-mLastReceivePingTime>ReceivePingOverSecond()*1000)
	{		
		// ����PING��ʱ
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
			// �������false, ������ͷ�
				return false;
		}
#if		_DEBUG_NET_
		else if (TimeManager::Now()-mRemoveTime>_DEBUG_REMOVE_OVER_TIME)
		{
			WARN_LOG("�����Ƴ�״̬���� [%d] S, Ӧ��ʱ����", _DEBUG_REMOVE_OVER_TIME);
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
			WARN_LOG("�Ƴ�����ʱ�����ջ����д��� [%u] ����δ����", mReceiveLoopData.dataSize());
#endif
    // NOTE: ��Ϊʹ�����߳̽���, ��ʹ���˽��ջ���Ŀռ�, �ڷ���֮ǰ, ��������
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
		ERROR_LOG("Socket close �߼�˳�����, �ر�ʱ, socket ��δ�ر�");
		_Free();
	}

	delete mSendOverlapped;
	delete mRecvOverlapped;
	delete mForSendNotifyOverlapped;
	mSendOverlapped = NULL;
	mRecvOverlapped = NULL;
	mForSendNotifyOverlapped = NULL;
#endif
	// ֻ��Ҫ�ر�Socket���ɣ�����ر�Iocp Handle, ������IOCP����Ч����������Ҳ��������������
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

				// NOTE: һ��ѭ���д���̫��, ����̫��ʱ��
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
	//��������ע��Ķ������¼�
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
		// NOTE: Ĭ����֤��Ϊ0ʱ, �����а�ȫ����֤
		// ֻ����֤OK���ִ��OnConnected()
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
		// NOTE: ���ÿ���������������ܻ���ִ���¼���ͬʱ������������ݣ������������ӵõ�ִ�еĻ���ͻ����
		// ����һ�δ�����գ�ֻ��Ҫ����һ�Σ�Ч��Ҳ��Ը�Щ
		int count = 0;
		UInt64 nowTick = TimeManager::NowTick();

		while (true) //count++<_ONCE_EXECUTE_MSG_COUNT) // NOTE: ʹ����������ʱ, ���ֽ��ջ�������������Ͽ�����
		{
			HandPacket packet = mNet->GetNetProtocol()->ReadPacket(this, &mReceiveLoopData);

			if (packet)
			{
				//if (++count>MAX_PROCESS_PACKET_NUM)
				//{
				//	NOTE_LOG("[%s:%d] ������հ�������ʱ����������Ƴ�����", GetIp(), GetPort());
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
