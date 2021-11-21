/********************************************************************
	created:	2014/06/20
	created:	20:6:2014   3:01
	filename: 	F:\GameCode\DaoJianXiao\BaseCommon\NetDll\Net\IOCPServerNet.h
	file path:	F:\GameCode\DaoJianXiao\BaseCommon\NetDll\Net
	file base:	IOCPServerNet
	file ext:	h
	author:		Yang Wenge
	
	purpose:	����IOCP����ģ��
	���������߳�
	���������߳��������У���ֻ��һ�������̣߳���
		�������С�GetQueuedCompletionStatus�������һ�������趨Ϊ����ѭ������ʱ��
	����Connect �ڱ��淢�ͼ������ص��ṹ
	���ͺ󣬡����ñ�ʶΪ�����У���ֻ����һ���ص��ṹ���غ��ٷ�����һ��
	����Ҳͬʱֻ����һ���ص�����
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
	// ��Ƶѭ��(���1��һ��)
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
// ÿ�������̶߳���IOCP��Eopll, ��֤������������ʹ��
//(Eopll ˮƽģʽLT������������, ������߽��������ظ�����. һ��ʹ�õ�ƿģʽET)
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
// IOCP ����������, ͬʱ������, �ɴ�10000����, ��CPUռ�÷ǳ���
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
// IOCP �ͻ�����������, ռCPUʹ���ʷǳ���
//-------------------------------------------------------------------------
class ConnectNetThread;

class Net_Export IOCPClientNet : public IOCPBaseNet
{
public:
	IOCPClientNet()
		: IOCPBaseNet(1, 1)		// NOTE: ����һ������ֻ�ܷ��䵽һ�������߳���, ����, �ͻ���ʱ, ֻ��Ҫ����һ�������߳�
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