//#include "stdafx.h"

#include "TimeManager.h"

#include "Assertx.h"
#include <memory>

#ifdef __LINUX__
    #include <unistd.h>
#else
    #include <windows.h>
#endif
#include "Common.h"

#define DEBUG_TIME_BOOST	0
#define TEST_TIME_RATE		(0.01)
#define TEST_TIME_BOOST		(100)


UInt64 TimeManager::m_uCurrentSecondTime = 0;

time_t gStartTime = 0;
//TimeManager* 	g_pTimeManager=NULL ;

TimeManager::TimeManager( )
{
__ENTER_FUNCTION

	Init();

__LEAVE_FUNCTION
}

TimeManager::~TimeManager( )
{
__ENTER_FUNCTION


__LEAVE_FUNCTION
}

BOOL TimeManager::Init( )
{
	__ENTER_FUNCTION

#if defined(__WINDOWS__)
		m_StartTime = GetTickCount64();
#elif defined(__LINUX__)
		timeval _tstart;
	gettimeofday(&_tstart, NULL);
	mStartMillsecond = (UInt64)_tstart.tv_sec * 1000 + (__suseconds_t)(_tstart.tv_usec*0.001f);
#endif
	SetTime( ) ;

	gStartTime = m_SetTime;

	return TRUE ;

__LEAVE_FUNCTION

	return FALSE ;
}

UInt64 TimeManager::NowTick()
{
	__ENTER_FUNCTION
		UInt64 m_CurrentTime = 0;
#if defined(__WINDOWS__)
	m_CurrentTime = GetTickCount64();
#elif defined(__LINUX__)
	gettimeofday(&GetMe()._tend, NULL);
	UInt64 t2 = (UInt64)GetMe()._tend.tv_sec * 1000 + (__suseconds_t)(GetMe()._tend.tv_usec*0.001f);
	m_CurrentTime = (t2 - GetMe().mStartMillsecond);
#endif

#if DEBUG_TIME_BOOST
	m_CurrentTime = m_StartTime + (m_CurrentTime - m_StartTime) * TEST_TIME_BOOST;
#endif

    return m_CurrentTime ;


    __LEAVE_FUNCTION

        return 0 ;
}


Int64 TimeManager::SysMicrosecond()
{
#if __LINUX__
	timeval tv;
	gettimeofday(&tv, NULL);
	return (Int64)tv.tv_sec * 1000000 + (Int64)tv.tv_usec;
#else	
	// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME   (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	Int64 tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	return tt;
#endif // _WIN32
//	return 0;
}

UInt64 TimeManager::GetSecondTime(int years, int month, int day, int hour, int minute, int second)
{
	tm tempTm;
	memset(&tempTm, 0, sizeof(tm));
	tempTm.tm_year = years -1900;
	if (tempTm.tm_year < 0)
		return 0;
	tempTm.tm_mon = month-1;
	if (tempTm.tm_mon < 0 || tempTm.tm_mon>11)
		return 0;
	tempTm.tm_mday = day;
	if (tempTm.tm_mday < 0 || tempTm.tm_mday>31)
		return 0;
	tempTm.tm_hour = hour;
	if (tempTm.tm_hour < 0 || tempTm.tm_hour>23)
		return 0;
	tempTm.tm_min = minute;
	if (tempTm.tm_min < 0 || tempTm.tm_min>59)
		return 0;
	tempTm.tm_sec = second;
	if (tempTm.tm_sec < 0 || tempTm.tm_sec>60)
		return 0;
	time_t t = mktime(&tempTm);
	if (t == -1)
		return t;
	tm tempTm2 = *localtime(&t);
	return t;
}


UInt64 TimeManager::AnalyzeTime(const AString &timeString, const char* dateSpace)
{
	StringArray dateList;
	StringArray dateStringList;
	StringArray timeList;
	int second = 0;

	if (AString::Split(timeString.c_str(), dateList, " ", 3) < 2)
	{
		goto _errorTime;
	}
	if (AString::Split(dateList[0].c_str(), dateStringList, dateSpace, 4) < 3)
	{
		goto _errorTime;
	}

	if (AString::Split(dateList[1].c_str(), timeList, ":", 3) < 2)
	{
		goto _errorTime;
	}
	if (timeList.size() >= 3)
		second = TOINT(timeList[2].c_str());
	return GetSecondTime(TOINT(dateStringList[0].c_str()), TOINT(dateStringList[1].c_str()), TOINT(dateStringList[2].c_str()), TOINT(timeList[0].c_str()), TOINT(timeList[1].c_str()), second);	

_errorTime:
	ERROR_LOG("%s日期时间不正确, 日期时间格式为 年/月/日 小时:分钟", timeString.c_str());
	return 0;
}

