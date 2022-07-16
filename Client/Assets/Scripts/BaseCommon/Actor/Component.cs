namespace Logic
{
    public class tComponent : BaseEvent , IComponent
    {
        public Actor mActor;
        
        public virtual void Awake()
        {
            
        }

        public virtual void Start()
        {
            
        }

        public virtual void Process()
        {
            
        }

        public virtual void LowProcess()
        {
            
        }

        public virtual void OnRemove()
        {
            
        }
        
        void Log(string log)
        {
            LOG.log(log);    
        }
    }
}