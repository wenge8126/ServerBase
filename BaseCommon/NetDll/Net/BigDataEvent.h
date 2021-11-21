
#ifndef _INCLUDE_BIGDATAEVENT_H_
#define _INCLUDE_BIGDATAEVENT_H_

#include "LoopDataBuffer.h"
#include "ServerEvent.h"
#include "NodeMsgEvent.h"
#include "ResponseEvent.h"
#include "UnitComponent.h"

using namespace Logic;
//-------------------------------------------------------------------------
#define BIG_MSG_SEND_PART_SIZE		(256*1024)		// 实际测试下载可以使用16K或更大, 客户端上传最后为2K,否则会出现接收数据不完整现在
//-------------------------------------------------------------------------

namespace NetCloud
{
	class WaitReceiveBigMsgData : public Logic::CEvent
	{
	public:
		virtual bool _OnTimeOver() override
		{
			DEBUG_LOG("Big msg wait over time %d", (int)get("MSG_ID"));
			Free();
			return true;
		}

		virtual void InitData() override
		{
			DEBUG_LOG("*** Big msg wait free %d", (int)get("MSG_ID"));
			Logic::CEvent::InitData();
			mReceiveBigData.clear(true);
			mMsgAllSize = 0;
			mReceiveSize = 0;
		}

		~WaitReceiveBigMsgData()
		{
			
		}

	public:
		Array<AutoData>		mReceiveBigData;
		DSIZE							mMsgAllSize = 0;
		DSIZE							mReceiveSize = 0;
		AString							mMsgInfo;
	};

	typedef Hand<WaitReceiveBigMsgData> AReceiveBigData;

	class Net_Export BigMsgUnit : public NetUnit
	{
	public:
		virtual void Ready() override;
		// 开始接收及进度回调
		virtual void BeginReceive(EVENT_ID receiveMsgID, DSIZE allWaitSize, DSIZE partCount, DSIZE partSize, const AString &msgInfo) {}
		virtual void ReceiveProgress(EVENT_ID receiveMsgID, DSIZE revDize, DSIZE totalSize) {}
		virtual void OnFinishReceive(EVENT_ID receiveMsgID, const AString &msgInfo) { }

	public:
		EasyHash<AString, AReceiveBigData> mWaitReceiveDataList;		//NOTE: 仅支持单点连接,  多连接间同时多个大包消息, 有可能ID重复
	};
	// 发送大数据事件
	//-------------------------------------------------------------------------
	class Net_Export Cloud_SendDataPartEvent : public tRequestEvent
	{
	protected:
		DataBuffer		mWaitSendData;
		int					mSendCode;
		bool					mbSendFinished;
		bool					mbSendFail;
		EVENT_ID			mBigMsgIndexID;
		EVENT_ID			mWaitEventID;

		AutoData			mSendPartData;
		AutoEvent		mSendEvent;
		AString				mSendMsgInfo;

	public:
		Cloud_SendDataPartEvent()
			: mSendCode(0)
			, mBigMsgIndexID(0)
			, mWaitEventID(0)
			, mbSendFinished(false)
			, mbSendFail(false)
		{}

		virtual void InitData() override
		{
			Logic::tServerEvent::InitData();
			mBigMsgIndexID = 0;
			mSendCode = 0;
			mbSendFinished = false;
			mbSendFail = false;
			mWaitEventID = 0;
			mWaitSendData.clear(false);
		}

	public:
		virtual void OnSucceedFinish()
		{
			mbSendFinished = true;
			int allSize = (int)mWaitSendData.dataSize();
			int count = (allSize + BIG_MSG_SEND_PART_SIZE - 1) / BIG_MSG_SEND_PART_SIZE;
			//NOTE_LOG("** Finish send big event data size %d, part count %d", allSize, count);		
			AutoEvent evt = GetSelf();
			if (mSendEvent)
				mSendEvent->OnEvent(evt);
		}
		virtual bool _NeedFinishWhenResponsed() const override { return false; }

