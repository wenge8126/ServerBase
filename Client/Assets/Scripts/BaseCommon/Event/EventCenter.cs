using System;
using System.Collections.Generic;
using UnityEngine;


namespace Logic
{
    public class EventCenter : tEventCenter
    {
        static public EventCenter Instance = null;

        public TimeEventManager mTimeEventManager = new GameTimeManager();

        public Dictionary<string, tEventFactory> mFactoryMap = new Dictionary<string, tEventFactory>();

        // 用于监听事件
        public Dictionary<string, List<tEvent>> mListenEventList = new Dictionary<string, List<tEvent>>();

        uint mNowSecond;
        ushort mTimePeriod = 0;

        ushort mCode = 0;

        //-------------------------------------------------------------------------------------------
        public EventCenter()
        {

            RegisterEvent("TM_WaitToRunAction", new DefineFactory<TM_WaitToRunAction>());
            RegisterEvent("TM_WaitUpdateAction", new DefineFactory<TM_WaitUpdateAction>());

            mTimeEventManager.Init();
        }

        static public void StaticRegister(string eventName, tEventFactory factory)
        {
            Instance.RegisterEvent(eventName, factory);
        }

        public void RemoveAllEvent()
        {
            mTimeEventManager.RemoveAll();

        }

        public void InitClearAllEvent()
        {
            if (mTimeEventManager != null)
                mTimeEventManager.RemoveAll();
        }

        public void ClearFactory(Type factoryType)
        {
            List<string> removeList = new List<string>();
            foreach (KeyValuePair<string, tEventFactory> f in mFactoryMap)
            {
                if (f.Value.GetType() == factoryType)
                {
                    removeList.Add(f.Key);
                    f.Value.Destroy();
                }
            }

            foreach (string key in removeList)
            {
                mFactoryMap.Remove(key);
            }
        }

        public void InitSetTimeManager(TimeEventManager timeManager)
        {
            mTimeEventManager = timeManager;
            if (mTimeEventManager != null)
                mNowSecond = (uint) (mTimeEventManager.GetNowTime() * 1000);
        }

        static public void LogError(string info)
        {
            Log(LOG_LEVEL.ERROR, info);
        }

        static public void Log(LOG_LEVEL level, string info)
        {

            if (Instance == null)
                return;

            switch (level)
            {
                case LOG_LEVEL.GENERAL:
                    Instance.Log(info);
                    break;

                case LOG_LEVEL.WARN:
                    Instance.Log("WARN: " + info);
                    break;

                case LOG_LEVEL.ERROR:
                    Instance.Log("ERROR:" + info);

                    break;

                case LOG_LEVEL.HIGH:
                    Instance.Log("HIGH: " + info);
                    break;

                default:
                    Instance.Log(info);
                    break;
            }
        }

        static public tEvent Start(string eventTypeName)
        {
            return Instance.StartEvent(eventTypeName);
        }

        static public tEvent WaitAction(object obj, string fun, object param, float waitSecond)
        {
            System.Reflection.MethodInfo info = obj.GetType().GetMethod(fun);
            if (info == null)
                return null;

            TM_WaitToRunAction waitEvt = Start("TM_WaitToRunAction") as TM_WaitToRunAction;
            if (waitEvt != null)
            {
                waitEvt.mObject = obj;
                waitEvt.mFunMethod = info;
                waitEvt.mParam = param;
                waitEvt.WaitTime(waitSecond);
                return waitEvt;
            }

            return null;
        }

        static public tEvent WaitAction(Action action, float waitSecond)
        {
            TM_WaitToRunAction waitEvt = Start("TM_WaitToRunAction") as TM_WaitToRunAction;
            if (waitEvt != null)
            {
                waitEvt.mPreferedAction = action;
                waitEvt.WaitTime(waitSecond);
                return waitEvt;
            }

            return null;
        }

        static public tEvent WaitUpdate(object obj, string fun, object param, float waitSecond)
        {
            System.Reflection.MethodInfo info = obj.GetType().GetMethod(fun);
            if (info == null)
            {
                LOG.logError("Not find fun >" + fun);
                return null;
            }

            TM_WaitUpdateAction waitEvt = Start("TM_WaitUpdateAction") as TM_WaitUpdateAction;
            if (waitEvt != null)
            {
                waitEvt.mSpaceTime = waitSecond;
                waitEvt.mObject = obj;
                waitEvt.mFunMethod = info;
                waitEvt.mParam = param;
                waitEvt.Start();
                return waitEvt;
            }

            return null;
        }

        public override tEvent StartEvent(string eventName)
        {
            return StartEvent(eventName, true);
        }

