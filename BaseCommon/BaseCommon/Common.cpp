#include "Common.h"
#include "Lock.h"
#include <stdarg.h>
#include "TableManager.h"
#include "TimeManager.h"
#include "Timer.h"
//-------------------------------------------------------------------------
// 库版本号, 为了验证是否加载的是最新版本
const char* TableTool::Version()
{
	return "BaseCommon_2020_03_19_11_38";
}
//-------------------------------------------------------------------------

#if __LINUX__
#include <cstdio>
void TableTool::Print(const char* lpszFormat, ...)
{
	static CsLock l;
	va_list argptr;
	va_start(argptr, lpszFormat);
	l.lock();
	static char szLogBuffer[8096];
	vsnprintf(szLogBuffer, 8096 - 1, (const char*)lpszFormat, argptr);
	printf("%02d:%02d:%02d %s\r\n", TimeManager::GetMe().GetHour(), TimeManager::GetMe().GetMinute(), TimeManager::GetMe().GetSecond(), szLogBuffer);
	l.unlock();
	va_end(argptr);
}
#else
#include <Windows.h>
void TableTool::Print(const char* lpszFormat, ...)
{
	if (msPrintLog)
	{
		va_list argptr;
		va_start(argptr, lpszFormat);		
		msPrintLog->logVa(argptr, lpszFormat);
		return;
	}

    static CsLock l;
    va_list argptr;
    va_start(argptr, lpszFormat);
    l.lock();
	static char szLogBuffer[8096];
   _vsnprintf_s ( szLogBuffer, 8096-1, (const char*)lpszFormat, argptr);

   UInt64 now = TimeManager::SysMicrosecond();
   UInt64 second = (UInt64)(now * 0.000001);
    printf("%s %d > %s\r\n", TimeManager::ToTimeString(second).c_str(), (int)(now*0.001-second*1000), szLogBuffer);
    l.unlock();
    va_end(argptr);
}


void TableTool::PrintInfo(const char *info)
{
	if (msPrintLog)
	{
		msPrintLog->Print(info);
		return;
	}

	static CsLock l;

	l.lock();
	printf(info);
	l.unlock();	
}

#endif

#include "Common.h"
#include <stdarg.h>

//-------------------------------------------------------------------------*


//--------------------------------------------------------------------------------
// 使用代码模拟方式, 主要用于其他语言实现功能
unsigned long string_id(const char *str);
//--------------------------------------------------------------------------------

AutoPtr<SaveTableLog>		TableTool::msTableLog;
AutoPtr<SaveTableLog>		TableTool::msErrorLog;
AutoPtr<SaveTableLog>		TableTool::msLogicLog;
AutoPtr<SaveTableLog>		TableTool::msPrintLog;
//--------------------------------------------------------------------------------

#ifndef CONVER_CHAR_MAX
#define CONVER_CHAR_MAX	1024*10 // 260
#endif

const int TEMP_BUFFER_SIZE = (CONVER_CHAR_MAX + 3) / 4;
const int TEMP_SIZE = TEMP_BUFFER_SIZE + 2;

//int l = ( int(int(strnlen_s(str,CONVER_CHAR_MAX)+7)*.25f)*4 );
#ifdef __LINUX__
#   define COPY_STRING_LENGTH ( int(int(strlen(str)+3)*.25f) )
#else
#   define COPY_STRING_LENGTH ( int(int(strnlen_s(str,CONVER_CHAR_MAX)+3)*.25f) )
#endif

//-------------------------------------------------------------------------*/
//class Common
//{
//	typedef int(*DllStringID)(unsigned int* m, int i);
//
//	HINSTANCE hdll;
//	DllStringID stringIdFun;
//
//public:
//	int StringID(const char *str)
//	{
//		unsigned int m[TEMP_SIZE];
//		int i = COPY_STRING_LENGTH;
//
//#pragma warning(push)
//#pragma warning(disable:4996)
//		// must use strncpy because strncpy do full zero for last char , but strncpy_s not done. by Ywg 20110612
//		strncpy((char *)m, str, i * 4);
//#pragma warning(pop)
//
//		int x = stringIdFun(m, i);
//		return x;
//	}
//
//public:
//	Common()
//	{
//		//hdll=LoadLibrary("Common.dll");
//		hdll = LoadLibrary("Common.dll");
//		DWORD e = GetLastError();
//		if (hdll != NULL)
//		{
//			stringIdFun = (DllStringID)GetProcAddress(hdll, "StringID");
//			if (stringIdFun == NULL)
//			{
//				FreeLibrary(hdll);
//				hdll = NULL;
//				ERROR_LOG("stringIdFun No find");
//			}
//		}
//		else
//			ERROR_LOG("加载 Common dll 失败");
//
//		if (stringIdFun == NULL)
//			PRINT("获取 GenerateStringID 函数失败\r\n");
//
//		AssertEx(stringIdFun != NULL, "获取 GenerateStringID 函数失败");
//	}
//};