		// 获取发送百分比率
		virtual int GetSendProgress()
		{
			int allSize = (int)mWaitSendData.dataSize();
			int count = (allSize + BIG_MSG_SEND_PART_SIZE - 1) / BIG_MSG_SEND_PART_SIZE;
			if (count <= 0)
				return 100;
			if (mbSendFinished)
				return 100;
			return (int)((float)mSendCode / count *100);
		}

		// 是否发送完成
		virtual bool IsSendFinished() const { return mbSendFinished; }
		virtual bool IsSendFailed() const { return mbSendFail; }

	public:
		bool SendBigEvent(AutoEvent sendEvent, const AString &msgInfo = AString(), int *pSendSize = NULL, int *pPartCount = NULL, int *pPartSize = NULL)
		{
			mSendEvent = sendEvent;
			mbSendFinished = false;
			mSendMsgInfo = msgInfo;

			if (pPartSize != NULL)
				*pPartSize = BIG_MSG_SEND_PART_SIZE;

			AlloctRespID();
			mSendCode = 0;
			mWaitSendData.clear(false);
			bool b = GetEventCenter()->SerializeMsg(sendEvent, &mWaitSendData);
			if (b)
			{
				Hand<ResponseEvent> evt = sendEvent;
				if (evt)
					mWaitEventID = evt->mID;
				Hand<BigMsgUnit> player = mUnit;
				mBigMsgIndexID = GetEventID(); //++player->mLastBigMsgIndexID;

				if (!mSendPartData)
					mSendPartData = MEM_NEW DataBuffer(BIG_MSG_SEND_PART_SIZE);
				int allSize = (int)mWaitSendData.dataSize();
				int count = (allSize + BIG_MSG_SEND_PART_SIZE - 1) / BIG_MSG_SEND_PART_SIZE;
				if (pSendSize != NULL)
					*pSendSize = allSize;
				if (pPartCount != NULL)
					*pPartCount = count;
				DEBUG_LOG("Ready send big event data size %d, part count %d", allSize, count);
				mWaitSendData.seek(0);
				SendOncePart();
				return true;
			}
			return false;
		}

		void SendOncePart()
		{
			int allSize = (int)mWaitSendData.dataSize();
			int count = (allSize + BIG_MSG_SEND_PART_SIZE - 1) / BIG_MSG_SEND_PART_SIZE;
			if (mSendCode >= count)
			{
				Finish();
				OnSucceedFinish();
				return;
			}
			setFinished(false);
			GetData().clear(false);
			mSendPartData->clear(false);
			int partSize = mWaitSendData.lastDataSize() >= BIG_MSG_SEND_PART_SIZE ? BIG_MSG_SEND_PART_SIZE : mWaitSendData.lastDataSize();
			mSendPartData->_write(mWaitSendData.data() + mWaitSendData.tell(), partSize);
			mWaitSendData.seek(mWaitSendData.tell() + partSize);
			set("PART", &mSendPartData, typeid(mSendPartData));
			set("ID", (UInt64)mBigMsgIndexID);
			set("EVENT_ID", (UInt64)mWaitEventID);
			if (mSendCode <= 0)
			{
				set("COUNT", count);
				set("SIZE", (int)mWaitSendData.dataSize());
				set("INFO", mSendMsgInfo);
			}
	
			set("CODE", ++mSendCode);
			WaitTime(6);
			Start();
		}

	public:
		virtual void _OnResp(AutoEvent &respEvent) override
		{
			if (respEvent["RESULT"])
			{
				AutoEvent evt = GetSelf();
				if (mSendEvent)
					mSendEvent->OnEvent(evt);
				SendOncePart();
			}
			else
			{
				mbSendFail = true;
				ERROR_LOG("Send part data error, other side receive fail");
			}
		}

