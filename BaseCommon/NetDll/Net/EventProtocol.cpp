
#include "EventProtocol.h"

#include "TableManager.h"

#include "EventPacket.h"

#include "ResoursePack.h"

#include "NetIndexPacket.h"
#include "EventData.h"
#include "IOCPConnect.h"
#include "TimeManager.h"
//#include "RequestPacket.h"
#include "ResponseEvent.h"

#if USE_BIG_PACKET_SIZE
const uint EventNetProtocol::_MAX_SIZE = 16*1024*1024;
#else
#define _GET_ID(v)  (v>>24)
#define  _GET_STATE(v) ( (v>>20)&0xF )
#define _MSG_SIZE(v) (v&0xFFFFF)

uint _MakeHead(byte id, byte state, uint size)
{
    uint v = id;
    v = v<<24;
    uint s = state;
    v += s <<20;
    return v+size;
}
const uint EventNetProtocol::_MAX_SIZE = 0xFFFFF;
#endif

//----------------------------------------------------------------------
EventNetProtocol::EventNetProtocol( ) 
	: mTempWriteBuffer(2048)
	//: mDefaultEventFactory()
	//, mCompressEventFactory()
{
	//mEventPacket = MEM_NEW EventPacket();
	//mCompressPacket = MEM_NEW CompressPacket();
	////mIndexPacket = MEM_NEW NotifyEventIndexPacket();
	////mHeartPacket = MEM_NEW HeartBeatPacket();

	mNetPacketFactoryList.Append(PACKET_EVENT, MEM_NEW DefinePacketFactory<EventPacket, PACKET_EVENT>());
	//mNetPacketFactoryList.Append(PACKET_COMPRESS_EVENT, MEM_NEW DefinePacketFactory<CompressPacket, PACKET_COMPRESS_EVENT>());
	mNetPacketFactoryList.Append(PACKET_EVENT_PROCESS, MEM_NEW DefinePacketFactory<EventProcessPacket, PACKET_EVENT_PROCESS>());
	
	// Ĭ�ϼ�����������Ping ������
	PacketFactory *pFactory = MEM_NEW IndexPacketFactory();
	mNetPacketFactoryList.Append(pFactory->GetPacketID(), pFactory);
	
	pFactory = MEM_NEW HeartBeatPacketFactory();
	mNetPacketFactoryList.Append(pFactory->GetPacketID(), pFactory);

}

bool EventNetProtocol::RegisterNetPacket(AutoPacketFactory f, bool bReplace)
{
	if (!f)
		return false;

	AutoPacketFactory existFactory = mNetPacketFactoryList.Find(f->GetPacketID());
	if (existFactory)
	{
		if (bReplace)
		{
			NOTE_LOG("WARN: xxxxxxx [%d] net packet factory exist, now replace", f->GetPacketID());
		}
		else if (typeid(*existFactory)!=typeid(*f))
		{
			NOTE_LOG("ERROR: Register [%d] fail, net packet factory exist", f->GetPacketID());
			return false;
		}
	}
	if (f->GetPacketID() == 19)
	{
		int x = 0; 
	}
	DEBUG_LOG(" !!! Register [%d] packet factory ", f->GetPacketID());
	mNetPacketFactoryList.Append(f->GetPacketID(), f);

	return true;
}

int EventNetProtocol::AppendNetPacketFrom(tNetProtocol *pOther, bool bReplace)
{
	EventNetProtocol *p = dynamic_cast<EventNetProtocol*>(pOther);
	if (p==NULL)
		return 0;
	int x = 0;
	for (size_t i=0; i<p->mNetPacketFactoryList.size(); ++i)
	{
		if (RegisterNetPacket(p->mNetPacketFactoryList[i], bReplace))
			++x;
	}
	return x;
}

//----------------------------------------------------------------------------------------------

HandPacket EventNetProtocol::CreatePacket( PacketID packetID )
{
	AutoPacketFactory &f = mNetPacketFactoryList.Find(packetID);
	if (f)
		return f->CreatePacket();
	NOTE_LOG("ERROR: [%d] no exist packet factory", packetID);
	return HandPacket();
}

//----------------------------------------------------------------------------------------------

