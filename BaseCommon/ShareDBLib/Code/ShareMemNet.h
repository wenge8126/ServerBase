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
	// ���ڹ����ڴ���� ����ϢͨѶ
	// ����: 1 ����Ϣ���ݼ�ʱ���浽�����ڴ����, �Ӷ���֤���ݵİ�ȫ��, ��ʹ���ͽ����쳣�˳�, Ҳ�ܹ���Ϣ������������
	//          2 ʹ�ù����ڴ���Ϣ����, �������շ�, �ٶ�Ч�ʸ� 
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
		// ����ʱ������� Ŀ�Ľ���� DBMemory ʹ�ö���ʽ����˶��� �ͷ�mMsgListʱ�� ��������ͬʱ�����ͷ�mMsgList
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
			ShareMemAO		mOtherWaitSendShareMem;			// ʹ�ô˹������ռ��ס�����Ϳռ�
			int				mOtherWaitSendShareKey;
			EasyMap<AString, ShareMemAO*>	mShareMemoryPoolUseList;				// �����ڴ��ʹ�õ��Ĺ����ڴ�, ����ռ�ò����ͷ�
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
			//������ֱ�ӷ��ͣ� ��ʼ��ʼʱ��δ��ʼOK SendSQLMsg(&msg); ��δ��������ô�С, ������ѭ��
			if (!mShareMemMsgBuffer.IsServerCreate()) //!!! ��Ϊ������, ���������ʱ, ����Ҫ���͵�DB��
				mNeedUpdateSendShareKey = sendShareKey;
		}

		virtual void Process() override;

		// �ָ���ȡ�Զ˴����͵Ļ�������
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
