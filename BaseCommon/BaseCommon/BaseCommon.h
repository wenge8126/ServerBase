

#ifndef __WINDOWS__
    #define __LINUX__		1
	#define USE_LIBCO		1
    #define MAX_PATH 256

    #define _UNICODE

    #define STATE_BASECOMMON_LIB		1
    #define STATE_NET_LIB
    #define STATE_EVENTCORE_LIB
	#define STATE_SHAREDBLIB
	#define  STATE_SHAREMEMCLOUDDB
    #include <stdlib.h>
#endif

//-------------------------------------------------------------------------
#ifndef BaseCommon_Export
#ifdef STATE_BASECOMMON_LIB
#	define BaseCommon_Export
#	define BaseCommon_Export_H
#else
#		ifdef BASECOMMON_EXPORTS
#			define BaseCommon_Export __declspec(dllexport)
#			define BaseCommon_Export_H	__declspec(dllexport)
#		else
#			define BaseCommon_Export __declspec(dllimport)
#			define BaseCommon_Export_H
#		endif
#endif
#endif

//-------------------------------------------------------------------------
#ifndef _INCLUDE_BASECOMMON_H_
#define _INCLUDE_BASECOMMON_H_

#ifndef _CRT_RAND_S 
# define _CRT_RAND_S 
#endif

#define _THREAD_LOCAL_		// thread_local  �йܴ����в���ʹ��, �ȹر�ʹ��

#pragma warning(push)
#pragma warning(disable:4005)

//#if __LINUX__
//	typedef long long			DSIZE;
//#else
//	typedef __int64				DSIZE;
//#endif
typedef int					DSIZE;

typedef unsigned char	BYTE;
typedef unsigned char	UCHAR;			//��׼�޷���CHAR
typedef char			CHAR;			//��׼CHAR
typedef unsigned int	UINT;			//��׼�޷���INT
typedef int				INT;			//��׼INT
typedef unsigned short	USHORT;			//��׼�޷���short
typedef short			SHORT;			//��׼short
typedef unsigned long	ULONG;			//��׼�޷���LONG(���Ƽ�ʹ��)
typedef long			LONG;			//��׼LONG(���Ƽ�ʹ��)
typedef float			FLOAT;			//��׼float
typedef unsigned long	DWORD;
typedef USHORT			WORD;

typedef UCHAR			uchar;
typedef USHORT			ushort;
typedef UINT			uint;
typedef ULONG			ulong;
typedef unsigned short	StrLenType;
#ifdef __LINUX__
    typedef signed char             BOOL;
typedef  unsigned	int			HANDLE;
	
	#define INT_MAX							(0xFFFFFFFF>>1)
	#define UINT_MAX				0xFFFFFFFF
	#define  __stdcall
#else
#	ifndef BOOL
    typedef int						BOOL;
#	endif
#	if !HANDLE
	typedef  void*					HANDLE;
#	endif
#endif
    typedef	unsigned char			byte;

#ifdef __LINUX__
    typedef long long               BIGINT;
    typedef long long               Int64;
    typedef unsigned long long		UInt64;
#else
    typedef __int64                 BIGINT;
    typedef __int64                 Int64;
    typedef unsigned __int64		UInt64;
#endif

typedef int							KEY;
typedef UInt64						GSKEY;
typedef	int							DBCODE;

#define TRUE	(1)
#define FALSE	(0)
#define VOID	void			//��׼��
#define NULL	0

#define INVALID_HANDLE	(-1)		//��Ч�ľ��
#define INVALID_ID		(-1)		//��Ч��IDֵ

#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4275)
#pragma warning(disable:4251)
#pragma warning(disable:4996)
#pragma warning(disable:4267)


#if defined(__WINDOWS__) || defined(WIN32)
#	define		tvsnprintf		_vsnprintf
#	define		tstricmp		_stricmp
#	define		tsnprintf		_snprintf
#ifndef _WIN32_WINNT
#	define		_WIN32_WINNT					0x0600
#endif
#elif defined(__LINUX__)
#	define		tvsnprintf		vsnprintf
#	define		tstricmp		strcasecmp
#	define		tsnprintf		snprintf
#endif

#define MIN(a, b)	(a<b ? a:b)
#define MAX(a, b)	(a>b ? a:b)

#define TABLE_USE_STL_INDEX		1
#define DB_BLOB_FIELD_DEFAULT_LENGTH				(64*1024-1)			// DB blob �ֶ�Ĭ�ϳ���


// ��־���
#define LOG_INFO_MAX_LENGTH		10000
#define LOG_DATA_TIME_LENGTH	21
#define CONFIG_LIMIT_MAX_ID		1000000

#if  __LINUX__
#define	INT64_FORMAT	"%ll"
#else
#define	INT64_FORMAT	"%I64d"
#endif

struct SHARE_PTR
{
	union
	{
		UInt64	ptr;
		struct
		{
			int		key;
			DSIZE	pos;
		};
	};

    SHARE_PTR()
        : key(0)
        , pos(0) {}
};


#endif //_INCLUDE_BASECOMMON_H_

//-------------------------------------------------------------------------