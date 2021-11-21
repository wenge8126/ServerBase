/********************************************************************
	created:	2013/04/12
	created:	12:4:2013   10:11
	filename: 	D:\Game\BaseCommon\NetDll\Net\UDPSocket.h
	file path:	D:\Game\BaseCommon\NetDll\Net
	file base:	UDPSocket
	file ext:	h
	author:		Wenge
	
	purpose:	UDP����ӿ�
	note:		����ʹ�÷���Լ����Ϣ����ģ��
*********************************************************************/


#ifndef _INCLUDE_UDPSOCKET_H_
#define _INCLUDE_UDPSOCKET_H_

#include "Socket.h"

#include "DataBuffer.h"

#include "NetHead.h"


#define USE_NET_CHECK	0

#define UPD_CONNECT_MSG	"UDP1.00"

const int MAX_SEND_MSG_COUNT	 = 10;

const int NET_CHECK_MSG_CODE	 = 0;
enum NET_MSG
{
	MSG_NET_CHECK	 = 0,
	MSG_QUEST_RESEND = 1,
	MSG_NET_CLOSE	 = 2,
	MSG_RESEND_RESPONSE = 3, // �ط�����Ϣ
};
//----------------------------------------------------------------------
// ����UDP���ӿͻ���, �ɽ��ܿɷ���
// 
//------------------------------------------------------------------------

class SocketInputStream;

class Net_Export UDPSocket : public Socket
{

public:
	UDPSocket();

public:
	virtual void Start(){}
	virtual void Process(){}

	virtual SocketInputStream* GetReceiveFromThread();
	virtual DataBuffer* GetSendData(){ return NULL; }


public:
	virtual BOOL create( );

	virtual BOOL connect ();

	virtual BOOL connect(const char* szIp, int port);

	virtual UINT send (const VOID* buf, UINT len, UINT flags);

	virtual UINT receive (VOID* buf, UINT len, UINT flags = 0);

	virtual UINT __send (const VOID* buf, UINT len, UINT flags, const sockaddr_in *targetSockAddr);

	virtual UINT __receive( VOID* buf, UINT len, UINT flags /*= 0*/, sockaddr *revSockAddr, bool bNeedWait );

	virtual bool ReadyConnect(){return true;}

	virtual void ReadyAddr(const SOCKADDR_IN &in);	

	virtual void setSOCKET( SOCKET sock );

	bool CanReadData()
	{
        FD_ZERO(&mFdSet);
		FD_SET(m_SocketID, &mFdSet);
		int num = select((int)m_SocketID+1, &mFdSet, NULL, NULL, &mTimeVal);
		return num>0 && FD_ISSET(m_SocketID, &mFdSet);
	}

	bool CanWriteData()
	{
        FD_ZERO(&mFdSet);
		FD_SET(m_SocketID, &mFdSet);
		int num = select((int)m_SocketID+1, NULL, &mFdSet, NULL, &mTimeVal);
		return num>0 && FD_ISSET(m_SocketID, &mFdSet);
	}

protected:
	virtual SOCKET CurrentSendSocketID(){ return m_SocketID; }


	// select ���
	fd_set		mFdSet;     
	timeval		mTimeVal;  

	
};

//------------------------------------------------------------------------
// ���� UDP ������ͬ��ͻ��˺���������
// ��һ�νӵ�һ�����ӵ�����󣬰����ӵ�IP��Ϣ��¼��������֮���ʹ�ô�IP���ͽ���
//----------------------------------------------------------------------
class UDPConnectSocket : public UDPSocket
{
public:
	UDPConnectSocket()
		: mServerSocketID(INVALID_SOCKET)
	{

	}

public:
	virtual void setSOCKET(SOCKET sock){ mServerSocketID = sock; }

	virtual SOCKET CurrentSendSocketID(){ return mServerSocketID; }

protected:
	SOCKET			mServerSocketID;		// ���ڼ����Ĺ���SOCKET, ���з��͹���
};

#endif