using System.Collections;
using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Reflection;


namespace Logic
{
    public class BaseEvent : tEvent
    {
        protected tEventFactory mFactory;
        protected bool mFinish = false;

        public tNiceData mData = null;

#if DEVELOP_MODE
        public      uint           mCode;      //for debug
        static protected uint      sNum = 0;
#endif

        public override void setData(tNiceData data) { mData = data; }
        public override tNiceData getData() { return mData; }

        public override void set(string key, object value) { if (mData!=null) mData.set(key, value); }
        public override object getObject(string key) { if (mData!=null) return mData.getObject(key); return null; }

        //-----------------------------------
        public override void Do()
        {            
        }
        public override void OnBegin() {  }
        public override void OnFinish() {  }

        public override void Update(float secondTime)
        {
            
        }

        public BaseEvent()
        {
#if DEVELOP_MODE
            mCode = ++sNum;
#endif
        }
        //----------------------------------- 

        public override void WaitTime(float waitTimeSecond)
        {
            GetEventCenter().OnEventWaitTime(this, waitTimeSecond);            
        }

        public override void StartUpdate()
        {
            GetEventCenter().StartUpdate(this);
        }

        public override void StopWaitTime()
        {
            GetEventCenter().StopWaitTimeEvent(this);
        }

        public override void StopUpdate()
        {
            GetEventCenter().StopUpdateEvent(this);
        }

        public override void SetEventFactory(tEventFactory fact)
        {
            mFactory = fact;
        }

        public override tEventFactory GetEventFactory()
        {
            return mFactory;
        }

        public override tEventCenter GetEventCenter()
        {
            if (mFactory != null)
            {
                return mFactory.GetEventCenter();
            }
            else
            {
                return EventCenter.Instance;
            } 
        }

        //-----------------------------------
        public override string GetEventName()
        {
            return mFactory.GetEventName();
        }

        public override string GetEventInfo()
        {
            string info = GetEventName();
#if DEVELOP_MODE
            info += "_";
            info += mCode;
#endif
            return info;
        }

        public override void Start()
        {	          
            try
            {
                OnBegin();
                Do();
            }
            catch (Exception e)
            {
                LOG.logError(GetEventInfo() + " >Error: do event fail >>> " + e.ToString()); 
            }
            
        }

        public override void Start(object obj) { Start(); }

        public override void Finish()
        {
            if (!mFinish)
            {
                //Log("event finish");
                mFinish = true;

                if (EventCenter.Instance.mTimeEventManager != null)
                    EventCenter.Instance.mTimeEventManager.RemoveEvent(this);
                
                OnFinish();
            }
        }

        public override void DoOverTime()
        {
            /// Log("time over ...");
            OnTimeOver();
        }

        public override void SetFinished(bool bFinish)
        {
            mFinish = bFinish;
        }

        public override bool GetFinished()
        {
            return mFinish ;
        }

        public override tEvent StartEvent(string evtName)
        {
            return EventCenter.Instance.StartEvent(evtName, true);
        }

        public override void Log(string info)
        {
            LOG.log("[" + GetEventInfo() + "] " + info);
        }

        public override void Dump()
        {
#if DEVELOP_MODE
            if (CommonParam.LogToFile)
            {
                Log("event data ...");
                getData().dump(GetEventInfo());
            }
#endif
        }

        public override void _dump()
        {
            
        }
        //------------------------------------


    }
}
