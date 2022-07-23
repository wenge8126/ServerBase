using System.Collections.Generic;
using System.Threading.Tasks;
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

        public async Task<NiceData> OnRequestMsg(string requestName, NiceData requestData)
        {
            var fun = mFactory.FindProcessRequestFunction(requestName);
            if (fun != null)
                return await fun.ProcessRequest(this, requestData);
            return null;
        }

        public static async Task<NiceData> ProcessRequest<REQUEST>(Actor actor, NiceData requestData)
            where REQUEST : BasePacket, new()
        {
            var requestMsg = new REQUEST();
            requestMsg.mMsgData = requestData; 
            return await actor.On<REQUEST>(requestMsg);
        }

        public async Task<NiceData> AsyncRequestMsg(UnitID targetActorID, BasePacket requestMsg, int overMilSecond = 10000)
        {
            var response = await MainStart.mNet.AsyncRequest(new UnitID(104, 1), requestMsg.MsgName(), requestMsg, overMilSecond);
            if (response != null)
                response.dump("ok=========");
            else
            {
                LOG.log("Request fail, response null");
            }

            return response;
        }
        
        public async Task<NiceData> AsyncRequestMsg(UnitID targetActorID, string msgName, NiceData requestMsgData, int overMilSecond = 10000)
        {
            var response = await MainStart.mNet.AsyncRequest(new UnitID(104, 1), msgName, requestMsgData, overMilSecond);
            if (response != null)
                response.dump("ok=========");
            else
            {
                LOG.log("Request fail, response null");
            }

            return response;
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

        public tComponent FindComponent(string componentName)
        {
            tComponent comp = null;
            mComponentList.TryGetValue(componentName, out comp);
            return comp;
        }

        public tComponent FindComponent<T>()
            where T : tComponent
        {
            tComponent comp = null;
            mComponentList.TryGetValue(typeof(T).Name, out comp);
            return comp;
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
            evt.mActorTComponent = tComponent;
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

        public virtual async Task<NiceData> On<T>(T req)
        {
            LOG.logError("No write process function code : "+typeof(T).Name);
            return null;
        }
    }




}