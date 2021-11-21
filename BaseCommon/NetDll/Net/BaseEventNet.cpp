
#include "BaseEventNet.h"
#include "Common.h"
#include "NetAddress.h"
#include <stdarg.h>
#include "ClientEvent.h"
//----------------------------------------------------------------------------------------------

//bool tBaseEventNet::Send( Logic::tEvent *sendEvent, int target )
//{
//	tNetConnect *p = GetConnect(target);
//	if (p)
//		return Send(sendEvent, p);
//	else
//		sendEvent->Log("����ʧ��X, ��ȡ������������[%d]", target);
//
//	return false;
//}
//
//bool tBaseEventNet::Send( Logic::tEvent *sendEvent, tNetConnect *pConnect )
//{
//	AssertEx(sendEvent!=NULL, "���͵��¼�Ϊ��");
//
//	Packet *pPacket = mEventNetProtocol.GenerateEventPacket(sendEvent, NeedZipSendPacket());
//	AssertEx(pPacket, "������Ϣ��ʧ��");
//	
//	pPacket->SetNeedEncrypt(NeedEncryptPacket());
//
//	if ( pConnect->Send(pPacket) )
//	{
//		pConnect->OnSucceedSendEvent(sendEvent, pPacket);		
//		return true;
//	}
//	else
//	{
//		sendEvent->Log("����ʧ�� X");
//	}
//
//	return false;
//}

//----------------------------------------------------------------------------------------------


tBaseEventNet::tBaseEventNet(bool bCreateEventProtocol)
{
	if (bCreateEventProtocol)
		mNetProtocol = MEM_NEW EventNetProtocol();
}
//-------------------------------------------------------------------------------------------------------------------

tBaseEventNet::~tBaseEventNet()
{

}


//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------


bool tBaseEventNet::OnReceiveEvent( tNetConnect *revConnect, Logic::tEvent *revEvent )
{
	__ENTER_FUNCTION

	AutoEvent hE = revEvent;
	if (hE)
	{
		hE->_OnBindNet(revConnect);
		hE->Log("�ɹ�����");
//		hE->Log("��ʼִ��������Ϣ�¼�...");
		try{
			bool re = hE->DoEvent(true);
			//		hE->Log("ִ�н��[%s]", re ? "TRUE":"FALSE");

			return re;
		}
		catch (std::exception &e)
		{
			bool bNeedRemove = NeedRemoveOnMsgError();
			if (bNeedRemove)
			{
				ERROR_LOG("[%s]�����¼�ִ�д���, �Ƴ�����>[%d]%s\n Exception>[%s]", hE->GetEventName(), revConnect->GetNetID(), NetAddress::GetAddrInfo(revConnect->GetIPKey()).c_str(), e.what());
				revConnect->Close();			
			}
			else
			{
				ERROR_LOG("[%s]�����¼�ִ�д���, ����[%d]%s\n Exception>[%s]", hE->GetEventName(), revConnect->GetNetID(), NetAddress::GetAddrInfo(revConnect->GetIPKey()).c_str(), e.what());
				Hand<Logic::tClientEvent> clientEvt = hE;
				hE->Finish();
			}
			
			hE->Dump();
			
		}
		catch (...)
		{
			bool bNeedRemove = NeedRemoveOnMsgError();
			if (bNeedRemove)
			{
				ERROR_LOG("[%s]�����¼�ִ�д���, �Ƴ�����>[%d]%s\n", hE->GetEventName(), revConnect->GetNetID(), NetAddress::GetAddrInfo(revConnect->GetIPKey()).c_str());
				revConnect->Close();			
			}
			else
			{
				ERROR_LOG("[%s]�����¼�ִ�д���, ����[%d]%s\n", hE->GetEventName(), revConnect->GetNetID(), NetAddress::GetAddrInfo(revConnect->GetIPKey()).c_str());
				Hand<Logic::tClientEvent> clientEvt = hE;
				hE->Finish();
			}
			
			hE->Dump();
			
		}
	}
	//}
	return false;
	__LEAVE_FUNCTION
		return false;
}

void tBaseEventNet::Log( const char *szInfo, ... )
{
#if DEVELOP_MODE
	va_list va;
	va_start(va, szInfo);
	TableTool::DebugLog(va, szInfo);
#endif
}

//----------------------------------------------------------------------