		virtual bool _OnTimeOver() override
		{
			mbSendFail = true;
			DEBUG_LOG("Send part data over time");
			AutoEvent evt = GetSelf();
			if (mSendEvent)
				mSendEvent->OnEvent(evt);
			return true;
		}
	};
	//-------------------------------------------------------------------------
	class Cloud_RevDataPartEvent : public tResponseEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			Hand<BigMsgUnit> player = mUnit;
			int partCode = get("CODE");
			EVENT_ID id = (EVENT_ID)(UInt64)get("ID");
			
			AString key;
			key.Format("%llu_%u", (UInt64)mSenderID, id);

			AReceiveBigData revData;
			if (partCode == 1)
			{
				int count = get("COUNT");
				DSIZE allSize = get("SIZE");
				revData = mUnit->GetEventCenter()->StartEvent("WaitReceiveBigMsgData"); //  MEM_NEW WaitReceiveBigMsgData();
				revData["MSG_ID"] = id;

				player->mWaitReceiveDataList.insert(key, revData);
				revData->mReceiveBigData.resize(count);
				revData->mMsgAllSize = allSize;
				revData->mMsgInfo = get("INFO");
				player->BeginReceive(id, allSize, count, BIG_MSG_SEND_PART_SIZE, get("INFO").string());
			}
			else
			{
				revData = player->mWaitReceiveDataList.find(key);
				if (!revData)
				{
					ERROR_LOG("等待接收大数据消息 数据不存在 >%d from %s", id, mSenderID.dump().c_str());
					Finish();
					return true;
				}
			}

			revData->WaitTime(6);

			Array<AutoData> &waitRevData = revData->mReceiveBigData;
			if (partCode > 0 && partCode <= waitRevData.size())
			{				
				AutoData d = (DataStream*)get("PART");
				if (!d)
				{
					ERROR_LOG("接收大数据消息 数据为空 >%u: %s", id, revData->mMsgInfo.c_str());
					Finish();
					return true;
				}
				revData->mReceiveSize += d->dataSize();
				player->ReceiveProgress(id, revData->mReceiveSize, revData->mMsgAllSize);

				waitRevData[partCode - 1] = d;
				GetResponseEvent()["RESULT"] = true;
				if (partCode == waitRevData.size())
				{
					// 完成处理, 大数据, 初始一个很大的缓存 96K
					thread_local static DataBuffer allData(1024 * 8 * 16);
					allData.clear(false);
					for (int i = 0; i < waitRevData.size(); ++i)
					{
						AutoData part = waitRevData[i];
						if (part)
							allData._write(part->data(), part->dataSize());
						else
						{
							ERROR_LOG("存在未接收到的数据包");
							GetResponseEvent()["RESULT"] = false;
							Finish();
							return true;
						}
					}
					player->OnFinishReceive(id, revData->mMsgInfo);
					DEBUG_LOG("接收完成数据包 %d, total size %d", partCode, (int)allData.dataSize());
					player->mWaitReceiveDataList.erase(id);
					revData->Free();

					//player->ProcessMsg(&allData);
					allData.seek(0);
					AutoEvent evt = player->GetEventCenter()->RestoreMsg(&allData);
					if (evt)
					{
						//NOTE_LOG("Rev msg event >%s", evt->GetEventName());
						//NOTE_LOG("%s", evt->GetData().dump().c_str());
						evt->OnEvent(player.getPtr(), typeid(tNetUnit));
						evt->OnEvent(&mSenderID, typeid(UnitID));
						evt->DoEvent(true);
					}
					else
					{
						ERROR_LOG("Restore big msg event fail >%d, then now remove connect", (int)allData.dataSize());
						//revFrameData->clear(false);
						//player->SetRemove(true);
					}
				}
			}
			else
			{
				GetResponseEvent()["RESULT"] = false;
				ERROR_LOG("Part code %d more then size %d", partCode, (int)waitRevData.size());
				player->mWaitReceiveDataList.erase(id);
			}
			Finish();
			return true;
		}
	};
	//-------------------------------------------------------------------------
}

#endif //_INCLUDE_BIGDATAEVENT_H_