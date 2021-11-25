#include "ShareMemNet.h"


namespace NetCloud
{
	//-------------------------------------------------------------------------
	class SMsgNotifyWaitSendShareKeyPak : public BasePacket
	{
	public:
		virtual BOOL		Read(DataStream& iStream, size_t packetSize) { return iStream.read(mShareKey); }
		virtual BOOL		Write(DataStream& oStream) const { return oStream.write(mShareKey); }

		virtual	UINT		GetPacketSize() const { return sizeof(mShareKey); }

		virtual UINT		Execute(tNetConnect* pConnect) 
		{ 		
			Hand<ShareMemNet> net = pConnect->GetNetHandle();
			Hand<CShareMemMsgHander> hander = net->mShareMemMsgHander;
			hander->OnWaitSendShareBufferChanded(mShareKey);
			return 0; 
		}

	public:
		int mShareKey;
	};
	//-------------------------------------------------------------------------
	// 实现内存网络专用网络协议, 用于接收到较大数据包时, 临时缓存, 直到完全接收后, 再进行处理
	class ShareMemNetProtocol : public EventNetProtocol
	{
	protected:
		PacketID_t		mLastPacketID;
		byte					mPacketStateData;
		DSIZE				mPacketSize;
		DataBuffer		mPacketData;

	public:
		ShareMemNetProtocol()
			: mPacketSize(0) 
			, mLastPacketID(0)
			, mPacketStateData(0)
			, mPacketData(1024)
		{}

	public:
		HandPacket RestorePacket()
		{
			HandPacket packet = CreatePacket(mLastPacketID);
			if (packet)
			{
				mPacketData.seek(0);
				packet->SetState(mPacketStateData);
				bool bReadOk = packet->Read(mPacketData, mPacketSize);
				if (!bReadOk)
				{
					ERROR_LOG("%d packet restore fail, now give up", mLastPacketID);
					packet.setNull();
				}
			}
			else
			{
				// 考虑网络安全, 一般需要作断开连接处理, 且记录黑名单
				AString errorInfo = "Error : no exist packet >>>";
				errorInfo += (int)mLastPacketID;
				OnReadPacketError(errorInfo);		
			}
			mLastPacketID = 0;
			mPacketSize = 0;
			mPacketStateData = 0;
			mPacketData.clear(false);

			return packet;
		}

		virtual HandPacket ReadPacket(tNetConnect *pConnect, DataStream *mSocketInputStream) override
		{
			__ENTER_FUNCTION_FOXNET

				if (mLastPacketID!=0)
				{
					DSIZE needSize = mPacketSize - mPacketData.dataSize();
					if (needSize > 0)
					{
						if (mPacketData.size() < mPacketSize)
							mPacketData.resize(mPacketSize);

						DSIZE nowDataSize = mSocketInputStream->dataSize();

						if (nowDataSize>=needSize)
						{
							if (mSocketInputStream->_read((char*)mPacketData.data() + mPacketData.dataSize(), needSize))
							{
								mPacketData.setDataSize(mPacketData.dataSize() + needSize);
								return RestorePacket();
							}
							else
								ERROR_LOG("Must can read");
						}
						else
						{
							// 把当前存在的全部读取
							if (mSocketInputStream->_read((char*)mPacketData.data() + mPacketData.dataSize(), nowDataSize))
							{
								mPacketData.setDataSize(mPacketData.dataSize() + nowDataSize);
							}
							else
								ERROR_LOG("Must can read all now data");
						}
					}
					else
						return RestorePacket();

					return HandPacket();
				}

#if USE_BIG_PACKET_SIZE
				if (mSocketInputStream->dataSize() < PACKET_HEADER_SIZE)
					return HandPacket();
			char szTemp[PACKET_HEADER_SIZE];
			if (!mSocketInputStream->peek(szTemp, PACKET_HEADER_SIZE))
				return NULL;

			PacketID_t uID = (byte)szTemp[0];
			byte packetStateData = (byte)szTemp[1];
			UINT packetSize = *(UINT*)(szTemp + 2);
#else
				uint headValue = 0;
			AssertEx(PACKET_HEADER_SIZE == sizeof(headValue), "消息头结构大小与定义长度不一至");
			// step 1: try get packet head info
			if (mSocketInputStream->peek((void*)&headValue, PACKET_HEADER_SIZE) == FALSE)
				return NULL;

			byte uID = _GET_ID(headValue);
			byte packetStateData = _GET_STATE(headValue);
			UINT packetSize = _MSG_SIZE(headValue);
#endif

			if (!CheckPacketInfo(pConnect, uID, packetSize))
			{
				return NULL;
			}

			mSocketInputStream->skip(PACKET_HEADER_SIZE);

			UINT currentLen = mSocketInputStream->dataSize();
			// step 2: check data length over need packet size.
			if (currentLen < packetSize)
			{
				mLastPacketID = uID;
				mPacketSize = packetSize;
				mPacketStateData = packetStateData;
				
				if (mPacketData.size() < (DSIZE)packetSize)
					mPacketData.resize(packetSize);
				mPacketData.clear(false);

				DSIZE nowDataSize = mSocketInputStream->dataSize();
				if (mSocketInputStream->_read((char*)mPacketData.data(), nowDataSize))
				{
					mPacketData.setDataSize(nowDataSize);
				}
				else
					ERROR_LOG("Must can read all now data");

				return NULL;
			}
			
			HandPacket p = CreatePacket(uID);
			if (!p)
			{
				// 考虑网络安全, 一般需要作断开连接处理, 且记录黑名单
				AString errorInfo = "Error : no exist packet >>>";
				errorInfo += (int)uID;

				OnReadPacketError(errorInfo);

				mSocketInputStream->skip(packetSize);

				AssertEx(0, errorInfo.c_str());

				return NULL;
			}
			// 根据状态确定是否已经是加密消息
			p->SetState(packetStateData);

			// step 5: read packet data
			BOOL bRe = p->Read(*mSocketInputStream, packetSize);

			// 由于无锁共用缓存, 此时接收线程有可能已经又接收到新的数据
			if (bRe == FALSE)
			{
				AString info;
				info.Format("[%s:%d] Read packet %d fail", pConnect->GetIp(), pConnect->GetPort(), uID);
				ERROR_LOG(info.c_str());
				OnReadPacketError(info);
			}

			return p;

			__LEAVE_FUNCTION_FOXNET

				return NULL;
		}
	};


