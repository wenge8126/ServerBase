
#ifndef __INCLUDE_NETCONFIG_H_
#define __INCLUDE_NETCONFIG_H_

#if __LINUX__
#	define USE_LIBCO										1
#else
#	define USE_LIBCO										0
#endif 

//-------------------------------------------------------------------------*/
#define _DEBUG_NET_									0			// �������磬�������Ϣ��ӡ
#define _DEBUG_REMOVE_OVER_TIME			(6)

// NOTE: ���������̼߳�����, ���Զ���ѹ������ 2018/11/12
// NOTE: ��ʵ���������������Ĳ���(һ������һ��), ʹ�õ����շ�Ч�ʸ�һЩ, ʹ���߳��շ�û���Ч������,�����������Ŀ���(�ڴ����ݽ���ʱ�俪��)
#define _IOCP_THREAD_NUM						(1)

#define _USE_THREAD_SEND_						1			// UDP������̨�̷߳���
#define _UDP_SENDTHREAD_ONCE_TIME	(1)			// UDP��̨�߳�ÿ��10���뷢��һ����

#define _IOCP_SEND_OVERTIME					(60*5)		// ���ͳ�ʱʱ��(��)��������ʱ�䱨���ҹر�����
#define _ONCE_EXECUTE_MSG_COUNT		(10)		// һ��ѭ������ദ�����Ϣ����
const int MAX_SEND_DATA_SIZE				= (1024*1024);	// ���ͬʱ����������󻺴� 1M
const int MAX_RECEIVE_DATA_SIZE			= (1024*1024*2);	// ���ͬʱ�ɽ������ݳߴ� 2M
#define MAX_PROCESS_PACKET_NUM			(100)		// һ������������������Ϣ�������������Ƴ�
//-----------------------------------------------------------------------------
#define SERVER_NET_CHECK_CONNECT_WAITTIME	60		// ���Ӱ�ȫ, �����Ӵ����Ӻú�, �����ʱ����δ���յ�����������Ƴ�
#define RECEIVE_THREAD_LOOP_SPACETIME			10		// ��̨�����߳�ѭ������ʱ��, ������Ӧ���߼�ѭ������һ��


#ifdef DEVELOP_MODE
#	define		MIN_NEED_COMPRESS_SIZE				512		//�Զ�����ѹ�����������ݴ�С, �趨��Ҫѹ��, �����ݴ�С���ڻ���ڴ�ֵ, �Ž���������Ϣѹ��
// UDP��Ϣ�������ֵ, ���ڴ�ֵ�Զ�����Ϊ�ְ�����
// NOTE: UDP ���ݰ���СΪ 1472, ��ͷ6���ֽ�, �ְ���Ϣ12, ������Ѵ�СΪ 1472 - 18, ���п��Կ���1450���ֽ�
const int MAX_UDP_PACKET_SIZE							= 1024;		
const int MAX_UDP_RECEIVE_BUFFER_SIZE			= 2 * 1024;		// һ�ν������ݵĻ��棬һ��һ�������1500�� �����ٴ�Ҳûʲô��Ҫ
#else
#	define		MIN_NEED_COMPRESS_SIZE			2048		//�Զ�����ѹ�����������ݴ�С, �趨��Ҫѹ��, �����ݴ�С���ڻ���ڴ�ֵ, �Ž���������Ϣѹ��

const int MAX_UDP_PACKET_SIZE							= 1280;
const int MAX_UDP_RECEIVE_BUFFER_SIZE			= 2 * 1024;		 
#endif

//-----------------------------------------------------------------------------
#define		UDP_USE_SEND_THREAD							0				// UDP �����Ƿ�ʹ�÷����̷߳���
#define		UDP_USE_RECEIVE_CHECK_PACKET			0				// UDP �Ƿ������ط����ƴ�����
#define		EVENTPACKET_DEFAULT_BUFFER_SIZE		(2048)

const int	UDP_DEFAULT_SEND_BUFFER_SIZE			=	(1024*256);	// Ĭ�Ϸ��ͻ����С
const int	UDP_DEFAULT_RECEIVE_BUFFER_SIZE		=	(1024*1024*1);	// Ĭ�Ͻ��ջ��� 1M

// ��̨�߳̽��շ�ʽUDP��������
const int	UDP_THREAD_RECEIVE_BUFFER_INIT_SIZE		=	(2 * 1024 * 1024 ); // ��̨�߳������ʼ����ֵ 2 M, ÿ�����������Ҫ4M
const int	UDP_THREAD_RECEIVE_BUFFER_MAX_SIZE		=	(16 * 1024 * 1026); // ������������ջ���ֵ 16M, ˫������๲ 32M
const int 	UDP_ONCE_SECOND_SEND_SIZE						=	(10 * 1024);	// ÿ�����Ʒ���10K
const int	UDP_ONCE_SECOND_SEND_PACKET_COUNT	=	10;				// ÿ������ÿ����෢��10����
const int	UPD_RECORD_SENDED_PACKET_COUNT			=	6;				// ÿ�����Ӽ�¼�Ѿ����͵İ����޶�����
const int	UDP_RECORD_QUEST_LOGIC_EVENTID_COUNT	=	100;			// ÿ�����Ӽ�¼��������߼�ID�������޶�����, ������, ����������ID
//-------------------------------------------------------------------------
#define PACK_COMPART "$-$"//����ָ���
#define PACK_COMPART_SIZE strlen(PACK_COMPART)


//-------------------------------------------------------------------------

enum NET_PACKET_ID
{
	PACKET_NONE = 0,
	PACKET_TRANSFER = 1,		// ����ڵ�֮����ת���ݵ���Ϣ�� (��������Ϣ������)
	PACKET_RESPONSE_MSG	= 2,		// �ظ������Ϣ
	PACKET_COMPRESS_EVENT = 3,		// �Ż�,ֻ�����ݵ��¼���Ϣ,�ֶ���Ϣ�̶���������
	PACKET_PART_PACKET = 4,		// �ְ�
	PACKET_QUEST_REPEAT_SEND = 5,		// �����ط�ָ���İ�, �����Ƕ���
	PACKET_QUEST_REPEAT_CONNECT = 6,	// ������������
	PACKET_EVENT = 7,		// Ĭ��һ����¼���Ϣ
	FIRST_PART_PACKET = 8,
	eNotifyNetEventID = 9,
	eNotifyHeartBeat = 10,		// ���������ɷ�����ͳһÿ��Լ10�����������ӷ���һ��������
	PACKET_EVENT_PROCESS = 11, // ֱ�Ӷ�ȡ������¼��� 
	PACKET_MAX,
};
//-------------------------------------------------------------------------

#endif
