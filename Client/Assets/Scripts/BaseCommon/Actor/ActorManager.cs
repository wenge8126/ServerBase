using System;
using System.Collections.Generic;

namespace Logic
{
    public class ActorManager
    {
        static public ActorManager Instance = null;
        
        private Dictionary<UInt64, Actor> mActorList = new Dictionary<ulong, Actor>();
        private Dictionary<int, ActorFactory> mActorFactoryList = new Dictionary<int, ActorFactory>();
        public Dictionary<string, tProcessServerRequest> mProcessRequestFunList = new Dictionary<string, tProcessServerRequest>();
        public Dictionary<string, tProcessServerNotify> mProcessNotifyFunList = new Dictionary<string, tProcessServerNotify>();

        public ActorManager()
        {
            Instance = this;
            EventCenter.Instance.RegisterEvent("ActorWaitStartEvent",  new DefineFactory<ActorWaitStartEvent>());
            EventCenter.Instance.RegisterEvent("ActorWaitDestoryEvent", new DefineFactory<ActorWaitDestoryEvent>());
            EventCenter.Instance.RegisterEvent("ComponentWaitRemoveEvent", new DefineFactory<ComponentWaitRemoveEvent>());
        }

        ~ActorManager()
        {
            Instance = null;
        }

        public void RegisterActor(ActorFactory factory)
        {
            if (mActorFactoryList.ContainsKey(factory.GetType()))
                LOG.logWarn("No exist : "+factory.GetType().ToString());
            factory.mManager = this;
            mActorFactoryList[factory.GetType()] = factory;
            var actor = factory.CreateActor();
            actor.RegisterMsg();
        }

        public void RegisterComponent<T>()
            where T : tComponent, new()
        {
            EventCenter.StaticRegister(typeof(T).Name, new DefineFactory<T>());
            var comp = new T();
            comp.RegisterMsg(this);
        }
        
        public Actor CreateActor(int type, Int64 id)
        {
            ActorFactory factory = null;
            if (mActorFactoryList.TryGetValue(type, out factory))
            {
                var actor = factory.CreateActor();
                actor.mID.id = id;
                if (mActorList.ContainsKey(actor.GetID()))
                    LOG.logWarn("Already exist actor : "+actor.GetID().dump());
                mActorList[actor.GetID()] = actor;
                actor.Awake();
                ActorWaitStartEvent evt = EventCenter.Instance.StartEvent("ActorWaitStartEvent") as ActorWaitStartEvent;
                evt.mActor = actor;
                evt.WaitTime(0);
                return actor;
            }
            LOG.logError("No exist actor factory : "+type.ToString());
            return null;
        }

        public Actor FindActor(int type, Int64 id)
        {
            return FindActor(new UnitID(type, id));
        }

        public Actor FindActor(UInt64 unitID)
        {
            Actor actor = null;
            if (mActorList.TryGetValue(unitID, out actor))
                return actor;
            return null;
        }

        public void DestoryActor(Actor actor)
        {
            ActorWaitDestoryEvent evt = EventCenter.Instance.StartEvent("ActorWaitDestoryEvent") as ActorWaitDestoryEvent;
            evt.mActor = actor;
            evt.WaitTime(0);
        }

        public bool RemoveActor(Actor actor)
        {
            return mActorList.Remove(actor.GetID());
        }

        public void Process()
        {
            foreach (var v in mActorList.Values)
            {
                v.Process();
            }
        }
        public void LowProcess()
        {
            foreach (var v in mActorList.Values)
            {
                v.LowProcess();
            }
        }
        
        public void RegisterMsg<ACTOR_COMP, REQUEST>(ComponentProcessServerRequest<ACTOR_COMP, REQUEST>.ProcessFunction fun)
            where REQUEST : BasePacket, new()
            where ACTOR_COMP : tComponent
        {
            var f = new ComponentProcessServerRequest<ACTOR_COMP, REQUEST>(fun);
            RegisterRequestMsg(f.GetRequestMsgName(), f);
        }
        public void RegisterRequestMsg(string msgName, tProcessServerRequest fun)
        {
            mProcessRequestFunList[msgName] = fun;
        }
        
        public tProcessServerRequest FindProcessRequestFunction(string msgName)
        {
            tProcessServerRequest fun = null;
            mProcessRequestFunList.TryGetValue(msgName, out fun);
            return fun;
        }
        
        
        // 通知消息处理
        public void RegisterMsg<ACTOR_COMP, NOTIFY_MSG>(ComponentProcessServerNotify<ACTOR_COMP, NOTIFY_MSG>.ProcessFunction fun)
            where NOTIFY_MSG : BasePacket, new()
            where ACTOR_COMP : tComponent
        {
            var f = new ComponentProcessServerNotify<ACTOR_COMP, NOTIFY_MSG>(fun);
            RegisterNotifyMsg(f.GetMsgName(), f);
        }
        public virtual tProcessServerNotify FindProcessNotifyFunction(string msgName)
        {
            tProcessServerNotify fun = null;
            mProcessNotifyFunList.TryGetValue(msgName, out fun);
            return fun;
        }
        
        public virtual void RegisterNotifyMsg(string msgName, tProcessServerNotify fun)
        {
            mProcessNotifyFunList[msgName] = fun;
        }

    }
    
    // 用于Actor 创建后, 下一个循环内执行Start
    public class ActorWaitStartEvent : BaseEvent
    {
        public Actor mActor;
        public override void OnTimeOver()
        {
            mActor.Start();
        }
    }
    
    public class ActorWaitDestoryEvent : BaseEvent
    {
        public Actor mActor;
        public override void OnTimeOver()
        {
            mActor.OnDestory();
            foreach (var v in mActor.mComponentList.Values)
            {
                v.OnRemove();
            }
            mActor.mComponentList.Clear();
            mActor.GetMgr().RemoveActor(mActor);
        }
    }
    
    public class ComponentWaitRemoveEvent : BaseEvent
    {
        public tComponent mActorTComponent;
        public override void OnTimeOver()
        {
            mActorTComponent.OnRemove();
            mActorTComponent.mActor.mComponentList.Remove(mActorTComponent.GetEventName());
        }
    }
}