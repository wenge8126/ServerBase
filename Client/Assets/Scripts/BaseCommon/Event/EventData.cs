

public enum LOG_LEVEL
{
    GENERAL,
    WARN,
    ERROR,
    HIGH,
}


namespace Logic
{
    public abstract class tLog
    {
        public abstract void Log(string info);
    }
}
