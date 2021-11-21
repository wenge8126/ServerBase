/********************************************************************
	created:	2013/10/26
	created:	26:10:2013   0:12
	filename: 	C:\Work\BaseCommon\NetDll\Net\FileNetTransfer.h
	file path:	C:\Work\BaseCommon\NetDll\Net
	file base:	FileNetTransfer
	file ext:	h
	author:		Yang WenGe
	
	purpose:	���������紫�ݹ���, ���ļ����ݴ���
				1 �����Ȼ�ȡ���ͷ���Ҫ��������Ϣ
				2 ���ͷ��ṩ������Ϣ
				3 ����
*********************************************************************/
#ifndef _INCLUDE_FILENETTRANSFER_H_
#define _INCLUDE_FILENETTRANSFER_H_

#include "NetHead.h"
#include "ServerEvent.h"
#include "ClientEvent.h"



//-------------------------------------------------------------------------
// ���Ͷ�
//-------------------------------------------------------------------------
class Net_Export FileNetTransferSendControl : public Base<FileNetTransferSendControl>
{
public:
	FileNetTransferSendControl(DSIZE partSize = 4096)
		: mPackSize(0)
		, mPackZipSize(0)
		, ONE_PART_PACK_DATA_SIZE(partSize)
	{

	}

public:
	static void RegisterMsgEvent(Logic::tEventCenter *pCenter);

public:
	virtual bool _DoEvent();

	virtual bool ReadySendData(AutoData sendScrData);

	AutoData GetNextSendPartData(int requestPartIndex)
	{
		if (requestPartIndex<(int)mZipPartDataList.size())
			return mZipPartDataList[requestPartIndex];

		return AutoData();
	}

	int GetPartCount(){ return mZipPartDataList.size(); }
	DSIZE GetScrDataSize(){ return mSendFileData->dataSize(); }
	DSIZE GetZipSize(){ return mPackZipSize; }
	AString GetDataMD5(){ return mCheckMD5; }

	virtual void OnNotifyUpload(HandConnect conn, int part, int nAllPart){}

public:
	AutoData	mSendFileData;
	DSIZE		mPackZipSize;
	DSIZE		mPackSize;

	AString		mCheckMD5;
	AutoData	mZipPackData;
	Array<AutoData>	mZipPartDataList;

	DSIZE		ONE_PART_PACK_DATA_SIZE;
};
//-------------------------------------------------------------------------
// �ظ����շ�������������, ��Ҫ�ṩ������Ϣ
//-------------------------------------------------------------------------
class Net_Export FILE_RequestDataInfo_S : public Logic::tClientEvent
{
public:
	virtual bool ReadySendData(int &partCount, DSIZE &scrSize, DSIZE &zipSize, AString &checkMD5, AutoEvent &respEvt) = 0;

public:
	virtual bool _DoEvent()
	{
		Finish();
		return true;
	}
	 
	virtual void SetRespData(AutoEvent &hResp);
};
//-------------------------------------------------------------------------
class Net_Export FILE_RequestPartData_S : public Logic::tClientEvent
{
public:
	FILE_RequestPartData_S()
	{
		SetAutoCompress(false);
	}	

	virtual Hand<FileNetTransferSendControl> GetSendControlTool() { return Hand<FileNetTransferSendControl>(); }

	virtual void OnAllDataSendFinish(){}

public:
	virtual bool _DoEvent()
	{
		Finish();
		return true;
	}

	virtual void SetRespData(AutoEvent &respEvent);
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#endif