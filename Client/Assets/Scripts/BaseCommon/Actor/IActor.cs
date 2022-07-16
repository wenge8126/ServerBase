namespace Logic
{
    public interface IActor
    {
        UnitID GetID();

        tComponent AddComponent(string componentName);

        tComponent RemoveComponent(string componentName);

        bool RemoveComponent(tComponent tComponent);
        
        void Awake();
        void Start();
        void Process();
        void LowProcess();
        void OnDestory();
    }
}