bool EventNetProtocol::WritePacket(int id, const Packet* pPacket, DataStream *mSocketOutputStream)
{
	__ENTER_FUNCTION_FOXNET

		PacketID_t packetID = id;
	byte packetState = (byte)pPacket->GetState();

	mTempWriteBuffer.clear(false);
	if (pPacket->Write(mTempWriteBuffer) == FALSE)
	{
		ERROR_LOG("%d :  %s write fail", id, pPacket->GetMsgName());
		return false;
	}

    UINT packetSize = mTempWriteBuffer.dataSize();

	if (packetID <= 0)
	{
#if _DEBUG_NET_
		ERROR_LOG("Find id = 0 packet, factory %d", pPacket->mFactoryPtr->mpFactory->GetPacketID());
#endif //_DEBUG_NET_
		return false;
	}

#if USE_BIG_PACKET_SIZE
	if (packetSize>_MAX_SIZE)
	{
#if _DEBUG_NET_
		ERROR_LOG("%d msg packet size %u > %u", (int)packetID, packetSize, _MAX_SIZE);
#endif //_DEBUG_NET_
		return false;
	}
	mSocketOutputStream->Write( (CHAR*)&packetID , sizeof(PacketID_t)) ;
	mSocketOutputStream->Write( (CHAR*)&packetState , sizeof(byte)) ;
	mSocketOutputStream->Write( (CHAR*)&packetSize , sizeof(UINT)) ;
    //NOTE_LOG("*** packet %d, %d, size = %u", packetID, packetState, packetSize);
#else
    if (packetSize>_MAX_SIZE || packetState>0xF)
    {
        ERROR_LOG("%d msg packet size %u > %u or state %u > %d", (int)packetID, packetSize, _MAX_SIZE, packetState, (int)0xF);
		return false;
    }
    uint headValue = _MakeHead(packetID, packetState, packetSize);
    AssertEx(PACKET_HEADER_SIZE==sizeof(headValue), "��Ϣͷ�ṹ��С�붨�峤�Ȳ�һ��");
    mSocketOutputStream->Write( (CHAR*)&headValue , PACKET_HEADER_SIZE) ;
#endif

	if (packetSize <= 0)
		return TRUE;

	UINT currentLen = mSocketOutputStream->dataSize(); 
	
	bool re = mSocketOutputStream->_write(mTempWriteBuffer.data(), (DSIZE)packetSize);
	if (re) 
    {
#if _DEBUG_NET_
		static int msTest = 0;
       DEBUG_LOG(">>>>>>>>>> %d Write packet %d = %u", ++msTest, packetID, packetSize);
	   mSocketOutputStream->write(msTest);
#endif //_DEBUG_NET_
		return TRUE;
    }
	// lost write data. restore buffer state.
	//!!! ���ٻָ��Ѿ�д������ �ر��Ǻ�̨������ȡʱ, �ı��ֵ�ر��п��ܻ�ʹͷ������β֮��, �����Ч����
	//mSocketOutputStream->_ForceRestoreTail( currentTail );

	AString errorInfo;
	errorInfo.Format("Error: write packet data fail >[%d], size %u", packetID, packetSize);
	//AssertEx(0, errorInfo.c_str());
	NOTE_LOG(errorInfo.c_str());
	
	OnWritePacketError(errorInfo);

	return FALSE;

	__LEAVE_FUNCTION_FOXNET

		return FALSE ;
}
//----------------------------------------------------------------------------------------------

