namespace Logic
{
    public class Actor : IActor
    {
        private UnitID mID = new UnitID();

        UnitID GetID()
        {
            return mID;
        }

        IComponent AddComponent(string componentName)
        {
            return null;
        }

        IComponent RemoveComponent(string componentName)
        {
            return null;
        }

        bool RemoveComponent(IComponent component)
        {
            return false;
        }
            
        void Awake()
        {
            
        }

        void Start()
        {
            
        }

        void Process()
        {
            
        }

        void LowProcess()
        {
            
        }

        void OnDestory()
        {
            
        }
    }
}