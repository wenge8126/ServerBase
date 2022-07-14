

namespace Logic
{
    public abstract class tEventCenter
    {
        public abstract tEvent StartEvent(string eventName, bool bWarn);       

        public abstract tEvent StartEvent(string eventName);
        public abstract void RegisterEvent(string eventName, tEventFactory fact);
        public abstract bool RemoveRegisterEvent(string eventName);        
        public abstract tEventFactory GetEventFactory(string eventName);

        public abstract bool ExistEventFactory(string eventName);

        public abstract void Process();

        protected abstract void Log(string info);

        public abstract tEvent StartDefaultEvent(string eventName);

        public abstract void OnEventWaitTime(tEvent evt, float waitTime);
        public abstract void StartUpdate(tEvent evt);

        public abstract void StopWaitTimeEvent(tEvent evt);
        public abstract void StopUpdateEvent(tEvent evt);


    }



}
