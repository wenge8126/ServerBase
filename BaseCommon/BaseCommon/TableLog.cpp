
#include "TableLog.h"
#include <stdarg.h>
#include "LogEvent.h"
#include "MemBase.h"

void SaveTableLog::log( const char* info,... )
{
	va_list va; va_start(va,info); logVa(va,info);
}

#if __LINUX__
TableLog::TableLog( const char* logFileName, AString mode /*= "wt"*/ )
{
	mLog = new EasyLog(logFileName, mode.c_str());
}

TableLog::~TableLog()
{
	delete (EasyLog*)mLog;
	mLog = NULL;
}

void TableLog::logVa( va_list &vaList, const char*info )
{
#if __WINDOWS__ || __IOS__ || __SERVER__
	//AString temp = AString::getUTF8(info);
	((EasyLog*)mLog)->logVa(vaList, info);
#endif
}

void TableLog::logVa(bool bPrint, va_list &vaList,const char*info)
{
#if __WINDOWS__ || __IOS__ || __SERVER__
	((EasyLog*)mLog)->logVa(vaList, info);
#endif
}

#else
TableLog::TableLog(const char* logFileName, AString mode /*= "wt"*/)
{
	mLog = new ThreadLog(logFileName, mode.c_str());
}

TableLog::~TableLog()
{
	delete (ThreadLog*)mLog;
	mLog = NULL;
}

void TableLog::logVa(va_list &vaList, const char*info)
{
#if __WINDOWS__ || __IOS__ || __SERVER__
	//AString temp = AString::getUTF8(info);
	((ThreadLog*)mLog)->logVa(vaList, info);
#endif
}

void TableLog::logVa(bool bPrint, va_list &vaList, const char*info)
{
#if __WINDOWS__ || __IOS__ || __SERVER__
	((ThreadLog*)mLog)->logVa(vaList, info, bPrint);
#endif
}
#endif