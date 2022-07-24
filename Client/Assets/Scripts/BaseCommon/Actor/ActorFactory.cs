using System.Collections.Generic;
using System.Threading.Tasks;

namespace Logic
{
    public delegate Task<NiceData> ProcessServerRequestFunction(Actor actor, NiceData requestData);
    
    public abstract class ActorFactory
    {
        public ActorManager mManager;
        public abstract int GetType();
        public abstract Actor NewActor();

        public Actor CreateActor()
        {
            Actor actor = NewActor();
            actor.mFactory = this;
            actor.mID.type = (ushort)GetType();
            return actor;
        }

        public virtual void RegisterActorMsg(string msgName, tProcessServerRequest fun)
        {
        }

        public virtual void RegisterActorMsg(tProcessServerRequest fun)
        {
            RegisterActorMsg(fun.GetRequestMsgName(), fun);
        }
        
        public virtual tProcessServerRequest FindProcessRequestFunction(string msgName)
        {
            return null;
        }

        // 通知消息处理
        public virtual tProcessServerNotify FindProcessNotifyFunction(string msgName)
        {
            return null;
        }
        
        public virtual void RegisterNotifyMsg(string msgName, tProcessServerNotify fun)
        {
        }

        public virtual void RegisterNotifyMsg(tProcessServerNotify fun)
        {
            RegisterNotifyMsg(fun.GetMsgName(), fun);
        }

       
    }

    public abstract class tActorFactor : ActorFactory
    {
        public int mType = 0;

        public Dictionary<string, tProcessServerRequest> mProcessRequestFunList = new Dictionary<string, tProcessServerRequest>();
        public Dictionary<string, tProcessServerNotify> mProcessNotifyFunList = new Dictionary<string, tProcessServerNotify>();

        public override int GetType()
        {
            return mType; 
        }
        
        public override void RegisterActorMsg(string msgName, tProcessServerRequest fun)
        {
            mProcessRequestFunList[msgName] = fun;
        }
        
        public override tProcessServerRequest FindProcessRequestFunction(string msgName)
        {
            tProcessServerRequest fun = null;
            mProcessRequestFunList.TryGetValue(msgName, out fun);
            return fun;
        }
        
        // 通知消息处理
        public override tProcessServerNotify FindProcessNotifyFunction(string msgName)
        {
            tProcessServerNotify fun = null;
            mProcessNotifyFunList.TryGetValue(msgName, out fun);
            return fun;
        }
        
        public override void RegisterNotifyMsg(string msgName, tProcessServerNotify fun)
        {
            mProcessNotifyFunList[msgName] = fun;
        }
        

      
    }
    
    public class DefineActorFactory<T> : tActorFactor where T : Actor, new()
    {
        public DefineActorFactory(int type)
            : base()
        {
            mType = type;
            
        }

        public override Actor NewActor() { return new T(); }

        
    }
}