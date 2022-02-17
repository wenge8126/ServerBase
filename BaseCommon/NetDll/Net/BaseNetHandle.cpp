#include "BaseNetHandle.h"
#include "EventProtocol.h"
#include <stdarg.h>

//bool tBaseNetHandle::Send( Logic::tEvent *sendEvent, int target )
//{
//	tNetConnect *p = GetConnect(target);
//	if (p)
//		return Send(sendEvent, p);
//	else
//		sendEvent->Log("����ʧ��X, ��ȡ������������[%d]", target);
//
//	return false;
//}

bool tBaseNetHandle::Send( Logic::tEvent *sendEvent, tNetConnect *pConnect )
{
	AssertEx(sendEvent!=NULL, "���͵��¼�Ϊ��");

	EventNetProtocol *pNetProtocol = dynamic_cast<EventNetProtocol*>(mpNetProtocol);
	if (pNetProtocol==NULL)
	{
		Log("ERRRO: Send fail, net protocol is not EventNetProtocol");
		return false;
	}
    
	HandPacket pPacket = pNetProtocol->GenerateEventPacket(pConnect, sendEvent, NeedZipSendPacket());
	AssertEx(pPacket, "������Ϣ��ʧ��");

	//pPacket->SetNeedEncrypt(NeedEncryptPacket());
	bool bEncrypt = NeedEncryptPacket() && sendEvent->GetEventFactory()->HasState(ePacketStateEncrypt);
	if ( pConnect->Send(pPacket.getPtr(), bEncrypt) )
	{
		pConnect->OnSucceedSendEvent(sendEvent, pPacket.getPtr());		
		return true;
	}
	else
	{
		sendEvent->Log("����ʧ�� X");
	}

	return false;
}

void tBaseNetHandle::ProcessReceivePacket(tNetConnect *pConnect, Packet *pPacket)
{
	pPacket->Execute(pConnect);
}

bool tBaseNetHandle::OnReceiveEvent( tNetConnect *revConnect, Logic::tEvent *revEvent )
{
	__ENTER_FUNCTION

		AutoEvent hE = revEvent;
	if (hE)
	{
		hE->_OnBindNet(revConnect);
		hE->Log("�ɹ�����");
		//		hE->Log("��ʼִ��������Ϣ�¼�...");
		bool re = hE->DoEvent(true);
		//		hE->Log("ִ�н��[%s]", re ? "TRUE":"FALSE");

		return re;
	}
	//}
	return false;
	__LEAVE_FUNCTION
		return false;
}

void tBaseNetHandle::Log( const char *szInfo, ... )
{
#if DEVELOP_MODE
	va_list va;
	va_start(va, szInfo);
	TableTool::DebugLog(va, szInfo);
#endif
}
