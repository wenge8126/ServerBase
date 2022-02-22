#include "EventPacket.h"

#include "Assertx.h"

#include "NetDataBuffer.h"
#include "TimeManager.h"
#include "ResoursePack.h"

//-----------------------------------------------------------------------------
BOOL EventPacket::Read(DataStream& iStream, size_t packetSize, tNetConnect* pConnect)
{
	UInt64 nowTime = TimeManager::NowTick();
	Logic::tEventCenter *pCenter = pConnect->GetNetHandle()->GetEventCenter();
	AssertEx(NULL != pCenter, "�¼����Ĳ�ӦΪ��");

	AutoEvent hRevEvent = pCenter->RestoreMsg(&iStream);
	if (!hRevEvent)
	{
		WARN_LOG("�ָ������¼�ʧ�� >[%s] size [%llu]", GetMsgName(), packetSize);
		pConnect->GetNetHandle()->GetNetProtocol()->OnPacketExecuteError(pConnect, this);

		return FALSE;
	}
	mEvent = hRevEvent;
	return TRUE;
}

BOOL EventPacket::Write(DataStream& oStream) const
{
	AutoEvent evt = mEvent;
	if (mEvent->GetEventCenter()->SerializeMsg(evt, &oStream))
	{
		DEBUG_LOG("Send event %s : %d", mEvent->GetEventName(), mEvent->GetEventFactory()->GetNameIndex());

		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
UINT EventPacket::Execute( tNetConnect* pConnect )
{
	AutoEvent hRevEvent = mEvent;
	if (hRevEvent)
	{		
#if DEVELOP_MODE
		auto nowTime = TimeManager::NowTick();
#endif
		//NOTE_LOG(" Run net msg [%s]", hRevEvent->GetEventName());
		hRevEvent->_OnBindNet(pConnect);
		pConnect->OnReceiveEvent(hRevEvent.getPtr(), this);
		pConnect->GetNetHandle()->OnReceiveEvent(pConnect, hRevEvent.getPtr());
#if DEVELOP_MODE
		int tickDiff = (int)(TimeManager::NowTick() - nowTime);
		if (tickDiff > 10)
		{
			NOTE_LOG("EventPacket::Execute %s tick=%d", hRevEvent->GetEventName(), tickDiff);
		}
#endif
	}
	else
	{
		return -1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
EventPacket::EventPacket()
{
}


//void EventPacket::SwapEventData( AutoData &eventData )
//{
//	AutoData temp = mData;
//	mData = eventData;
//	eventData = temp;
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//UINT CheckRequestPacket::Execute( tNetConnect* pConnect )
//{
//	// ��ȡID, ��Ҫ��֤����Ϣ
//
//	return 0;
//}
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//UINT CheckResponsePacket::Execute( tNetConnect* pConnect )
//{
//	// ��������ID, ������֤ID
//	// ...
//	return 0;
//}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#define SCR_SIZE_TYPE	unsigned int
//#define SCR_SIZE_INFO_LENGTH	sizeof(SCR_SIZE_TYPE)
//
//BOOL CompressPacket::Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect)
//{
//	size_t size = packetSize;
//
//	if (mZipBuffer->size()<(DSIZE)size)
//		mZipBuffer->resize(size);
//
//	if (iStream.Read(mZipBuffer->data(), (UINT)size)==size)
//	{
//		mZipBuffer->seek(size);
//
//		//_OnReceiveData(mZipBuffer->data(), size);
//
//		AssertEx (size>sizeof(size_t), "ѹ�����ݴ�С����,������С����Ϣ���ݳ���");
//		mZipBuffer->seek(0);
//		SCR_SIZE_TYPE scrSize = 0;
//		if (!mZipBuffer->read(scrSize))
//		{
//			ERROR_LOG("��ѹ���ݳ���, ��ѹ�������ж�ȡԴʼ��Сʧ��");
//			return FALSE;
//		}
//		HandDataBuffer d = mZipBuffer;
//		// ��ѹ��
//		if (d->UnZipData(mData, 0, scrSize, SCR_SIZE_INFO_LENGTH, size-SCR_SIZE_INFO_LENGTH))		
//		{
//			//INFO_LOG("OK >>> �ɹ���ѹ�������� scr size [%u], Zip size [%u], RATE[%.4f]", scrSize, size-SCR_SIZE_INFO_LENGTH, (float)(size-SCR_SIZE_INFO_LENGTH)/(float)scrSize);
//		}
//		else
//		{
//			ERROR_LOG("XXX >>> ��ѹ��ʧ��, ����ѹ�����ݼ���С��Ϣ�Ƿ���ȷ.");
//			return FALSE;
//		}		
//
//	}
//	return TRUE;
//}

//-------------------------------------------------------------------------
//CompressPacket::CompressPacket()
//	: EventPacket()
//{
//	//AssertEx(m_pNetProtocol!=NULL, "����Э��Ϊ��");
//	mZipBuffer = MEM_NEW DataBuffer(EVENTPACKET_DEFAULT_BUFFER_SIZE);
//}
//
//int CompressPacket::SetEvent( AutoEvent &hEvent )
//{
//	mZipBuffer->clear(false);
//	
//	if (! hEvent->GetEventCenter()->SerializeMsg(hEvent, mZipBuffer.getPtr()) )
//	{
//		ERROR_LOG("�����¼�����, ���л��¼�����ʧ��");
//		return 0;
//	}
//	mZipBuffer->setDataSize(mZipBuffer->tell());
//	return mZipBuffer->tell();
//}
//
//bool CompressPacket::ReadySendData()
//{
//	size_t size = mZipBuffer->dataSize();
//
//	AssertEx(size<0x00FFFFFF, "�������ݱ��� < 65535");
//
//	// ʹ�� zip ѹ��
//	HandDataBuffer d = mZipBuffer;
//	int zipSize = d->ZipData(mData, SCR_SIZE_INFO_LENGTH, 0, size, MAX_SPEED);	
//	if (zipSize<=0)
//	{
//		ERROR_LOG("ѹ����Ϣ������ʧ��, ����Ŀ��ռ䲻��");
//		return false;
//	}
//
//	mData->seek(0);
//	// д��ԭ���ݴ�С
//	mData->write((SCR_SIZE_TYPE)size);
//
//	// ����ǰλ���ƶ�����������֮��, ��ȷ�����ʹ�С
//	mData->seek(SCR_SIZE_INFO_LENGTH + zipSize);
//
//	//INFO_LOG("��Ϣ����ѹ�� >>> Size[%u] >>> Zip size [%d], RATE[%.4f]", size, zipSize, (float)zipSize/(float)size);
//
//	return true;
//}
//
//void CompressPacket::SwapEventData( AutoData &eventData )
//{
//	AutoData temp = mZipBuffer;
//	mZipBuffer = eventData;
//	eventData = temp;
//}
//
//bool CompressPacket::UnZip(AutoData zipBuffer,  AutoData &resultData )
//{
//	AssertEx(resultData, "�������ݱ����Ƚ��г�ʼ, �Ա㿼��ʹ�� NetDataBuffer");
//	
//	size_t size = zipBuffer->tell();
//	zipBuffer->seek(0);
//	SCR_SIZE_TYPE scrSize = 0;
//	if (!zipBuffer->read(scrSize))
//	{
//		DEBUG_LOG("��ѹ���ݳ���, ��ѹ�������ж�ȡԴʼ��Сʧ��", "");
//		return false;
//	}
//
//	HandDataBuffer zipData = zipBuffer;
//	// ��ѹ��
//	if (zipData->UnZipData(resultData, 0, scrSize, SCR_SIZE_INFO_LENGTH, size-SCR_SIZE_INFO_LENGTH))		
//	{
//		//!!! INFO_LOG("OK >>> �ɹ���ѹ�������� scr size [%u], Zip size [%u], RATE[%.4f]", scrSize, size-SCR_SIZE_INFO_LENGTH, (float)(size-SCR_SIZE_INFO_LENGTH)/(float)scrSize);
//	}
//	else
//	{
//		DEBUG_LOG("XXX >>> ��ѹ��ʧ��, ����ѹ�����ݼ���С��Ϣ�Ƿ���ȷ.", "");
//		return false;
//	}
//
//	return true;
//}
//
//bool CompressPacket::FullZipData( AutoData zipData )
//{
//	size_t size = zipData->dataSize();
//	if (size<=SCR_SIZE_INFO_LENGTH)
//	{
//		ERROR_LOG("ѹ�����ݳ���̫С");
//		return false;
//	}
//	zipData->seek(0);
//	SCR_SIZE_TYPE scrSize = 0;
//	if (!zipData->read(scrSize))
//	{
//		INFO_LOG("��ѹ���ݳ���, ��ѹ�������ж�ȡԴʼ��Сʧ��");
//		return FALSE;
//	}
//	HandDataBuffer d = zipData;
//	// ��ѹ��
//	if (d->UnZipData(mData, 0, scrSize, SCR_SIZE_INFO_LENGTH, size-SCR_SIZE_INFO_LENGTH))		
//	{
//		//!!! INFO_LOG("OK >>> �ɹ���ѹ�������� scr size [%u], Zip size [%u], RATE[%.4f]", scrSize, size-SCR_SIZE_INFO_LENGTH, (float)(size-SCR_SIZE_INFO_LENGTH)/(float)scrSize);
//	}
//	else
//	{
//		ERROR_LOG("XXX >>> ��ѹ��ʧ��, ����ѹ�����ݼ���С��Ϣ�Ƿ���ȷ.");
//		return FALSE;
//	}		
//
//	return TRUE;
//}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

bool DataPacket::_OnSendData( void *szSendData, size_t size ) const
{
	if (HasState(ePacketStateEncrypt) && !mbAlreadyEncrypt)
	{
		mbAlreadyEncrypt = true;
		bool b = _EncryptData(szSendData, size);
		if (!b)
		{
			DEBUG_LOG("Error: Encrypt net Data fail");
			return false;
		}
	}
	return true;
}

bool DataPacket::_OnReceiveData( void *szRevData, size_t size )
{
	if (HasState(ePacketStateEncrypt))
	{
		bool b = _DecryptData(szRevData, size);
		if (!b)
		{
			DEBUG_LOG("Error: Decrypt net Data fail");
			return false;
		}
	}
	return true;
}

bool DataPacket::_EncryptData( void *scrData, DSIZE length ) const
{
	return ResourcesPack::_EncryptData(scrData, length);
}

bool DataPacket::_DecryptData( void *scrData, DSIZE length ) const
{
	return ResourcesPack::_DecryptData(scrData, length);
}

//-------------------------------------------------------------------------

BOOL EventProcessPacket::ReadEvent( tNetConnect *pConnect, DataStream& iStream, size_t packetSize )
{
	Logic::tEventCenter *pCenter = pConnect->GetNetHandle()->GetEventCenter();
	AssertEx(NULL!=pCenter, "�¼����Ĳ�ӦΪ��");

	mMsgEvent = pCenter->RestoreMsg(&iStream);

	if (!mMsgEvent)
	{
		WARN_LOG("�ָ������¼�ʧ�� >[%s] size [%llu]", GetMsgName(), packetSize);
		pConnect->GetNetHandle()->GetNetProtocol()->OnPacketExecuteError(pConnect, this);

		return false;
	}		

	return true;
}

UINT EventProcessPacket::Execute( tNetConnect* pConnect )
{
	if (mMsgEvent)
	{		
		//NOTE_LOG(" Run net msg [%s]", hRevEvent->GetEventName());
		mMsgEvent->_OnBindNet(pConnect);
		pConnect->OnReceiveEvent(mMsgEvent.getPtr(), this);
		pConnect->GetNetHandle()->OnReceiveEvent(pConnect, mMsgEvent.getPtr());
	}

	return 0;
}
//-------------------------------------------------------------------------