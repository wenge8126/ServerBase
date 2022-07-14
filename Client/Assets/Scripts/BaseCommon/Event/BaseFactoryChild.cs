
using System;
using System.Collections.Generic;


namespace Logic
{
    public class DefineFactory<T> : BaseFactory where T : tEvent, new()
    {
        public DefineFactory()
            : base() { }

        public override tEvent NewEvent() { return new T(); }

        public override bool NeedLog()
        {
            return true;
        }
    }

    class DefineFactoryLog<T> : BaseFactory where T : tEvent, new()
    {
        public DefineFactoryLog()
            : base() { }

        public override tEvent NewEvent() { return new T(); }

        public override bool NeedLog()
        {
            return true;
        }
    }


    // 具有缓存事件功能工厂
    //public class DefineEventPoolFactory<T> : BaseFactory where T : tEvent, new()
    //{
    //    Stack<tEvent> mFreeList = new Stack<tEvent>();

    //    public DefineEventPoolFactory()
    //        : base() { }

    //    public override tEvent NewEvent()
    //    {
    //        if (mFreeList.Count > 0)
    //        {
    //            tEvent evt = mFreeList.Pop();
    //            evt.SetFinished(false);
               
    //            return evt;
    //        }

    //        return new T();
    //    }

    //    public override void FreeEvent(tEvent evt)
    //    {
    //        if (evt.mbFree)
    //            return;
    //        evt.InitData();
    //        if (evt.getData() != null)
    //            evt.getData().clear();
           
    //        evt.SetFinished(true);
    //        mFreeList.Push(evt);
    //    }
    //}

    class DefaultEventFactory : BaseFactory
    {
        public override tEvent NewEvent() { return new BaseEvent(); }
    }

    // 空的工厂，临时保存ID使用
    public class EmptyEventFactory : BaseFactory
    {
        public EmptyEventFactory()
        {
            mName = "Empty";
        }

        public override tEvent NewEvent()
        {
            LOG.logError("使用了空的工厂 >" + GetNameIndex().ToString());
            return null;
        }
    }

  


}
