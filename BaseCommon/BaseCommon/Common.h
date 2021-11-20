/********************************************************************
	created:	2011/07/10
	created:	10:7:2011   1:03
	filename: 	d:\NewTable__0709\NewTable\Common\DataBase\DataTool.h
	file path:	d:\NewTable__0709\NewTable\Common\DataBase
	file base:	DataTool
	file ext:	h
	author:		Yang Wenge
	
	purpose:	
*********************************************************************/

#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

#include "BaseCommon.h"

#if __WINDOWS__
#include <vadefs.h>
#endif

#include "AutoString.h"
#include "TableLog.h"

//-------------------------------------------------------------------------*/
#if 0
#		define PRINT printf
#else
#		define PRINT(logInfo, ...)	{ TableTool::Print(logInfo, ##__VA_ARGS__); }
#endif
 
#ifndef DEBUG_LOG
#	if DEVELOP_MODE
#		
#		define DEBUG_LOG(logInfo, ...) {} //	{ TableTool::Log(logInfo, ##__VA_ARGS__); }
#		define INFO_LOG(logInfo, ...) {} //	{ TableTool::blue(); TableTool::Log(logInfo, ##__VA_ARGS__); TableTool::white(); }

#		define LOG_GREEN	TableTool::green();
#		define LOG_WHITE	TableTool::white();
#		define LOG_RED		TableTool::red();
#		define LOG_YELLOW	TableTool::yellow();
#		define LOG_BLUE		TableTool::blue();
#		define NOTE_LOG(logInfo, ...)	{ TableTool::blue(); TableTool::Log(logInfo, ##__VA_ARGS__); TableTool::white(); }

#		define GREEN_LOG(logInfo, ...)	{ LOG_GREEN TableTool::Log(logInfo, ##__VA_ARGS__); LOG_WHITE }
#		define ERROR_LOG(logInfo, ...) { AString __info__; __info__.Format("ERROR [%s]>[%d], [%s]\r\n   X: %s", __FILE__, __LINE__, __FUNCTION__, logInfo); LOG_RED TableTool::LogError(__info__.c_str(), ##__VA_ARGS__); LOG_WHITE TableTool::onError(logInfo); }
#		define WARN_LOG(logInfo, ...) { AString __info__; __info__.Format("WARN [%s]>[%d], [%s]\r\n   WARN: %s", __FILE__, __LINE__, __FUNCTION__, logInfo); LOG_YELLOW TableTool::Log(__info__.c_str(), ##__VA_ARGS__); LOG_WHITE }

#	else
#		define DEBUG_LOG(logInfo, ...)	{}
#		define INFO_LOG(logInfo, ...)	{}

#		define LOG_GREEN	
#		define LOG_WHITE	
#		define LOG_RED		
#		define LOG_YELLOW	 
#		define LOG_BLUE		
#		define NOTE_LOG(logInfo, ...) { TableTool::Print(logInfo, ##__VA_ARGS__); }  //{ TableTool::Log(1, logInfo, ##__VA_ARGS__); }
#		define GREEN_LOG(logInfo, ...)	{  }
#		define ERROR_LOG(logInfo, ...) { AString __info__; __info__.Format("ERROR [%s]>[%d], [%s]\r\n   X: %s", __FILE__, __LINE__, __FUNCTION__, logInfo);   TableTool::LogError(__info__.c_str(), ##__VA_ARGS__);   }
#		define WARN_LOG(logInfo, ...)  { AString __info__; __info__.Format("WARN [%s]>[%d], [%s]\r\n   WARN: %s", __FILE__, __LINE__, __FUNCTION__, logInfo);  TableTool::Log(0, __info__.c_str(), ##__VA_ARGS__);  }
#	endif
#endif

#define LOG(logInfo, ...)  { TableTool::Log(1, logInfo, ##__VA_ARGS__); }
#define GLOG(logInfo, ...)	{ TableTool::LogLogic(logInfo, ##__VA_ARGS__); }

#define ErrorNote(expr, msg, ...)  if (!(expr)){ ERROR_LOG(msg, ##__VA_ARGS__); } 
#define ErrorNoteReFlase(expr, msg, ...)  if (!(expr)){ ERROR_LOG(msg, ##__VA_ARGS__) return false; } 
#define ErrorNoteReturn(expr, msg, ...)  if (!(expr)){ ERROR_LOG(msg, ##__VA_ARGS__) return; } 


