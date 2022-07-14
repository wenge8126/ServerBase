/********************************************************************
	created:	2012/11/02
	created:	2:11:2012   3:20
	filename: 	C:\NewGame\SkyLive\Assets\Script\Base\TimeEvent.cs
	file path:	C:\NewGame\SkyLive\Assets\Script\Base
	file base:	TimeEvent
	file ext:	cs
	author:		Wenge Yang
	
	purpose:	
*********************************************************************/
//#define  LOG_WAIT_TIME_EVENT_LIST
#define USE_HEIGHT_TIME     // 使用高精度时间处理
using System.Collections.Generic;

using System;


class GameTimeManager : Logic.TimeEventManager
{
    public GameTimeManager()
    {
        mLastTime = (float)Environment.TickCount * 0.001f;
    }
    public override float GetNowTime()
    {
        return (float)Environment.TickCount*0.001f;
    }

    static public readonly DateTime BasicTime = new DateTime(1970, 1, 1);
    static public DateTime CheckServerTime(ulong seconds)
    {        
        return BasicTime.AddSeconds(seconds);
    }
}

namespace Logic
{
	public abstract class TimeEventManager
	{

        public delegate bool CheckRemoveFunction(tEvent evt);

        public struct WaitTimeEventData 
        {
            public tEvent mWaitEvent;
            public float mWaitTime;
            public ushort mFlag;

            public WaitTimeEventData(float waitTime, tEvent waitEvt)
            {
                mWaitEvent = waitEvt;
                mWaitTime = waitTime;
                mFlag = waitEvt.mWaitTimeFlag;
            }
        }

        public struct UpdateData
        {
            public tEvent mEvent;
            public ushort mFlag;

            public UpdateData(tEvent evt)
            {
                mEvent = evt;
                mFlag = evt.mUpdateFlag;
            }
        }


        public abstract float GetNowTime();

        //public List<tEvent> mRemoveWaitList = new List<tEvent>();
        //public List<tEvent> mRemoveUpdateList = new List<tEvent>();

        public LinkedList<WaitTimeEventData> mWaitEventList = new LinkedList<WaitTimeEventData>();
		public List<WaitTimeEventData> mWillWaitEventList = new List<WaitTimeEventData>();

        public List<UpdateData> mWillUpdateList = new List<UpdateData>();
        public LinkedList<UpdateData> mUpdateList = new LinkedList<UpdateData>();

        protected float mUpdateTime = 0;

        protected float mLastTime = 0;

        public void Init() { mLastTime = GetNowTime(); }

        public void RemoveEvent(CheckRemoveFunction checkNeedRemove)
        {
            foreach (WaitTimeEventData wait in mWaitEventList)
            {
                if (checkNeedRemove(wait.mWaitEvent))
                    RemoveWaitEvent(wait.mWaitEvent);
            }

            for (int i = 0; i < mWillWaitEventList.Count;)
            {
                WaitTimeEventData d = mWillWaitEventList[i];
                if ( checkNeedRemove(d.mWaitEvent) )
                    mWillWaitEventList.RemoveAt(i);
                else
                    ++i;
             }

            foreach(UpdateData evt in mUpdateList)
            {
                if (checkNeedRemove(evt.mEvent))
                    RemoveUpdateEvent(evt.mEvent);
            }

            for (int i = 0; i < mWillUpdateList.Count; )
            {
                if (checkNeedRemove(mWillUpdateList[i].mEvent))
                    mWillUpdateList.RemoveAt(i);
                else
                    ++i;
            }            
        }

        public void  RemoveAll()
        {
            mWillWaitEventList.Clear();
            mWillUpdateList.Clear();

            foreach (var data in mWaitEventList)
            {
                data.mWaitEvent.mWaitTimeFlag++;
            }

            foreach (var data in mUpdateList)
            {
                data.mEvent.mUpdateFlag++;
            }            
        }

