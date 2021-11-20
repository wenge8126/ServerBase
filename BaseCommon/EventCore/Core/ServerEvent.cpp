
#include "ServerEvent.h"
#include "NetHandle.h"

#include "EventCenter.h"
#include "TimeManager.h"
#include "FileDataStream.h"

namespace Logic
{

	tServerEvent::tServerEvent()
		: mID(0)
#if DEVELOP_MODE
		, mSendTime(0)
		, mResponseTime(0)
#endif		
	{

	}

	tServerEvent::~tServerEvent()
	{
		ReleaseRespID();
		if (GetEventCenter()!=NULL)
			GetEventCenter()->GetResponseEventFactory()->FreeServerEvent(this);
	}

	bool tServerEvent::_OnEvent( AutoEvent &hEvent )
	{
#if DEVELOP_MODE
		mResponseTime = TimeManager::Now();
#endif
		//����ֹͣ�ȴ�ʱ��, ������ջظ���ʱ
		StopWaitTime();
		// �������֤�Ѿ��ڻظ��¼�������ȷ��
		//hEvent->Dump();
		_OnResp(hEvent);
		ReleaseRespID();
		if (_NeedFinishWhenResponsed())
			Finish();
		return true;		
	}

	void tServerEvent::StartByConnect(tNetConnect *pConn)
	{
		mNetConnect = pConn;
		Start();
	}

	void tServerEvent::InitData()
	{
		tBaseNetEvent::InitData();
		GetEventCenter()->GetResponseEventFactory()->FreeServerEvent(this);
		mID = 0;
	}

	void tServerEvent::AlloctRespID(void) 
	{
		GetEventCenter()->GetResponseEventFactory()->AllotEventID(this);
		//ReleaseRespID();
		//size_t time = 0;

		//mID = GetEventCenter()->AutoAlloctKey();
		//
		////set(RESP_ID_KEY, (EVENT_ID)id);
		//Log("Alloct respID >[ %llu ]", mID);
	}

	void tServerEvent::ReleaseRespID(void)
	{
	}


	bool tServerEvent::_currentCanFinish( void )
	{
		Data d = get(CURRENT_CAN_FINISH);
		if (d.empty() || (bool)d)
			return true;
		return false;
	}


	bool tServerEvent::DoEvent( bool bImmediately /*= true*/ )
	{
#if DEVELOP_MODE
		if (bImmediately)
		{
			mSendTime = TimeManager::Now();
		}
#endif
		if (bImmediately)
			AlloctRespID();
		bool re = parent::DoEvent(bImmediately);
		
		if (getFinished())
		{
			return re;
		}

		if (!bImmediately)
			return re;


		bool b = Send();
		if (!b)
		{
			NOTE_LOG("Error: [%s] send fail", GetEventName());
			OnSendFail();
		}

		return re;
	}

	bool tServerEvent::DoTimeOver()
	{
#if DEVELOP_MODE
		DEBUG_LOG("%s SeverEvent %s over time\r\n", TimeManager::ToTimeString(TimeManager::Now()).c_str(), name.c_str());
#endif 
		bool re = parent::DoTimeOver();
#if NEED_RESEND_ONTIMEOVER
		// ÿһ�γ�ʱ��, ���û�����, �����ظ�����һ��
		if ( !getFinished() 
			&& !hasState(STATE_ALREADY_RESPONSE)
			)
		{			
			setState(STATE_ALREADY_RESPONSE, true);

			WaitTime(6);
			Send();			
			return re;
		}
#endif

		setState(STATE_EVENT_OVERTIME, true);

		return re;
	}


#if DEVELOP_MODE
	void tServerEvent::Finish()
	{
		if (getFinished())
			return;

		parent::Finish();


#if DEVELOP_MODE
		//AString info;
		//if (mSendTime!=0)
		//{
		//	info += "Send ";
		//	info += TimeManager::ToTimeString(mSendTime);
		//	info += ", Response ";
		//	if (mResponseTime!=0)
		//		info += TimeManager::ToTimeString(mResponseTime);
		//	else
		//		info += " no response when finish!";
		//	info +=", Total use time > ";
		//	info += TimeManager::Now()-mSendTime;
		//}
		//else
		//	info += "WARN: not send may be do not DoEvent";
		//Log(info.c_str());
#endif 
	}




#endif

}


