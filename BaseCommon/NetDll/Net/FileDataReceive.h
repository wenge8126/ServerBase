
#ifndef _INCLUDE_FILEDATARECEIVE_H_
#define _INCLUDE_FILEDATARECEIVE_H_

#include "NetHead.h"
#include "ServerEvent.h"



//-------------------------------------------------------------------------
//���ն�
//-------------------------------------------------------------------------
class Net_Export FileNetTransferReceiveControl : public Base<FileNetTransferReceiveControl>
{

public:
	FileNetTransferReceiveControl()
		: mAlreadyReceivePartCount(0)
	{
		mReceivePartData = MEM_NEW DataBuffer();
	}

	static void RegisterMsgEvent(Logic::tEventCenter *pCenter);

public:
	virtual void OnReadyReceiveBegin() {}
	virtual void OnReceiveOnePartData(int partIndex, AutoData partData){}
	virtual int GetNextResquestPartIndex() { return mAlreadyReceivePartCount; }

	virtual AutoEvent StartEvent(const char *szEventName) = 0;

	virtual void Process(){}

public:
	void StartReadyReceiveData(DSIZE allPartCount, DSIZE scrDataSize, DSIZE zipDataSize, const char *md5);

	virtual void StartRequestData();

	// ��Ӧ˳�����, ����Ų���ȷ, ���Դ�����
	virtual RECEIVE_RESULT OnReceivePartData(int partIndex, AutoData partData);

	virtual RECEIVE_RESULT OnReceiveFinish();

	virtual bool OnReceiveSucceed();

	virtual void OnReceiveFail()
	{
		DEBUG_LOG("�����ļ�ʧ��");
	}

	virtual void OnReceiveDataCheckFail()
	{
		DEBUG_LOG("����������֤ʧ��");
	}

public:
	int						mAllPartCount;
	int						mAlreadyReceivePartCount;
	DSIZE					mScrDataSize;
	DSIZE					mZipDataSize;
	AString					mCheckMD5;
	AString					mResourceName;

	AutoData				mReceivePartData;

	AutoData				mResultData;
	UInt64					mStartTime;
};
//-------------------------------------------------------------------------
// ���շ�����������Ϣ
class Net_Export FILE_RequestDataInfo_R : public Logic::tServerEvent
{
public:
	virtual void OnRequestDataBegin() = 0;

public:
	virtual bool _DoEvent()
	{
		OnRequestDataBegin();		
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent);

public:
	Hand<FileNetTransferReceiveControl> mReceiveControl;
};
//-------------------------------------------------------------------------
// �������ݰ�
//-------------------------------------------------------------------------
class Net_Export FILE_RequestPartData_R : public Logic::tServerEvent
{
public:
	virtual void OnReceiveFinish(){}
	virtual void OnReceiveFail(){}

public:
	virtual bool _NeedFinishWhenResponsed() const { return false; }

	virtual bool _DoEvent()
	{
		int mPartIndex = mReceiveControl->GetNextResquestPartIndex();
		set("PARTINDEX", mPartIndex);

		WaitTime(20);

		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent);

	virtual bool _OnTimeOver()
	{
		Log("�������ݳ�ʱ");
		mReceiveControl->OnReceiveFail();

		return true;
	}

public:
	Hand<FileNetTransferReceiveControl> mReceiveControl;
};
//-------------------------------------------------------------------------


#endif