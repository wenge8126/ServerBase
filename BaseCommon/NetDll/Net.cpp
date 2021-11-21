// Net.cpp : Defines the entry point for the DLL application.
//
#include "Net/NetHead.h"
#include "stdafx.h"
#include "winsock2.h"
#include <string>

#include "Net/Socket.h"

void InitNet()
{
	WORD ver;
	//WSADATA�ṹ�����ڴ洢WSAStartup��õ����ݡ�
	WSADATA lpWSADATA;
	ver = (WORD)GetVersion(); // VerifyVersionInfo()
	if ( WSAStartup(ver, &lpWSADATA) != 0 ) 
	{
		char szError[256];
		printf_s("WSAStartup�����ʼ��ʧ�ܣ������룺%u\n", ::GetLastError());
		throw new std::exception(szError);
	}
}



#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if (DLL_PROCESS_ATTACH==ul_reason_for_call)
	{
		InitNet();
	}

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

