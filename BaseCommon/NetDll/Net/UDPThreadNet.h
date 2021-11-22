//-------------------------------------------------------------------------
/*!
 * \file UDPThreadNet.h
 *
 * \author Administrator
 * \date ���� 2015
 *
 * ����: ʵ��UDP����, ���첽����
 */
//-------------------------------------------------------------------------
#ifndef _INCLUDE_UDPTHREADNET_H_
#define _INCLUDE_UDPTHREADNET_H_

#include "LoopDataStream.h"
#include "UDPNet.h"
#include "PoolLoop.h"
#include "WorkThread.h"
#include "Lock.h"
#include "DataBuffer.h"
#include "NetHead.h"
#include "ThreadEvent.h"

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
class UDPSenderTool;
class Net_Export UDPThreadNet : public Base<UDPThreadNet>
{
	friend class UDPByTcpClientConnect;
	friend class UDPSenderTool;

public:
	class DataList : public PoolLoop<DataBuffer>
	{
	public:
		DataList(int freeMaxCount)
			: PoolLoop<DataBuffer>(freeMaxCount){}
		virtual void _FreeNode(Node *pNode)
		{ 
			//if (mFreeMaxCount>0 && mFreeNodeList.size()>=mFreeMaxCount)
			//	delete pNode; 
			//else
			{
				pNode->mpPreviou = NULL;
				//pNode->mValue.clear(false);
				if (mFreeNodeRoot==NULL)
				{ 
					pNode->mpNext = NULL;
					mFreeNodeRoot = pNode;
				}
				else
				{
					pNode->mpNext = mFreeNodeRoot;
					mFreeNodeRoot = pNode;
				}
			}
		}
	};

	//typedef LoopPool<AutoData>	DataList;	

public:
	// NOTE: ���� revData ֻ�ܼ�ʱʹ��, ִ�к�, �����ܱ��л��������߳�ʹ��
	virtual void OnReceiveMsg(UInt64 scrAddr, DataStream *revData, int revSize) = 0;

public:
	UDPThreadNet(bool bThreadSend = true);
	
	virtual ~UDPThreadNet();

public:
	virtual bool StartNet( const char *szIP, int lowPort, int highPort );
	virtual void Close();
	AString getIpPort(int &port){ return mUDP.getIpPort(port); }

	bool Send(DataStream *sendData, UInt64 destAddr);
	bool Send(DataStream *sendData, const char *szIp, int port);

	// NOTE: ���մ˹��ܷ��͵����ݰ���Ŀǰֻʵ����C#
	bool SendPacket(DataStream *sendData, Array<UInt64> &destAddr, UInt64 packetCode);

	virtual void Process();

protected:
	// NOTE: �߳���ִ��
	void _ProcessNet();	

	// NOTE: �����߳̽��յ�����
	virtual void _onThreadReceive(UInt64 scrAddr, DataStream *revData, int revSize){}

protected:
	UDPNet			mUDP;
	UInt64			mRevAddr;

	DataList::Node *mpNewNode;
	DataList		*mThreadReceiveDataList;
	DataList		*mMainReceiveDataList;

	CsLock			mDataLock;
	bool			mbHaveReceiveData;

	UDPSenderTool		*mUDPSender;
	class UDPReceiveThread;
	UDPReceiveThread	*mpUDPReceiveThread;

	class UDPReceiveThread : public WorkThread
	{
	public:
		UDPReceiveThread(UDPThreadNet *pNet)
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
		UDPThreadNet *mpNet;
	};
};
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
class UDPSenderTool : public WorkThread
{
public:
	bool Send(UInt64 targetAddr, const char *szSendData, int sendSize);
	bool Send(Array<UInt64> &targetAddr, const char *szSendData, int sendSize, bool bPacket, UInt64 packetCode);

protected:
	friend class UDPThreadNet;

	struct SendData
	{
		Array<UInt64>		mSendAddr;
		DataBuffer			mSendBuffer;
		UInt64				mPacketCode;
		bool				mbIsPacket;
	};
	typedef PoolLoop<SendData>		SendList;

protected:
	// NOTE: �߳���ִ��
	virtual void backWorkThread(void);

	void _ProcessSendThread();

	virtual void onBeforeClose()
	{
		event_set(mSendThreadEvent);
	}

public:
	UDPSenderTool(UDPThreadNet *pUDP);

	~UDPSenderTool();

protected:
	UDPThreadNet	*mOwnerUDPNet;
	// ���߷��Ͳ���, ƽ������, ���ⶪ��
	SendList		*mThreadSendList;
	SendList		*mMainSendList;
	CsLock			mSendLock;

	event_handle	mSendThreadEvent;		// �������߻��ѷ����߳�
};
//-------------------------------------------------------------------------
#endif //_INCLUDE_UDPTHREADNET_H_