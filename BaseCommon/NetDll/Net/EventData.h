#ifndef _INCLUDE_EVENTDATA_H_
#define _INCLUDE_EVENTDATA_H_
#include "CEvent.h"



class tNetEvent : public Logic::BaseEvent
{
	friend class tNetEventFactory;
public:
	virtual void SetData(AutoNice &scrData)
	{ 
		//AssertNote(0, "tNetEvent can not use SetData"); 
		GetData().clear(); GetData().append(*scrData, true);
	}
	virtual tNiceData& GetData(void) 
	{ 
		return mData;
	}
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

protected:
	virtual void __setFactory(Logic::AutoEventFactory hFactory)
	{
		BaseEvent::SetFactory(hFactory);
	}

protected:
	NiceData		mData;
};

class tNetEventFactory : public Logic::CEventFactory
{
public:
	virtual AutoEvent StartEvent( void ) override;

};

template<typename Evt>
class NetEventFactory : public tNetEventFactory
{
public:
	virtual AutoEvent NewEvent() override
	{
		MEM_NEW Evt();
	}
};

#endif //_INCLUDE_EVENTDATA_H_