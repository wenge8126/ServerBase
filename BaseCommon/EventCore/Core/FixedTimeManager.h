/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   21:11
	filename: 	e:\Work\Common\EventCore\LogicSystem\TimeEvent.h
	file path:	e:\Work\Common\EventCore\LogicSystem
	file base:	TimeEvent
	file ext:	h
	author:		Wenge Yang
	
	purpose:	
*********************************************************************/
#ifndef _INCLUDE_FIXEDTIMEMANGER_H_
#define _INCLUDE_FIXEDTIMEMANGER_H_

#include "EventCore.h"
#include "Event.h"
#include "EventCallBack.h"

//-------------------------------------------------------------------------
#	define SECORDS_TIME_MODE	0
#	define TIME_UNIT			1000				// ʱ�侫�� 100����
#	define TIME_UNIT_SECOND		0.001f


#define UPDATE_USE_SPACE_TIME	0   // Ϊ0ʱ,ִ��һ��Update(), �����������һ��UpdateLiat

#if __LINUX__
#   include <sys/time.h>
    typedef time_t					TIME_VAL;
#else
    typedef UInt64					TIME_VAL;
#endif
//-------------------------------------------------------------------------
namespace Logic
{
	// ��ʱ������
	class EventCoreDll_Export FixedTimeManager
	{
		friend class ResponseEvent;
		//----------------------------------------------------------------------
		struct WaitEvent
		{
#if SECORDS_TIME_MODE
			Int64				mOverTime;
#else
			UInt64              mOverTime;			
#endif
			AutoEvent			mWaitEvent;

			WaitEvent(UInt64 overTime, AutoEvent waitEvent)
				: mOverTime(overTime)
				, mWaitEvent(waitEvent)
			{

			}

			WaitEvent()
				: mOverTime(0)
			{

			}
		};


		//---------------------------------------------------------------------
#if UPDATE_USE_SPACE_TIME
		struct UpdateEvent
		{
			unsigned short	mSpaceMilSecond;  // ���� 65.535��
			unsigned short	mTotalMilSecond;
			AutoEvent		mUpdateEvent;

			UpdateEvent()
				: mTotalMilSecond(0)
				, mSpaceMilSecond(0)
			{

			}

			bool operator == (const UpdateEvent &other)
			{
				return mUpdateEvent==other.mUpdateEvent;
			}
		};
#else
        struct UpdateEvent
        {
            int	mSpaceMilSecond;  
            unsigned int mAlreayCount;
            UInt64	mWaitSecond;
            UInt64  mStartTime;

            AutoEvent		mUpdateEvent;

            UpdateEvent()
                : mAlreayCount(0)
                , mSpaceMilSecond(0)
                , mWaitSecond(0)
                , mStartTime(0)
            {

            }

            bool operator == (const UpdateEvent &other)
            {
                return mUpdateEvent==other.mUpdateEvent;
            }
        };
#endif
		//-------------------------------------------------------------------------
	public:
		// ���ص�ǰ��� 1970.1.1������ * TIME_UNIT (����)
		virtual TIME_VAL Now();

	public:
		virtual void Process();

		virtual void SetUpdateMinIntervalTime(int minIntervalTime){ mMinIntervalTime = minIntervalTime; }

	public:
		FixedTimeManager();

		void ClearAllEvent();

		void ReaydyListNodePool( AutoPtr<NodePool> hPool);

		void _Dump();

	public:
		virtual void _OnWaitEvent(AutoEvent hEvent, float waitTimeSecond);
		virtual void _OnStartUpdateEvent(AutoEvent hEvent, float spaceTime);		

		virtual void RemoveEvent(AutoEvent hEvent);

		virtual void RemoveWaitEvent(AutoEvent hEvent);
		virtual void RemoveUpdateEvent(AutoEvent hEvent);

		virtual void PauseEvent(AutoEvent hEvent);
		virtual void ContinueEvent(AutoEvent hEvent);

    protected:
#if !UPDATE_USE_SPACE_TIME
        void _AppendUpdate(UpdateEvent &updateEvt);
#endif

	protected:
		TIME_VAL						mCurrentTime;

		// һ��ѭ����̼��ʱ�����, ��ʼΪ 20���룬 ��ÿ��50��
		int								mMinIntervalTime;

		typedef PoolList<UpdateEvent>	UpdateEventList;

		UpdateEventList					mWillUpdateEventList;
		UpdateEventList					mUpdateEventList;		


		typedef PoolList<WaitEvent>		WaitEventList;
		WaitEventList					mWaitEventList;
		WaitEventList					mWillWaitTimeEventList;
		WaitEventList					mTempWaitList;

	};

	//-------------------------------------------------------------------------
}


#endif //_INCLUDE_FIXEDTIMEMANGER_H_