UInt64 TimeManager::AnalyzeZeroTime(UInt64 time)
{
	if (time < 1)
		return 0;

	time_t t = (time_t)time;
	tm *tempTm = localtime(&t);

	tempTm->tm_hour = 0;
	tempTm->tm_min = 0;
	tempTm->tm_sec = 0;

	time_t zeroTime = mktime(tempTm);
	return (UInt64)zeroTime;
}

UINT	TimeManager::CurrentDate()
{
	__ENTER_FUNCTION

	SetTime( ) ;
	UINT Date;
	ConvertTU(&m_TM,Date);

	return Date;

	__LEAVE_FUNCTION

		return 0;
}


VOID TimeManager::CurrentDate(int &year, int &month, int &day, int &hour, int &minute, int &second)
{
	SetTime( ) ;
	year = m_TM.tm_year;
	month = m_TM.tm_mon;
	day = m_TM.tm_mday;
	hour = m_TM.tm_hour;
	minute = m_TM.tm_min;
	second = m_TM.tm_sec;
}

VOID TimeManager::SetTime( )
{
__ENTER_FUNCTION

	time( &m_SetTime ) ;
#if __LINUX__
	m_TM = * localtime(&m_SetTime);
#else
	localtime_s( &m_TM, &m_SetTime ) ;
#endif

__LEAVE_FUNCTION
}

// 得到标准时间
time_t TimeManager::GetANSITime( )
{
	__ENTER_FUNCTION

		SetTime();

	__LEAVE_FUNCTION

#if DEBUG_TIME_BOOST
		m_SetTime = m_SetTime + (m_SetTime-gStartTime)*TEST_TIME_BOOST;
#endif

	return m_SetTime;
}

UINT TimeManager::Time2DWORD( )
{
__ENTER_FUNCTION

	SetTime( ) ;

	UINT uRet=0 ;

	uRet += GetYear( ) ;
	uRet -= 2000 ;
	uRet =uRet*100 ;

	uRet += GetMonth( )+1 ;
	uRet =uRet*100 ;

	uRet += GetDay( ) ;
	uRet =uRet*100 ;

	uRet += GetHour( ) ;
	uRet =uRet*100 ;

	uRet += GetMinute( ) ;

	return uRet ;

__LEAVE_FUNCTION

	return 0 ;
}

UINT TimeManager::DiffTime( UINT Date1, UINT Date2 )
{
__ENTER_FUNCTION

	tm S_D1, S_D2 ;
	ConvertUT( Date1, &S_D1 ) ;
	ConvertUT( Date2, &S_D2 ) ;
	time_t t1,t2 ;
	t1 = mktime(&S_D1) ;
	t2 = mktime(&S_D2) ;
	UINT dif = (UINT)(difftime(t2,t1)*1000) ;
	return dif ;

__LEAVE_FUNCTION

	return 0 ;
}

VOID TimeManager::ConvertUT( UINT Date, tm* TM )
{
__ENTER_FUNCTION

	Assert(TM) ;
	memset( TM, 0, sizeof(tm) ) ;
	TM->tm_year = (Date>>26)&0xf ;
	TM->tm_mon  = (Date>>22)&0xf ;
	TM->tm_mday = (Date>>17)&0x1f ;
	TM->tm_hour = (Date>>12)&0x1f ;
	TM->tm_min  = (Date>>6) &0x3f ;
	TM->tm_sec  = (Date)    &0x3f ;

__LEAVE_FUNCTION
}

VOID TimeManager::ConvertTU( tm* TM, UINT& Date )
{
__ENTER_FUNCTION

	Assert( TM ) ;
	Date = 0 ;
	Date += (TM->tm_yday%10) & 0xf ;
	Date = (Date<<4) ;
	Date += TM->tm_mon & 0xf ;
	Date = (Date<<4) ;
	Date += TM->tm_mday & 0x1f ;
	Date = (Date<<5) ;
	Date += TM->tm_hour & 0x1f ;
	Date = (Date<<5) ;
	Date += TM->tm_min & 0x3f ;
	Date = (Date<<6) ;
	Date += TM->tm_sec & 0x3f ;

__LEAVE_FUNCTION
}

UINT TimeManager::GetDayTime( )
{
__ENTER_FUNCTION

	time_t st ;
	time( &st ) ;
	tm* ptm = localtime( &m_SetTime ) ;

	UINT uRet=0 ;

	uRet  = (ptm->tm_year-100)*1000 ;
	uRet += ptm->tm_yday ;

	return uRet ;

__LEAVE_FUNCTION

	return 0 ;
}