        public override tEvent StartEvent(string eventName, bool bWarn)
        {
            tEventFactory fact = GetEventFactory(eventName);
            if (fact == null)
            {
                if (bWarn)
                {
                    string info = "Warning: no exist [";
                    info += eventName;
                    info += "] event factory.";
                    LogError(info);
                }

                return null;
            }

            if (!(fact is EmptyEventFactory))
            {
                tEvent evt = fact.NewEvent();
                if (evt != null)
                {
                    evt.SetEventFactory(fact);
                }
                else
                    EventCenter.Log(LOG_LEVEL.ERROR, "Factory create null event");

                return evt;
            }

            return null;
        }

        public override void RegisterEvent(string eventName, tEventFactory fact)
        {
            fact.SetEventName(eventName);
            fact.SetEventCenter(this);

            int id = 0;

            tEventFactory f = GetEventFactory(eventName);
            if (f != null)
            {
                //if (f.GetEventName() == "Empty")
                //{
                //    int x = 2;
                //}
                id = f.mID;
                f.Destroy();
            }

            if (RemoveRegisterEvent(eventName))
                Log("Warn: event alread exist :" + eventName);
            fact.mID = id;
            mFactoryMap.Add(eventName, fact);
            fact.Init();

        }

        public override bool RemoveRegisterEvent(string eventName)
        {
            return mFactoryMap.Remove(eventName);
        }

        public override tEventFactory GetEventFactory(string eventName)
        {
            tEventFactory fact;
            mFactoryMap.TryGetValue(eventName, out fact);
            return fact;
        }

        public override bool ExistEventFactory(string eventName)
        {
            return mFactoryMap.ContainsKey(eventName);
        }

        //public Stack<tEvent> mFreeList = new Stack<tEvent>();

        public override void Process()
        {

            if (null != mTimeEventManager)
                mTimeEventManager.Update();

            //while (mFreeList.Count > 0)
            //{
            //    tEvent evt = mFreeList.Pop();
            //    if (!evt.mbFree)
            //        evt.GetEventFactory().FreeEvent(evt);
            //}
        }

        protected override void Log(string info)
        {
            LOG.log(info);
        }

        public override tEvent StartDefaultEvent(string eventName)
        {
            tEvent evt = StartEvent(eventName, false);
            if (null == evt)
            {
                RegisterEvent(eventName, new DefaultEventFactory());
                evt = StartEvent(eventName, false);
                if (null == evt)
                    throw new Exception(
                        "Error: already register default event, but can not create event >>" + eventName);
            }

            return evt;
        }

        public override void OnEventWaitTime(tEvent evt, float waitTime)
        {
            mTimeEventManager.OnWaitEvent(evt, waitTime);
        }

        public override void StartUpdate(tEvent evt)
        {
            mTimeEventManager.StartUpdate(evt);
        }

        public override void StopWaitTimeEvent(tEvent evt)
        {
            if (mTimeEventManager != null)
                mTimeEventManager.RemoveWaitEvent(evt);
        }

        public override void StopUpdateEvent(tEvent evt)
        {
            if (mTimeEventManager != null)
                mTimeEventManager.RemoveUpdateEvent(evt);
        }

        public UInt64 AlloctEventID()
        {
            uint nowTime = (uint) (mTimeEventManager.GetNowTime() * 1000);
            if (nowTime < mNowSecond)
                ++mTimePeriod;

            mNowSecond = nowTime;

            ++mCode;

            UInt64 key = mNowSecond;
            uint m = mTimePeriod;
            key = (mNowSecond << 32) + (m << 16) + mCode;
            return key;
        }

        /// <summary>
        /// 添加监听事件
        /// </summary>
        /// <param name="evt"></param>
        public void AppendListen(tEvent evt)
        {
            string evtName = evt.GetEventName();
            List<tEvent> evtList = null;
            if (!mListenEventList.TryGetValue(evtName, out evtList))
            {
                evtList = new List<tEvent>();
                mListenEventList.Add(evtName, evtList);
            }

            evtList.Add(evt);
        }

        public tEvent AppendListen(string evtName)
        {
            var evt = StartEvent(evtName);
            if (evt != null)
                AppendListen(evt);
            return evt;
        }

        /// <summary>
        /// 移除监听事件
        /// </summary>
        /// <param name="evtName"></param>
        public void RemoveListen(string evtName)
        {
            mListenEventList.Remove(evtName);
        }
        
        /// <summary>
        /// 投递事件
        /// </summary>
        /// <param name="evtName"></param>
        /// <param name="eventData"></param>
        /// <returns></returns>
        public bool DispatchEvent(string evtName, tNiceData eventData)
        {
            List<tEvent> evtList = null;
            if (!mListenEventList.TryGetValue(evtName, out evtList))
            {
                return false;
            }

            foreach (var evt in evtList)
            {
                evt.setData(eventData);
                evt.Do();
            }

            return true;
        }
    }
}
