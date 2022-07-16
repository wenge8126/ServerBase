namespace Logic
{
    public interface IComponent
    {
        void Awake();
        void Start();
        void Process();
        void LowProcess();
        void OnRemove();
    }
}