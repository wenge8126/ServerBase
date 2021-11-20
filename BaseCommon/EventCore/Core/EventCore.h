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


#define OPTIMIZE_MODE				0				// �Ƿ�ʹ���Ż�ģʽ(�����¼���)

#define  _USE_NICE_PROTOCOL_  0                 // ��Ϣ�¼�����ʱ�Ƿ�ʹ��NiceDataЭ���Ż�
#define SERIALIZE_EVENT_NAME	0
#define FREE_EVENT_MAX_COUNT	(100)		// �����ͷŵ����¼��������, ������������ֱ���ͷ��¼�

#define SHOW_EVENT_TICK			0

typedef unsigned int	EVENT_ID;


#endif //_INCLUDE_EVENTCORE_H_
//-------------------------------------------------------------------------
