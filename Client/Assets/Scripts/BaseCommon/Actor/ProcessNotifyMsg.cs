using System.Threading.Tasks;

namespace Logic
{
    public abstract class tProcessServerNotify
    {
        public abstract string GetMsgName();
        public virtual void ProcessNotify(Actor actor, NiceData notifyMsgData)
        {
           
        }
    }
    
    /// <summary>
    /// Actor 处理服务器的消息
    /// </summary>
    /// <typeparam name="ACTOR"></typeparam>
    /// <typeparam name="REQUEST"></typeparam>
    public class ProcessServerNotify<ACTOR, NOTIFY_MSG> : tProcessServerNotify
        where NOTIFY_MSG : BasePacket, new()
        where ACTOR : Actor
    {
        public override string GetMsgName()
        {
            return typeof(NOTIFY_MSG).Name;
        }
        
        public delegate void ProcessFunction(ACTOR actor, NOTIFY_MSG msg);

        public ProcessFunction mFunction;
        public override void ProcessNotify(Actor actor, NiceData notifyMsgData)
        {
            var msg = new NOTIFY_MSG();
            msg.mMsgData = notifyMsgData; 
            mFunction(actor as ACTOR, msg);
        }

        public ProcessServerNotify(ProcessFunction function)
        {
            mFunction = function;
        }
    }
    
    public class ComponentProcessServerNotify<ACTOR_COMP, NOTIFY_MSG> : tProcessServerNotify
        where NOTIFY_MSG : BasePacket, new()
        where ACTOR_COMP : tComponent
    {
        
        public override string GetMsgName()
        {
            return typeof(NOTIFY_MSG).Name;
        }
        
        public delegate void ProcessFunction(ACTOR_COMP actor, NOTIFY_MSG notifyMsg);

        public ProcessFunction mFunction;
        
        public override void ProcessNotify(Actor actor, NiceData notifyMsgData)
        {
            var msg = new NOTIFY_MSG();
            msg.mMsgData = notifyMsgData;

            var comp = actor.FindComponent<ACTOR_COMP>();
            if (comp!=null)
                mFunction(comp as ACTOR_COMP, msg);
            else
            {
                LOG.logError($"{actor.mFactory.GetType().ToString()} No exist component : {typeof(ACTOR_COMP).Name}");
            }
        }

        public ComponentProcessServerNotify(ProcessFunction function)
        {
            mFunction = function;
        }
    }
}