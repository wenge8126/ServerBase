using System.Collections.Generic;
using UnityEngine;

namespace Logic
{
    public class Actor : IActor
    {
        public UnitID mID = new UnitID();
        public ActorFactory mFactory = null;

        public Dictionary<string, tComponent> mComponentList = new Dictionary<string, tComponent>();

        public UnitID GetID()
        {
            return mID;
        }

        public ActorManager GetMgr()
        {
            return mFactory.mManager;
        }

        public tComponent AddComponent(string componentName)
        {
            tComponent comp = EventCenter.Instance.StartEvent(componentName) as tComponent;
            if (comp != null)
            {
                comp.mActor = this;
                mComponentList[componentName] = comp;
                comp.Awake();
                EventCenter.WaitAction(comp.Start, 0);
            }
            return null;
        }

        public tComponent RemoveComponent(string componentName)
        {
            tComponent comp;
            if (mComponentList.TryGetValue(componentName, out comp))
            {
                RemoveComponent(comp);
                return comp;
            }
            return null;
        }

        public bool RemoveComponent(tComponent tComponent)
        {
            ComponentWaitRemoveEvent evt =
                EventCenter.Instance.StartEvent("ComponentWaitRemoveEvent") as ComponentWaitRemoveEvent;
            evt.MActorTComponent = tComponent;
            evt.WaitTime(0);
            return false;
        }
            
        public virtual void Awake()
        {
            
        }

        public virtual void Start()
        {
            
        }

        public void Process()
        {
            // 具体需要快速Process的组件, 直接使用StartUpdate()
            // foreach (var v in mComponentList.Values)
            // {
            //     v.Process();
            // }
        }

        public void LowProcess()
        {
            foreach (var v in mComponentList.Values)
            {
                v.LowProcess();
            }
        }

        public virtual void OnDestory()
        {
            
        }
        
        public void Destory()
        {
            GetMgr().DestoryActor(this);
        }

        public void Log(string info)
        {
            LOG.log(info);    
        }
    }
    
    
}