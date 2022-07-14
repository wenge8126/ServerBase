
using System;


namespace Logic
{
    public abstract class tEvent
    {
        public ushort mWaitTimeFlag = 0;
        public ushort mUpdateFlag = 0;
        //-----------------------------------

        public abstract void Do();
        public abstract void OnBegin();
        public abstract void OnFinish();
        public virtual void OnTimeOver() { }
       
        //-----------------------------------
     
        public abstract void WaitTime(float waitTimeSecond);
        public abstract void StartUpdate();
        public abstract void StopWaitTime();
        public abstract void StopUpdate();
        

        public abstract void SetEventFactory(tEventFactory fact);
        public abstract tEventFactory GetEventFactory();
        public abstract tEventCenter GetEventCenter();
        //-----------------------------------

        public abstract string GetEventName();
        public abstract string GetEventInfo();

        public abstract void Start();

        public abstract void Start(object obj);
        
        public abstract void Finish();
        public abstract void DoOverTime();
        public abstract void SetFinished(bool bFinish);
        public abstract bool GetFinished();
        public abstract void Update(float secondTime);

        public abstract void Log(string info);
        public abstract void Dump();
        public abstract void _dump();
        //-----------------------------------

        public abstract tEvent StartEvent(string evtName);

        public virtual void InitData() { SetFinished(false); StopUpdate(); StopWaitTime(); }

        public virtual void setData(tNiceData data) { }
        public virtual tNiceData getData() { return null; }

        public virtual void set(string key, object value) { }
        public virtual object getObject(string key) { return null; }
    }
}