        public void RemoveEvent(tEvent removeEvt)
        {
            RemoveWaitEvent(removeEvt);
            RemoveUpdateEvent(removeEvt);
        }

        public void RemoveWaitEvent(tEvent removeEvt)
        {
            ++removeEvt.mWaitTimeFlag;
            //mRemoveWaitList.Add(removeEvt);
            //for (int i = 0; i < mWillWaitEventList.Count; ++i)
            //{
            //    WaitTimeEventData d = mWillWaitEventList[i];
            //    if (d.mWaitEvent == removeEvt)
            //    {
            //        mWillWaitEventList.RemoveAt(i);
            //        break;
            //    }
            //}
        }

        public void RemoveUpdateEvent(tEvent removeEvt)
        {
            ++removeEvt.mUpdateFlag;
            //mRemoveUpdateList.Add(removeEvt);
            //mWillUpdateList.Remove(removeEvt);
        }

        public void OnWaitEvent(tEvent waitEvent, float timeSecond)
        {
            //for (int i=0; i<mRemoveWaitList.Count; ++i)
            //{
            //    if (mRemoveWaitList[i] == waitEvent)
            //        mRemoveWaitList.RemoveAt(i);
            //    else
            //        ++i;
            //}
            float overTime = GetNowTime()+timeSecond;
            ++waitEvent.mWaitTimeFlag;

			mWillWaitEventList.Add(new WaitTimeEventData(overTime, waitEvent));
			
		}

        public void _AppendWillWaitList()
        {
            for (int n = 0; n < mWillWaitEventList.Count; ++n)
            {
                WaitTimeEventData waitData = mWillWaitEventList[n];                

                // Note: Check event is exist when insert before, because wait time will change, so list is not sqrt state. change bad.
                // so must delete already exist this event.
                //for (int i = 0; i < mWaitEventList.Count; ++i)
                //{
                //    if (mWaitEventList[i].mWaitEvent == waitData.mWaitEvent)
                //    {
                //       // waitData.mWaitEvent.Log(" !!!!!! remove because already exist");
                //        mWaitEventList.RemoveAt(i);
                //        break;
                //    }
                //}

                bool bInsert = false;
                for (LinkedListNode<WaitTimeEventData> node = mWaitEventList.First; node != null; node = node.Next )
                {
                    tEvent evt = node.Value.mWaitEvent;

                    if (node.Value.mWaitTime > waitData.mWaitTime)
                    {
                        // waitData.mWaitEvent.Log("+++++++ Insert to " + nIndex.ToString());
                        mWaitEventList.AddBefore(node, waitData);
                        bInsert = true;
                        break;                        
                    }
                }
                if (!bInsert)
                {
                   // waitData.mWaitEvent.Log("******* Insert to " + mWaitEventList.Count.ToString());
                    mWaitEventList.AddLast(waitData);
                }
#if LOG_WAIT_TIME_EVENT_LIST
                waitData.mWaitEvent.Log("---------------------------- wait time evnt list ------------------------------------");
                for (int i = 0; i < mWaitEventList.Count; ++i)
                {
                    mWaitEventList[i].mWaitEvent.Log("[" + i.ToString() + "] Wait >>>" + mWaitEventList[i].mWaitTime.ToString() + ", now = "+GetNowTime().ToString());
                }
                waitData.mWaitEvent.Log("---------------------------- end list ------------------------------------");
#endif
            }
            mWillWaitEventList.Clear();
        }

        public void StartUpdate(tEvent needUpdateEvent)
        {
            //for (int i = 0; i < mRemoveUpdateList.Count; ++i)
            //{
            //    if (mRemoveUpdateList[i] == needUpdateEvent)
            //        mRemoveUpdateList.RemoveAt(i);
            //    else
            //        ++i;
            //}
            ++needUpdateEvent.mUpdateFlag;
            mWillUpdateList.Add(new UpdateData(needUpdateEvent));
            //mWillUpdateList.Insert(mUpdateList.Count, needUpdateEvent);
        }

