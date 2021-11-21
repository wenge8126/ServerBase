
#include "FileNetTransfer.h"
#include "Md5Tool.h"
#include "EventCenter.h"

#include "FileDataReceive.h"

void FileNetTransferSendControl::RegisterMsgEvent(Logic::tEventCenter *pCenter)
{
	pCenter->RegisterEvent("FILE_RequestPartData", MEM_NEW Logic::ClientEventFactory<FILE_RequestPartData_S>());
}


bool FileNetTransferSendControl::_DoEvent()
{
	return true;
}

bool FileNetTransferSendControl::ReadySendData(AutoData sendData)
{
	if (!sendData || sendData->dataSize()<=0)
	{
		return false;
	}
	mSendFileData = sendData;
	// ѹ��
	HandDataBuffer packData = mSendFileData;
	AutoData zipPackData = MEM_NEW DataBuffer();

	mPackZipSize = packData->ZipData(zipPackData, 0, 0, 0, MAX_SPEED);
	if (mPackZipSize <= 0)
	{
		ERROR_LOG("Zip fail > %d", packData->dataSize());
		return false;
	}
	mPackSize = packData->dataSize();		

	// �����MD5
	MD5 md(zipPackData->data(), zipPackData->dataSize());
	AString strMD5 = md.toString();
	mCheckMD5 =  strMD5.c_str();

	// �ֿ�
	size_t num = (mPackZipSize +ONE_PART_PACK_DATA_SIZE-1)/ ONE_PART_PACK_DATA_SIZE;
	mZipPartDataList.resize(num);

	for (int i=0; i<num; ++i)
	{
		size_t dataPos = i * ONE_PART_PACK_DATA_SIZE;
		DSIZE dataSize = zipPackData->dataSize() - dataPos;
		if (dataSize>ONE_PART_PACK_DATA_SIZE)
			dataSize = ONE_PART_PACK_DATA_SIZE;

		AutoData partData = MEM_NEW DataBuffer(dataSize);
		partData->_write(zipPackData->data()+dataPos, dataSize);

		mZipPartDataList[i] = partData;
	}
	return true;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void FILE_RequestDataInfo_S::SetRespData(AutoEvent &hResp)
{
	int partCount = 0;
	DSIZE scrSize = 0;
	DSIZE zipSize = 0;
	AString checkMD5;

	if (ReadySendData(partCount, scrSize, zipSize, checkMD5, hResp))
	{
		hResp->set("PARTCOUNT", partCount);
		hResp->set("SCRDATASIZE", scrSize);
		hResp->set("ZIPSIZE", zipSize);
		hResp->set("CHECKMD5", checkMD5.c_str());
	}
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void FILE_RequestPartData_S::SetRespData(AutoEvent &respEvent)
{
	respEvent->SetAutoCompress(false);
	Data partIndex = get("PARTINDEX");

	if (partIndex.empty())
	{
		Log("Error: δ�ṩ��������ݰ�����");
		return;
	}

	Hand<FileNetTransferSendControl> sendControl = GetSendControlTool();
	if (!sendControl)
	{
		Log("ERROR: ��������Դ���� [%s]", get("RES_NAME").string());
		return;
	}
	AutoData nextPartData = sendControl->GetNextSendPartData((int)partIndex);
	if (!nextPartData)
	{
		Log("Error: ��������������ݰ�[%d]", (int)partIndex);
		return;
	}
	respEvent->set("PARTINDEX", (int)partIndex);
	respEvent->set("PARTDATA", nextPartData);

	sendControl->OnNotifyUpload(mNetConnect, (int)partIndex+1, sendControl->GetPartCount());

	if ((int)partIndex+1>=sendControl->GetPartCount())
		OnAllDataSendFinish();
}
//-------------------------------------------------------------------------