WORD TimeManager::GetTodayTime()
{
__ENTER_FUNCTION
	time_t st ;
	time( &st ) ;
	tm* ptm = localtime( &m_SetTime ) ;

	WORD uRet=0 ;

	uRet  = ptm->tm_hour*100 ;
	uRet += ptm->tm_min ;

	return uRet ;

__LEAVE_FUNCTION

return 0 ;
}

BOOL TimeManager::FormatTodayTime(WORD& nTime)
{
__ENTER_FUNCTION
	BOOL ret = FALSE;

	WORD wHour = nTime / 100;
	WORD wMin = nTime % 100;
	WORD wAddHour = 0;
	if( wMin > 59 )
	{
		wAddHour = wMin / 60;
		wMin = wMin % 60;
	}
	wHour += wAddHour;
	if( wHour > 23 )
	{
		ret = TRUE;
		wHour = wHour % 60;
	}

	return ret;

__LEAVE_FUNCTION

return FALSE ;
}

TimeManager& TimeManager::GetMe()
{
	static TimeManager sTimeMgr;
	return sTimeMgr;
}


int TimeManager::GetTimeSecond(int hour, int minute, int second)
{
	time_t now;
	time(&now);

	tm tempTm = *localtime(&now);

	tempTm.tm_hour = hour;
	tempTm.tm_min = minute;
	tempTm.tm_sec = second;
	time_t t = mktime(&tempTm);

	tempTm.tm_hour = 0;
	tempTm.tm_min = 0;
	tempTm.tm_sec = 0;

	time_t t2 = mktime(&tempTm);

	return (int)(t - t2);
}


int TimeManager::GetTimeSecond(const AString &time)
{
	StringArray temp;
	if (AString::Split(time.c_str(), temp, ":", 4) >= 3)
	{
		return GetTimeSecond(TOINT(temp[0].c_str()), TOINT(temp[1].c_str()), TOINT(temp[2].c_str()));
	}
	return 0;
}


int TimeManager::GetTimeSecond(UInt64 time)
{
	time_t t = (time_t)time;
	tm tempTm = *localtime(&t);

	time_t t1 = mktime(&tempTm);

	tempTm.tm_hour = 0;
	tempTm.tm_min = 0;
	tempTm.tm_sec = 0;

	time_t t2 = mktime(&tempTm);

	return (int)(t1 - t2);
}

void TimeManager::Sleep( int milSecond )
{
#if __WINDOWS__
	::Sleep(milSecond);
#elif __LINUX__
	usleep(milSecond*1000);
#else
	AssertEx(0, "²»Ö§³ÖµÄÏµÍ³");
#endif
}


UInt64 TimeManager::Now()
{
	time_t t;
	time( &t ) ;
	return (UInt64)t;
}

AString TimeManager::GetDate()
{
	AString date;
	date.Format("%4d/%02d/%02d", GetYear(), GetMonth()+1, GetDay());
	return date;
}

AString TimeManager::GetTime()
{
	AString time;
	time.Format( "%02d:%02d:%02d", GetHour(), GetMinute(), GetSecond() );
	return time;
}

AString TimeManager::GetDateTime()
{
	AString dateTime;
	dateTime = GetDate();
	dateTime += " ";
	dateTime += GetTime();

	return dateTime;
}

AString TimeManager::ToDateTime( UInt64 dateTime, bool bHaveSecond )
{
    time_t t = (time_t)dateTime;
    tm *tempTm = localtime(&t);

	AString strData;
	if (tempTm!=NULL)
	{
		if (bHaveSecond)
			strData.Format("%4d/%02d/%02d %02d:%02d:%02d", tempTm->tm_year+1900, tempTm->tm_mon+1, tempTm->tm_mday, tempTm->tm_hour, tempTm->tm_min, tempTm->tm_sec);
		else
			strData.Format("%4d/%02d/%02d %02d:%02d", tempTm->tm_year + 1900, tempTm->tm_mon + 1, tempTm->tm_mday, tempTm->tm_hour, tempTm->tm_min);
	}
	else
		strData.Format("*Invalid data time (%llu)", dateTime);

	return strData;
}

AString TimeManager::ToTimeString( UInt64 dateTime )
{
	time_t t = (time_t)dateTime;
	tm *tempTm = localtime(&t);

	AString strData;
	if (tempTm!=NULL)
	{
		strData.Format("%2d:%2d:%2d", tempTm->tm_hour, tempTm->tm_min, tempTm->tm_sec);
	}
	else
		strData.Format("*Invalid data time (%llu)", dateTime);
	return strData;
}