HandPacket EventNetProtocol::ReadPacket( tNetConnect *pConnect, DataStream *mSocketInputStream )
{
	__ENTER_FUNCTION_FOXNET

#if USE_BIG_PACKET_SIZE
		if (mSocketInputStream->dataSize()<PACKET_HEADER_SIZE)
			return HandPacket();
	char szTemp[PACKET_HEADER_SIZE];
	if (!mSocketInputStream->peek( szTemp, PACKET_HEADER_SIZE ))
		return NULL;

	PacketID_t uID = (byte)szTemp[0];
	byte packetStateData = (byte)szTemp[1];
	UINT packetSize = *(UINT*)(szTemp+2);
#else
    uint headValue = 0;   
    AssertEx(PACKET_HEADER_SIZE==sizeof(headValue), "��Ϣͷ�ṹ��С�붨�峤�Ȳ�һ��");
	// step 1: try get packet head info
	if (mSocketInputStream->peek( (void*)&headValue, PACKET_HEADER_SIZE )==FALSE)
		return NULL;

    byte uID = _GET_ID(headValue);
    byte packetStateData = _GET_STATE(headValue);
    UINT packetSize = _MSG_SIZE(headValue);
#endif

//#if _DEBUG_NET_
//	int nowDataSize = mSocketInputStream->dataSize();
//	int head = mSocketInputStream->Head();
//	int tail = mSocketInputStream->Tail();
//#endif
	if (!CheckPacketInfo(pConnect, uID, packetSize))
	{
//#if _DEBUG_NET_
//		IOCPConnect *pConn = (IOCPConnect*)pConnect;
//		NOTE_LOG("*** Now all rev size %llu, process size %llu = %llu, last %d, head %d, tail %d", pConn->mReceiveSize, pConn->mProcessSize, pConn->mReceiveSize-pConn->mProcessSize, nowDataSize, head, tail);
//#endif
		return NULL;
	}
	UINT currentLen = mSocketInputStream->dataSize();
	// step 2: check data length over need packet size.
	if (currentLen < (PACKET_HEADER_SIZE + packetSize) )
	{
		//??? NOTE: ����Ǹ�����Ĵ��, ��ʹ����ʱ�������, �м����Ϣ������������� (δʵ�� 2020/9/7)
		if (mSocketInputStream->size() - 1 < (int)(PACKET_HEADER_SIZE + packetSize))
		{
			ERROR_LOG(">>>Msg %d size %d more then receive buffer size %d, now remove connect", (int)uID, (int)(PACKET_HEADER_SIZE + packetSize), mSocketInputStream->size() - 1);
			pConnect->Close();
		}
		return NULL;
	}
#if _DEBUG_NET_
    UINT nowLength = mSocketInputStream->dataSize();
#endif
	// step 3 : now ok, must had a packet
	//PacketID_t *packetID = (PacketID_t *)szBuf;

	// step 4: move to packet data pos
	mSocketInputStream->skip(PACKET_HEADER_SIZE);

	// step 5: create packet
	//UINT currentHead = mSocketInputStream->GetHead();

	HandPacket p;
	//if (uID==PACKET_EVENT && (packetStateData & ePacketStateEncrypt)==0)
	//{
	//	//NOTE: ����ֱ�ӽ�һ���¼�����ת����ֱ���������Ϣ������	
	//	uID = PACKET_EVENT_PROCESS;
	//	Auto<EventProcessPacket> packet = CreatePacket(uID);
	//	if (packet)
	//	{			
	//		if (packet->ReadEvent(pConnect, *mSocketInputStream, packetSize))
	//		{
	//			p = packet;
	//			p->SetState(packetStateData);
	//		}
	//	}
	//}
	//else
	BOOL bRe = FALSE;
	{
		p = CreatePacket(uID); 
		if (!p)
		{
			// �������簲ȫ, һ����Ҫ���Ͽ����Ӵ���, �Ҽ�¼������
			AString errorInfo = "Error : no exist packet >>>";
			errorInfo += (int)uID;
			OnReadPacketError(errorInfo);

			mSocketInputStream->skip(packetSize);

			return NULL;
		}
		// ����״̬ȷ���Ƿ��Ѿ��Ǽ�����Ϣ
		p->SetState(packetStateData);

		// step 5: read packet data
		bRe = p->Read(*mSocketInputStream, packetSize, pConnect);
	}
#if _DEBUG_NET_
    ((IOCPConnect*)pConnect)->mProcessSize += packetSize+PACKET_HEADER_SIZE;
#endif
	// �����������û���, ��ʱ�����߳��п����Ѿ��ֽ��յ��µ�����
	if (bRe==FALSE) //currentLen-mSocketInputStream->dataSize()!=packetSize+ PACKET_HEADER_SIZE)
	{
		// ����������
		//AssertEx(0, "Error : read packet fail, size is not right");
#if _DEBUG_NET_
		AString errorInfo;
		errorInfo.Format( "Error : %d read packet fail, size %d is not right %d >>>", (int)uID, currentLen - mSocketInputStream->dataSize(), packetSize + PACKET_HEADER_SIZE);		
		PRINT("ERROR: %s\r\n", errorInfo.c_str());
#endif 
		AString info;
		info.Format("[%s:%d] Read packet %d fail", pConnect->GetIp(), pConnect->GetPort(), uID);
		ERROR_LOG(info.c_str());
		OnReadPacketError(info);
	}
#if _DEBUG_NET_
	DSIZE now = mSocketInputStream->dataSize();
	int msCode = 0;
	mSocketInputStream->read(msCode);
	DEBUG_LOG("%d Read packet %d = %u, data %u", msCode, uID, packetSize, nowLength-now-PACKET_HEADER_SIZE);
#endif //_DEBUG_NET_
	return p;

	__LEAVE_FUNCTION_FOXNET

		return NULL;
}