#define MAKE_INDEX_ID(strIndex)	TableTool::GenerateID(strIndex)
//-------------------------------------------------------------------------*/
enum LOG_TYPE
{
	NORMAL		= 0,
	LOG_INFO,
	LOG_ERROR,
	LOG_FAIL,
	LOG_ASSERT, 
	LOG_THROW,
};
//-------------------------------------------------------------------------*/
class BaseCommon_Export TableTool
{
public:
	static AString BinaryToString(const char* scrData, size_t scrLength, const char* szPrefix);
	static int StringToBinary(AString scrString, char *destDataBuffer, size_t destSize);

public:
	static	int GenerateID( const char* str );
	static	int GenerateID( const AString& str );

	//static void Log( LOG_TYPE logType, const char* info, ... );
	//static void LogError(const char *szScrFile, UINT line, const char *szInfo, ...);
    static void Print(const char* info, ...);
	static void PrintInfo(const char *info);

	static void Log(const char* info, ...);
	static void Log(va_list &vaList, const char* info);
	static void Log(int bPrint, const char* info, ...);

	static void LogError(const char* info, ...);
	static void LogError(va_list &vaList, const char* info);
	static void LogErrorWithoutPrint(const char* info, ...);

	static void LogLogic(const char* info, ...);
	static void LogLogic(va_list &vaList, const char* info);

	static void DebugLog(va_list &vaList, const char* info);

	static void SetLog(AutoPtr<SaveTableLog> hLog);
	static void SetErrorLog(AutoPtr<SaveTableLog> hLog);
	static void SetLogicLog(AutoPtr<SaveTableLog> hLog);
	static void SetPrintLog(AutoPtr<SaveTableLog> hLog);

	static void CleanUp();

	static void blue();
	static void red();
	static void green();
	static void yellow();
	static void white();

	static void onError(const char *sz);

	static const char* Version();

protected:
	static AutoPtr<SaveTableLog>	msTableLog;
	static AutoPtr<SaveTableLog>	msErrorLog;
	static AutoPtr<SaveTableLog>	msLogicLog;	
	static AutoPtr<SaveTableLog>	msPrintLog;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#define READ_RECORD_STRING(field, reusltString)	\
{\
	Data _tempData_ = hRecord->getData(field);\
	if (!_tempData_.empty())\
	reusltString = _tempData_.string();\
	else\
{\
	DEBUG_LOG("read_data fail field=%s",field);\
}\
}
//-------------------------------------------------------------------------

#define READ_TABLE(a,b) if ( !hRecord->get(a,b) ) {\
	NiceTable::Log( "����:��ȡ��¼����,�ֶ�[%s]������:",a );\
	goto ReadError;}
//-------------------------------------------------------------------------
// LUA��������һ�ȡ
//-------------------------------------------------------------------------
#define PARAM_CHECK_INT(n) \
	int arg##n = 0;\
	if (!LuaStackObject(state, n).IsInteger())\
{\
	AString str = "Lua��������"#n"����";\
	TableTool::Log(str.string());\
	state->ArgError(n, str.string());\
}\
else\
	arg##n = LuaStackObject(state, n).GetInteger();
//-------------------------------------------------------------------------

#define PARAM_CHECK_STRING(n) \
	const char *arg##n = NULL;\
	if (!LuaStackObject(state, n).IsString())\
{\
	AString str = "Lua��������"#n"����";\
	TableTool::Log(str.string());\
	state->ArgError(n, str.string());\
}\
else\
	arg##n = LuaStackObject(state, n).GetString();
//-------------------------------------------------------------------------
#define PARAM_CHECK_FLOAT(n) \
	float arg##n = 0.0f;\
	if (!LuaStackObject(state, n).IsNumber())\
{\
	AString str = "Lua��������"#n"����";\
	TableTool::Log(str.string());\
	state->ArgError(n, str.string());\
}\
else\
	arg##n = LuaStackObject(state, n).GetNumber();
//-------------------------------------------------------------------------



#endif //_INCLUDE_COMMON_H_