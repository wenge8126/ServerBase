#include "BigMsgPacket.h"
#include "DefinePacketFactory.h"

void WaitReceiveBigMsgEvent::InitRegister(tNetHandle *pNet)
{
	pNet->GetEventCenter()->RegisterEvent("WaitReceiveBigMsgEvent", MEM_NEW EventFactory<WaitReceiveBigMsgEvent>());
	pNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<PartPacket, PACKET_PART_PACKET>(), true);
	pNet->GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory<FirstPartPacket, FIRST_PART_PACKET>(), true);
}

bool WaitReceiveBigMsgEvent::SendBigMsg(Packet *pPacket, tNetConnect *pTargetConnect, int onePartSize /*= 4 * 1024*/)
{
	Hand< WaitReceiveBigMsgEvent> evt = pTargetConnect->GetNetHandle()->GetEventCenter()->StartEvent("WaitReceiveBigMsgEvent");
	if (evt)
		return evt->_sendBigMsg(pPacket, pTargetConnect, onePartSize);
	else
		ERROR_LOG("No register WaitReceiveBigMsgEvent  of connect %s:%d", pTargetConnect->GetIp(), pTargetConnect->GetPort());
	return false;
}

void WaitReceiveBigMsgEvent::OnConnectRevEvent(tNetConnect *pConnect, EasyHash<EVENT_ID, AutoEvent> &waitRevBigMsgList, Logic::tEvent *pEvent, const Packet *pPacket)
{
	if (pPacket == NULL)
		return;
	if (pEvent != NULL)
	{
		// 开始接收
		if (pEvent->getFinished())
			return;
		Hand<WaitReceiveBigMsgEvent> evt = pEvent;
		Auto<FirstPartPacket> p = (Packet*)pPacket;
		if (!p)
		{
			ERROR_LOG("First part is not FirstPartPacket");
			return;
		}
		evt->mFlagID = p->mPacketFlag;
		waitRevBigMsgList.insert(p->mPacketFlag, evt);
	}
	else
	{
		// 接收分包
		Auto<PartPacket> p = (Packet*)pPacket;
		if (!p)
		{
			ERROR_LOG("First part is not PartPacket");
			return;
		}
		Hand<WaitReceiveBigMsgEvent> evt = waitRevBigMsgList.find(p->mPacketFlag);
		if (evt)
			evt->OnRevPartDataMsg(pConnect, p.getPtr());
		else
			ERROR_LOG("No find wait big event %u", p->mPacketFlag);
	}
}

bool WaitReceiveBigMsgEvent::_sendBigMsg(Packet *pScrPacket, tNetConnect *pTargetConnect, int onePartSize /*= 4*1024*/)
{
	DataStream &mWaitSendData = *(dynamic_cast<DataStream*>(mRevData.getPtr()) );
	mWaitSendData.clear(false);
	bool b = pScrPacket->Write(mWaitSendData);
	if (!b)
	{
		ERROR_LOG("Big msg %s save data fail", pScrPacket->GetMsgName());
		return false;
	}
	mWaitSendData.seek(0);
	EVENT_ID flagID = (EVENT_ID)GetEventCenter()->AutoAlloctKey();
	int allSize = (int)mWaitSendData.dataSize();
	int count = (allSize + onePartSize - 1) / onePartSize;
	DEBUG_LOG("Will big msg %s, part count %d, total size %d", pScrPacket->GetMsgName(), count, mRevData->dataSize());
	for (int i = 0; i < count; ++i)
	{
		Auto<PartPacket> pPacket = pTargetConnect->GetNetHandle()->GetNetProtocol()->CreatePacket(i == 0 ? FIRST_PART_PACKET : PACKET_PART_PACKET);
		if (pPacket == NULL)
		{
			ERROR_LOG("%s:%d No register PACKET_PART_PACKET packet", pTargetConnect->GetIp(), pTargetConnect->GetPort());
			return false;
		}
		int lastSize = allSize - onePartSize * i;
		int partSize = lastSize >= onePartSize ? onePartSize : lastSize;
		pPacket->mData->clear(false);
		//pPacket->mData->_write(mWaitSendData.data() + mWaitSendData.tell(), partSize);
		pPacket->mTempSendData.InitSet(mWaitSendData.data() + i*onePartSize, partSize);
		pPacket->mTempSendData.setDataSize(partSize);		

		pPacket->mPartCode = i+1;
		pPacket->mPacketFlag = flagID;
		if (i == 0)
		{
			Auto<FirstPartPacket> p = pPacket;
			AssertNote(p, "first part packet must is FirstPartPacket");
			p->mTotalDataSize = allSize;
			p->mPartCount = count;
			p->mBigMsgID = pScrPacket->GetFactory()->GetPacketID();
		}
		IOCPConnect *pConn = dynamic_cast<IOCPConnect*>(pTargetConnect);
		if (pConn != NULL)
		{
			if (!pConn->SendPacket(pPacket.getPtr(), false))
			{
				ERROR_LOG("%s:%d send part msg packet fail", pTargetConnect->GetIp(), pTargetConnect->GetPort());
				return false;
			}			
		}
		else
		{
			ERROR_LOG("No is IOCPConnect");
			return false;
		}
	}
	return true;
}

