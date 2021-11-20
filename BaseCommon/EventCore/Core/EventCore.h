#include "BaseCommon.h"
//-------------------------------------------------------------------------
#ifndef EventCoreDll_Export
#	ifdef STATE_EVENTCORE_LIB
#		define EventCoreDll_Export
#		define EVENTCORE_EXPORTS_H
#	else
#		ifdef EVENTCORE_EXPORTS
#			define EVENTCORE_EXPORTS_H	__declspec(dllexport)
#			define EventCoreDll_Export __declspec(dllexport)
#		else
#			define EventCoreDll_Export __declspec(dllimport)
#			define EVENTCORE_EXPORTS_H
#		endif
#	endif
#endif

//-------------------------------------------------------------------------
#ifndef _INCLUDE_EVENTCORE_H_
#define _INCLUDE_EVENTCORE_H_


#define OPTIMIZE_MODE				0				// 是否使用优化模式(启用事件池)

#define  _USE_NICE_PROTOCOL_  0                 // 消息事件序列时是否使用NiceData协议优化
#define SERIALIZE_EVENT_NAME	0
#define FREE_EVENT_MAX_COUNT	(100)		// 缓存释放掉的事件最多数量, 超过此数量后，直接释放事件

#define SHOW_EVENT_TICK			0

typedef unsigned int	EVENT_ID;


#endif //_INCLUDE_EVENTCORE_H_
//-------------------------------------------------------------------------