void EventNetProtocol::OnReadPacketError( AString strErrorInfo )
{
	ERROR_LOG(strErrorInfo.c_str());
}

void EventNetProtocol::OnBeforeSendEvent(tNetConnect *pConnect, Logic::tEvent *pEvent)
{
#if    _USE_NICE_PROTOCOL_
    AutoEventFactory f = pEvent->GetEventFactory();
    if (UseNiceDataProtocol() && !f->HasState(eFactoryNoProtocolSave))
    {
        if (f->GetID()>0 && pEvent->GetData().getType()==NICEDATA && pEvent->GetData().count()>0 )
        {
            pEvent->setState(STATE_EVENT_USE_PROTOCOL_SAVE, true);
            int key = f->GetNiceDataProtocolKey();
            pEvent->setState(STATE_EVENT_NEED_SAVE_PROTOCOL, key==0 || key!=pConnect->mInfoData.Find(f->GetID()));
        }
    }
#endif
}

void EventNetProtocol::OnWritePacketError( AString strErrorInfo )
{
	ERROR_LOG(strErrorInfo.c_str());
}

HandPacket EventNetProtocol::GenerateEventPacket(tNetConnect *pConnect,  Logic::tEvent *sendEvent, bool bNeedZip )
{
	Auto<EventPacket> sendPacket = CreatePacket(PACKET_EVENT);
	AssertEx(sendPacket, "��Ϣ����ȡʧ��");
    OnBeforeSendEvent(pConnect, sendEvent);
	AutoEvent evt = sendEvent->GetSelf();
	sendPacket->SetEvent(evt);

#if    _USE_NICE_PROTOCOL_
    if (sendEvent->hasState(STATE_EVENT_NEED_SAVE_PROTOCOL))
        pConnect->mInfoData.Append(sendEvent->GetEventFactory()->GetID(), sendEvent->GetEventFactory()->GetNiceDataProtocolKey());
#endif
	//??? ʹ���Զ�ѹ����ZIPѹ����ʱ�ϴ�
//	if (bNeedZip)
//	{
//#if DEVELOP_MODE
//		int scrSize = 0;
//		UInt64 now = 0;
//#endif
//		// ʵ�ʲ���, 1KB����,ѹ��Ϊ���ٸ��ֽ�,ѹ���Ⱥ�С,Ч�ʷ����ϵ�, Լ4~5K,ѹ��Ϊ1~2K, ѹ���Ƚϸ�, ����ֻ�г���2K�Ž���ѹ��
//		if (sendEvent->NeedAutoCompress() && eventDataSize>=MIN_NEED_COMPRESS_SIZE)
//		{
//			Auto<EventPacket> p2 = CreatePacket(PACKET_COMPRESS_EVENT);
//
//			auto data = sendPacket->GetData();
//			p2->SwapEventData(data);
//			sendPacket = p2;
//#if DEVELOP_MODE
//			//sendEvent->Log("�Զ�����Ϊ [ѹ��] ���� >>> [%d]", eventDataSize);
//			scrSize = eventDataSize;
//			now = TimeManager::NowTick();
//#endif
//		}					
//
//		if (!sendPacket->ReadySendData())
//		{
//			sendEvent->Log("��Ϣ��׼����������ʧ��, ѹ������ʧ��.");
//			ERROR_LOG("��Ϣ��׼����������ʧ��, ѹ������ʧ��");
//			return NULL;
//		}
//#if DEVELOP_MODE
//		if (scrSize>0)
//			NOTE_LOG("Zip [%d]~[], use time [%llu]", scrSize, TimeManager::NowTick()-now);
//#endif
//	}
	return sendPacket;
}