//Common		gCommon;
//-------------------------------------------------------------------------*
int stringID_Ex(const char *str);
int TableTool::GenerateID(const char* str)
{
	if (str == NULL)
		return 0;

	if (strlen(str) >= CONVER_CHAR_MAX)
	{
		ERROR_LOG("1024 * 10 too more length >%s", str);
		return 0;
	}
	//return gCommon.StringID(str);

#if __LINUX__ || _WIN64 || 1
	return stringID_Ex(str);
#else

	unsigned int m[TEMP_SIZE];
	int i = COPY_STRING_LENGTH;

#pragma warning(push)
#pragma warning(disable:4996)
	// must use strncpy because strncpy do full zero for last char , but strncpy_s not done. by Ywg 20110612
	strncpy((char *)m, str, i * 4);
#pragma warning(pop)

	unsigned int v = 0;
	//for (i=0;i<TEMP_BUFFER_SIZE && m[i];i++) ;
	m[i++] = 0x9BE74448, m[i++] = 0x66F42C48;
	v = 0xF4FA8928;

	__asm {
		mov esi, 0x37A8470E; x0 = 0x37A8470E
		mov edi, 0x7758B42B; y0 = 0x7758B42B
		xor ecx, ecx
		_loop :
		mov ebx, 0x267B0B11; w = 0x267B0B11
			rol v, 1
			lea eax, m
			xor ebx, v

			mov eax, [eax + ecx * 4]
			mov edx, ebx
			xor esi, eax
			xor edi, eax

			add edx, edi
			or edx, 0x2040801; a = 0x2040801
			and edx, 0xBFEF7FDF; c = 0xBFEF7FDF

			mov eax, esi
			mul edx
			adc eax, edx
			mov edx, ebx
			adc eax, 0

			add edx, esi
			or edx, 0x804021; b = 0x804021
			and edx, 0x7DFEFBFF; d = 0x7DFEFBFF

			mov esi, eax
			mov eax, edi
			mul edx

			add edx, edx
			adc eax, edx
			jnc _skip
			add eax, 2
			_skip:
		inc ecx;
		mov edi, eax
			cmp ecx, i
			jnz _loop
			xor esi, edi
			mov v, esi
	}

	return (int)v;
#endif
}

int TableTool::GenerateID(const AString& str)
{
	if (str.empty())
		return 0;
	return GenerateID(str.c_str());
}

void TableTool::Log(const char* info, ...)
{
#if _DEBUG || __SERVER__/* || __WINDOWS__*/
	if (msTableLog)
	{
		va_list va;
		va_start(va, info);
		msTableLog->logVa(true, va, info);
	}
#endif
}

void TableTool::Log(va_list &vaList, const char* info)
{
#if _DEBUG || __SERVER__/* || __WINDOWS__*/
	if (msTableLog)
		msTableLog->logVa(true, vaList, info);
#endif
}


void TableTool::Log(int bPrint, const char* info, ...)
{
#if _DEBUG || __SERVER__/* || __WINDOWS__*/
	if (msTableLog)
	{
		va_list va;
		va_start(va, info);
		msTableLog->logVa(bPrint != 0, va, info);
	}
#endif
}

void TableTool::LogError(const char* info, ...)
{
	if (msErrorLog)
	{
		va_list va;
		va_start(va, info);
		msErrorLog->logVa(true, va, info);
	}
#if (_DEBUG || __SERVER__) && DEVELOP_MODE/* || __WINDOWS__*/
	if (msTableLog)
	{
		va_list va;
		va_start(va, info);
		msTableLog->logVa(false, va, info);
	}
#endif
}

void TableTool::LogError(va_list &vaList, const char* info)
{
	if (msErrorLog)
	{
		msErrorLog->logVa(true, vaList, info);
		return;
	}
#if (_DEBUG || __SERVER__) && DEVELOP_MODE/* || __WINDOWS__*/
	if (msTableLog)
		msTableLog->logVa(false, vaList, info);
#endif
}

void TableTool::LogLogic(const char* info, ...)
{
	if (msLogicLog)
	{
		va_list va;
		va_start(va, info);
		msLogicLog->logVa(0, va, info);
	}
}

void TableTool::LogLogic(va_list &vaList, const char* info)
{
	if (msLogicLog)
	{
		msLogicLog->logVa(0, vaList, info);
		return;
	}
}

void TableTool::DebugLog(va_list &vaList, const char* info)
{
	va_end(vaList);
}

