#pragma once

#include "BaseCommon.h"
#include "DBDefine.h"

#ifndef ShareSQLUpdateLib_Export
#ifdef STATE_SHARESQLUPDATE_LIB
#define ShareSQLUpdateLib_Export
#define ShareSQLUpdateLib_Export_H	
#else
#		ifdef SHARESQLUPDATELIB_EXPORTS
#			define ShareSQLUpdateLib_Export __declspec(dllexport)
#			define ShareSQLUpdateLib_Export_H	__declspec(dllexport)
#		else
#			define ShareSQLUpdateLib_Export __declspec(dllimport)
#			define ShareSQLUpdateLib_Export_H	
#		endif
#endif



#endif