int TimeManager::UnixTimeToDate( UInt64 unixTime, int timeArray[] )
{
	if(unixTime < 1)
		return 0;

	time_t t = (time_t)unixTime;
	tm *tempTm = localtime(&t);

	timeArray[0] = tempTm->tm_year + 1900;
	timeArray[1] = tempTm->tm_mon + 1;
	timeArray[2] = tempTm->tm_mday;
	timeArray[3] = tempTm->tm_hour;
	timeArray[4] = tempTm->tm_min;
	timeArray[5] = tempTm->tm_sec;

	return 1;
}

bool TimeManager::UnixTimeisToday( UInt64 unixTime )
{	
	static int oneDaySecond = 24 * 3600;
	UInt64 nowTime = (int)TimeManager::Now();
	return (nowTime/oneDaySecond == unixTime/oneDaySecond);	
}

int TimeManager::GetWeekBySecond(UInt64 sec)
{
	time_t t = (time_t)sec;
	tm *tempTm = localtime(&t);
	return tempTm->tm_wday;
}

int TimeManager::GetMonthBySecond(UInt64 sec)
{
	time_t t = (time_t)sec;
	tm *tempTm = localtime(&t);
	return tempTm->tm_mon;
}

int TimeManager::GetTimingSecond(int mWeek, int mHour, int mMinute, bool bNowNext, bool bLog, UInt64 *pTimeSecond)
{
	if (mWeek<0 || mWeek>6 || mHour<0 || mHour>23 || mMinute<0 || mMinute>59)
	{
		ERROR_LOG("提供的时间不正确 >%d - %d - %d, 格式为 星期(0~6)-小时(0~23)-分钟(0~59)", mWeek, mHour, mMinute);
		return -1;
	}
	// 下下周一, 用当前时间+ 7 天后的, 即在下下周内, 然后取周一的时间
	time_t now;
	time(&now);

	time_t startTime = now;

	tm tempTm = *localtime(&now);
	if (mWeek > tempTm.tm_wday)
		startTime = now + (mWeek -tempTm.tm_wday)*24*3600; // 如果取得相同星期必须加上当前星期与指定星期之间的差, 不可以直接设置星期, 转换后无效
	else if (mWeek<tempTm.tm_wday)
		startTime = now + (mWeek+7-tempTm.tm_wday) * 24 *3600;

	tempTm = *localtime(&startTime);

	tempTm.tm_hour = mHour;		
	tempTm.tm_min = mMinute;
	tempTm.tm_sec = 0;

	time_t waitTime = mktime(&tempTm);

	if (!bNowNext && waitTime==now)
		return 0;

	if (waitTime<=now)
	{
		time_t tempSecond = startTime + (7*24*3600);
		tempTm = *localtime(&tempSecond);

		tempTm.tm_hour = mHour;		
		tempTm.tm_min = mMinute;
		tempTm.tm_sec = 0;
		waitTime = mktime(&tempTm);
	}

	if (pTimeSecond != NULL)
		*pTimeSecond = (UInt64)waitTime;

	int t = (int)(waitTime-now);
	if (bLog)
		NOTE_LOG("定时时间 [星期小时分钟]>[%d], 日期 [%d-%d %d:%d], Config [%d-%d-%d],  now %s", t, tempTm.tm_mon+1, tempTm.tm_mday, tempTm.tm_hour, tempTm.tm_min, mWeek, mHour, mMinute, TimeManager::GetMe().GetDateTime().c_str());
	return t;
}


int TimeManager::GetTimingSecond(int mHour, int mMinute, bool bNowNext /*= true*/)
{
	if (mHour<0 || mHour>23 || mMinute<0 || mMinute>59)
	{
		ERROR_LOG("提供的时间不正确 >%d - %d, 格式为 小时(0~23)-分钟(0~59)",  mHour, mMinute);
		return -1;
	}

	time_t now;
	time(&now);

	time_t startTime = now;

	tm tempTm = *localtime(&now);

	tempTm.tm_hour = mHour;		
	tempTm.tm_min = mMinute;
	tempTm.tm_sec = 0;
	time_t waitTime = mktime(&tempTm);

	//if (!bNowNext && waitTime==now)
	//	return 0;

	if (bNowNext && waitTime<now)
	{
		time_t tempSecond = startTime + (24*3600);
		tempTm = *localtime(&tempSecond);

		tempTm.tm_hour = mHour;		
		tempTm.tm_min = mMinute;

		waitTime = mktime(&tempTm);
	}

	int t = (int)(waitTime-now);
	DEBUG_LOG("定时时间 >[%d], 日期 [%d-%d %d:%d], Config [%d-%d],  now %s", t, tempTm.tm_mon+1, tempTm.tm_mday, tempTm.tm_hour, tempTm.tm_min, mHour, mMinute, TimeManager::GetMe().GetDateTime().c_str());
	return t;
}