void TableTool::SetLog(AutoPtr<SaveTableLog> hLog)
{
	msTableLog._free();
	msTableLog = hLog;
}

void TableTool::SetErrorLog(AutoPtr<SaveTableLog> hLog)
{
	msErrorLog._free();
	msErrorLog = hLog;
}

void TableTool::SetLogicLog(AutoPtr<SaveTableLog> hLog)
{
	msLogicLog._free();
	msLogicLog = hLog;
}

void TableTool::SetPrintLog(AutoPtr<SaveTableLog> hLog)
{
	msPrintLog._free();
	msPrintLog = hLog;
}

void TableTool::CleanUp()
{
	TimerCenter::CloseClear();

	AutoPtr<SaveTableLog> temp1 =	msTableLog;
	AutoPtr<SaveTableLog> temp2 = 	msErrorLog;
	AutoPtr<SaveTableLog> temp3 =	msLogicLog;
	AutoPtr<SaveTableLog> temp4 =	msPrintLog;

	msTableLog.setNull();
	msErrorLog.setNull();
	msLogicLog.setNull();
	msPrintLog.setNull();

	temp1._free();
	temp2._free();
	temp3._free();
	temp4._free();
	msPrintLog._free();
	if (TableManager::Exist())
		delete TableManager::getSingletonPtr();
}

extern AString	Binary2String(const CHAR* pIn, UINT InLength, const char *preFix);
extern BOOL String2Binary(const CHAR* pIn, UINT InLength, CHAR* pOut, UINT OutLimit, UINT& OutLength);

AString TableTool::BinaryToString(const char* scrData, size_t scrLength, const char* szPrefix)
{
	return Binary2String(scrData, scrLength, szPrefix);
}

int TableTool::StringToBinary(AString scrString, char *destDataBuffer, size_t destSize)
{
	UINT outlen;
	if (String2Binary(scrString.c_str(), scrString.length(), destDataBuffer, destSize, outlen) == TRUE)
	{
		return outlen;
	}
	return -1;
}

void TableTool::blue()
{
#if __WINDOWS__
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE
		| FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
}

void TableTool::red()
{
#if __WINDOWS__
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
}

void TableTool::green()
{
#if __WINDOWS__
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
}

void TableTool::yellow()
{
#if __WINDOWS__
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
}

