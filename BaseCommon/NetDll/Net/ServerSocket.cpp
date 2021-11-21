//#include "stdafx.h"
//#include "KeyDog.h"

#include "stdio.h"
//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////
#include "ServerSocket.h"
#include "Assertx.h"


bool TCPServerSocket::InitCreate( UINT port , UINT backlog ) 
{
	__ENTER_FUNCTION_FOXNET

	BOOL ret = FALSE ;

#define KEY_MAC 12
#ifdef __WINDOWS__

//#ifndef GAME_CLIENT
//	char lpszMac[128] = {0};
//	char lpszKey[128] = {0};
//	Ini ini( FILE_SERVER_INFO ) ;
//	ini.ReadText( "KeyDog", "Key", lpszKey, sizeof(lpszKey) ) ;
//	GetMacByCmd(lpszMac);
//	int uret = strncmp(lpszKey, lpszMac,KEY_MAC);
//	if (0 != uret)
//	{
//		while (1)
//		{
//			MessageBox(NULL,"���кŴ���Ϊ�˲����˶��ѣ���ʾ��һ�£�","���ش���!",MB_ICONERROR);//��ʽʱע�͵�
//			//double *d = new double[10000];//��ʽʱ��
//		}
//		//throw 1 ;//��ʽʱ��
//	}
//	else
//	{
	//		;
	//	}
	//#else
	//	//if(!CheckSystem())
	//	//	throw 1;
	//#endif

#endif

	// create socket implementation object
	m_Socket = new Socket;
	Assert( m_Socket ) ;
	if( m_Socket==NULL )
		throw 1 ;
	
	// create socket
	ret = m_Socket->create( ) ;
	if( ret==FALSE )
		throw 1 ;
	Assert( ret ) ;
	
	// reuse address before Bind()
	// ���ε� �ϱ� ���� �ּ� ����� �ý��ۿ� �˷��� �Ѵ�.
	ret = m_Socket->setReuseAddr( ) ;
	if( ret==FALSE )
		throw 1 ;
	Assert( ret ) ;
	
	// bind address to socket
	// �̹� port�� m_Impl�� ����Ǿ� �����Ƿ�, �Ķ���;��� Bind()�� ȣ���ص� �ȴ�.
	ret = m_Socket->bind( port ) ;
	if( ret==FALSE )
		throw 1 ;
	Assert( ret ) ;

	m_Socket->setSendBufferSize( 1024*100 );
	m_Socket->setReceiveBufferSize( 1024*100 );
	
	// set listening queue size
	ret = m_Socket->listen( backlog ) ;
	if( ret==FALSE )
		throw 1 ;
	Assert( ret ) ;

	return true;

	__LEAVE_FUNCTION_FOXNET

}

ServerSocket::~ServerSocket () 
{
	__ENTER_FUNCTION_FOXNET
		
	if ( m_Socket != NULL ) 
	{
		m_Socket->close();
		delete m_Socket;
		m_Socket = NULL;
	}

	__LEAVE_FUNCTION_FOXNET
}
	
VOID ServerSocket::close ()
{
	__ENTER_FUNCTION_FOXNET
		
	if( m_Socket )
	{
		m_Socket->close( ) ;
	}

	__LEAVE_FUNCTION_FOXNET
}

BOOL ServerSocket::accept ( Socket* socket ) 
{
	__ENTER_FUNCTION_FOXNET
	
	UINT addrlen = sizeof(SOCKADDR_IN) ;
	socket->close( ) ;

	socket->setSOCKET( m_Socket->accept( (struct sockaddr *)(&(socket->getSocketAddr())), &addrlen ) ) ;
	if( socket->getSOCKET() == INVALID_SOCKET )
		return FALSE ;

	socket->setPort( ntohs( socket->getSocketAddr().sin_port ) );
	socket->setHost( inet_ntoa(socket->getSocketAddr().sin_addr) );

	return TRUE;

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}







