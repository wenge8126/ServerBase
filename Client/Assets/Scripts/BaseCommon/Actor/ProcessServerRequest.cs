using System.Threading.Tasks;

namespace Logic
{
    public abstract class tProcessServerRequest
    {
        public virtual async Task<NiceData> ProcessRequest(Actor actor, NiceData requestData)
        {
            return null;
        }
    }
    
    /// <summary>
    /// Actor 处理服务器的消息
    /// </summary>
    /// <typeparam name="ACTOR"></typeparam>
    /// <typeparam name="REQUEST"></typeparam>
    public class ProcessServerRequest<ACTOR, REQUEST> : tProcessServerRequest
        where REQUEST : BasePacket, new()
        where ACTOR : Actor
    {
        public delegate Task<NiceData> ProcessFunction(ACTOR actor, REQUEST req);

        public ProcessFunction mFunction;
        public override async Task<NiceData> ProcessRequest(Actor actor, NiceData requestData)
        {
            var requestMsg = new REQUEST();
            requestMsg.mMsgData = requestData; 
            return await mFunction(actor as ACTOR, requestMsg);
        }

        public ProcessServerRequest(ProcessFunction function)
        {
            mFunction = function;
        }
    }
    
    public class ComponentProcessServerRequest<ACTOR_COMP, REQUEST> : tProcessServerRequest
        where REQUEST : BasePacket, new()
        where ACTOR_COMP : tComponent
    {
        public delegate Task<NiceData> ProcessFunction(ACTOR_COMP actor, REQUEST req);

        public ProcessFunction mFunction;
        public override async Task<NiceData> ProcessRequest(Actor actor, NiceData requestData)
        {
            var requestMsg = new REQUEST();
            requestMsg.mMsgData = requestData;

            var comp = actor.FindComponent<ACTOR_COMP>();
            if (comp!=null)
                return await mFunction(comp as ACTOR_COMP, requestMsg);
            else
            {
                LOG.logError($"{actor.mFactory.GetType().ToString()} No exist component : {typeof(ACTOR_COMP).Name}");
                return null;
            }
        }

        public ComponentProcessServerRequest(ProcessFunction function)
        {
            mFunction = function;
        }
    }
}