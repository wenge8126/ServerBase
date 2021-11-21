#ifndef _INCLUDE_EVENTDATA_H_
#define _INCLUDE_EVENTDATA_H_
#include "CEvent.h"

//using namespace Logic;

class EventData
{
public:
	//const char* getEventName() const { return mEventName.c_str(); }
	//void setEventName(const char *szName){ mEventName = szName; } 
	int getEventIndex() const { return mEventIndex; }
	void setEventIndex(int index){ mEventIndex = index; }

	int getTarget() const { return mTarget; }
	void _setTarget(int target){ mTarget = target; }

	void clear()
	{
		mEventIndex = 0;
		mTarget = 0;
		//mEventName.setNull();
		mData.clear(false);
	}
	
private:
	int mEventIndex;
	//AString mEventName;		// ˽�еĿ��Ա�֤��������,�����̼߳�ͬ������
	int mTarget;

public:
	UInt64	 mExtValue;				// ����serverEvent ʱ�� mID, ���ж���
	bool mbArray;
	NiceData	mData;
	ArrayNiceData mArrayData;
};
typedef AutoPtr<EventData>	AEventData;
//--------------------------------------------------------------
// ֧���߳�ͬ�����¼����ݹ����, ȫ��Ψһ
class EventDataPool
{
public:
	static void Push(EventData *d)
	{
		d->clear();
		mLock.lock();
		mPoolList.push(d);
		mLock.unlock();
	}

	static EventData* Pop()
	{
		mLock.lock();
		if (!mPoolList.empty())
		{
			EventData *p = mPoolList.pop();
			mLock.unlock();
			return p;
		}
		mLock.unlock();
		return new EventData();
	}

protected:
	static EasyStack<EventData*> mPoolList;
	static CsLock			mLock;
};

struct PacketData
{
public:
	byte mPacketID;
	byte mState;
	LoopDataStream mData;

	void clear()
	{
		mPacketID = 0;
		mState = 0;
		mData.clear(false);
	}

	PacketData() : mData(128){}
};

class PacketDataPool
{
public:
	static void Push(PacketData *d)
	{
		d->clear();
		mLock.lock();
		mPoolList.push(d);
		mLock.unlock();
	}

	static PacketData* Pop()
	{
		mLock.lock();
		if (!mPoolList.empty())
		{
			PacketData *p = mPoolList.pop();
			mLock.unlock();
			return p;
		}
		mLock.unlock();
		return new PacketData();
	}

protected:
	static EasyStack<PacketData*> mPoolList;
	static CsLock			mLock;
};

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
		if (mpEventData==NULL)
			mpEventData = EventDataPool::Pop();
		return mpEventData->mData; 
	}
	virtual const tNiceData& GetData(void) const { return mpEventData->mData; }

	virtual NICEDATA_TYPE _RestoreDataType(DataStream *scrData) override
	{
		NICEDATA_TYPE niceType = BaseEvent::_RestoreDataType(scrData);
		if (niceType!=NICEDATA)
		{
			ERROR_LOG("[%s] ����ָ� NiceData ���͵�����, ��ǰ [%d]", GetEventName(), niceType);
			return NULL_NICEDATA;
		}
		return niceType;
	}

	virtual void InitData() override
	{
		BaseEvent::InitData();

		EventDataPool::Push(mpEventData);
		mpEventData = NULL;
	}

	virtual bool _Serialize(DataStream *destData)
	{
		destData->write(mpEventData->mExtValue);
		return BaseEvent::_Serialize(destData);
	}
	virtual bool _Restore(DataStream *scrData)
	{
		if (!scrData->read(mpEventData->mExtValue) )
			return false;
		return BaseEvent::_Restore(scrData);
	}

public:
	// ����һ�㹤��ʹ��, ���յ������¼�������ô˺���
	virtual void SetFactory(Logic::AutoEventFactory hFactory) override
	{
		mpEventData = EventDataPool::Pop();
		BaseEvent::SetFactory(hFactory);
	}

protected:
	// ���ض���������, ʹ�ú�̨�������¼����� NetEventFactory���ж���ע��
	virtual void SetEventData(EventData *pData)
	{ 
		if (mpEventData!=NULL)
			EventDataPool::Push(mpEventData);
		mpEventData = pData;  
	}

	virtual void __setFactory(Logic::AutoEventFactory hFactory)
	{
		BaseEvent::SetFactory(hFactory);
	}

protected:
	EventData*		mpEventData;
};

class tNetEventFactory : public Logic::CEventFactory
{
public:
	virtual AutoEvent StartEvent( void ) override;

	virtual void SetEventData(EventData *p, tNetEvent *evt){ evt->SetEventData(p); }
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