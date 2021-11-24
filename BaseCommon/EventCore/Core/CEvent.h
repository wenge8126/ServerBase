//-------------------------------------------------------------------
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Untitled
//  @ File Name : tEvent.h
//  @ Date : 2011-5-18
//  @ Author : 杨文鸽
// 事件驱动系统
//-------------------------------------------------------------------


#if !defined(_CEVENT_H)
#define _CEVENT_H

#include "Event.h"
#include "BaseTable.h"
#include "EventFactory.h"
#include "NetHandle.h"
//-------------------------------------------------------------------------
#define _D				mData
#define _CHECK(a, b)	{ AssertEx(!get(a).empty(), b); }
//-------------------------------------------------------------------------


namespace LuaPlus
{
	class LuaObject;
}

class tNetHandle;
class tNetConnect;

namespace Logic
{
	class tEventFactory;
	class tEventCenter;

	class EventCoreDll_Export BaseEvent : public tEvent
	{		
		friend class tEventFactory;

	public:
		virtual const char* GetEventName(void) const;
		virtual int GetNameIndex(void);

	public:
		BaseEvent();
		virtual ~BaseEvent();

	protected:
		virtual bool _Serialize(DataStream *destData);
		virtual bool _Restore(DataStream *scrData);		
		virtual bool _SaveDataType(DataStream *destData){ return destData->write((byte)GetData().getType()); }
		virtual NICEDATA_TYPE _RestoreDataType(DataStream *scrData);

	public:
		virtual bool Begin(){ _OnBegin(); return true; }
		virtual void Wait();
		virtual void Update(float onceTime) override { StopUpdate(); }
		virtual bool WaitTime(float waitTimeSecond) override;
		
		virtual bool StartUpdate(float spaceSecond);
		virtual AutoEvent StartEvent(const char* eventName);
		virtual AutoEvent StartEvent(const char* eventName, bool bDefault);
		virtual void SetFactory(AutoEventFactory hFactory);
		virtual AutoEventFactory GetEventFactory(void) const { return mEventFactory; }
		virtual tEventCenter* GetEventCenter(void) const;

		virtual bool Send(int nType = 0, int nTarget = 0 );
		virtual void _OnBindNet(tNetConnect *netTool){}

		virtual void setFinished(bool bFinish);
		virtual bool getFinished(void) const;

		virtual void OnForceFreeBefore() override { DEBUG_LOG("%s force free", GetEventName());  }
		virtual void Free();
		virtual bool needFree(void) const { return mState.isOpen((StateDataType)STATE_EVENT_NEED_DESTORY); }

		virtual void SetAutoCompress(bool bUse) { mState.set(STATE_EVENT_NOAUTO_COMPRESS, !bUse); }
		virtual bool NeedAutoCompress(void) { return !mState.isOpen(STATE_EVENT_NOAUTO_COMPRESS); }
		
		virtual void setState(int state, bool bOpen) { mState.set((StateDataType)state, bOpen); }
		virtual bool hasState(int state) const { return mState.isOpen( (StateDataType)state ); }

	public:
		virtual bool _DoEvent();
		virtual void _Process(void*){}
		virtual bool _OnTimeOver(void);
		virtual bool _OnEvent(AutoEvent hEvent) { return true; }
		virtual void _OnBegin() {}
		virtual void _OnFinish() {}
		virtual bool _AutoFinish() { return true; }
		virtual void _NodifyEventFinish();

		void initData(void);

	protected:
		virtual  void InitData(void) {}

	public:
		virtual size_t AppendData(const tNiceData &scrData, bool bReplace) override;

		virtual void set(const char* szKey, int nVal){ GetData().set(szKey, nVal); }
		virtual void set(const char* szKey, byte nVal){ GetData().set(szKey, nVal); }
		virtual void set(const char* szKey, float fVal){ GetData().set(szKey, fVal); }
		virtual void set(const char* szKey, const char* szVal) { GetData().set(szKey, szVal); }
		virtual void set(const char* szKey, const AString &strVal) { GetData().set(szKey, strVal); }
		virtual void set(const char* szKey, bool bVal) { GetData().set(szKey, bVal); }
		virtual void set(const char* szKey, UInt64 uVal);

		virtual bool set(const char* szKey, AutoNice &nice);
		virtual bool set(const char* szKey, AutoEvent &hEvent);
		virtual bool set(const char* szKey, AutoData &hData);
		virtual bool set(const char* szKey, ARecord &hRecord);
		virtual bool set(const char* szKey, AutoTable &hTable);
		virtual bool set(const char* szKey, void *obj, const type_info &typeInfo);

		virtual Data get(const char* szKey, const char *throwInfo = NULL) const;
		virtual Data get(int nKey) const { return GetData()._getData(nKey); }
		virtual bool get(const char* szKey, AutoNice &nice);
		virtual bool get(const char* szKey, AutoEvent &hEvent);
		virtual bool get(const char* szKey, AutoData &hData);
		virtual bool get(const char* szKey, ARecord &hRecord);
		virtual bool get(const char* szKey, AutoTable &hTable);
		virtual bool get(const char* szKey, void *obj, const type_info &typeInfo);

	public:
		//virtual void* _GetScriptObject( void );
		//virtual void _FreeScriptObject(void);
		virtual void _OnEventFinishNodify(AutoEvent event);
		virtual AutoEvent NextAction(){ return AutoEvent(); };

		virtual void NotifyAction(void *pAction, const type_info &) {}

		virtual void setUseData(void *pData) override { mUseData = pData; }
		virtual void* getUseData() const override { return mUseData; }
		virtual void setUpdateInfo(void *pData) override { mUpdateInfo = pData; }
		virtual void* getUpdateInfo() const override { return mUpdateInfo; }
		
	protected:
		StateData			mState;
		void				*mUseData;		// 用于记录等待时间列表的节点指针，目的避免重复的等待时间
		void				*mUpdateInfo;	// 记录Update 节点的指针，用于避免重复Update

	protected:
		AutoEventFactory	mEventFactory;
	};

	//-------------------------------------------------------------------------*/
	class CEvent : public BaseEvent
	{
	public:
		virtual void SetData(AutoNice &scrData) { mData.clear(); mData.append(*scrData, true); }
		virtual tNiceData& GetData(void) { return mData; }
		virtual const tNiceData& GetData(void) const { return mData; }

		virtual NICEDATA_TYPE _RestoreDataType(DataStream *scrData) override
		{
			NICEDATA_TYPE niceType = BaseEvent::_RestoreDataType(scrData);
			if (niceType!=NICEDATA)
			{
				ERROR_LOG("[%s] 必须恢复 NiceData 类型的数据, 当前 [%d]", GetEventName(), niceType);
				return NULL_NICEDATA;
			}
			return niceType;
		}

	public:
		NiceData			mData;
	};
	typedef CEvent NiceEventEx;
	//-------------------------------------------------------------------------*/

}


#endif  //_TEVENT_H
