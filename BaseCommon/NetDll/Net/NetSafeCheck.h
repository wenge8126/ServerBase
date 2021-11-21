/********************************************************************
	created:	2012/07/01
	created:	1:7:2012   19:16
	filename: 	d:\New\Common\NetDll\Net\NetSafeCheck.h
	file path:	d:\New\Common\NetDll\Net
	file base:	NetSafeCheck
	file ext:	h
	author:		
	
	purpose:	���簲ȫ���
*********************************************************************/
#ifndef _INCLUDE_NETSAFECHECK_H_
#define _INCLUDE_NETSAFECHECK_H_

#include "NetHead.h"

class SocketInputStream;
class SocketOutputStream;
class tServerNet;

enum NET_CHECK_RESULT
{
	NET_CHECK_NO_CHECK,
	NET_CHECK_SUCCEED,
	NET_CHECK_FAIL,
};
//------------------------------------------------------------------------
// ��ѡ�������
// step 1: �ͻ������ӷ�������, ����������һ���̶�����֤����, ��Ҫ���ں�ˮ����, ����Ҫ�����ֱ�ӷ���true
// step 2: ����һ��OK��, �������ͻ��˷���һ����֤�����Ϣ,Ȼ��ȴ��ظ�, �������Ƿ�ʱ
//------------------------------------------------------------------------
class Net_Export_H tSafeCheck__
{
public:
	virtual ~tSafeCheck__(){}

public:
	// Begin step 1
	virtual NET_CHECK_RESULT SafeCheck(SocketInputStream *pInputStream, tConnect *pConnect) = 0;
	virtual bool ResponeCheck(SocketOutputStream *pOutputStream) = 0;
	virtual int GetCheckOverTime(void) = 0;

public:
	// Begin step 2
	virtual bool OnConnectCheckPass(tServerNet *pNetManager, int connectID) = 0;

public:
	// step check result
	virtual void OnSafeCheckSucceed(tServerNet *pNetManager, tConnect *p) = 0;
	virtual void OnSafeCheckFail(tServerNet *pNetManager, tConnect *p) = 0;


};



#endif