int TimeManager::GetMdayTimingSceond(int Mday, int mHour, int mMinute, bool bNowNext /*= true*/)
{
	if (Mday<1 || Mday>31 || mHour<0 || mHour>23 || mMinute<0 || mMinute>59)
	{
		ERROR_LOG("提供的时间不正确 >%d号 - %d - %d, 格式为 (1~31)号-小时(0~23)-分钟(0~59)", Mday, mHour, mMinute);
		return -1;
	}

	time_t now;
	time(&now);

	time_t startTime = now;

	tm tempTm = *localtime(&now);
	tempTm.tm_mday = Mday;

	tempTm.tm_hour = mHour;		
	tempTm.tm_min = mMinute;

	time_t waitTime = mktime(&tempTm);

	if (!bNowNext && waitTime==now)
		return 0;

	if (waitTime<=now)
	{
		tempTm.tm_mon += 1;
		if (tempTm.tm_mon>11)
		{
			tempTm.tm_year += 1;
			tempTm.tm_mon = 0;
		}

		tempTm.tm_hour = mHour;		
		tempTm.tm_min = mMinute;

		waitTime = mktime(&tempTm);
	}

	int t = (int)(waitTime-now);
	DEBUG_LOG("定时时间 [号小时分钟] >[%d], 日期 [%d-%d %d:%d], Config [%d-%d-%d],  now %s", t, tempTm.tm_mon+1, tempTm.tm_mday, tempTm.tm_hour, tempTm.tm_min, Mday, mHour, mMinute, TimeManager::GetMe().GetDateTime().c_str());
	return t;
}


Int64 TimeManager::GetDaySec(int mHour, int mMin, int mSec)
{
	if (mHour < 0 || mHour>23 || mMin < 0 || mMin > 59 || mSec < 0 || mSec > 59)
	{
		ERROR_LOG("提供的时间不正确 >%2d:%2d:%2d, 格式为 小时(0~23)-分钟(0~59)-秒(0~59)", mHour, mMin, mSec);
		return -1;
	}

	time_t now;
	time(&now);

	time_t startTime = now;

	tm tempTm = *localtime(&now);

	tempTm.tm_hour = mHour;
	tempTm.tm_min = mMin;
	tempTm.tm_sec = mSec;
	time_t waitTime = mktime(&tempTm);

	return (Int64)waitTime;
}

Int64 TimeManager::GetWeekSec(int mWeek, int mHour, int mMin, int mSec)
{
	if (mWeek < 0 || mWeek > 6 || mHour < 0 || mHour>23 || mMin < 0 || mMin > 59 || mSec < 0 || mSec > 59)
	{
		ERROR_LOG("提供的时间不正确 >%d %2d:%2d:%2d, 格式为 星期(0~6) 小时(0~23)-分钟(0~59)-秒(0~59)", mWeek, mHour, mMin, mSec);
		return -1;
	}

	time_t now;
	time(&now);

	time_t startTime = now;
	tm tempTm = *localtime(&now);
	tempTm.tm_hour = mHour;
	tempTm.tm_min = mMin;
	tempTm.tm_sec = mSec;
	time_t waitTime = mktime(&tempTm);

	int day = tempTm.tm_wday - mWeek;
	if (day < 0) day += 7;
	return (Int64)waitTime - day * 84600;
}

Int64 TimeManager::GetMonthSec(int day, int mHour, int mMin, int mSec)
{
	if (day <= 0 || day > 31 || mHour < 0 || mHour>23 || mMin < 0 || mMin > 59 || mSec < 0 || mSec > 59)
	{
		ERROR_LOG("提供的时间不正确 >%d %2d:%2d:%2d, 格式为 天数(1~31) 小时(0~23)-分钟(0~59)-秒(0~59)", day, mHour, mMin, mSec);
		return -1;
	}

	time_t now;
	time(&now);

	time_t startTime = now;

	tm tempTm = *localtime(&now);

	if (tempTm.tm_mon == 11)
	{
		tempTm.tm_mon = 0;
		tempTm.tm_year++;
	}
	else
		tempTm.tm_mon++;

	tempTm.tm_mday = day;
	tempTm.tm_hour = mHour;
	tempTm.tm_min = mMin;
	tempTm.tm_sec = mSec;
	time_t waitTime = mktime(&tempTm);

	return (UInt64)waitTime - Now();
}
