#pragma once
#ifndef _INCLUDE_SHAREMEMNET_H_
#define _INCLUDE_SHAREMEMNET_H_

#include "ShareMemMsg.h"
#include "ArrayIndex.h"
#include "ShareDBLib.h"
#include "DataBuffer.h"
#include "BaseEventNet.h"


enum _ShareMemMsgType
{
	SMsg_NotifyWaitSendShareKey = PACKET_MAX + 50,
	SMsg_Begin = 100,
};

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// 用于共享内存进程 间消息通讯
	// 优势: 1 将消息数据即时缓存到共享内存进程, 从而保证数据的安全性, 即使发送进程异常退出, 也能够消息数据正常接收
	//          2 使用共享内存消息传送, 且无锁收发, 速度效率高 
	//-------------------------------------------------------------------------
	class ShareDBLib_Export ShareMemNet : public tBaseEventNet
	{
		friend class ShareMemConnect;
		friend class SMsgNotifyWaitSendShareKeyPak;
	public:
		virtual HandConnect GetClientConnect() override { return mShareMemConnect; }

		virtual void Process() override
		{
			mShareMemConnect->Process();
			mShareMemMsgHander->Process();
		}

		virtual bool StartNet(const char *szIP, int port) override 
		{ 
			DSIZE bufferSize = port;
			return Init(TOINT(szIP), bufferSize, mbServer);
		}

		virtual void StopNet() override
		{
			
		}

		bool Init(SM_KEY memKey, DSIZE &bufSize, bool bServerNet);

		virtual const char* GetIp(void) const { return "share memory"; };
		virtual int		GetPort(void) const { return 0; };

		virtual Logic::tEventCenter* GetEventCenter(void) const { return mEventCenter.getPtr(); }

	public:
		ShareMemNet(bool bServer=false);

		Hand<ShareMemMsgHander> GetMemHander() { return mShareMemMsgHander; }

	protected:
		Hand<ShareMemMsgHander>			mShareMemMsgHander;
		HandConnect									mShareMemConnect;
		mutable AutoEventCenter					mEventCenter;
		bool													mbServer;
	};
	//-------------------------------------------------------------------------
	class ShareMemConnect : public tNetConnect
	{
	public:
		virtual void SetNetID(int netID) {  }
		virtual int GetNetID() const { return 0; }

		virtual bool IsRemove() { return false; }
		virtual bool IsDisconnect() const 
		{ 
			Hand<ShareMemNet> net = mpNet;
			return !net->mShareMemMsgHander->ExistConnect();
		}

	public:
		virtual bool SendEvent(Logic::tEvent *pEvent)
		{
			Auto<EventPacket> evtPak = GetNetHandle()->GetNetProtocol()->CreatePacket(PACKET_EVENT);		
			AutoEvent evt = pEvent->GetSelf();
			evtPak->SetEvent(evt);
			return Send(evtPak.getPtr(), false);			
		}
		virtual bool Send(const Packet  *msgPacket, bool bEncrypt)
		{
			mSendBuffer.clear(false);
			if (GetNetHandle()->GetNetProtocol()->WritePacket(msgPacket, &mSendBuffer))
			{
				Hand<ShareMemNet> net = mpNet;
				return net->mShareMemMsgHander->SendData(&mSendBuffer);
			}
			return false;
		}

		virtual tNetHandle* GetNetHandle(void) { return mpNet; }

		virtual bool Process() { return true; }

	public:
		virtual void OnConnected() {}
		virtual void OnDisconnect() {}
		virtual void OnSucceedSendEvent(Logic::tEvent *pEvent, Packet *p) {}

		virtual void Close() override
		{
			WARN_LOG("Now need close, but nothing");
		}

	public:
		ShareMemConnect(tNetHandle *pNet)
			:mpNet(pNet) 			
		{
			
		}

	public:
		tNetHandle					*mpNet;
		LoopDataStream			mSendBuffer;
	};
	//-------------------------------------------------------------------------
	class ShareDBLib_Export CShareMemMsgHander : public ShareMemMsgHander
	{
		// 用临时处理对象， 目的解决如 DBMemory 使用对象方式保存此对象， 释放mMsgList时， 触发构造同时两次释放mMsgList
		class TempProcess : public Base<TempProcess>
		{
		public:
			TempProcess(CShareMemMsgHander *pSql)
				: mpOwnerShareMemSQL(pSql)
				, mOtherWaitSendShareKey(0)
			{

			}
			~TempProcess()
			{
				mOtherWaitSendShareMem.Destory();
				for (auto it = mShareMemoryPoolUseList.begin(); it.have(); it.next())
				{
					delete it.get();
				}
				mShareMemoryPoolUseList.clear();
			}

			void ProcessNotifyWaitSendShareKeyMsg(int key);


		public:
			CShareMemMsgHander		*mpOwnerShareMemSQL;
			ShareMemAO		mOtherWaitSendShareMem;			// 使用此共享对象占用住待发送空间
			int				mOtherWaitSendShareKey;
			EasyMap<AString, ShareMemAO*>	mShareMemoryPoolUseList;				// 共享内存池使用到的共享内存, 用于占用不被释放
		};

	public:
		CShareMemMsgHander(tBaseEventNet *pNet);
		~CShareMemMsgHander()
		{		
			mTempProcess._free();
		}
		

		virtual void PrcessReceiveData(LoopData &loopData) override;

		void OnReceiveRestoreMsgError(int msgID)
		{
			ERROR_LOG("Rev restore msg error >%d, then now skip msg", msgID);

		}

		virtual void OnWaitSendShareBufferChanded(int sendShareKey) override
		{
			//不可以直接发送， 开始初始时还未初始OK SendSQLMsg(&msg); 还未完成重设置大小, 会引起循环
			if (!mShareMemMsgBuffer.IsServerCreate()) //!!! 当为创建方, 即共享管理方时, 不需要发送到DB方
				mNeedUpdateSendShareKey = sendShareKey;
		}

		virtual void Process() override;

		// 恢复收取对端待发送的缓存数据
		virtual void OnRestoreOtherWillSendData();


	protected:		
		tBaseEventNet							*mpShareMemNet;
		Hand<TempProcess>				mTempProcess;
		RestoreWillSendTempBuffer		mOtherWillSendShareBuffer;
		int mNeedUpdateSendShareKey;
	};
	//-------------------------------------------------------------------------

}
#endif //_INCLUDE_SHAREMEMSQL_H_
