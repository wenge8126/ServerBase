/********************************************************************
	created:	2014/06/20
	created:	20:6:2014   3:01
	filename: 	F:\GameCode\DaoJianXiao\BaseCommon\NetDll\Net\IOCPServerNet.h
	file path:	F:\GameCode\DaoJianXiao\BaseCommon\NetDll\Net
	file base:	IOCPServerNet
	file ext:	h
	author:		Yang Wenge
	
	purpose:	精简IOCP网络模型
	１　监听线程
	２　处理线程在主线中，且只有一个处理线程，　
		在主线中　GetQueuedCompletionStatus　的最后一个参数设定为　主循环限制时间
	３　Connect 内保存发送及接收重叠结构
	发送后，　设置标识为发送中，　只有下一个重叠结构返回后再发送下一个
	接收也同时只会有一个重叠任务
*********************************************************************/
#ifndef _INCLUDE_IOCPSERVERNET_H_
#define _INCLUDE_IOCPSERVERNET_H_

#include "NetHead.h"                                            
#include "NetHandle.h"
#include "BaseEventNet.h"

#include "WorkThread.h"
#include "ArrayList.h"

#if __LINUX__
#include <sys/epoll.h>
#endif
//-------------------------------------------------------------------------
class IOCPConnect;
class IocpOrEpollProcessNetThread;

class Net_Export IOCPBaseNet : public tBaseEventNet
{
	friend class IOCPConnect;
public:
	IOCPBaseNet(size_t connectCount = 1, int threadNum = _IOCP_THREAD_NUM);
	~IOCPBaseNet();

public:
	virtual HandConnect CreateConnect();
	virtual int GetConnectCount();

	virtual ConnectList& GetConnectList(){ return mConnectList; }
	 
	virtual bool StartNet(const char *szIP, int port) override;
	virtual void StopNet();

	virtual void Process(void);
	// 低频循环(间隔1秒一次)
	virtual void LowProcess(int spaceTime) override;

	virtual void CloseConnect(tNetConnect *pConnect);

	virtual int GetSafeCheckOverTime(){ return SERVER_NET_CHECK_CONNECT_WAITTIME; } 

	virtual int SendBufferMax() { return MAX_SEND_DATA_SIZE; }
	virtual int ReceiveBufferMax(){ return MAX_RECEIVE_DATA_SIZE; }	

public:
	// NOTE: in main thread.
	virtual void _AppendConnect(HandConnect newConnect);
	virtual void _RemoveConnect(HandConnect conn);
	virtual void _OnConnectStart(tNetConnect *pConnect){}

protected:
	bool						mbStop;	
	ConnectList			mConnectList;
	
#if __LINUX__	 
	epoll_event				mTempEpollEvent;
#endif

	EasyHash<short, IocpOrEpollProcessNetThread*>	mCompletionThreadList;

#if _DEBUG_NET_
public:
	int					mRevPacketCount;
#endif
};
//-------------------------------------------------------------------------
// 每个网络线程独立IOCP或Eopll, 保证无锁缓存正常使用
//(Eopll 水平模式LT下能正常接收, 解决多线接收数据重复问题. 一般使用电瓶模式ET)
class IocpOrEpollProcessNetThread : public WorkThread
{
public:
	IocpOrEpollProcessNetThread(IOCPBaseNet *pNet, int connectCount);
	~IocpOrEpollProcessNetThread();

public:
	bool _ProcessCompletion();

	// Back net process loop once wait milsecond
	virtual int OnceNetThreadMilSecond() const { return 2000; }

	virtual void onBeforeClose(void) override { _ForceClose(10); };

	virtual void backWorkThread(void)
	{
		while (IsActive())
		{
			if (!_ProcessCompletion())
			{
				Close();
				return;
			}
		}
	}

public:
	short					mID;
	IOCPBaseNet *mpNet;
	void				*m_hCompletionPort;			
#if __LINUX__
	epoll_event				*mEpollEvent;
	int							mMaxConnectCount;
	Buffer						mTempRevBuffer;

	class NetThreadHash : public EasyHash<int, IOCPConnect*>
	{
	public:
		NetThreadHash(int maxCount)
			: EasyHash(maxCount)
		{

		}