void TableTool::white()
{
#if __WINDOWS__
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

void TableTool::onError(const char *sz)
{

}

void TableTool::LogErrorWithoutPrint(const char* info, ...)
{
	if (msErrorLog)
	{
		va_list va;
		va_start(va, info);
		msErrorLog->logVa(false, va, info);
	}
#if (_DEBUG || __SERVER__) && DEVELOP_MODE/* || __WINDOWS__*/
	if (msTableLog)
	{
		va_list va;
		va_start(va, info);
		msTableLog->logVa(false, va, info);
	}
#endif
}

//--------------------------------------------------------------------------------------
// 使用代码方式重写字符串转换成唯一数字,测试通过,方法是直接模拟汇编指令
// (搞了有几个小时的样子,没想到直接模拟汇编就可以了)
//--------------------------------------------------------------------------------------

#define  Add(x, y, CF) \
{\
	UInt64 t64 = (UInt64)x + y;\
	if ((t64>>32)>0)\
		CF = 1;\
	else\
		CF = 0;\
	x = (unsigned int)t64;\
}
//--------------------------------------------------------------------------------------


#define Adc(x, y, CF)	\
{\
	UInt64 t64 = (UInt64)x + y + CF;\
	if ((t64>>32)>0)\
		CF = 1;\
	else\
		CF = 0;\
	x = (unsigned int)t64;\
}
//--------------------------------------------------------------------------------------

#define  Mul(one, two, CF)	\
{\
	UInt64 t64 = (UInt64)one * two;\
	two = (unsigned int)(t64>>32);\
	one = (unsigned int)t64;\
	if (two==0)\
		CF = 0;\
	else\
		CF = 1;\
}
//--------------------------------------------------------------------------------------

#define  Mov(x, val)	\
{\
	x = val;\
}
//--------------------------------------------------------------------------------------

#define  Xor(x, val)	\
{\
	x = x^val;\
}
//--------------------------------------------------------------------------------------
//(先放)有可能后做 //有可能后做
#define Rol(v, CF)	\
{\
	CF = v>>(32-1);\
	v = v<<1;\
	v += CF;\
}
//--------------------------------------------------------------------------------------

unsigned long string_id(const char *str)
{
	//------------------------------------------
	AssertEx(str, "错误:对空字符指针进行转换索引");

	if (strlen(str) >= CONVER_CHAR_MAX)
		return 0;

	unsigned int m[TEMP_SIZE];
	int i = COPY_STRING_LENGTH;

#pragma warning(push)
#pragma warning(disable:4996)
	// must use strncpy because strncpy do full zero for last char , but strncpy_s not done. by Ywg 20110612
	strncpy((char *)m, str, i * 4);
#pragma warning(pop)

	unsigned int v = 0;
	//for (i=0;i<TEMP_BUFFER_SIZE && m[i];i++) ;
	m[i++] = 0x9BE74448, m[i++] = 0x66F42C48;
	v = 0xF4FA8928;
	//------------------------------------------

	unsigned int eax = 0;
	unsigned int ebx = 0;

	unsigned int edx = 0;
	unsigned int esi = 0;
	unsigned int edi = 0;

	char cf = 0;

	int j = 0;
	//------------------------------------------
	// 以下是模拟汇编部分
	{
		Mov(esi, 0x37A8470E);
		Mov(edi, 0x7758B42B);

		j = 0;

	_loop:
		Mov(ebx, 0x267B0B11);
		Rol(v, cf);
		Xor(ebx, v);

		Mov(eax, m[j]);
		Mov(edx, ebx);

		//1
		Xor(esi, eax);
		Xor(edi, eax);

		//2
		Add(edx, edi, cf);
		edx = edx | 0x2040801;
		edx = edx & 0xBFEF7FDF;
		Mov(eax, esi);
		Mul(eax, edx, cf);

		//3
		Adc(eax, edx, cf);
		//4
		Mov(edx, ebx);
		Adc(eax, 0, cf);

		//5
		Add(edx, esi, cf);

		edx = edx | 0x804021;
		edx = edx & 0x7DFEFBFF;
		//6
		Mov(esi, eax);
		Mov(eax, edi);
		Mul(eax, edx, cf);

		//7
		Add(edx, edx, cf);
		Adc(eax, edx, cf);
		if (cf == 0)
			goto _skip;
		//8
		Add(eax, 2, cf);

	_skip:
		j++;
		Mov(edi, eax);
		if (j < i)
			goto _loop;

		//9
		Xor(esi, edi);
		Mov(v, esi);
	}
	//------------------------------------------

	return v;

}
//--------------------------------------------------------------------------------------

int stringID_Ex(const char *str)
{
	//------------------------------------------
	AssertEx(str, "错误:对空字符指针进行转换索引");

	if (strlen(str) >= CONVER_CHAR_MAX)
		return 0;

	unsigned int m[TEMP_SIZE];
	int i = COPY_STRING_LENGTH;

#pragma warning(push)
#pragma warning(disable:4996)
	// must use strncpy because strncpy do full zero for last char , but strncpy_s not done. by Ywg 20110612
	strncpy((char *)m, str, i * 4);
#pragma warning(pop)

	unsigned int v = 0;
	//for (i=0;i<TEMP_BUFFER_SIZE && m[i];i++) ;
	m[i++] = 0x9BE74448, m[i++] = 0x66F42C48;
	v = 0xF4FA8928;
	//------------------------------------------

	unsigned int eax = 0;
	unsigned int ebx = 0;

	unsigned int edx = 0;
	unsigned int esi = 0;
	unsigned int edi = 0;

	char cf = 0;

	int j = 0;
	//------------------------------------------
	// 以下是模拟汇编部分
	{
		esi = 0x37A8470E;
		edi = 0x7758B42B;

		j = 0;

	_loop:
		ebx = 0x267B0B11;
		Rol(v, cf);
		ebx ^= v;

		eax = m[j];
		edx = ebx;

		//1
		esi ^= eax;
		edi ^= eax;

		//2
		Add(edx, edi, cf);
		edx = (edx | 0x2040801) & 0xBFEF7FDF;
		//edx = edx & 0xBFEF7FDF;
		eax = esi;
		Mul(eax, edx, cf);

		//3
		Adc(eax, edx, cf);
		//4
		edx = ebx;
		Adc(eax, 0, cf);

		//5
		Add(edx, esi, cf);

		edx = (edx | 0x804021) & 0x7DFEFBFF;
		//edx = edx & 0x7DFEFBFF;
		//6
		esi = eax;
		eax = edi;
		Mul(eax, edx, cf);

		//7
		Add(edx, edx, cf);
		Adc(eax, edx, cf);
		if (cf == 0)
			goto _skip;
		//8
		Add(eax, 2, cf);

	_skip:
		j++;
		edi = eax;
		if (j < i)
			goto _loop;

		//9
		esi ^= edi;
		v = esi;
	}
	//------------------------------------------

	return v;

}

//--------------------------------------------------------------------------------------
