//-------------------------------------------------------------------
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Untitled
//  @ File Name : tEvent.cpp
//  @ Date : 2011-5-18
//  @ Author : 杨文鸽
// 事件驱动系统
//-------------------------------------------------------------------

#include <stdarg.h>
#include "Event.h"

#include "DataBuffer.h"

#include "EventCenter.h"

#include "LogEvent.h"

#if SAVE_DEBUG_INFO
#	include "Common.h"
#endif
#include "CoroutineTool.h"
#include "Timer.h"
#include "EventFactory.h"

namespace Logic
{
#if SAVE_DEBUG_INFO
	size_t tEvent::gCode = 0;
	int gCurrentCount = 0;
	//ThreadLog  gCountLog("__Event_Count_.txt", "wt");
#endif
	//-------------------------------------------------------------------
	tEvent::tEvent()
#if DEVELOP_MODE
		: mbNowFree(false)
#endif
	{
#if SAVE_DEBUG_INFO
		//mCode = gCode++;
		++gCurrentCount;
		WARN_LOG("+ [now][%d]\n", gCurrentCount);
#endif
		
	}
	tEvent::~tEvent()
	{		
#if SAVE_DEBUG_INFO
		--gCurrentCount;
		WARN_LOG("- [now][%d]\n", gCurrentCount);
#endif
	}
	//-------------------------------------------------------------------

	AString tEvent::GetEventNameInfo() const
	{
#if SAVE_DEBUG_INFO
		char szTemp[256];
#ifdef __LINUX__
        AssertEx(strlen(GetEventName())<=255, "event name more lenght");
        sprintf(szTemp, "%s_%d",GetEventName(),mCode);
#else
		sprintf_s(szTemp,256, "%s_%d", GetEventName(), mCode);
#endif
		return szTemp;
#else
		return GetEventName();

#endif
	}
	
	void tEvent::Log( const char* lpszFormat, ... )
	{		
#if DEVELOP_MODE
		va_list argptr;
		va_start(argptr, lpszFormat);
		AString strInfo = "[";
		strInfo += GetEventNameInfo();
		strInfo += "] ";
		strInfo += lpszFormat;
		TableTool::DebugLog(argptr, strInfo.c_str());
#endif		
	}
	
	//-------------------------------------------------------------------

	bool tEvent::DoEvent(bool bImmediately/* = true*/)
	{ 
		if (mSelf.getPtr()==NULL)
		{
			ERROR_LOG("[%s]已经释放状态, 不可执行 DoEvent", GetEventName());
			return false;
		}
		if (!bImmediately)
		{
			GetEventCenter()->Waiting(mSelf);
			return true;
		}
//#if NEED_MULT_THREAD
//		if (!_CheckInCurrentThread())
//		{
//#if SAVE_DEBUG_INFO
//			Log("〈〉转移到其他线程执行...");
//#endif
//			return true;
//		}
//#endif
		
//#if SAVE_DEBUG_INFO
//		Log("→●开始执行...");
//#endif
		bool bRe = false;
		
		bRe = _DoEvent();
		
		//gLogForEventRun.log("[%s] run %s", GetEventNameInfo().c_str(), (bRe? "OK" : "no"));
#if SAVE_DEBUG_INFO
		//if (bRe) 
		//{
		//	//Log("→▼成功执行事件主体_DoEvent()");
		//}
//		Log("▼ 执行结束 > [%s]", bRe ? "TRUE": "FALSE");
#endif

//#if SAVE_DEBUG_INFO
//		AString strInfo;
//		if (_MakeInfo(strInfo))
//			Log(strInfo.c_str());
//		Log("→□执行结束");
//#endif
		return bRe; 
	}

	bool tEvent::OnEvent( AutoEvent &hEvent )
	{
		if (mSelf.getPtr()==NULL)
		{
			ERROR_LOG("[%s]已经释放状态, 不可执行 OnEvent", GetEventName());
			return false;
		}
		if (/*hEvent==mSelf ||*/ getFinished())
			return false; 
//#if SAVE_DEBUG_INFO
//		if (hEvent)
//			Log(_MakeLinkInfo(hEvent).c_str()); 
//#endif
		/*if (_HasRelation(hEvent)) */return _OnEvent(hEvent); /*return true;*/
	}

	void tEvent::Finish( void )
	{
		if (mSelf.getPtr()==NULL)
		{
			AssertEx(0, "[%s]已经释放状态, 不可执行 Finish", GetEventName());
			return;
		}

		if (getFinished())
			return;

		setFinished(true);  
		//setDie(true);
#if SAVE_DEBUG_INFO
		Log("→■");
#endif
		_NodifyEventFinish();

		_OnFinish(); 
		//_FreeScriptObject();
		//InitData();
		//NiceData temp;
		//temp.swap(GetData());
		//_FreeScriptObject();

		return;
	}

	int tEvent::_GetEnvironmentID(void) const
	{
		return GetEventCenter()->_GetEnvironmentID();
	}


	void tEvent::StopWaitTime()
	{
		GetEventCenter()->StopWaitTimeEvent(GetSelf());
	}


	void tEvent::StopUpdate()
	{
		GetEventCenter()->StopUpdateEvent(GetSelf());
	}


	void tEvent::PauseTime()
	{
		GetEventCenter()->PauseEvent(GetSelf());
	}


	void tEvent::ContinueTime()
	{
		if (GetEventFactory() && !getFinished())
			GetEventCenter()->ContinueEvent(GetSelf());
	}

	void tEvent::Dump( void )
	{
//#if SAVE_DEBUG_INFO
		if (GetEventFactory()->HasState(eNeedLog))
			Log( GetData().dump().c_str() );
//#endif
	}

	bool tEvent::DoTimeOver( void )
	{
		return _OnTimeOver();
	}

	bool tEvent::removeData( const char* dataName )
	{
		return GetData().remove(dataName);
	}

	bool tEvent::existData( const char *dataName, AString *resultInfo )
	{
		return GetData().existData(dataName, resultInfo);
	}

	bool tEvent::SameEventType( int eventTypeIndex )
	{
		return GetEventFactory()->GetNameIndex()==eventTypeIndex;
	}


    void tEvent::_dump()
    {
        NOTE_LOG("%s data >\r\n%s", GetEventNameInfo().c_str(), GetData().dump().c_str());
    }

 //   void tEvent::setFormat( const char* key, const char* szVal, ... )
	//{
	//	AString val;
	//	va_list va;
	//	va_start(va, szVal);
	//	AString tempVal;
	//	tempVal.Format(va, szVal);
	//	
	//	set(key, tempVal.c_str());
	//}

	void tEvent::Release()
	{
#if USE_SAFT_RELEASE_EVENT
		tEventCenter *pCenter = GetEventCenter();
		if (pCenter!=NULL)
			pCenter->mFreeEventList.push_back(this);
		else
			delete this;
#else
		if (GetEventFactory())
		{
			GetEventFactory()->FreeEvent(this);
		}
		else
			delete this;
#endif
	}


	void tEvent::AsyncDo()
	{
		ASYNC(&tEvent::_AsyncDo, this);
	}


	bool tEvent::AWait(float waitTime)
	{
		return tTimer::AWaitTime((uint)(waitTime * 1000));
	}



}