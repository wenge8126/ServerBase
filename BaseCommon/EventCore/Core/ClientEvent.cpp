
#include "ClientEvent.h"
#include "EventCenter.h"
#include "NetHandle.h"
#include "ResponseEvent.h"

#define  NET_MAIN	0

namespace Logic
{
	//----------------------------------------------------------------------
	tClientEvent::tClientEvent() 
		: mID(0)
	{

	}

	//----------------------------------------------------------------------
	void tClientEvent::Finish()
	{
		if (!hasState(STATE_ALREADY_RESPONSE))
		{
			setState(STATE_ALREADY_RESPONSE, true);

			if (!CheckOnResponse())
			{
				Log("Error: ׼���ظ�ʱ, ���ʧ��");
				return;
			}
			EVENT_ID respID = mID; // get(RESP_ID_KEY);
			// ���û�з���ظ�ID, Ĭ��Ϊ����Ҫ�ظ�
			if (respID!=0) 
			{						
				AutoEvent hResp = GetResponseEvent();

				//!!! WARN: �����溯��������Ե�ǰ�¼���������������, ��ʹ�� RESP_ID_KEY ��Ӧ��ֵ�仯, ԭ����δ���
				SetRespData(hResp);
				//hResp->set(RESP_ID_KEY, respID); 
				Hand<ResponseEvent> r = hResp;
				r->mID = mID;
				//hResp->set(RESP_KEY_TIME, (int)time);
#if DEVELOP_MODE
				hResp->set("_SERVER_EVENT_", GetEventNameInfo().c_str());
#endif

				SendResponce(hResp);
			}			
			else
			{
				LOG_RED;
				ERROR_LOG("ERROR: %s Code error, reqeust event id = 0", GetEventName());
				LOG_WHITE;
			}
		}
		parent::Finish();
	}
	//----------------------------------------------------------------------

	EVENT_ID tClientEvent::GetEventID()
	{
		return mID;
	}

	AutoEvent tClientEvent::GetResponseEvent()
	{
		if (!mResponseEvt && GetEventFactory())
			mResponseEvt = GetEventCenter()->GetResponseEventFactory()->StartEvent(); 

		return mResponseEvt;
	}

	void tClientEvent::SendResponce( AutoEvent hRespEvent )
	{
		if (mNetConnect)
		{
			if (!mNetConnect->SendEvent(hRespEvent.getPtr()) )
			{
				Log("Error: �ظ�����ʧ��!");
			}			
		}
		else
		{
			hRespEvent->Send(NET_MAIN, 0);
			Log("�ͻ����¼����緢�Ͷ���Ϊ�� >>> ����_onReciveRun() ���ܱ�����");
		}
	}


	//----------------------------------------------------------------------
	//----------------------------------------------------------------------


}