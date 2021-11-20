#ifndef _HIK_EVENT_H_ 
#define _HIK_EVENT_H_
#include "BaseCommon.h"
#ifdef __WINDOWS__
	typedef void*  event_handle;
#elif __LINUX__
#	define event_handle event_t*
#include <pthread.h>
struct  event_t
{
	bool state;
	bool manual_reset;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};
#endif


//����ֵ��NULL ����
event_handle BaseCommon_Export event_create(bool manual_reset, bool init_state);

//����ֵ��0 �ȵ��¼���-1����
int BaseCommon_Export event_wait(event_handle hevent);

//����ֵ��0 �ȵ��¼���1 ��ʱ��-1����
int BaseCommon_Export event_timedwait(event_handle hevent, long milliseconds);

//����ֵ��0 �ɹ���-1����
int BaseCommon_Export event_set(event_handle hevent);

//����ֵ��0 �ɹ���-1����
int BaseCommon_Export event_reset(event_handle hevent);

//����ֵ����
void BaseCommon_Export event_destroy(event_handle &hevent);

#endif

////////////
