#include "NetTest.h"
#include "IOCPServerNet.h"
//#include "TCPThreadNet.h"
#include "TimeManager.h"
#include "EventCenter.h"
#if __WINDOWS__
#include <windows.h>
#else
UINT GetCurrentThreadId() { return 1; }
#endif
//-------------------------------------------------------------------------
//using namespace Logic;

#define  TEST_THREAD_CLIENT_NET 1

#define NET_TEST_COUNT	1
#define NET_TEST_TIME	12000

//-------------------------------------------------------------------------
class TestClientNet : public IOCPClientNet //TCPThreadClientNet //
{
public:
	AutoEventCenter mCenter;
	NetTest	*mNetTest;

	virtual Logic::tEventCenter* GetEventCenter(void) const { return (Logic::tEventCenter*)mCenter.getPtr(); }

	virtual void SetEventCenter(Logic::tEventCenter *pCenter){ mCenter = pCenter; }

	virtual int GetSafeCode(){ return 1; }

	virtual void OnConnected() override 
	{
		Hand<CS_NetTestEvent> testEvt = GetEventCenter()->StartEvent("CS_NetTestEvent");
		//???testEvt->mNetConnect = mTestNet[i]->GetConnect(0);
		testEvt->mNetID = 10;
		testEvt->mpNetTest = mNetTest;
		testEvt->DoEvent();
	}
};


void NetTest::Run(const char *szIp, int nPort)
{
#if TEST_THREAD_CLIENT_NET
	Hand<TestClientNet> mTestNet[NET_TEST_COUNT];
#else
	Hand<EventClientNet> mTestNet[NET_TEST_COUNT];
#endif
	bool mbConnect[NET_TEST_COUNT];
	AutoEventCenter mEventCenter[NET_TEST_COUNT];
	for (int i=0; i<NET_TEST_COUNT; ++i)
	{
#if TEST_THREAD_CLIENT_NET
		mTestNet[i] = MEM_NEW TestClientNet();
		Hand<TestClientNet> tt = mTestNet[i];
		tt->mNetTest = this;
#else
		mTestNet[i] = MEM_NEW EventClientNet();
#endif
		AutoEventCenter testCenter = MEM_NEW Logic::EventCenter();
		mEventCenter[i] = testCenter;
		testCenter->RegisterEvent("CS_NetTestEvent", MEM_NEW Logic::EventFactory<CS_NetTestEvent>());
		mTestNet[i]->SetEventCenter(testCenter.getPtr());
#if TEST_THREAD_CLIENT_NET
		//mTestNet[i]->StartNet();
#endif
		testCenter->_SetNetTool(0, mTestNet[i]);
#if TEST_THREAD_CLIENT_NET
		mTestNet[i]->Connect(szIp, nPort, 6000); 
#else
		mTestNet[i]->Connect(szIp, nPort, 0);
#endif
		mbConnect[i] = false;		
	
#if !TEST_THREAD_CLIENT_NET
		Hand<CS_NetTestEvent> testEvt = mTestNet[i]->GetEventCenter()->StartEvent("CS_NetTestEvent");
		testEvt->mNetID = i;
		testEvt->mpNetTest = this;
		testEvt->DoEvent();	
#endif
	}

	UInt64  testTime = TimeManager::NowTick();
	//while (!mbFinish)
	//{
//		if (TimeManager::NowTick()-testTime>NET_TEST_TIME*1000)
//			mbFinish = true;
//		for (int i=0; i<NET_TEST_COUNT; ++i)
//		{
//#if TEST_THREAD_CLIENT_NET
//			if (!mbConnect[i])
//			{
//				if (mTestNet[i]->IsConnected())
//				{
//					mbConnect[i] = true;					
//					for (int n=0; n<100; ++n)
//					{
//						Hand<CS_NetTestEvent> testEvt = mTestNet[i]->GetEventCenter()->StartEvent("CS_NetTestEvent");
//						//???testEvt->mNetConnect = mTestNet[i]->GetConnect(0);
//						testEvt->mNetID = i;
//						testEvt->mpNetTest = this;
//						testEvt->DoEvent();					
//					}
//				}
//			}
//#endif
//			mTestNet[i]->GetEventCenter()->ProcessEvent();
//			mTestNet[i]->Process();
//		}
//
//		TimeManager::Sleep(10);
//	}

	while (true)
	{
		for (int i = 0; i < NET_TEST_COUNT; ++i)
		{
			mTestNet[i]->GetEventCenter()->ProcessEvent();
			mTestNet[i]->Process();
			TimeManager::Sleep(100);
		}
	}

	DEBUG_LOG("Net test finish s [%llu] rev [%llu] at thread [%u]", msCode, msLastCode, GetCurrentThreadId());
}

void CS_NetTestEvent::_OnResp( AutoEvent &respEvent )
{
	//!!! DEBUG_LOG("[%d] net rev response, test code [%llu] at thread [%u]", mNetID, mTestCode, ::GetCurrentThreadId());
	mpNetTest->msLastCode = mTestCode;

	if (mpNetTest->mbFinish)
	{
		DEBUG_LOG("*[%d] Net test finish s [%llu] rev [%llu] at thread [%u]", mNetID, mpNetTest->msCode, mpNetTest->msLastCode, GetCurrentThreadId());
	}
	else
	{
		//setFinished(false);
		
		//DoEvent(false);
		Hand<CS_NetTestEvent> evt = respEvent->StartEvent("CS_NetTestEvent");
		evt->mNetID = mNetID;
		evt->mpNetTest = mpNetTest;
		//evt->_setTarget(_getTarget());
		evt->DoEvent();

		evt = respEvent->StartEvent("CS_NetTestEvent");
		evt->mNetID = mNetID;
		evt->mpNetTest = mpNetTest;
		//evt->_setTarget(_getTarget());
		evt->DoEvent();
	}

}
