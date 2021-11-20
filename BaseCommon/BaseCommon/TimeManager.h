
#ifndef __TIMEMANAGER_H__
#define __TIMEMANAGER_H__
#include "BaseCommon.h"
#include "AutoString.h"

#ifdef __LINUX__
#	include <sys/utsname.h>
#	include <sys/time.h>
#	include <time.h>
#else
#	include <time.h>
#endif
//-------------------------------------------------------------------------*/
// NOTE: GetANSITime δ�������������޸�Ϊֱ�ӵ��� Now()
//-------------------------------------------------------------------------*/
class BaseCommon_Export TimeManager
{
public:	
	static TimeManager& GetMe();

public :
	TimeManager( ) ;
	~TimeManager( ) ;

	BOOL			Init( ) ;

	//ȡ�÷������˳�������ʱ��ʱ�����ֵ
	UInt64			StartTime( ){ return m_StartTime ; } ;

	//����ǰ��ϵͳʱ���ʽ����ʱ���������
	VOID			SetTime( ) ;

	// �õ���׼ʱ��
	time_t			GetANSITime( );

public :
	//***ע�⣺
	//���½ӿڵ���û��ϵͳ���ã�ֻ���ʱ��������ڵ�����
	//

	//ȡ������ʱ��ʱ��ġ��ꡢ�¡��ա�Сʱ���֡��롢���ڵ�ֵ��
	INT				GetYear( ){ return m_TM.tm_year+1900 ; } ;	//[1900,????]
	INT				GetMonth( ){ return m_TM.tm_mon ; } ;		//[0,11]
	INT				GetDay( ){ return m_TM.tm_mday ; } ;		//[1,31]
	INT				GetHour( ){ return m_TM.tm_hour ; } ;		//[0,23]
	INT				GetMinute( ){ return m_TM.tm_min ; } ;		//[0,59]
	INT				GetSecond( ){ return m_TM.tm_sec ; } ;		//[0,59]
	//ȡ�õ�ǰ�����ڼ���0��ʾ�������죬1��6��ʾ������һ��������
	UINT			GetWeek( ){ return m_TM.tm_wday ; } ;
	//����ǰ��ʱ�䣨�ꡢ�¡��ա�Сʱ���֣�ת����һ��UINT����ʾ
	//���磺0,507,211,233 ��ʾ "2005.07.21 12:33"
	UINT			Time2DWORD( ) ;
	//ȡ�õ�ǰ������[4bit 0-9][4bit 0-11][5bit 0-30][5bit 0-23][6bit 0-59][6bit 0-59]
	UINT			CurrentDate( ) ;
	VOID			CurrentDate(int &year, int &month, int &day, int &hour, int &minute, int &second);

	//ȡ����������ʱ���ʱ����λ�����룩, Ret = Date2-Data1
	UINT			DiffTime( UINT Date1, UINT Date2 ) ;
	//��һ��UINT������ת����һ��tm�ṹ
	VOID			ConvertUT( UINT Date, tm* TM ) ;
	//��һ��tm�ṹת����һ��UINT������
	VOID			ConvertTU( tm* TM, UINT& Date ) ;
	//ȡ������Ϊ��λ��ʱ��ֵ, ǧλ��������ݣ�������λ����ʱ�䣨������
	UINT			GetDayTime( ) ;
	//�õ���ǰ�ǽ����ʲôʱ��2310��ʾ23��10��
	WORD			GetTodayTime();
	BOOL			FormatTodayTime(WORD& nTime);

	AString			GetDate();
	AString			GetTime();
	AString			GetDateTime();
	
	static AString	ToDateTime(UInt64 dateTime, bool bHaveSecond = true);
	static AString	ToTimeString(UInt64 dateTime);
	// ʱ���ת��Ϊ������ʱ����
	static int      UnixTimeToDate(UInt64 unixTime, int timeArray[]);
	// �ж�һ��ʱ����ǲ��ǽ��췶Χ��
	static bool     UnixTimeisToday( UInt64 unixTime );
	// ��ȡ���1970���������Ӧ������ֵ
	static int		GetWeekBySecond(UInt64 sec);
	static int		GetMonthBySecond(UInt64 sec);

	// ȡ��ָ�����ڶ�ʱʱ��, , bNowNext, ��ǰ�ѵ��Ƿ�ȡ��һ����ʱ, pTimeSecond �ɷ��ص���ľ���ʱ������
	static int		GetTimingSecond(int week, int hour, int minute, bool bNowNext = true, bool bLog = false, UInt64 *pTimeSecond = NULL);
	
	// ȡ��ָ�����ڶ�ʱʱ��, bNowNext, ��ǰ�ѵ��Ƿ�ȡ��һ����ʱ
	static int		GetMdayTimingSceond(int Mday, int hour, int minute, bool bNowNext = true);

	// ��ȡ����Ķ���ʱ������ Ĭ�ϵ���0��
	static Int64	GetDaySec(int mHour = 0, int mMin = 0, int mSec = 0);

	// ��ȡ�ܵĶ���ʱ������
	static Int64	GetWeekSec(int wWeek, int mHour = 0, int mMin = 0, int mSec = 0);

	// ��ȡ�µĶ���ʱ������
	static Int64	GetMonthSec(int day, int mHour = 0, int mMin = 0, int mSec = 0);

	// ȡ�ö�ʱ���� , bNowNext, ��ǰ�ѵ��Ƿ�ȡ��һ����ʱ
	static int		GetTimingSecond(int hour, int minute, bool bNowNext = true);

	// �õ�ָ��ʱ�����0��ĺ����� (ȥ������ʱ�䲿�ֵ�ֵ)
	static int GetTimeSecond(int hour, int minute, int second);
	static int GetTimeSecond(const AString &time);
	static int GetTimeSecond(UInt64 time);

	// 
	static	void	Sleep(int milSecond);

	// ��ȡ��ǰ�����1970.1.1�ŵ�ʱ��(��)
	static UInt64	Now();

	// ��ȡ��ǰ��������ʱ��(����)
	static UInt64	NowTick();

	// ��1970��1��1��0:0:0:000�����ڵ�΢����(UTCʱ��)����λ΢��(us)
	static Int64	SysMicrosecond();

    // ��ȡ ָ������ʱ�������
    static UInt64   GetSecondTime(int years, int month, int day, int hour, int minute, int second);

	// ����ָ�������ַ���, ��ȡ �� 1970.1.1 ������, ��ʽ: ��/��/�� Сʱ:����[:��]
	static UInt64 AnalyzeTime(const AString &timeString, const char *dateSpace = "/");

	// NOTE: ֻ��ʹ����һ���߳���(�߳�ͬ������ȫ), �߳�ѭ����ʼ����һ��, Ȼ��ͨ�� NowSecond() ��Чȡ�õ�ǰʱ��
	static void ThreadSetCurrentTime(){  m_uCurrentSecondTime = Now(); }	
	static UInt64 NowSecond(){ return m_uCurrentSecondTime; }

	// ��ȡʱ�����0��ʱ��
	static UInt64 AnalyzeZeroTime(UInt64 time);

public :
	UInt64			m_StartTime ;
	static  UInt64	m_uCurrentSecondTime ;		// һ��ѭ������һ��
	time_t			m_SetTime ;
	tm				m_TM ;
#ifdef __LINUX__
	UInt64		mStartMillsecond;
	struct timeval  _tend;
	//struct timezone tz;
#endif



};






#endif