	ShareMemNet::ShareMemNet(bool bServer)
		: tBaseEventNet(false)
		, mbServer(bServer)
	{
		mNetProtocol = MEM_NEW ShareMemNetProtocol();

		mShareMemMsgHander = MEM_NEW CShareMemMsgHander(this);

		mShareMemConnect = MEM_NEW ShareMemConnect(this);

		GetNetProtocol()->RegisterNetPacket(MEM_NEW DefinePacketFactory < SMsgNotifyWaitSendShareKeyPak, SMsg_NotifyWaitSendShareKey>());
		mEventCenter = MEM_NEW Logic::EventCenter();
	}

	bool ShareMemNet::Init(SM_KEY memKey, DSIZE &bufSize, bool bServerNet)
	{
		return mShareMemMsgHander->Init(memKey, bufSize, bServerNet);
	}

	//-------------------------------------------------------------------------


	CShareMemMsgHander::CShareMemMsgHander(tBaseEventNet *pNet) : mTempProcess(MEM_NEW TempProcess(this))
		, mNeedUpdateSendShareKey(0)
		, mpShareMemNet(pNet)
	{	
		//RegisterShareMsg(eShareMsg_NotifyWaitSendShareKey, MEM_NEW DBShareSQLNotifyWaitSendShareKeyMsg(), CallBack<ShareSQLMsg*>(&TempProcess::ProcessNotifyWaitSendShareKeyMsg, mTempProcess.getPtr()));
		//RegisterShareMsg(eShareMsg_NotifyShareKeyName, MEM_NEW DBShareSQLNotifyShareKeyNameMsg(), CallBack<ShareSQLMsg*>(&TempProcess::ProcessNotifyShareKeyNameMsg, mTempProcess.getPtr()));
	}

	//-------------------------------------------------------------------------
	void CShareMemMsgHander::PrcessReceiveData(LoopData &loopData)
	{		
		do
		{
			HandPacket packet = mpShareMemNet->GetNetProtocol()->ReadPacket(mpShareMemNet->GetClientConnect().getPtr(), &loopData);
			if (!packet)
				break;

			packet->Execute(mpShareMemNet->GetClientConnect().getPtr());

		} while (loopData.dataSize() > 0);
	}

	void CShareMemMsgHander::Process()
	{
		ShareMemMsgHander::Process();
		if (mNeedUpdateSendShareKey != 0 && GetSendBuffer().GetWaitSendBuffer().dataSize() <= 0)
		{
			//??? 发送通知等待的共享内存信息
			Auto<SMsgNotifyWaitSendShareKeyPak> pak = mpShareMemNet->GetNetProtocol()->CreatePacket(SMsg_NotifyWaitSendShareKey);
			mpShareMemNet->GetClientConnect()->Send(pak.getPtr(), false);
			//msg.SendTo(GetSendBuffer());
			mNeedUpdateSendShareKey = 0;
		}
	}

	void CShareMemMsgHander::OnRestoreOtherWillSendData()
	{
		if (mOtherWillSendShareBuffer.Restore(mTempProcess->mOtherWaitSendShareKey))
		{
			DSIZE waitlen = mOtherWillSendShareBuffer.dataSize();
			// 一次读取
			if (waitlen > 0)
			{
				DSIZE revLen = GetReceiveBuffer().dataSize();
				ThreadLoopData  tempData(revLen + waitlen + 1);
				GetReceiveBuffer()._read(tempData.data(), revLen);
				mOtherWillSendShareBuffer._read(tempData.data() + revLen, waitlen);
				tempData._setTail(revLen + waitlen);

				PrcessReceiveData(tempData);
				if (tempData.dataSize() <= 0)
				{
					NOTE_LOG("Process succeed other wait send %d, all %d", waitlen, revLen + waitlen);
				}
				else
					ERROR_LOG("Process succeed other wait send %d, all %d, last %d", waitlen, revLen + waitlen, tempData.dataSize());
			}
		}
	}

	void CShareMemMsgHander::TempProcess::ProcessNotifyWaitSendShareKeyMsg(int key)
	{
		DEBUG_LOG("Receive notify SendShareKeyMsg");

		int thisWaitSendShareKey = 0;
		int memoryDBWaitSendShareKey = mpOwnerShareMemSQL->GetWaitSendShareMemKey(thisWaitSendShareKey);

		UINT size = 0;
		mOtherWaitSendShareMem.Destory();
		mOtherWaitSendShareMem.Attach(key, size);
		mOtherWaitSendShareKey = key;

		NOTE_LOG("Notify changed wait send buffer share key %d = size %u, this %d, other key %d", key, size, thisWaitSendShareKey, memoryDBWaitSendShareKey);


	}




}