        virtual public void Update()
        {
            float onceTime = GetNowTime() - mLastTime;
            mLastTime = GetNowTime();
            //if (mRemoveWaitList.Count > 0)
            //{
            //    foreach (tEvent evt in mRemoveWaitList)
            //    {
            //        for(int i=0; i<mWaitEventList.Count; ++i)
            //        {
            //            WaitTimeEventData d = mWaitEventList[i];
            //            if (d.mWaitEvent==evt)
            //            {
            //                mWaitEventList.RemoveAt(i);
            //                break;
            //            }
            //        }
            //    }
            //    mRemoveWaitList.Clear();
            //}

            //if (mRemoveUpdateList.Count > 0)
            //{
            //    foreach (tEvent evt in mRemoveUpdateList)
            //    {                   
            //        mUpdateList.Remove(evt);
            //    }
            //    mRemoveUpdateList.Clear();
            //}

#if !USE_HEIGHT_TIME
            bool bLowLoop = false;
            mUpdateTime += onceTime;
            if (mUpdateTime >= 0.1f)    // Once loop by 0.1 second
            {
                mUpdateTime -= 0.1f;
                bLowLoop = true;
#endif
                if (mWillWaitEventList.Count > 0)
                    _AppendWillWaitList();

                float nowTime = GetNowTime();
                for (LinkedListNode<WaitTimeEventData> node = mWaitEventList.First; node != null; )
                {
                    tEvent evt = node.Value.mWaitEvent;
                    if (evt.GetFinished() || node.Value.mFlag != evt.mWaitTimeFlag)
                    {
                        ///evt.Log(" ------- remove from wait time list, because already finish.");
                        LinkedListNode<WaitTimeEventData> t = node;
                        node = node.Next;
                        mWaitEventList.Remove(t);
                    }
                    else
                    {
                        float overTime = node.Value.mWaitTime;

                        if (overTime <= nowTime)
                        {
                            // Note : must remove before Do over time, because may be repeat wait time in over time process.
                            // then will remove this event, so after romove event is other event.
                            // So, use will wait time list to temp save append event.
                            LinkedListNode<WaitTimeEventData> t = node;
                            node = node.Next;
                            mWaitEventList.Remove(t);

                            //if (evt.GetFinished())
                            //    UnityEngine.Debug.LogError(evt.GetEventName() + " > when evt over time is finish");
                            //evt.Log(" ======= Run over time ... >>>>>>>>>>>>>>>>>>>>>>>>>>");
                            evt.DoOverTime();
                        }
                        else
                        {
                            // this will exit loop.
                            break;
                        }
                    }

                }
#if !USE_HEIGHT_TIME
            }
#endif

            if (mWillUpdateList.Count>0)
            {
                foreach (UpdateData evt in mWillUpdateList)
                {
                    //mUpdateList.Remove(evt);
                    //if (!mUpdateList.Contains(evt))
                    mUpdateList.AddLast(evt);
                }
                mWillUpdateList.Clear();
            }

            if (mUpdateList.Count>0)
            {
                for (LinkedListNode<UpdateData> node=mUpdateList.First; node!=null; )
                {
                    UpdateData d = node.Value;
                    
                    tEvent evt = d.mEvent;
                    if (evt.GetFinished() || d.mFlag != evt.mUpdateFlag)
                    {
                        LinkedListNode<UpdateData> t = node;
                        node = node.Next;
                        mUpdateList.Remove(t);
                        continue;
                    }
#if !USE_HEIGHT_TIME
                    else if (evt.needLowUpdate())
                    {
                        if (bLowLoop)
                        {
                            if (!evt.Update(0.1f))
                            {
                                mUpdateList.RemoveAt(i);
                                continue;
                            }
                        }   
                    }
#endif
                    else
                        evt.Update(onceTime);
                   
                    node = node.Next;
                }
            }

        }       
	}
}
