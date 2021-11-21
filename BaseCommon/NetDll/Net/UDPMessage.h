#ifndef _INCLUDE_UDPMESSAGE_H_
#define _INCLUDE_UDPMESSAGE_H_

#include "NetHead.h"
#include "UDPThreadNet.h"

#include "EasyMap.h"
#include "Array.h"
#include <EasyHash.h>


//using namespace std;
//using namespace stdext;

//-------------------------------------------------------------------------
#define MAX_WAIT_MSG_COUNT		(5)			// ���256


//-------------------------------------------------------------------------
// ����ģʽ��Ϣ����, �˹���, C# ��ҲҪʵ��, ���ڽ��տɶ���������, ����������Ƶ
// UDP��ͷ����: ����ʱ��4�ֽ�(��Կ�������ʱ�䣩; 1�ֽ� ��Ϣ���(���ڿ���ȡ��,�ж϶���); 1�ֽ���Ϣ�ְ����(0 �Ű��ڵ������ֽ��� �ְ�����)
// NOTE: ��Ϣͷ��Ϣ�����ڰ����ݺ��棬Ŀ��Ϊ�˷��㴦�������
//-------------------------------------------------------------------------
class UDPMessageNet;

class Net_Export UDPConnect : public Base<UDPConnect>
{
	friend class UDPMessageNet;

public:

	// DataBuffer ��Ϣ�ķְ�����
	typedef Array<DataBuffer>	MsgDataArray;
	struct MsgData
	{
		UINT			mTime;			// ����ʱ��������ʱ��
		byte			mPartCount;
		MsgDataArray	mMsgData;
		bool			mUse;

		MsgData()
		{
			Init();
		}

		void Init()
		{
			mUse = false;
			mTime = 0;
			mPartCount = 0;
			for (size_t i=0; i<mMsgData.size(); ++i)
			{
				mMsgData[i].setDataSize(0);
			}
		}
	};
	typedef Array<MsgData*>	MsgList;

public:
	virtual void OnReceivePacket(DataStream *revPacket, int packetSize){}
	virtual bool Process(UDPMessageNet *pUDPMessageNet){ return true; }
	virtual void Close(){}

	// ���ֶ���
	virtual void OnHappenGiveupPacket(int beginCode, int endCode, struct MsgData *msg = NULL)
	{
		WARN_LOG("����UDP����>[%d - %d]", beginCode, endCode);
	}

public:
	UDPConnect();

	~UDPConnect();



public:
	virtual void OnReceiveMsg(DataStream *revData, int revSize);

	void _AppendMsgData(MsgData *msg, byte partCode, DataStream *msgData, int msgDataSize);

	//void _CheckClear();

	void _CheckRunMsg();


	bool _RunMsg(MsgData *msg);

	bool _HaveWaitMsg()
	{
		for (int i=0; i<mWaitMsgList.size(); ++i)
		{
			if (mWaitMsgList[i]->mUse)
				return true;
		}
		return false;
	}

protected:
	// bAddPacketCode ����UDP���ӵİ���
	void SendData(UDPMessageNet *net, byte serverMsgCode, DataStream *sendData, UInt64 addr, bool bAddPacketCode);

//	MsgData* GetFreeMsgData();

protected:
	MsgList						mWaitMsgList;	
	EasyMap<UINT, byte>			mTimeSqrtList;			// ���ڰ�ʱ���Ⱥ�����

	int							mLastRunMessageCode;	// �����ڶ����ж�,��ǰִ�а��򲻵��ڴ�ֵ+1�������ֶ���

	byte						mSendPacketCode;
};

typedef Hand<UDPConnect>		AutoUDPConnect;

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
class Net_Export UDPMessageNet : public UDPThreadNet
{
public:
	virtual void OnReceivePacket(UInt64 scrAddr, DataStream *revPacket, int packetSize){}
	virtual AutoUDPConnect NewUDPConnect(UInt64 scrAddr);
	virtual AutoUDPConnect RemoveUDPConnect(UInt64 scrAddr, AutoUDPConnect connect);

	virtual void AppendUDPConnect(UInt64 scrAddr, AutoUDPConnect udpConn);

	virtual AutoUDPConnect OnReceiveNewConnect(UInt64 newAddr);

	virtual AutoUDPConnect GetUDPConnect(UInt64 addr);

	typedef EasyHash<UInt64, AutoUDPConnect>	UDPList;
public:
	virtual void OnReceiveMsg(UInt64 scrAddr, DataStream *revData, int revSize);

	virtual bool SendPacket(UInt64 targetAddr, DataStream *sendData, int sendSize, Array<UInt64> *targetList = NULL);
	virtual bool BocastPacket(DataStream *sendData, int sendSize, UInt64 exclude);

	virtual void Process();

	virtual void Close();

public:
	UDPMessageNet();

protected:
	UDPList			mUDPConnectList;

	DataBuffer		mSendDataBuffer;
	Array<UInt64>	mBocastList;

	byte				mServerPacketCode;
};
//-------------------------------------------------------------------------
class Net_Export DefaultUDPConnect : public UDPConnect
{
public:
	virtual void OnReceivePacket(DataStream *revPacket, int packetSize);

public:
	DefaultUDPConnect(UDPMessageNet *pUdp, UInt64 scrAddr)
		: mpUDPNet(pUdp)
		, mScrAddr(scrAddr)
	{

	}

protected:
	UDPMessageNet	*mpUDPNet;
	UInt64			mScrAddr;
};
//-------------------------------------------------------------------------

#endif //_INCLUDE_UDPMESSAGE_H_