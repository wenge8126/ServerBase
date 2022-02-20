
#ifndef _INCLUDE_ASYNCGATE_H_
#define _INCLUDE_ASYNCGATE_H_

#pragma once

class AsyncGate : public tNetProcess
{
public:
	AsyncGate(size_t maxConnectCount = NET_CONNECT_MAX, int threadNum = _IOCP_THREAD_NUM)
	{
		mGateNet = MEM_NEW MeshNet(maxConnectCount, threadNum);
	}

public:
	void StartGate(int gateCount, int gateCode, const char* szGateIP, int gatePort, const char *szMainIP, int mainPort, int nSafeCode)
	{
		mGateNet->mSafeCode = nSafeCode;
		mGateNet->StartNet(szGateIP, gatePort);

		if (strcmp(szGateIP, szMainIP) != 0 || gatePort != mainPort)
			mGateNet->ConnectNode(szMainIP, mainPort);
	}

	// 接收到获取Gate信息
	//void On(tNetConnect *pConnect, RQ_RequestGateInfo &req, RS_GateNodeInfo &info){}

public:
	Hand<MeshNet>		mGateNet;
};



#endif //_INCLUDE_ASYNCGATE_H_


