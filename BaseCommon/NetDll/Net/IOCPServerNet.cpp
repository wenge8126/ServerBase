#include "IOCPServerNet.h"
#include "Common.h"
#include "SocketAPI.h"
#include "TimeManager.h"
#include "Common.h"
#include "IOCPCommon.h"

#include "IOCPConnect.h"

#include "NetIndexPacket.h"
#if __WINDOWS__
#	include <MSTcpip.h>
#include <process.h>
#else
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#endif
#define  USE_IPV6		0

#if USE_IPV6
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#endif

#include "Dump.h"
#include "UDPNet.h"
#include "NetAddress.h"

//-------------------------------------------------------------------------
bool IOCPListenThread::InitStart( const char* szIp, int nPort )
{
	Close();
#if USE_IPV6
	SOCKET	sockfd;

	struct sockaddr_in6 my_addr; 

	if ((sockfd = socket(PF_INET6, SOCK_STREAM, 0)) == -1) { // IPv6
		perror("socket");
		exit(1);
	} else
		PRINT("socket created/n");

	ULONG argp = 0;
	int r = ioctlsocket( sockfd,FIONBIO,&argp);

	memset(&my_addr, 0, sizeof(my_addr));
	/* my_addr.sin_family = PF_INET; */ // IPv4
	my_addr.sin6_family = PF_INET6;    // IPv6
	/* my_addr.sin_port = htons(myport); */ // IPv4
	my_addr.sin6_port = htons(nPort);   // IPv6
	//if (argv[3])
	//	/* my_addr.sin_addr.s_addr = inet_addr(argv[3]); */ // IPv4
	//	inet_pton(AF_INET6, argv[3], &my_addr.sin6_addr);  // IPv6
	//else
	/* my_addr.sin_addr.s_addr = INADDR_ANY; */ // IPv4
	my_addr.sin6_addr = in6addr_any;            // IPv6

	/* if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) */ // IPv4
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in6))  // IPv6
		== -1) {
			perror("bind");
			exit(1);
	} else
		PRINT("binded/n");

	mListenSocket = sockfd;

#else
	// 创建一个监听用的Socket
#if __LINUX__
	mListenSocket = SocketAPI::socket_ex(AF_INET, SOCK_STREAM, 0);
#else
	// 第3个参数IPPROTO_TCP
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#endif
	if (mListenSocket == INVALID_SOCKET)
	{
		ERROR_LOG("[%s:%d]创建监听Socket失败, 可能端口被占用", szIp, nPort);
		ErrorExit("WSASocket");

		tProcessException(NULL);
		return false;   
	}

	NOTE_LOG("========== [%s : %d] listen ===========", szIp, nPort);
	UDPNet	tempUDP;
	
	NiceData info;
	info["PID"] = (int)getpid();
	info["TID"] = (UInt64)GetCurrentThreadId();
	info["IP"] = szIp;
	info["PORT"] = nPort;
	info["LOCAL"] = NetAddress::GetLocalIP();

	AString strSend = info.ToJSON();
	tempUDP.Send("127.0.0.1", 2020, strSend.c_str(), strSend.length());

	INT opt = 1;
	SocketAPI::setsockopt_ex( mListenSocket , SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt) );

	// 绑定IP和端口
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	//SockAddr.sin_addr.s_addr = inet_addr(localIP);	// 只允许指定IP的客户端连接
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	SockAddr.sin_port = htons(nPort);
	if ( bind(mListenSocket, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR )
	{   
		ERROR_LOG("[%s:%d]绑定监听端口Socket失败", szIp, nPort);
		ErrorExit("bind");
		AssertNote(0, "[%s:%d]绑定监听端口Socket失败", szIp, nPort);
		return false;   
	}
#endif
	// 准备监听Socket连接
	if ( listen(mListenSocket, SOMAXCONN) == SOCKET_ERROR )
	{   
		ERROR_LOG("[%s:%d] Socket 开启监听 失败", szIp, nPort);
		ErrorExit("listen");
		return false;   
	}

	WorkThread::InitThread();
	DEBUG_LOG("NET: 初始网络服务成功 [%s:%d]", szIp, nPort);
	return true;
}

