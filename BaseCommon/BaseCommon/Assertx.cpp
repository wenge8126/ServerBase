//--------------------------------------------------------------------------------
// 
// Filename   : Assert.cpp 
//
//--------------------------------------------------------------------------------
#include "Assertx.h"
//#include "Lock.h"
#include "LogEvent.h"
#include <exception>
#include <time.h>
#include <stdarg.h>
#include <string>
#ifdef __LINUX__
//#include <execinfo.h>
#else
#	include <windows.h>
#endif

#include "stdlib.h"

#include "Common.h"
#include "Dump.h"

#define  MSG_BUFFER_LEGHTH	1024

#define DEBUG_DUMP_ERROR	{ int *p = NULL; *p = 1; }

int	g_Command_Assert=0 ;//���Ʋ�����0:��ͨ�������Ի������û�ѡ��(ȱʡֵ)
						//			1:����
						//			2:�����׳��쳣���ڻ�ȡ���ж�ջ
int g_Command_IgnoreMessageBox=false ;//���Ʋ���������MyMessageBox���ж�


EasyLog gAssertLog;


void __show__( const char* szTemp )
{
	ERROR_LOG("%s", szTemp);
#ifdef __LINUX__
	PRINT("Assert:%s",szTemp);
#endif

	//������־
#ifndef GAME_CLIENT
	time_t			now;
	char datestr[20] ={0};
	time(&now);
#pragma warning(push)
#pragma warning(disable:4996)
	struct tm* tm_now = localtime(&now);
	strftime(datestr, 20, "%Y_%m_%d", tm_now);
#pragma warning(pop)
	//char fileName[100] = {0};

	char szFileName[MAX_PATH];
#ifdef __WINDOWS__
    sprintf_s(szFileName, MAX_PATH, "./assert_%s.log", datestr );
#else
	sprintf(szFileName, "./assert_%s.log", datestr );
#endif
	if (!gAssertLog.setFile(szFileName, "a"))
	{
		gAssertLog.setFile(szFileName, "w");
	}
	TableTool::red();
	gAssertLog.log(szTemp);
	TableTool::white();
	//FILE* f = fopen( fileName, "a" ) ;
	//fwrite( szTemp, 1, strlen(szTemp), f ) ;
	//fwrite( "\r\n", 1, 2, f ) ;
	//fclose(f) ;
#endif

	// ����ʹ�ÿ�ָ�룬Dumpջ��Ϣ
	DEBUG_DUMP_ERROR

#if defined(__WINDOWS__) || defined (WIN32) 
	

	//static CsLock lock ;
	//if( g_Command_Assert!=1 )
	//{
	//	//lock.lock() ;
	//	//int iRet = ::MessageBoxA( NULL, szTemp, "�쳣", MB_OK ) ;
	//	//lock.unlock() ;
	//}
#elif defined(__LINUX__)
	
#endif

// Ϊ�˿���, ����ʱ, ��ʹ����� ���ܶϵ�.
#if __LINUX__
    throw(1);
#else
    throw( std::exception(szTemp) );
#endif
    
//#ifdef _DEBUG
//	throw( std::exception(szTemp) );
//#else
//	throw(1);
//#endif

}


 void __messagebox__(const char*msg )
{
	if( g_Command_IgnoreMessageBox )
		return ;
#if defined(__WINDOWS__)
	::MessageBoxA( NULL, msg, "��Ϣ", MB_OK ) ;
#elif defined(__LINUX__)
#endif
}
//--------------------------------------------------------------------------------
//
// __assert__
//
//
//--------------------------------------------------------------------------------
void __assert__ ( const char * file , uint line , const char * func , const char * expr )
{
	char szTemp[1024] = {0};
	
#ifdef __LINUX__ //������ʽ
	sprintf( szTemp, "[%s][%d][%s][%s]\n", file, line, func, expr ) ;
#else
	sprintf_s( szTemp, MSG_BUFFER_LEGHTH,  "[%s][%d][%s][%s]", file, line, func, expr ) ;
#endif
	__show__(szTemp) ;
}

void __assertex__ ( const char * file , uint line , const char * func , const char * expr ,const char* msg, ...)
{
	char szTemp[1024] = {0};

	va_list	arglist;
	va_start(arglist, msg);

#if __LINUX__
	sprintf( szTemp, "[%s][%d][%s][%s]>", file, line, func, expr);
	PRINT("%s",szTemp);
	int len = strlen(szTemp) + 1;
	vsnprintf( szTemp, 1024-len, (const char*)msg, arglist);
#else
	sprintf_s( szTemp, MSG_BUFFER_LEGHTH,  "[%s][%d][%s][%s]>", file, line, func, expr );
	int len = strlen(szTemp)+1;
	_vsnprintf_s ( szTemp+len-1, 1024-len, _TRUNCATE, (const char*)msg, arglist);
#endif
	szTemp[1023] = '\0';
	va_end(arglist);	

	__show__(szTemp) ;
}

void __assertspecial__ ( const char * file , uint line , const char * func , const char * expr ,const char* msg)
{
	char szTemp[1024] = {0};
	
#ifdef __LINUX__
	sprintf( szTemp, "S[%s][%d][%s][%s]\n[%s]\n", file, line, func, expr ,msg ) ;
#else
	sprintf_s( szTemp, MSG_BUFFER_LEGHTH,  "S[%s][%d][%s][%s]\n[%s]", file, line, func, expr ,msg ) ;
#endif
	__show__(szTemp) ;
}
//--------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------
void __protocol_assert__ ( const char * file , uint line , const char * func , const char * expr )
{
	gAssertLog.log( "[%s][%d][%s][%s]", file, line, func, expr ) ;
}
