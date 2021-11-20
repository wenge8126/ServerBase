/********************************************************************
created:	2013/06/07
created:	7:6:2013   11:05
filename: 	C:\Work\BaseCommon\EventCore\Core\ResponseEvent.h
file path:	C:\Work\BaseCommon\EventCore\Core
file base:	ResponseEvent
file ext:	h
author:		Wenge Yang

purpose:	应答事件给等待事件回复的事件
*********************************************************************/
#ifndef _INCLUDE_RESPONSEEVENT_H_
#define _INCLUDE_RESPONSEEVENT_H_

#include "CEvent.h"

namespace Logic
{
	class tServerEvent;
	class EventCoreDll_Export ResponseEvent : public CEvent
	{
		typedef CEvent base;
	public:
		ResponseEvent()
			: mID(0)
		{
			
		}
		virtual ~ResponseEvent()
		{

		}

	public:
		virtual bool _Serialize(DataStream *destData)
		{
			destData->write(mID);
			return base::_Serialize(destData);
		}
		virtual bool _Restore(DataStream *scrData)
		{
			if (!scrData->read(mID))
				return false;
			return base::_Restore(scrData);
		}

		virtual void InitData() override
		{
			base::InitData();
			mID = 0;
		}

	public:
		virtual bool _DoEvent();

		virtual void Dump()
		{
			AString s = "From > [";
			s += get("_SERVER_EVENT_").string();
			s += "]";
			Log( s.c_str() );
			base::Dump();
		}

		virtual AString GetEventNameInfo() const
		{
			AString nameInfo = base::GetEventNameInfo();
			nameInfo += "*";
			if (GetData().exist("_SERVER_EVENT_"))
				nameInfo += GetData().get("_SERVER_EVENT_").string();
			else
				nameInfo += "UNKOWN";
			return nameInfo;
		}

	public:
		EVENT_ID	mID;
	};
    /*/----------------------------------------------------------------------
     ID 生成及应用优化 NOTE: 2021.7.14 修改为8位用于自增变化, 低24位用于在栈中的位置, 增加同时容纳的数量
     1 共4字节32位无符号整数
     2 高8位生成时比上次增加1 (0~255)
     3 低24位表示在数组中的位置, 数组同时最多只能保存2的24次方个事件,即同时等待事件不可超过2的24次方
     4 空闭中的栈内保存释放的ID
     分析: 当释放后,再次使用低24位表示的位置, 则高8位增加1后, 造成ID变化, 不再会与原来相同
     极端情况,快速分配释放256次后,此时的ID会有概率一样, 但此应用情况理论上不存在
    //---------------------------------------------------------------------*/
	class EventCoreDll_Export ResponseEventFactory : public CEventFactory
	{
	public:
		virtual AutoEvent NewEvent()
		{
			AutoEvent evt = MEM_NEW ResponseEvent();
			//evt->setLog(true);
			return evt;
		}

		virtual void AllotEventID(tServerEvent *serverEvent);

		virtual void FreeServerEvent( tServerEvent *serverEvent );

		AutoEvent FindServerEvent(EVENT_ID id);

	public:
		ResponseEventFactory()
		{
			mEventList.push_back(NULL);
            SetState(eFactoryNoProtocolSave, true);
		}

	protected:
		Array<tServerEvent*>		mEventList;
		EasyStack<UINT>				mIDStack;
	};

}
#endif