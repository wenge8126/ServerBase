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

        public virtual void RegisterActorMsg(string msgName, ProcessServerRequestFunction fun)
        {
        }

        public virtual ProcessServerRequestFunction FindProcessRequestFunction(string msgName)
        {
            return null;
        }
    }

    public abstract class tActorFactor : ActorFactory
    {
        public int mType = 0;

        public Dictionary<string, ProcessServerRequestFunction> mProcessRequestFunList = new Dictionary<string, ProcessServerRequestFunction>();

        public override int GetType()
        {
            return mType; 
        }
        
        public override void RegisterActorMsg(string msgName, ProcessServerRequestFunction fun)
        {
            mProcessRequestFunList[msgName] = fun;
        }
        
        public override ProcessServerRequestFunction FindProcessRequestFunction(string msgName)
        {
            ProcessServerRequestFunction fun = null;
            mProcessRequestFunList.TryGetValue(msgName, out fun);
            return fun;
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