		virtual void _rehash(float rate = 2) override
		{
			WARN_LOG("Net thread hash can not resize hash data, donot change hash data");
		}
	};

	NetThreadHash	mConnectIndex;
#endif
};

//-------------------------------------------------------------------------
// IOCP 服务器网络, 同时连接数, 可达10000以上, 且CPU占用非常低
//-------------------------------------------------------------------------
class IOCPListenThread;
struct SocketInfo;

class Net_Export IOCPServerNet : public IOCPBaseNet
{
public:
	IOCPServerNet(size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM);
	~IOCPServerNet();

public:
	virtual HandConnect CreateConnect();
	
	virtual bool StartNet( const char *szIP, int nPort );
	//virtual bool StartNet( void );
	virtual void StopNet();

	//virtual tNetConnect* GetConnect(int netID) 
	//{
	//	AssertEx(0, "Can not allow use function");
	//	return NULL;
	//}
	virtual const char* GetIp(void) const { return mIp.c_str(); }
	virtual int		GetPort(void) const { return mPort; }

	virtual void OnMsgRegistered(int eventNameIndex);

	virtual bool NeedUpdateMsgIndex() const override { return false; }

public:
	virtual void Process();

public:
	virtual void OnAcceptConnect(HandConnect newConnect);
	virtual HandConnect _CreateNewConnect(SocketInfo *acceptSocket);

	virtual void _OnConnectStart(tNetConnect *pConnect) override;

protected:
	IOCPListenThread			*mListenThread;

	AString					mIp;
	int							mPort;
	bool						mStartSucceed;

};

//-------------------------------------------------------------------------
class Net_Export_H DefaultServerNet : public IOCPServerNet
{
public:
	DefaultServerNet(size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM)
		: IOCPServerNet(maxConnectCount, threadNum)
	{
		mEventCenter = MEM_NEW Logic::EventCenter();
	}
	~DefaultServerNet()
	{		
		mEventCenter._free();
	}

	virtual Logic::tEventCenter* GetEventCenter(void) const { return ((DefaultServerNet*)this)->mEventCenter.getPtr(); }

	virtual void Process() override
	{
		mEventCenter->ProcessEvent();
		IOCPServerNet::Process();
	}

public:
	AutoEventCenter			mEventCenter;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// IOCP 客户端连接网络, 占CPU使用率非常低
//-------------------------------------------------------------------------
class ConnectNetThread;

class Net_Export IOCPClientNet : public IOCPBaseNet
{
public:
	IOCPClientNet()
		: IOCPBaseNet(1, 1)		// NOTE: 由于一个连接只能分配到一个网络线程中, 所以, 客户端时, 只需要启用一个网络线程
		, mPort(0)
		, mWaitConnectThread(NULL)
	{		
	}

public:
	virtual bool Connect(const char *szIp, int nPort, int overmilSecond);

	virtual void OnConnected(){  }
	virtual void OnConnectFail(){}
	virtual void OnCloseConnect(tNetConnect *pConnect) {}

	virtual bool IsConnected(){ return !mConnectList.empty() && mConnectList[0] && !mConnectList[0]->IsDisconnect(); }
	virtual bool IsConnecting(){ return mWaitConnectThread!=NULL; }

	virtual HandConnect GetClientConnect() override
	{
		if (!mConnectList.empty())
			return mConnectList[0];
		return HandConnect();
	}

public:
	virtual bool StartNet(const char *szIP, int port){ IOCPBaseNet::StartNet(szIP, port); mIp = szIP; mPort = port; return TryReady(); }

	virtual void StopNet(void);

	//virtual tNetConnect* GetConnect(int netID) { return IOCPBaseNet::GetConnect(0); }
	virtual const char* GetIp(void) const{ return mIp.c_str(); }
	virtual int	GetPort(void) const {  return mPort; }
	virtual bool IsOk(void) { return IsConnected(); }
	virtual bool TryReady(void){ return Connect(GetIp(), GetPort(), GetConnectOverTime()); }

	virtual void Process();

	virtual void _SendSafeCode(IOCPConnect *pRecConnect);

protected:
	ConnectNetThread			*mWaitConnectThread;
	AString					mIp;
	int							mPort;
};
//-------------------------------------------------------------------------

#endif //_INCLUDE_IOCPSERVERNET_H_