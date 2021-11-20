/********************************************************************
	created:	2011/11/22
	created:	22:11:2011   17:29
	filename: 	e:\01_Langrisser\third\DataTable\EventCore\ServerEvent.h
	file path:	e:\01_Langrisser\third\DataTable\EventCore
	file base:	ServerEvent
	file ext:	h
	author:		Wenge Yang
	description:
	purpose:	用于让客户端同步执行动作
*********************************************************************/
#ifndef _INCLUDE_SERVEREVENT_H_
#define _INCLUDE_SERVEREVENT_H_

#include "EventCore.h"

#include "AutoKey.h"

#include "LogicNode.h"
#include "NetHandle.h"
#include "ServerEvent.h"
#include "CoroutineTool.h"

#define NEED_RESEND_ONTIMEOVER		0

#define REGISTER_REQUEST_EVENT(center, eventClass)	 center->RegisterEvent(#eventClass, MEM_NEW Logic::ReqeustEventFactory<eventClass>());

// NOTE: 使用此宏注册的事件, 回复事件注册名称必须为 _R_eventName, 用于网络单元之间请求回复
#define REGISTER_AWAIT_REQUEST(center, eventName)	 center->RegisterEvent(eventName, MEM_NEW Logic::ReqeustEventFactory<AsyncRequestEvent>());

namespace Logic
{
	//-------------------------------------------------------------------------
	// 基本网络事件
	class tBaseNetEvent : public Logic::CEvent
	{
		typedef CEvent  base;
	public:
		virtual void InitData()
		{
			//base::InitData();
			mNetConnect.setNull();
		}

		virtual void _OnBindNet(tNetConnect *netTool)
		{
			if (netTool!=NULL)
				mNetConnect = netTool->GetSelf();
		}

		virtual bool Send(int nType = 0, int nTarget = 0)
		{
			if (mNetConnect)
				return mNetConnect->SendEvent(this);

			return base::Send(nType, nTarget);
		}

		bool SendEvent(AutoEvent hSendEvent)
		{
			if (mNetConnect)
				return mNetConnect->SendEvent(hSendEvent.getPtr());
			Log("Warn: 连接不存在, 可能已经被释放");
			return false;
		}

		virtual AutoEvent StartEvent(const char* eventName)
		{
			AutoEvent hEvent = base::StartEvent(eventName);
			if (hEvent)
				hEvent->_OnBindNet(mNetConnect.getPtr());
			return hEvent;
		}

		virtual AutoEvent StartEvent( const char* eventName, bool bDefault )
		{
			AutoEvent hEvent = base::StartEvent(eventName, bDefault);
			if (hEvent)
				hEvent->_OnBindNet(mNetConnect.getPtr());
			return hEvent;
		}

	public:
		HandConnect  mNetConnect;
	};
	//-------------------------------------------------------------------------
	// 用于驱动客户端执行事件, 发送的事件即是事件本身, 
	// 即客户端也要有与当前事件有相对应的事件
	// 流程: S-->C(执行事件), C-->S(回复ResponsionC2S事件, 用来完成服务器当前事件)
	// Note: 设置了等待时间后, 才具备回复功能, 默认后, 将不进行回复
	//-------------------------------------------------------------------------

	class EventCoreDll_Export tServerEvent : public tBaseNetEvent // tLogicNode
	{
		typedef tBaseNetEvent		parent;
		typedef tLogicNode	ParentClass;
		friend class ResponseEventFactory;

	public: 
		tServerEvent();
		~tServerEvent();

	public:
		virtual void Start(){ DoEvent(); }
		virtual void StartByConnect(tNetConnect *pConn);

		virtual void InitData() override;

		virtual bool _Serialize(DataStream *destData)
		{
			destData->write(mID);
			return tBaseNetEvent::_Serialize(destData);
		}
		virtual bool _Restore(DataStream *scrData)
		{
			if (!scrData->read(mID))
				return false;
			return tBaseNetEvent::_Restore(scrData);
		}

	public:		
		virtual bool DoEvent(bool bImmediately = true);
		virtual bool _OnEvent(AutoEvent &hEvent);
		// 指令执行完后,接收到回复后的处理
		virtual void _OnResp(AutoEvent &respEvent){}

		virtual bool _NeedFinishWhenResponsed() const { return true; }

		// 重发处理
		virtual bool DoTimeOver();

#if DEVELOP_MODE
		virtual void Finish();
#endif

	public:
		EVENT_ID GetEventID(void)
		{
			return mID; //(EVENT_ID)get(RESP_ID_KEY);
		}

		virtual void AlloctRespID();

		virtual void ReleaseRespID();		

	public:
		// 超时处理
		virtual bool _OnTimeOver(void)
		{ 
			return parent::_OnTimeOver();
		}
		virtual void OnSendFail() {}
		virtual bool _currentCanFinish(void);

#if DEVELOP_MODE
	protected:
		UInt64		mSendTime;
		UInt64		mResponseTime;
#endif

		EVENT_ID		mID;
	};
	//-------------------------------------------------------------------------
	//  在同一连接同一事件中心定义请求回复消息
	//  请求发送的事件名为 _R_[MsgName], 接收回复消息注册 即 _R_[MsgName]
	template<typename T>
	class ReqeustEventFactory : public Logic::EventFactory<T, true>
	{
	public:
		virtual void SetEventName(const char* strEventName) override
		{
			Logic::EventFactory<T, true>::SetEventName(strEventName);
			mNetEventName.Format("_R_%s", strEventName);
			mNetNameIndex = MAKE_INDEX_ID(mNetEventName.c_str());
		}

	public:
		virtual bool SaveEventName(DataStream *destData)
		{
			return destData->writeString(mNetEventName);
		}
		virtual bool SaveEventNameIndex(DataStream *destData)
		{
			return destData->write(mNetNameIndex);
		}

	public:
		ReqeustEventFactory()
			: mNetNameIndex(0) {}

	protected:
		AString mNetEventName;
		int			mNetNameIndex;
	};

	//-------------------------------------------------------------------------

	class tAsyncRequestEvent : public Logic::tServerEvent
	{
	public:
		AutoEvent AwaitRequest(int waitMilSecond)
		{
			if (CORO == 0)
			{
				ERROR_LOG("tAsyncRequestEvent::AwaitRequest must run in coroutine");
				return AutoEvent();
			}
			Start();
			WaitTime(waitMilSecond*0.001f);
			mCoroID = CORO;
			YIELD
				return mRespEvent;
		}

	public:
		virtual void _OnResp(AutoEvent &respEvent)
		{
			mRespEvent = respEvent;
			RESUME(mCoroID);
		}

		virtual bool _OnTimeOver() override
		{
			ERROR_LOG("%s request over time", GetEventName() );
			RESUME(mCoroID);
			return true;
		}

		virtual void InitData() override
		{
			tServerEvent::InitData();
			mCoroID = 0;
			mRespEvent.setNull();
		}

	protected:
		AutoEvent mRespEvent;
		CoroID			mCoroID;
	};
	//-------------------------------------------------------------------------

}


//-------------------------------------------------------------------------

#endif //_INCLUDE_SERVEREVENT_H_ 