#include "BaseCommon.h"

#include "NetConfig.h"

//-------------------------------------------------------------------------
#ifndef Net_Export
#ifdef STATE_NET_LIB
#define Net_Export
#define Net_Export_H	
#else
#		ifdef NET_EXPORTS
#			define Net_Export __declspec(dllexport)
#			define Net_Export_H	__declspec(dllexport)
#		else
#			define Net_Export __declspec(dllimport)
#			define Net_Export_H	
#		endif
#endif

typedef unsigned long	IP_t;
typedef unsigned int		MSG_ID;

#define IP_SIZE				(24)
#define NET_CONNECT_MAX		(10280)
#define	MAX_UDP_MSG_SIZE	(1400)		//1472 - 9(packet code and index)

#define USE_BIG_PACKET_SIZE			1
#define CLOUD_WHITE_LIST_NAME	"CloudWhiteList"			

#if USE_BIG_PACKET_SIZE
	const int PACKET_HEADER_SIZE = sizeof(UINT)+2;
#else
	const int PACKET_HEADER_SIZE = sizeof(UINT);
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE			(NET_CONNECT_MAX)
#endif


enum NET_CONNECT_STATE
{
	NET_NO_USE,
	NET_INIT_CONNECT,
	NET_CONNECT_BEGIN,	
	NET_CONNECT_PASS,
	NET_CONNECT_OK,
};

enum RECEIVE_RESULT
{
	RECEIVE_FAIL,
	RECEIVE_SUCCEED,
	RECEIVE_SUCCEED_FINISH,
};

//-------------------------------------------------------------------------
// 基本运行状态, 可设置16种
enum EBASE_RUN_STATE
{
	eRunOk = 0,
	eMemoryWarn = 1,
	eDiskWarn = 1<<1,
	eLoopWarn = 1<<2,
	eLowLoopWarn = 1 << 3,

	eMemoryTooBad = 1 << 8,		// 高位开始表示错误
	eDiskTooBad = 1<<9,
	eLoopTooBad = 1<<10,
	eLowLoopTooBad = 1<<11,
	eStartError = 1 << 12,
	eLogicError = 1<<13,	
	eLogicMaintain = 1<<14,	// 系统维护
};
//-------------------------------------------------------------------------

#endif

#include "NetConfig.h"
#include "EventCore.h"