AutoEvent WaitReceiveBigMsgEvent::InitReady(tNetConnect *pConn, PartPacket *pPartMsg)
{
	Hand<WaitReceiveBigMsgEvent> revEvent = pConn->GetNetHandle()->GetEventCenter()->StartEvent("WaitReceiveBigMsgEvent");
	if (!revEvent)
	{
		ERROR_LOG("No exist event WaitReceiveBigMsgEvent");
		return revEvent;
	}

	revEvent->mOwnerConnect = pConn;
	revEvent->OnRevPartDataMsg(pConn, pPartMsg);
	pConn->OnReceivePacket(pPartMsg, revEvent.getPtr());

	return revEvent;
}

void WaitReceiveBigMsgEvent::OnRevPartDataMsg(tNetConnect *pConn, PartPacket *pPartMsg)
{
	if (mOwnerConnect != pConn)
	{
		ERROR_LOG("Now owner connect %s:%d is not same receive const %s:%d", mOwnerConnect->GetIp(), mOwnerConnect->GetPort(), pConn->GetIp(), pConn->GetPort());
		return;
	}

	if (pPartMsg->mPartCode != mAlreadyReceiveCode + 1)
	{
		ERROR_LOG("Error part code %d, must is %d", pPartMsg->mPartCode, mAlreadyReceiveCode + 1);
		return;
	}

	mAlreadyReceiveCode = pPartMsg->mPartCode;

	if (mAlreadyReceiveCode == 1)
	{
		FirstPartPacket *p = dynamic_cast<FirstPartPacket*>(pPartMsg);
		if (p == NULL)
		{
			ERROR_LOG("Error  part code 1 must is FirstPartPacket");
			Finish();
			return;
		}
		mTotalDataSize = p->mTotalDataSize;
		mPartCount = p->mPartCount;
		mBigMsgID = p->mBigMsgID;
	}

	mRevData->_write(pPartMsg->mData->data(), pPartMsg->mData->dataSize());

	if (mAlreadyReceiveCode == mPartCount)
	{
		DEBUG_LOG("All rev big msg %d, part count %d, total size %d", mBigMsgID, mPartCount, mRevData->dataSize());
		// 全部接收完成
		if (mRevData->dataSize() != mTotalDataSize)
		{
			ERROR_LOG("Big msg total size no same %d != rev %d", mTotalDataSize, mRevData->dataSize());
			Finish();
			return;
		}
		mRevData->seek(0);
		HandPacket mBigMsgPacket = pConn->GetNetHandle()->GetNetProtocol()->CreatePacket(mBigMsgID);
		if (!mBigMsgPacket)
		{
			ERROR_LOG("No exist big msg %d", mBigMsgID);
			Finish();
			return;
		}
		if (!mBigMsgPacket->Read(*mRevData, mRevData->dataSize(), pConn))
		{
			Finish();
			ERROR_LOG("big msg %d restore fail", mBigMsgID);
			return;
		}
		mBigMsgPacket->Execute(pConn);
		Finish();
	}
}

