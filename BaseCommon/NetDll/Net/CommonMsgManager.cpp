

#include "CommonMsgManager.h"
//#include "PacketFactoryManager.h"
#include "A_CommonData.h"
#include "TableManager.h"

#include "Packet.h"

#ifdef SHOW_TABLE_PACKET_REV
#include "TimeManager.h"
#endif

// ��Լһ���ӷ�10SEND_SLEEP_TIME����
#define	 SEND_SLEEP_TIME	50
//--------------------------------------------------------------------------------------
CommonMsgManager::CommonMsgManager()
{

}
//--------------------------------------------------------------------------------------

unsigned int CommonMsgManager::OnCommonMsg( Player *pPlayer, Packet *pPacket )
{
	A_CommonData *pData = dynamic_cast<A_CommonData*>(pPacket);
	if (pData)
	{		
		const AString &type = pData->mEventName;
		if (type.empty())
			return PACKET_EXE_ERROR;
#ifdef SHOW_TABLE_PACKET_REV
		printf("���յ����ݰ���Ϣ[%s]-->[%u]-->[%d:%d:%d]\n",type.c_str(),pData->mData->size(),g_pTimeManager->GetHour(),g_pTimeManager->GetMinute(),g_pTimeManager->GetSecond());
#endif
		AutoData hData;
		// ����Ƿ��Ƕ�����
		if ( pData->mIsBegin && pData->mIsEnd )
		{
			hData = pData->mData;
		}
		else
		{
			if (pData->mIsBegin)
			{
				mRevDataMap[MAKE_INDEX_ID(type.c_str())] = pData->mData;
				pData->mData->seek(0,SEEK_END);
			}
			else
			{
				AutoDataMap::iterator it = mRevDataMap.find(MAKE_INDEX_ID(type.c_str()));
				if (it!=mRevDataMap.end())
				{
					hData = it->second;
					hData->write(pData->mData->data(),pData->mSize);
					if (pData->mIsEnd)
					{
						mRevDataMap.erase(it);
					}
				}
			}
		}

		if (hData)
		{
			//hTable->_SaveToCSV(("e:/"+type+".csv").c_str());
			return ExeCommonPacket(pPlayer, pPacket, type, hData);
		}
		return PACKET_EXE_CONTINUE;		
	}
	return PACKET_EXE_ERROR;
}
//--------------------------------------------------------------------------------------


void CommonMsgManager::Tick()
{
	// ���ͻ����еı�� [���ĸ� 2010/6/9]
	if (!m_waitSendTableList.empty())
	{
		SendEventData &sendT = m_waitSendTableList.front();		
		if (GetTickCount()-sendT.upTime>SEND_SLEEP_TIME) 
		{
			if (__SendData(sendT))
			{
				m_waitSendTableList.pop_front();
			}
			else
				sendT.upTime = GetTickCount();
		}
	}
}


//----------------------------------------------------------------------------------------------