#if __LINUX__
void IOCPListenThread::backWorkThread()
{
	sockaddr saClient;
	SocketInfo info;

	while (IsActive())
	{
		// 处理连接请求

		socklen_t iClientSize = sizeof(saClient);
		SOCKET acceptSocket = accept(mListenSocket, &saClient, &iClientSize);
		if (acceptSocket == SOCKET_ERROR)
		{
			// 异常错误事件
			ERROR_LOG("Accetp socket error");
			continue; 
		}
#if _DEBUG_NET_
		sockaddr_in* addr_v4 = (sockaddr_in*)&saClient;
		NOTE_LOG("Accetp socket >[%s, %d]", inet_ntoa(addr_v4->sin_addr), ntohs(addr_v4->sin_port));
#endif
		//!!! NOTE: Linux 必须为非阻塞, 目的为了, 不停的尝试读取, 直到 返回 EAGAIN, 让epoll 再次触发可读状态
		int flags = fcntl(acceptSocket, F_GETFL, 0);                       //获取文件的flags值。
		fcntl(acceptSocket, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；

		mConnectListLock.lock();
		info.mSocket = acceptSocket;
		info.mAddr = saClient;
		mConnectList.push_back(info);
		mbHasConnect = true;
		mConnectListLock.unlock();
	}
}
#else
void IOCPListenThread::backWorkThread()
{
	sockaddr saClient;
	SocketInfo info;
	MEMORYSTATUSEX sysMemory;

	while(IsActive())
	{
		// 处理连接请求
#if USE_IPV6
		memset(&saClient, 0, sizeof(saClient));
		struct sockaddr_in6 their_addr;
		int iClientSize = sizeof(their_addr);
		SOCKET acceptSocket = WSAAccept(mListenSocket, (struct sockaddr *)&their_addr, &iClientSize, NULL, NULL);
		if ( acceptSocket == SOCKET_ERROR )
		{
			// 异常错误事件
			//OnError(INVALID_ID, WSAGetLastError());
			Sleep(1);
			continue;
		}			
		char buf[1024];
		PRINT("server: got connection from %s, port %d\r\n",
			/* inet_ntoa(their_addr.sin_addr), */ // IPv4
			inet_ntop(AF_INET6, &their_addr.sin6_addr, buf, sizeof(buf)), // IPv6
			/* ntohs(their_addr.sin_port), new_fd); */ // IPv4
			their_addr.sin6_port);
#else
		int iClientSize = sizeof(saClient);
		SOCKET acceptSocket = WSAAccept(mListenSocket, &saClient, &iClientSize, NULL, NULL);
		if ( acceptSocket == SOCKET_ERROR )
		{
			// 异常错误事件
			OnError(INVALID_ID, WSAGetLastError());
			continue;
		}			
#endif
        // 进行内存安全检查
		::GlobalMemoryStatusEx(&sysMemory);
		//???
		//if (sysMemory.ullAvailPhys < 128 * 1024 * 1024)
		//{
		//	sockaddr_in* addr_v4 = (sockaddr_in*)&saClient;
		//	NOTE_LOG("WARN: 内存少于128M, 放弃连接 [%s:%d]登陆连接", inet_ntoa(addr_v4->sin_addr), ntohs(addr_v4->sin_port));
		//	continue;
		//}

		mConnectListLock.lock();
		info.mSocket = acceptSocket;
		info.mAddr = saClient;
		mConnectList.push_back(info);
		mbHasConnect = true;
		mConnectListLock.unlock();
	}
}
#endif 
void IOCPListenThread::Process()
{
	if (mbHasConnect)
	{
		mConnectListLock.lock();
		mMainConnectList.swap(mConnectList);
		mbHasConnect = false;
		mConnectListLock.unlock();

		for (size_t i=0; i<mMainConnectList.size(); ++i)
		{
			HandConnect connect = _CreateNewConnect(mMainConnectList[i]);
            if (connect)
			    OnAcceptConnect(connect);
		}
		mMainConnectList.clear(false);

	}
}

void IOCPListenThread::ErrorExit( const char *szInfo )
{
	NOTE_LOG("NET ERROR: [%s]", szInfo);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IOCPBaseNet::IOCPBaseNet(size_t connectCount /*= 1*/, int threadNum/* = _IOCP_THREAD_NUM*/)
	: mbStop(false)
#if _DEBUG_NET_
	, mRevPacketCount(0)
#endif
{
	if (threadNum > 0)
	{
		DEBUG_LOG("Server net ready %d thread, for max %d connect", threadNum, connectCount);
	}
	else
	{
		ERROR_LOG("XXXXXX thread count = 0, can not net work, may be no need this net ?!");
		return;
	}
	//NOTE: Must use one net thread, because async work thread data	
	// So one thread with once IOCP or epoll

	int onceCount = (connectCount + threadNum - 1) / threadNum;
	for (short i = 0; i < threadNum; ++i)
	{
		IocpOrEpollProcessNetThread *pNetThread = MEM_NEW IocpOrEpollProcessNetThread(this, onceCount + 1);
		pNetThread->mID = i;
		mCompletionThreadList.insert(i, pNetThread);
		//Move to StartNet mCompletionThreadList[i]->InitThread();
	}
	// NOTE: InitThread 网络线程, 不哥多次执行, 老InitThread会关闭当前线程, 造成之前连接都无法再使用, 而且 mysql_init会无响应
	// 在网络关闭之前, 不可以关闭网线线程
	for (auto it = mCompletionThreadList.begin(); it.have(); it.next())
	{
		WorkThread *pNetThread = it.get();
		if (pNetThread != NULL)
			pNetThread->InitThread();
	}
}

IOCPBaseNet::~IOCPBaseNet()
{
	mbStop = true;

	for (size_t i = 0; i < mConnectList.size(); ++i)
	{
		if (mConnectList[i])
		{
			mConnectList[i]->Close();
		}
	}

	UInt64 waitTime = TimeManager::NowTick();
	if (GetConnectCount() > 0)
		NOTE_LOG("Net now exist connect %d, may be wait 3 second ...\r\n", GetConnectCount());
	while (GetConnectCount() > 0)
	{
		TimeManager::Sleep(100);
		Process();
		if (TimeManager::NowTick() - waitTime > 3000)
		{
			//PRINT("Warn: [%s:%d] Now exist connect %d, then fore close\r\n", GetIp(), GetPort(), GetConnectCount());
			break;
		}
	}

	if (GetConnectCount() > 0)
	{
		for (size_t i = 0; i < mConnectList.size(); ++i)
		{
			Hand<IOCPConnect> conn = mConnectList[i];
			if (conn)
			{
				conn->_Free();
			}
		}
		waitTime = TimeManager::NowTick();
		while (GetConnectCount() > 0)
		{
			TimeManager::Sleep(10);
			Process();
			if (TimeManager::NowTick() - waitTime > 3000)
			{
				//PRINT("Warn: [%s:%d] Now exist connect %d, then fore close\r\n", GetIp(), GetPort(), GetConnectCount());
				break;
			}
		}
	}

	for (auto it=mCompletionThreadList.begin(); it.have(); it.next())
	{
		IocpOrEpollProcessNetThread *pThread = it.get();
		if (pThread != NULL)
		{
			pThread->Close();
			delete pThread;			
		}
	}
	mCompletionThreadList.clear(true);

}

void IOCPBaseNet::_AppendConnect( HandConnect newConnect )
{
	if (mbStop)
	{
		newConnect->Close();
		return;
	}

	mConnectList.add(newConnect);
	int count = mConnectList.size();

	IOCPConnect *pRecConnect = dynamic_cast<IOCPConnect*>(newConnect.getPtr());
	AssertEx(pRecConnect!=NULL, "创建连接必须继承 IOCPConnect ");

	// 修改缓存限制
	//pRecConnect->mSendData.SetMaxLength(SendBufferMax());
	//pRecConnect->mReceiveData.SetMaxLength(ReceiveBufferMax());
	
	int i = rand() % mCompletionThreadList.size();
	IocpOrEpollProcessNetThread *pThread = mCompletionThreadList.find(i);
	AssertEx(pThread != NULL, "Must exist %d net work", i);

#if __LINUX__
	struct epoll_event ev;
	ev.data.fd = pRecConnect->mSocketID;
	//设置用于注册的读写操作事件
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET; // 电瓶模式
	if (epoll_ctl((int)(size_t)pThread->m_hCompletionPort, EPOLL_CTL_ADD, pRecConnect->mSocketID, &ev) < 0)
		ERROR_LOG("Add to epoll fail >%d", errno)
	else
		pRecConnect->mNetThreadID = i;
	pThread->mConnectIndex.insert(pRecConnect->mSocketID, pRecConnect);
#else
	// 给当前连接进来的客户端SOCKET创建IO完成端口
	if (pRecConnect->mIocpHandle)
		CloseHandle((HANDLE)pRecConnect->mIocpHandle);

	pRecConnect->mIocpHandle = CreateIoCompletionPort((HANDLE)pRecConnect->mSocketID, pThread->m_hCompletionPort, (ULONG_PTR)newConnect.getPtr(), 0);
	if ( pRecConnect->mIocpHandle == NULL )
	{
		ERROR_LOG("NET ERROR: [%s:%d] CreateIoCompletionPort", pRecConnect->GetIp(), pRecConnect->GetPort());
		pRecConnect->SetRemove(true);
		return;
	}
#endif
#if _DEBUG_NET_ 
	DEBUG_LOG("NET: ++[%s:%d] [%d] Succeed accept connect", pRecConnect->GetIp(), pRecConnect->GetPort(), count);
#endif
	// 发送一个接收请求
	pRecConnect->_Start();
	_OnConnectStart(pRecConnect);
}
// 分离包含的连接，之后可以将连接再次加入到其他IOCP网络管理里
void IOCPBaseNet::_RemoveConnect(HandConnect conn)
{
	IOCPConnect *pRecConnect = dynamic_cast<IOCPConnect*>(conn.getPtr());
	AssertEx(pRecConnect!=NULL, "创建连接必须继承 IOCPConnect ");
#if __LINUX__
	IocpOrEpollProcessNetThread *pNetThread = mCompletionThreadList.find(pRecConnect->mNetThreadID);
	AssertEx(pNetThread != NULL, "Net thread %d must exist", pRecConnect->mNetThreadID);
	struct epoll_event ev;
	ev.data.fd = pRecConnect->mSocketID;
	//设置用于删除事件
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	if (epoll_ctl((int)(size_t)pNetThread->m_hCompletionPort, EPOLL_CTL_DEL, pRecConnect->mSocketID, &ev)<0)
		ERROR_LOG("Add to epoll fail >%d", errno);
	pNetThread->mConnectIndex.erase(pRecConnect->mSocketID);
#else
	if (pRecConnect->mIocpHandle!=NULL)
		CloseHandle(pRecConnect->mIocpHandle);
	pRecConnect->mIocpHandle = NULL;
#endif
	for (int i=0; i<mConnectList.size(); ++i)
	{
		if (mConnectList[i]==conn)
		{
			mConnectList.removeAt(i);
			break;
		}
	}
	pRecConnect->mNet.setNull();
}

void IOCPBaseNet::Process( void )
{		
	for (size_t i=0; i<mConnectList.size(); )
	{
		HandConnect conn = mConnectList[i];
		if (conn)
		{		
			if (conn->Process())
			{
				++i;
				continue;
			}
			else
			{
#if _DEBUG_NET_ 
				DEBUG_LOG("NET: --[%s:%d] [%d] Now free connect socket", conn->GetIp(), conn->GetPort(), GetConnectCount()-1);
#endif
				IOCPConnect *pC = dynamic_cast<IOCPConnect*>(conn.getPtr());
				AssertEx(pC!=NULL, "Must is IOCPConnect");
				pC->_Free();

				conn->OnDisconnect();
				OnCloseConnect(pC);
			}
		}
		// 无效 或 Process 失败后, 都进行移除
		mConnectList.removeAt(i);
	}
}

void IOCPBaseNet::LowProcess(int spaceTime)
{
	for (size_t i=0; i<mConnectList.size(); ++i)
	{
		Hand<IOCPConnect> conn = mConnectList[i];
		if (conn)
			conn->ProcessPing();
	}
}

bool IOCPBaseNet::StartNet(const char *szIP, int port)
{
	// 不可在这里 InitThread 网络线程, 可能会被多次执行, 老InitThread会关闭当前线程, 造成之前连接都无法再使用, 而且 mysql_init会无响应
	// 在网络关闭之前, 不可以关闭网线线程
	return true;
}

void IOCPBaseNet::StopNet()
{
	mbStop = true;
	//IOCPListenThread::Close();

	for (int i = 0; i < mConnectList.size(); ++i)
	{
		HandConnect conn = mConnectList[i];
		if (conn)
			conn->Close();
	}

	UInt64 waitTime = TimeManager::NowTick();
	if (GetConnectCount() > 0)
		NOTE_LOG("Net now exist connect %d, may be wait 3 second ...\r\n", GetConnectCount());
	while (GetConnectCount()>0)
	{
		TimeManager::Sleep(100);
		Process();
		if (TimeManager::NowTick() - waitTime > 3000)
		{  
			//PRINT("Warn: [%s:%d] Now exist connect %d, then fore close\r\n", GetIp(), GetPort(), GetConnectCount());
			break;
		}
	}

	if (GetConnectCount() > 0)
	{
		for (size_t i = 0; i < mConnectList.size(); ++i)
		{
			Hand<IOCPConnect> conn = mConnectList[i];
			if (conn)
			{
				conn->_Free();
			}
		}
		waitTime = TimeManager::NowTick();
		while (GetConnectCount() > 0)
		{
			TimeManager::Sleep(10);
			Process();
			if (TimeManager::NowTick() - waitTime > 3000)
			{
				//PRINT("Warn: [%s:%d] Now exist connect %d, then fore close\r\n", GetIp(), GetPort(), GetConnectCount());
				break;
			}
		}
	}

	for (auto it = mCompletionThreadList.begin(); it.have(); it.next())
	{
		WorkThread *pNetThread = it.get();
		if (pNetThread != NULL)
			pNetThread->Close();
	}

	//for (int i = 0; i < mCompletionThreadList.size(); ++i)
	//{
	//	mCompletionThreadList[i]->Close();
	//}
}

int IOCPBaseNet::GetConnectCount()
{
	return (int)mConnectList.size();
	//int count = 0;
	//for (size_t i=0; i<mConnectList.size(); ++i)
	//{
	//	if (mConnectList[i])
	//		++count;
	//	else
	//		break;
	//}
	//return count;
}

//void IOCPBaseNet::CloseConnect( int netID )
//{
//	tNetConnect *p = GetConnect(netID);
//	if (p!=NULL)
//	{			
//		p->Close();
//	}
//}

void IOCPBaseNet::CloseConnect( tNetConnect *pConnect )
{
	if (pConnect!=NULL)
	{			
		pConnect->Close();
	}
}

HandConnect IOCPBaseNet::CreateConnect()
{
	return MEM_NEW IOCPConnect();
}

//-------------------------------------------------------------------------*/
//-------------------------------------------------------------------------
HandConnect IOCPServerNet::_CreateNewConnect( SocketInfo *acceptSocket )
{
	HandConnect conn = CreateConnect();
    if (!conn)
        return conn;
	IOCPConnect *pRecConnect = dynamic_cast<IOCPConnect*>(conn.getPtr());
	AssertEx(pRecConnect!=NULL, "创建连接必须继承 IOCPConnect ");
	pRecConnect->mNet = GetSelf();
	pRecConnect->mSocketID = acceptSocket->mSocket;
	sockaddr_in *p = (sockaddr_in *)&(acceptSocket->mAddr);
	pRecConnect->mIp = inet_ntoa(p->sin_addr);
	pRecConnect->mPort = ntohs( p->sin_port );

	return conn;
}

IOCPServerNet::IOCPServerNet( size_t maxConnectCount /*= NET_CONNECT_MAX*/, int threadNum ) 
	: IOCPBaseNet(maxConnectCount, threadNum)
	, mPort(0)
	, mStartSucceed(false)
{
	mListenThread = MEM_NEW IOCPListenThread(this);
}

IOCPServerNet::~IOCPServerNet()
{
	mListenThread->Close();
	delete mListenThread;
	mListenThread = NULL;
}


bool IOCPServerNet::StartNet( const char *szIP, int nPort )
{
	mIp = szIP;
	mPort = nPort;
	
	if (GetSafeCode()==0)
	{
		WARN_LOG("[%s:%d]网络连接安全验证码为0, 默认不进行安全验证", szIP, nPort); 
		NOTE_LOG("NOTE: 连接[%s:%d] 不需要发送验证码, 否则解包错误", szIP, nPort);
	}

	{
		mStartSucceed = mListenThread->InitStart(GetIp(), GetPort());
		//mStartSucceed = true;
		NOTE_LOG("[%s:%d] server start, safe code %d", szIP, nPort, GetSafeCode());
		if (mStartSucceed)
			IOCPBaseNet::StartNet(szIP, nPort);
		return mStartSucceed;
	}
	return false;
}

void IOCPServerNet::StopNet()
{
	mListenThread->Close();
	IOCPBaseNet::StopNet();
}

void IOCPServerNet::OnMsgRegistered(int eventNameIndex)
{
	// 通知给所有的连接
	NotifyEventIndexPacket	indexPacket;

	if (GetEventCenter()->GenerateMsgIndex(indexPacket.GetData(), eventNameIndex))
	{
		for (size_t i=0; i<mConnectList.size(); ++i)
		{
			if (mConnectList[i])
			{
				mConnectList[i]->Send(&indexPacket, false);
			}
		}
	}

}

void IOCPServerNet::Process()
{
	if (mStartSucceed)
	{
		mListenThread->Process();
		IOCPBaseNet::Process();
	}
}

void IOCPServerNet::OnAcceptConnect( HandConnect newConnect )
{
	IOCPServerConnect *p = dynamic_cast<IOCPServerConnect*>(newConnect.getPtr());
	p->mStartTime = TimeManager::Now();

	IOCPBaseNet::_AppendConnect(newConnect);
}

HandConnect IOCPServerNet::CreateConnect()
{
	return MEM_NEW IOCPServerConnect();
}

void IOCPServerNet::_OnConnectStart( tNetConnect *pConnect )
{
	// 通知给所有的连接
	NotifyEventIndexPacket	indexPacket;

	if (GetEventCenter()->GenerateMsgIndex(indexPacket.GetData(), 0))
	{
		pConnect->Send(&indexPacket, false);
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool IOCPClientNet::Connect( const char *szIp, int nPort, int overmilSecond )
{
	mIp = szIp;
	mPort = nPort;

	if (mIp=="" || mPort<=0)
		return false;

	if (mWaitConnectThread!=NULL)
	{
		mWaitConnectThread->Close();
		delete mWaitConnectThread;
	}
	mWaitConnectThread = MEM_NEW ConnectNetThread();
	mWaitConnectThread->StartConnect(szIp, nPort, overmilSecond);

	return true;
}

bool IOCPClientNet::AwaitConnect(const char *szIp, int nPort, int overmilSecond)
{
	if (CORO == 0)
	{
		ERROR_LOG("AwaitConnect must in coro");
		return false;
	}
	mIp = szIp;
	mPort = nPort;

	if (mIp == "" || mPort <= 0)
		return false;

	if (mWaitConnectThread != NULL)
	{
		mWaitConnectThread->Close();
		delete mWaitConnectThread;
	}
	mWaitConnectThread = MEM_NEW ConnectNetThread();
	mWaitConnectThread->mConnectCoroID = CORO;
	mWaitConnectThread->StartConnect(szIp, nPort, overmilSecond);

	YIELD;

	return IsConnected();
}

void IOCPClientNet::StopNet( void )
{
	if (mWaitConnectThread!=NULL)
	{
		mWaitConnectThread->Close();
		delete mWaitConnectThread;
		mWaitConnectThread = NULL;
	}

	for (size_t i=0; i<mConnectList.size(); ++i)
	{
		if (mConnectList[i])
			mConnectList[i]->Close();
	}
	Process();
	if (GetClientConnect())
	{
		TimeManager::Sleep(10);
		Process();
	}
	//IOCPBaseNet::StopNet();
}

void IOCPClientNet::Process()
{
	if (mWaitConnectThread!=NULL)
	{
		if (mWaitConnectThread->IsConnectFinish())
		{
			if (mWaitConnectThread->mSocket!=0)
			{
				HandConnect conn = CreateConnect();

				IOCPConnect *pRecConnect = dynamic_cast<IOCPConnect*>(conn.getPtr());
				AssertEx(pRecConnect!=NULL, "创建连接必须继承 IOCPConnect ");
				pRecConnect->mNet = GetSelf();
				pRecConnect->mSocketID = mWaitConnectThread->mSocket;
				mWaitConnectThread->mSocket = 0;
				pRecConnect->mIp = mIp.c_str();
				pRecConnect->mPort = mPort;
				//tNetConnect *p = GetClientConnect().getPtr(); //GetConnect(0);
				//if (p!=NULL)
				//	p->Close();
				for (size_t i=0; i<mConnectList.size(); ++i)
				{
					if (mConnectList[i])
					{		
						mConnectList[i]->Close();
					}
				}

				UInt64 waitTime = TimeManager::NowTick();

				while (GetConnectCount() > 0)
				{
					TimeManager::Sleep(100);
					IOCPBaseNet::Process();
					if (TimeManager::NowTick() - waitTime > 3000)
					{
						ERROR_LOG("Net now exist connect %d \r\n", GetConnectCount());
						break;
					}
				}
				
				_AppendConnect(conn);
				_SendSafeCode(pRecConnect);
				conn->OnConnected();
				OnAddConnect(conn.getPtr());
				OnConnected();
			}
			else
			{
				Log("NET: WARN Connect fail [%s:%d]", mIp.c_str(), mPort);
				OnConnectFail();
			}

			if (mWaitConnectThread->mConnectCoroID != 0)
				RESUME(mWaitConnectThread->mConnectCoroID);

			if (mWaitConnectThread!=NULL)
			{
				mWaitConnectThread->Close();
				delete mWaitConnectThread;
				mWaitConnectThread = NULL;
			}
		}
		else if (mWaitConnectThread->IsOverTime())
		{
			Log("NET: WARN Connect over time [%s:%d]", mIp.c_str(), mPort);
			mWaitConnectThread->Close();
			delete mWaitConnectThread;
			mWaitConnectThread = NULL;

			OnConnectFail();

			if (mWaitConnectThread->mConnectCoroID != 0)
				RESUME(mWaitConnectThread->mConnectCoroID);
		}

	}
	else
		IOCPBaseNet::Process();
}

void IOCPClientNet::_SendSafeCode(IOCPConnect *pConnect)
{
	if (GetSafeCode()!=0)
	{		
		int safe = GetSafeCode();
		if (safe!=0)
		{
            pConnect->_Send((const CHAR*)&safe, sizeof(int));
		}
	}
}


//-------------------------------------------------------------------------

ConnectNetThread::ConnectNetThread() 
	: mPort(0)
	, mSocket(0)
	, mbFinish(false)
{
	mOverTime = TimeManager::NowTick();	
}

void ConnectNetThread::StartConnect( const char *sIp, int nPort, int milSecondOverTime )
{
	mbFinish = false;
	mIP = sIp;
	mPort = nPort;
	//??? DEBUG_LOG("开始连接[%s:%d]", sIp, nPort);
	mOverTime = TimeManager::NowTick() + milSecondOverTime*1000;
	InitThread();
}

bool ConnectNetThread::IsConnectFinish()
{
	return mbFinish;
//#if __LINUX__
//	return mbFinish;
//#else
//	if (mBackTread == NULL)
//		return true;
//	//!!! 注意，此处如果存在尝试连接时，耗时1毫秒判断完成
//	DWORD dw = WaitForSingleObject( (HANDLE)mBackTread, 1 );
//	if (dw==WAIT_OBJECT_0 || dw==WAIT_FAILED)
//	{
//		CloseHandle((HANDLE)mBackTread);
//		mBackTread = NULL;
//		return true;
//	}
//	return false;
//#endif
}

bool ConnectNetThread::IsOverTime()
{
	return TimeManager::NowTick()>mOverTime;
}

void ConnectNetThread::backWorkThread( void )
{
#if __LINUX__
	mSocket = SocketAPI::socket_ex(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (mSocket == INVALID_SOCKET)
	{
		DEBUG_LOG("NET ERROR: Client socket create fail");
		return;
	}
#else
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (mSocket == INVALID_SOCKET)
	{
		DEBUG_LOG("NET ERROR: IOCP client socket create fail > [%u]", GetLastError());
		return;
	}
#endif
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( mIP.c_str() );
	addr.sin_port = htons(mPort);

	SocketAPI::setsocketnonblocking_ex(mSocket, FALSE);
	if (SocketAPI::connect_ex( mSocket , (const struct sockaddr *)&addr , sizeof(addr) )==TRUE)
		SocketAPI::setsocketnonblocking_ex(mSocket, TRUE);
	else
	{
		SocketAPI::closesocket_ex(mSocket);
		mSocket = 0;
	}
	mbFinish = true;
}

ConnectNetThread::~ConnectNetThread()
{
	if (mSocket!=0)
		SocketAPI::closesocket_ex(mSocket);
	mSocket = 0;
}
//-------------------------------------------------------------------------

void NetListen::ProcessToNet( tNetHandle *destHandle )
{

	if (mbHasConnect)
	{
		mConnectListLock.lock();
		mMainConnectList.swap(mConnectList);
		mbHasConnect = false;
		mConnectListLock.unlock();

		for (size_t i=0; i<mMainConnectList.size(); ++i)
		{
			IOCPServerNet *pNet = dynamic_cast<IOCPServerNet*>(destHandle);
			HandConnect connect = pNet->_CreateNewConnect(&mMainConnectList[i]);
			pNet->OnAcceptConnect(connect);		
		}
		mMainConnectList.clear(false);
	}

	//if (mbHasConnect)
	//{
	//	mConnectListLock.lock();

	//	if (mConnectList.empty())
	//	{
	//		mbHasConnect = false;
	//		mConnectListLock.unlock();
	//		return;
	//	}

	//	if (mCheckCount<mNetList.size() && mConnectList.size()<mNetList.size())
	//	{
	//		if (mNetList.size()>0 && mNetList.get(0)!=destHandle)
	//		{
	//			mCheckCount++;
	//			mConnectListLock.unlock();
	//			return;
	//		}
	//	}						
	//	IOCPServerNet *pNet = dynamic_cast<IOCPServerNet*>(destHandle);
	//	HandConnect connect = pNet->_CreateNewConnect(&mConnectList.back());
	//	pNet->OnAcceptConnect(connect);			
	//	mConnectList.pop_back();

	//	mbHasConnect = !mConnectList.empty();
	//	mCheckCount = 0;
	//	mConnectListLock.unlock();
	//}
}


//-------------------------------------------------------------------------
// 网络处理线程
//-------------------------------------------------------------------------
IocpOrEpollProcessNetThread::IocpOrEpollProcessNetThread(IOCPBaseNet *pNet, int connectCount)
	: m_hCompletionPort(NULL)
	, mID(0)
#if __LINUX__
	, mEpollEvent(NULL)
	, mConnectIndex(connectCount*1.3f)	
#endif
{
	mpNet = pNet;

#if __LINUX__
	m_hCompletionPort = (void*)epoll_create(connectCount + 1);
	mEpollEvent = new epoll_event[connectCount + 1];
	mMaxConnectCount = connectCount;
	mTempRevBuffer.resize(1024 * 16);
#else
	// 创建IO完成端口
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hCompletionPort == NULL)
	{
		DEBUG_LOG("NET ERROR: CreateIoCompletionPort");
		return;
	}
#endif
}

IocpOrEpollProcessNetThread::~IocpOrEpollProcessNetThread()
{
#if __WINDOWS__
	// 需要发送一个退出线程的指令
	PostQueuedCompletionStatus(m_hCompletionPort, 0, NULL, NULL);

	CloseHandle(m_hCompletionPort);
	m_hCompletionPort = NULL;
#else
	if (mEpollEvent != NULL)
	{
		delete[] mEpollEvent;
		mEpollEvent = NULL;
	}
	close((int)(size_t)m_hCompletionPort);
	m_hCompletionPort = NULL;
#endif
}

#if __LINUX__

bool IocpOrEpollProcessNetThread::_ProcessCompletion()
{
	UInt64 now = TimeManager::NowTick();
	do {
		if (m_hCompletionPort == NULL)
		{
			//Close();
			return false;
		}
		int nfds = epoll_wait((int)(size_t)m_hCompletionPort, mEpollEvent, mMaxConnectCount, OnceNetThreadMilSecond());
		if (nfds < 0)
		{
			//ERROR_LOG("epoll: err >%d", errno);
			//Close();
			return true;
		}
		else if (nfds == 0)
			return true;
		for (int i = 0; i < nfds; ++i)
		{
			epoll_event &ev = mEpollEvent[i];
			IOCPConnect *conn = mConnectIndex.find(ev.data.fd);
			if (conn==NULL)
			{
				AssertNote(0, "Code error");
				ev.data.fd = -1;
				continue;
			}
			if (ev.events&EPOLLIN)
			{
				size_t n = 0;
				bool bHave = false;
				do
				{
					if ((n = read(ev.data.fd, mTempRevBuffer.data(), mTempRevBuffer.size())) == (size_t)-1) {
						if (errno == ECONNRESET) {
							//close(sockfd);
							mpNet->CloseConnect(conn);
							ev.data.fd = -1;
						}
						else if (errno != EAGAIN)
						{
							ERROR_LOG("Read socket error");
						}
						else
						{
							//NOTE_LOG("Read socket finish >%u", n);
						}
						break;
					}
					else if (n == 0) {
						if (!bHave)
						{
							NOTE_LOG("Net socket close");
							mpNet->CloseConnect(conn);
							ev.data.fd = -1;
						}
						break;
					}
					bHave = true;
					conn->mReceiveLoopData.SendData(mTempRevBuffer.data(), n);

					//DEBUG_LOG("++++Socket rev %llu, thread id %llu\r\n", n, mThreadID);
					//break;
				} while (true);

			}
			if (ev.events&EPOLLOUT)
			{
				conn->_SendTo();
				//DEBUG_LOG(" >>>> Send to \r\n");
				// 当工作线程取完缓存后, 检查待传缓存有数据, 则重触发发送事件, 在此将待传数据继续转移
				conn->mReceiveLoopData.ProcessSendData();
			}
		}
	} while (TimeManager::NowTick() - now < OnceNetThreadMilSecond());

	return true;
}

#else

bool IocpOrEpollProcessNetThread::_ProcessCompletion()
{
	UInt64 now = TimeManager::NowTick();
	do
	{
		if (m_hCompletionPort == NULL)
		{
			//Close();
			return false;
		}
		PULONG_PTR netConnectPtr = NULL;
		DWORD   BytesTransferred = 0;
		BaseOverlapped* pSOverlapped = NULL;
		// NOTE: 无事件时, 10毫秒执行一次, 让工作线程处理数据后, 再进行中转接收数据
		if (GetQueuedCompletionStatus(m_hCompletionPort, &BytesTransferred,
			(PULONG_PTR)&netConnectPtr, (LPOVERLAPPED*)&pSOverlapped, 5000) == 0)
		{
			//DEBUG_LOG(" \r\n\r\n########### [%s: %d] GetQueuedCompletionStatus \r\n\r\n", mpNet->GetIp(), mpNet->GetPort());

			if (netConnectPtr == NULL)
			{
				//ERROR_LOG("GetQueuedCompletionStatus get connect is NULL >%u, now thread close", ::GetLastError());	// error: 258 Wait time over
				//Close();
				return true;
			}
			IOCPConnect *pConnect = dynamic_cast<IOCPConnect*>((tNetConnect*)netConnectPtr);

			DWORD dwLastError = GetLastError();
			if (dwLastError == WAIT_TIMEOUT || pConnect == NULL)
				break;			

			if (dwLastError == ERROR_OPERATION_ABORTED || dwLastError == ERROR_NETNAME_DELETED)
			{				
				WARN_LOG("WARN: IOCP error>[%u]", dwLastError);
				mpNet->CloseConnect(pConnect);
			}
			// 64错误号表示"指定的网络名不再可用"，客户端异常退出会产生这个错误号
			else if (dwLastError != WAIT_TIMEOUT && dwLastError != ERROR_SEM_TIMEOUT)
			{
				ERROR_LOG("GetQueuedCompletionStatus   发生了如下错误： %d\n", GetLastError());
				mpNet->CloseConnect(pConnect);
			}
			else
				WARN_LOG("WARN: IOCP error>[%u]", dwLastError);

			if (pSOverlapped != NULL)
				pSOverlapped->mbWaiting = false;

			break;
		}

		IOCPConnect *pConnect = dynamic_cast<IOCPConnect*>((tNetConnect*)netConnectPtr);
		if (pSOverlapped == NULL || pConnect == NULL) {
			PRINT("pSOverlapped == NULL or connect is null\n");
			break;
		}

		if (pSOverlapped->IoMode == IoNotifySend)
		{
			pConnect->_SendTo();
			// 这个是技巧性的设计, 如果Net线程端还有待传数据, 则直接触发发送过程, 然后再发送事件后, 进行继续转移接收的待转移数据
			pConnect->mReceiveLoopData.ProcessSendData();
		}
		else if (pSOverlapped->IoMode == IoSend)
		{
			// 发送事件
			SendOverlapped* pSendOverlapped = (SendOverlapped*)pSOverlapped;
#if _DEBUG_NET_
			pConnect->mSendSize += BytesTransferred;
			//DEBUG_LOG("[%s:%d] sended >%u, all %llu", pConnect->GetIp(), pConnect->GetPort(), BytesTransferred, pConnect->mSendSize);
			int remotePort = 0;
			NOTE_LOG("--------- Net [%s:%d]  >>> [%s: %d] %u", pConnect->GetNetHandle()->GetIp(), pConnect->GetNetHandle()->GetPort(), pConnect->GetRemoteAddr(remotePort).c_str(), remotePort, BytesTransferred);
#endif
			pSendOverlapped->mbWaiting = false;
			pConnect->mSendLoopData.skip(BytesTransferred); // _ForceRestoreHead(pConnect->mSendData.GetHead() + BytesTransferred);
			pConnect->_SendTo();
			
		}
		else if (pSOverlapped->IoMode == IoRecv)
		{
			if (BytesTransferred == 0)
			{
				// 直到要求断开, 才设置为false, 主线才会执行移除处理, 否则会在 _PostReceive  中, 有可能被移除, 从而同步问题报错
				pSOverlapped->mbWaiting = false;
				if (pConnect != NULL)
				{
					DEBUG_LOG("NET: May be other party connect close [%s:%d], Now set remove", pConnect->GetIp(), pConnect->GetPort());
					pConnect->Close();
				}
			}
			else
			{
#if _DEBUG_NET_
				pConnect->mReceiveSize += BytesTransferred;
				//DEBUG_LOG("> [%s:%d] rev size > %u, all %llu", pConnect->GetIp(), pConnect->GetPort(), BytesTransferred, pConnect->mReceiveSize);
				int remotePort = 0;
				 NOTE_LOG("<<<<<< Net [%s:%d] rev size from [%s: %d]  %u ", pConnect->GetNetHandle()->GetIp(), pConnect->GetNetHandle()->GetPort(), pConnect->GetRemoteAddr(remotePort).c_str(), remotePort, BytesTransferred);
#endif

				mpNet->AddReceiveTotalSize(BytesTransferred);
				RecvOverlapped* pRecvOverlapped = (RecvOverlapped*)pSOverlapped;

				pConnect->mReceiveLoopData.SendData(pRecvOverlapped->buffer, BytesTransferred);
				pConnect->_PostReceive();

			}
		}
	} while (TimeManager::NowTick() - now < OnceNetThreadMilSecond());

	return true;
}

#endif //__LINUX__