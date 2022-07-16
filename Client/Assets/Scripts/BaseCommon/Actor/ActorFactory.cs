namespace Logic
{
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
    }

    public abstract class tActorFactor : ActorFactory
    {
        public int mType = 0;

        public override int GetType()
        {
            return mType; 
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