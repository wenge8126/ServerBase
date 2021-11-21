
#include "FileDataReceive.h"
#include "Md5Tool.h"
#include "EventCenter.h"
#include "TimeManager.h"

RECEIVE_RESULT FileNetTransferReceiveControl::OnReceiveFinish()
{
	// MD5У��
	MD5  md(mReceivePartData->data(), mReceivePartData->dataSize());
 	AString code = md.toString();
	if (strcmp(code.c_str(), mCheckMD5.c_str()) == 0)
	{
		OnReceiveSucceed();
		return RECEIVE_SUCCEED_FINISH;
	}
	else
	{
		DEBUG_LOG("MD5 У��ʧ��");
		OnReceiveDataCheckFail();
		//OnReceiveFail();
		return RECEIVE_FAIL;
	}
}

bool FileNetTransferReceiveControl::OnReceiveSucceed()
{
	if (!mResultData)
		mResultData = MEM_NEW DataBuffer(mScrDataSize);
	else if( mResultData->size()<mScrDataSize )
		mResultData->resize(mScrDataSize);

	mResultData->setDataSize(0);
	mResultData->seek(0);

	HandDataBuffer d = mReceivePartData;
	if (d->UnZipData(mResultData, 0, mScrDataSize))
	{
		DEBUG_LOG("��ѹԭ���ݳɹ�");
		return true;
	}
	else
	{
		DEBUG_LOG("Error: ��ѹԭ����ʧ��");
		OnReceiveDataCheckFail();
	}

	DEBUG_LOG("�����ļ����");
	return false;
}

void FileNetTransferReceiveControl::RegisterMsgEvent( Logic::tEventCenter *pCenter )
{
	pCenter->RegisterEvent("FILE_RequestPartData", MEM_NEW Logic::EventFactory<FILE_RequestPartData_R>());
}

void FileNetTransferReceiveControl::StartReadyReceiveData( DSIZE allPartCount, DSIZE scrDataSize, DSIZE zipDataSize, const char *md5 )
{
	mStartTime = TimeManager::NowTick();
	mAllPartCount = allPartCount;
	mScrDataSize = scrDataSize;
	mZipDataSize = zipDataSize;
	mCheckMD5 = md5;

	if (mReceivePartData->size()<mZipDataSize)
		mReceivePartData->resize(mZipDataSize);

	mReceivePartData->clear(false);
	mAlreadyReceivePartCount = 0;
	OnReadyReceiveBegin();
}

RECEIVE_RESULT FileNetTransferReceiveControl::OnReceivePartData( int partIndex, AutoData partData )
{
	if (mAlreadyReceivePartCount !=partIndex )
	{
		DEBUG_LOG("��������������������ȷ");
		OnReceiveFail();
		return RECEIVE_FAIL;
	}

	++mAlreadyReceivePartCount;

	mReceivePartData->_write(partData->data(), partData->dataSize());
	OnReceiveOnePartData(partIndex, partData);

	if (partIndex+1>=mAllPartCount)
	{
		return OnReceiveFinish();
	}

	return RECEIVE_SUCCEED;
}

void FileNetTransferReceiveControl::StartRequestData()
{
	Hand<FILE_RequestPartData_R> hRequestEvt = StartEvent("FILE_RequestPartData");
	hRequestEvt->mReceiveControl = GetSelf();
	hRequestEvt["RES_NAME"] = mResourceName;
	hRequestEvt["RES_MD5"] = mCheckMD5;
	hRequestEvt->DoEvent(false);
}

//-------------------------------------------------------------------------

void FILE_RequestDataInfo_R::_OnResp(AutoEvent &respEvent)
{
	Data allPartCount = respEvent["PARTCOUNT"];
	Data scrSize = respEvent["SCRDATASIZE"];
	Data zipSize = respEvent["ZIPSIZE"];
	AString checkMD5 = respEvent["CHECKMD5"];
	if ( allPartCount.empty()
		|| scrSize.empty()
		|| zipSize.empty()
		|| checkMD5.empty()
		)
	{
		Log("��ʼ׼������ʧ��, δ�ṩ��Ҫ��������Ϣ");
		Finish();
		return;
	}		
	else
	{
		mReceiveControl->mResourceName = get("RES_NAME");
		mReceiveControl->StartReadyReceiveData(allPartCount, scrSize, zipSize, checkMD5.c_str());
	}
}

void FILE_RequestPartData_R::_OnResp( AutoEvent &respEvent )
{
	if (!mReceiveControl)
	{
		Log("Error: receive tool not exist");
		return;
	}

	Data partIndex = respEvent->get("PARTINDEX");
	AutoData partData;

	if (!partIndex.empty() && respEvent->get("PARTDATA", partData))
	{
		RECEIVE_RESULT re = mReceiveControl->OnReceivePartData((int)partIndex, partData);
		if (re==RECEIVE_SUCCEED)
		{
			setFinished(false);
			DoEvent(false);
			return;
		}
		else if (re==RECEIVE_SUCCEED_FINISH)
		{
			//mReceiveControl->OnReceiveFinish();
			OnReceiveFinish();
			Finish();
			return;
		}
	}
	mReceiveControl->OnReceiveFail();
	OnReceiveFail();
	Finish();
}
