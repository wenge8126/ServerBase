#include "IOCPClientSetNet.h"
#include "IOCPCommon.h"
#include "IOCPConnect.h"


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

bool IOCPClientSetNet::Connect( const char *szIp, int nPort, int overmilSecond )
{
	if (szIp==NULL || nPort<=0)
	{
		DEBUG_LOG("ERROR: 未提供正常的连接IP或端口[%d]", nPort);
		return false;
	}

	ConnectNetThread *pWaitConnectThread = MEM_NEW ConnectNetThread();
	pWaitConnectThread->StartConnect(szIp, nPort, overmilSecond);
	mWaitConnectThreadList.push_front(pWaitConnectThread);
	 
	return true;
}

HandConnect IOCPClientSetNet::AwaitConnect(const char *szIp, int nPort, int overmilSecond)
{
	if (CORO == 0)
	{
		ERROR_LOG("AwaitConnect must in coro");
		return false;
	}

	if (szIp == NULL || nPort <= 0)
	{
		DEBUG_LOG("ERROR: 未提供正常的连接IP或端口[%d]", nPort);
		return false;
	}

	ConnectNetThread *pWaitConnectThread = MEM_NEW ConnectNetThread();
	pWaitConnectThread->mConnectCoroID = CORO;
	pWaitConnectThread->StartConnect(szIp, nPort, overmilSecond);
	mWaitConnectThreadList.push_front(pWaitConnectThread);

	YIELD;

	return pWaitConnectThread->mConnect;
}

void IOCPClientSetNet::StopNet( void )
{
	for (ConnectThreadList::iterator it=mWaitConnectThreadList.begin(); it; ++it)
	{
		(*it)->Close();
		delete *it;
	}
	mWaitConnectThreadList.clear();

	for (size_t i=0; i<mConnectList.size(); ++i)
	{
		if (mConnectList[i])
			mConnectList[i]->Close();
	}
}

void IOCPClientSetNet::Process()
{
	IOCPBaseNet::Process();

	if (!mWaitConnectThreadList.empty())
	{
		for (ConnectThreadList::iterator it= mWaitConnectThreadList.begin(); it; )
		{
			ConnectNetThread *pConnectThread = *it;

			if (pConnectThread->IsConnectFinish())
			{
				mWaitConnectThreadList.erase(it);

				if (pConnectThread->mSocket!=0)
				{
					HandConnect conn = CreateConnect();

					IOCPClientConnect *pRecConnect = dynamic_cast<IOCPClientConnect*>(conn.getPtr());
					AssertEx(pRecConnect!=NULL, "创建连接必须继承 IOCPConnect ");
					pRecConnect->mNet = GetSelf();
					pRecConnect->mSocketID = pConnectThread->mSocket;
					pConnectThread->mSocket = 0;
					pRecConnect->mIp = pConnectThread->mIP.c_str();
					pRecConnect->mPort = pConnectThread->mPort;

					_AppendConnect(conn);
					// 在_AppendConnect() 已经执行了以下功能
					//_SendSafeCode(pRecConnect);
					//pRecConnect->OnConnected();
					OnConnected(pRecConnect);
					pConnectThread->mConnect = conn;
				}
				else
				{
					Log("NET: WARN Connect fail [%s:%d]", pConnectThread->mIP.c_str(), pConnectThread->mPort);
					OnConnectFail( pConnectThread->mIP.c_str(), pConnectThread->mPort );
				}
				pConnectThread->Close();
				if (pConnectThread->mConnectCoroID != 0)
					RESUME(pConnectThread->mConnectCoroID);
				delete pConnectThread;
				pConnectThread = NULL;
			}
			else if (pConnectThread->IsOverTime())
			{
				mWaitConnectThreadList.erase(it);

				OnConnectFail(pConnectThread->mIP.c_str(), pConnectThread->mPort);
				Log("NET: WARN Connect over time [%s:%d]", pConnectThread->mIP.c_str(), pConnectThread->mPort);
				pConnectThread->Close();
				if (pConnectThread->mConnectCoroID != 0)
					RESUME(pConnectThread->mConnectCoroID);
				delete pConnectThread;
				pConnectThread = NULL;				
			}
			else
				++it;
		}
	}
}

void IOCPClientSetNet::_SendSafeCode( IOCPConnect *pConnect )
{
	int safe = GetSafeCode();
	if (safe!=0)
	{
			pConnect->_Send((const CHAR*)&safe, sizeof(int));				
	}
}

void IOCPClientSetNet::_OnConnectStart(tNetConnect *pConnect)
{
    _SendSafeCode(dynamic_cast<IOCPConnect*>(pConnect));
}

HandConnect IOCPClientSetNet::CreateConnect()
{
	return MEM_NEW IOCPClientConnect();
}

//-------------------------------------------------------------------------