
/********************************************************************
	created:	2015/05/24
	created:	24:5:2015   19:50
	filename: 	G:\HomeGame\Code\BaseCommon\NetDll\Net\UDPbroadcast.h
	file path:	G:\HomeGame\Code\BaseCommon\NetDll\Net
	file base:	UDPbroadcast
	file ext:	h
	author:		YangWenge
	
	purpose:	UDPֱ����ת����
				���������߳�
				ֻ��תָ����ַ������Դ����ͬʱֻ����һ������ת��Դ��ַ
*********************************************************************/
#ifndef _INCLUDE_UDPBROADCAST_H_
#define _INCLUDE_UDPBROADCAST_H_

#include "UDPThreadNet.h"
#include "NetHead.h"
#include "EventCenter.h"
#include "ServerEvent.h"

#include "Easyhash.h"

//using namespace Logic;
//-------------------------------------------------------------------------
// 1 ����֪ͨ�ͻ���Ҫ����ָ����֤��ַ������֤KEY��Ŀ�Ļ�ȡ�ͻ��˵Ĺ���UDP��ַ
// 2 ��֤ͨ���󣬽��㲥�����ַ���͸��ͻ��ˣ��ͻ��˼���������UDP��Ϣ���Ա��ܽ��յ��㲥����
//-------------------------------------------------------------------------
class Net_Export UDPBroadcast : public  UDPThreadNet
{
public:
	virtual void OnAppendTarget(UInt64 targetAddr, HandConnect tcpConnect, AutoEvent notifyEvent) = 0;

	AutoEvent StartNotifyClient(HandConnect tcpConnect);

	void SetSourceAddr(UInt64 sourceAddr);
	UInt64 GetSourceAddr();

public:
	// NOTE: ���� revData ֻ�ܼ�ʱʹ��, ִ�к�, �����ܱ��л��������߳�ʹ��
	virtual void OnReceiveMsg(UInt64 scrAddr, DataStream *revData, int revSize);

	static void RegisterNetEvent(AutoEventCenter center);

	virtual void Process()
	{
		UDPThreadNet::Process();
		//mEventCenter->ProcessEvent();
	}	

	void _AppendConnect(UInt64 newAddr);

	void _RemoveConnect(UInt64 targetAdd);

	void AppendWaitEvent(UInt64 checkKey, AutoEvent waitEvent)
	{
		mWaitEventList[checkKey] = waitEvent;
	}

protected:
	typedef EasyHash<UInt64, AutoEvent>		WaitEventList;

	WaitEventList	mWaitEventList;

public:
	UDPBroadcast();

	virtual ~UDPBroadcast();

public:
	virtual bool StartNet( const char *szIP, int lowPort, int highPort );
	
	virtual void Close()
	{
		mpBroadcastUDPReceiveThread->_ForceClose(12);
		//mpBroadcastUDPReceiveThread->Close();
		UDPThreadNet::Close();
	}

	AString getIpPort(int &checkPort, int &broadcastServerPort)
	{ 
		mBroadcastUDP.getIpPort(broadcastServerPort); 

		return UDPThreadNet::getIpPort(checkPort);
	}

public:
	// ������Ϣ����
	//AutoEventCenter		mEventCenter;
	AString				mServerIP;
	int					mCheckPort;
	int					mBroadcastPort;

protected:
	// NOTE: �߳���ִ��
	void _ProcessNet();

protected:
	// �߳���ʹ��
	UDPNet			mBroadcastUDP;
	DataBuffer		mBroadcastReceiveBuffer;
	UInt64			mRevBroadcastAddr;

	PoolList<UInt64>	mTargetAddrList;
	
	CsLock			mListLock;
	bool			mbHaveWaitConnect;

	UInt64			mDataSourceAddr;

	UInt64			mSetSourceAddr;
	bool			mbHaveSourceAddrChanged;

	CsLock			mSourceAddLock;

	struct WaitConnect 
	{
		bool	mbAppend;
		UInt64	mTargetAddr;

		WaitConnect()
			: mbAppend(true)
			, mTargetAddr(0)
		{

		}
	};

	PoolList<WaitConnect>	mWaitConnectList;

	class UDPReceiveThread;
	UDPReceiveThread	*mpBroadcastUDPReceiveThread;

	class UDPReceiveThread : public WorkThread
	{
	public:
		UDPReceiveThread(UDPBroadcast *pNet)
			: mpNet(pNet)
		{

		}

		virtual void backWorkThread(void)
		{
			while (IsActive())
			{
				mpNet->_ProcessNet();
			}
		}

		virtual void onBeforeClose(void) 
		{
			_ForceClose(10);		
		};

	public:
		UDPBroadcast *mpNet;
	};
};
//-------------------------------------------------------------------------
//class BoroadcastEventCenter : public EventCenter
//{
//public:
//	BoroadcastEventCenter(UDPBroadcast *pNet)
//		: mpNet(pNet)
//	{
//
//	}
//
//	UDPBroadcast	*mpNet;
//};

//-------------------------------------------------------------------------
class CS_NotityConnectUDP : public Logic::tBaseNetEvent
{
public:
	virtual bool _DoEvent();

	bool OnUDPResponce(UInt64 targetAdd, UInt64 checkCode);

	virtual bool _OnTimeOver()
	{
		return true;
	}

public:
	UInt64 mCheckCode;

public:
	Hand<UDPBroadcast>	mUDP;
};
//-------------------------------------------------------------------------


#endif