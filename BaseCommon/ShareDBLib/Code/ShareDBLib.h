#pragma once

#include "BaseCommon.h"
#include "DBDefine.h"

#ifndef ShareDBLib_Export
#ifdef STATE_SHAREDBLIB
#define ShareDBLib_Export
#define ShareDBLib_Export_H	
#else
#		ifdef SHAREDBLIB_EXPORTS
#			define ShareDBLib_Export __declspec(dllexport)
#			define ShareDBLib_Export_H	__declspec(dllexport)
#		else
#			define ShareDBLib_Export __declspec(dllimport)
#			define ShareDBLib_Export_H	
#		endif
#endif



#endif