namespace Logic
{
    public interface IActor
    {
        UnitID GetID();

        IComponent AddComponent(string componentName);

        IComponent RemoveComponent(string componentName);

        bool RemoveComponent(IComponent component);
        
        void Awake();
        void Start();
        void Process();
        void LowProcess();
        void OnDestory();
    }
}