// ����MSG��Ϣ���ݶ�����, ��ȡ�򴴽���Ϣ�е����ݼ�¼
//ARecord EventNetProtocol::GetStructRecord( AutoEvent evt, const AString &structDefineIndex, AutoEvent parentEvent, int nIndex, bool bCreate /*= true */ )
//{
//	AutoTable hTable;
//	if (evt->get(structDefineIndex.c_str(), hTable) && hTable)
//	{
//		return hTable->CreateRecord(nIndex, false);
//	}
//
//	AString tableIndex;
//	if (parentEvent)
//	{
//		tableIndex = evt->GetEventName();
//		tableIndex += ".";
//	}
//	tableIndex += evt->GetEventName();
//
//	AutoTable msgDefineTable = TableManager::getSingleton().GetTable(tableIndex.c_str());
//
//	for (TableIt  tIt(msgDefineTable); tIt._Have(); tIt._End())
//	{
//		ARecord hRecond = tIt.getCurrRecord();
//		Data name = hRecond->get(VALUENAME_COL);
//		Data type = hRecond->get(DEFAULT_COL);
//		if (name.mFieldInfo != NULL
//			&& structDefineIndex == name.mFieldInfo->getTypeString()				
//			)
//		{
//			if ( strcmp(type.c_str(), TABLE_TYPE_STRING_FLAT)==0 )
//			{
//				hTable = TableManager::getSingleton().CreateNewObject("NICE");
//				evt->set(structDefineIndex.c_str(), hTable);
//			}
//			else if (hTable)
//			{
//
//			}
//		}
//	}
//
//	return ARecord();
//}


bool EventNetProtocol::CheckPacketInfo( tNetConnect *pConnect, PacketID_t packetID, UINT packetSize )
{
	if (packetID<=0 || packetID>=mNetPacketFactoryList.size())
	{
		int port;
		AString ip = ((IOCPConnect*)pConnect)->GetRemoteAddr(port);
		NOTE_LOG("WARN: [%s : %d] Read packet data error, packet id %d != [%d] and id >= [%d], size %u then now set remove", 
			ip.c_str(), port, packetID, PACKET_EVENT, (int)mNetPacketFactoryList.size(), packetSize);

		pConnect->SetRemove(true);
		return false;
	}

	//if (packetSize>1280000)
	//{
	//	NOTE_LOG("WARN: [%s : %d] Read packet data error, more 1280000, or packet id > [%d], then now set remove", pConnect->GetIp(), pConnect->GetPort(), packetID);
	//	pConnect->SetRemove(true);
	//	return false;
	//}
	return true;
}


bool EventNetProtocol::SendEvent( tNetConnect *connect, Logic::tEvent *sendEvent )
{
	AssertEx(connect!=NULL, "�����¼�ʱ, ����ΪNULL");
	
	tNetHandle * net = connect->GetNetHandle();
	AssertEx(net!=NULL, "NetHandle is NULL of connect");

	EventNetProtocol  *pEventProtocol = dynamic_cast<EventNetProtocol*>(net->GetNetProtocol());
	AssertEx(pEventProtocol!=NULL, "���͵��¼�Ϊ��");
    
	Auto<EventPacket> pPacket = pEventProtocol->GenerateEventPacket(connect, sendEvent, net->NeedZipSendPacket());
	if (!pPacket)
	{
		ERROR_LOG("������Ϣ��ʧ��>[%s]", sendEvent->GetEventName());
		return false;
	}

	//bool bEncrypt = net->NeedEncryptPacket() && sendEvent->GetEventFactory()->HasState(ePacketStateEncrypt);

	if ( connect->Send(pPacket->GetFactory()->GetPacketID(), pPacket.getPtr()) )
	{
		connect->OnSucceedSendEvent(sendEvent, pPacket.getPtr());		
		return true;
	}
	else
	{
		sendEvent->Log("����ʧ�� X");
	}
	return false;
}


