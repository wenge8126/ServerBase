
using System;


namespace Logic
{
    public abstract class tEventFactory
    {
        public int mID = 0;

        public virtual void Init() { }
        public abstract tEvent NewEvent();

        public abstract void SetEventName(string name);
        public abstract string GetEventName();

        public abstract void SetEventCenter(tEventCenter center);
        public abstract tEventCenter GetEventCenter();

        public virtual bool NeedLog() { return false; }

        public virtual void Destroy() { }

        public abstract int GetNameIndex();

        public virtual void FreeEvent(tEvent evt) { }



    }



}
