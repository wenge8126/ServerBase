
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.Rendering;

namespace Logic
{
    public delegate void ProcessFunction(tNetTool netTool, NetPacket packet);
    public delegate NiceData RequestFunction(tNetTool netTool, RequestPacket packet);
    
    public enum NET_PACKET_ID
    {
        PACKET_NONE = 0,
        PACKET_TRANSFER = 1,		// 网络节点之间中转数据的消息包 (里面是消息包数据)
        PACKET_RESPONSE_MSG	= 2,		// 回复结果消息
        PACKET_COMPRESS_EVENT = 3,		// 优化,只有数据的事件消息,字段信息固定在配置中
        PACKET_PART_PACKET = 4,		// 分包
        PACKET_QUEST_REPEAT_SEND = 5,		// 请求重发指定的包, 可能是丢包
        PACKET_QUEST_REPEAT_CONNECT = 6,	// 请求重新连接
        PACKET_EVENT = 7,		// 默认一般的事件消息
        FIRST_PART_PACKET = 8,
        eNotifyNetEventID = 9,
        eNotifyHeartBeat = 10,		// 心跳包，由服务器统一每隔约10，向所有连接发送一次心跳包
        PACKET_EVENT_PROCESS = 11, // 直接读取处理的事件包 
        PACKET_MAX,
    };

    public abstract class Packet
    {
        public abstract byte GetPacketID();
        public abstract uint GetState();
        public abstract void SetState(uint stateData);

        public abstract bool Read(DataBuffer iStream, uint packetSize);
        public abstract bool Write(ref DataBuffer oStream);
        public abstract void Execute(tNetTool net);

        //public abstract Packet New();
        public abstract void InitData();
    }


    public class NetPacket : Packet
    {
        public byte mID = 0;


        public override byte GetPacketID()
        {
            return mID;
        }

        public override uint GetState()
        {
            return 0;
        }

        public override void SetState(uint stateData)
        {
        }

        public override bool Read(DataBuffer iStream, uint packetSize)
        {
            return true;
        }

        public override bool Write(ref DataBuffer oStream)
        {
            return true;
        }


        public override void InitData()
        {
            
        }

        public override void Execute(tNetTool net)
        {
            LOG.logError("No write code : Execute");
        }
    }
    
    public class BasePacket : NetPacket
    {
        public NiceData mMsgData = new NiceData();
        public ProcessFunction mProcessFunction;
        
        public override bool Read(DataBuffer iStream, uint packetSize)
        {
            return mMsgData.restore(ref iStream);
        }

        public override bool Write(ref DataBuffer oStream)
        {
            return mMsgData.serialize(ref oStream);
        }


        public override void InitData()
        {
            mMsgData.clear();
        }

        public override void Execute(tNetTool net)
        {
            if (mProcessFunction != null)
                mProcessFunction(net, this);
        }
    }

    public class PingPacket : NetPacket
    {
        public override bool Read(DataBuffer iStream, uint packetSize)
        {
            return true;
        }

        public override bool Write(ref DataBuffer oStream)
        {
            return true;
        }
        
        public override void Execute(tNetTool net)
        {
            
        }
    }

    public class RequestPacket : BasePacket
    {
        public RequestFunction mRequestFunction;
        
        public TaskCompletionSource<NiceData> mTcs = null;
        public float mBeginWaitSecond = 0;
        public float mOverTimeSecond = 10;
        
        public uint mRequestID
        {
            set { mMsgData.set("mRequestID", value); }
            get { return (uint) mMsgData.get("mRequestID"); }
        }

        /// <summary>
        /// 异步请求
        /// </summary>
        /// <param name="net"></param>
        /// <returns></returns>
        public async Task<NiceData> AsyncRequest(tNetTool net, float overSecond = 10)
        {
            mOverTimeSecond = overSecond;
            mBeginWaitSecond = Time.time;
            AllotWaitID(this);
            net.SendPacket(this);
            return await mTcs.Task;
        }
        
        public override void Execute(tNetTool net)
        {
            if (mRequestFunction != null)
            {
                NiceData responseMsg = mRequestFunction(net, this);
                if (responseMsg != null)
                {
                    ResponsePacket responsePacket = net.CreatePacket((byte)NET_PACKET_ID.PACKET_RESPONSE_MSG) as ResponsePacket;
                    responsePacket.InitData();
                    responsePacket.mRequestID = mRequestID;
                    if (responseMsg.serialize(ref responsePacket.mResponseData))
                        net.SendPacket(responsePacket);
                    else
                    {
                        LOG.logError("Serialize response msg fail");
                        responseMsg.dump("Serialize response msg fail");
                    }
                }
            }
            else
            {
                LOG.logError("No register request process function");
            }
        }

        public static Dictionary<uint, RequestPacket> mRequestList = new Dictionary<uint, RequestPacket>();

        private static uint msNowCode = 0;
        
        public static uint AllotWaitID(RequestPacket request)
        {
            request.mTcs = new TaskCompletionSource<NiceData>();
            uint x = ++msNowCode;
            
                request.mRequestID = x;
                mRequestList[x] = request;
                return x;
        }
        
        public static void FreeWaitID(RequestPacket pWaitRequest)
        {
            RequestPacket existPacket;
            mRequestList.TryGetValue(pWaitRequest.mRequestID, out existPacket);
            if (existPacket == pWaitRequest)
                mRequestList.Remove(pWaitRequest.mRequestID);
        }

        public static RequestPacket FindWaitResponse(uint requestID)
        {
            RequestPacket existPacket = null;
            if (mRequestList.TryGetValue(requestID, out existPacket))
                return existPacket;
            return null;
        }
        
        public static void OnResponse(tNetTool net, NetPacket pak)
        {
            //pak.mMsgData.dump("GN_ResponseGateListInfo");
            var response = pak as ResponsePacket;
            if (response != null)
            {
                RequestPacket waitRequest = FindWaitResponse(response.mRequestID);
                if (waitRequest != null && waitRequest.mTcs != null)
                {
                    mRequestList.Remove(response.mRequestID);
                    var data = new NiceData();
                    response.mResponseData.seek(0);
                    if (data.restore(ref response.mResponseData))
                    {
                        waitRequest.mTcs.SetResult(data);
                        return;
                    }
                    else
                    {
                        LOG.logError("Response msg restore fail");
                    }
                    waitRequest.mTcs.SetResult(null);
                }
                else
                {
                    LOG.logWarn("No exist wait request : "+response.mRequestID.ToString());
                    //response.mMsgData.dump("X: No exist wait reqeust");
                }
            }
            else
            {
                LOG.logError("Is not ResponsePacket");
            }
        }

        static List<RequestPacket> mTempList = new List<RequestPacket>();
        /// <summary>
        /// 低速循环检测请求消息是否超时
        /// </summary>
        public static void LowProcess()
        {
            foreach (var vk in mRequestList)
            {
                if (Time.time - vk.Value.mBeginWaitSecond > vk.Value.mOverTimeSecond)
                {
                    LOG.logError($"Request {vk.Value.GetPacketID().ToString()} msg over time");
                    mTempList.Add(vk.Value);
                }
            }

            foreach (var reqMsg in mTempList)
            {
                FreeWaitID(reqMsg);
                reqMsg.mTcs.SetResult(null);
            }
            mTempList.Clear();
        }
    }

    public class ResponsePacket : NetPacket
    {
        public uint mRequestID = 0;

        public DataBuffer mResponseData = new DataBuffer();

        public override byte GetPacketID()
        {
            return (byte)NET_PACKET_ID.PACKET_RESPONSE_MSG;
        }
        
        public override bool Read(DataBuffer iStream, uint packetSize)
        {
            iStream.read(out mRequestID);
            
            return iStream.readData(ref mResponseData);
        }

        public override bool Write(ref DataBuffer oStream)
        {
            oStream.write(mRequestID);
            return oStream.writeData(ref mResponseData, mResponseData.tell());
        }

        public override void InitData()
        {
            mResponseData.clear(false);
        }
        
        public override void Execute(tNetTool net)
        {
            RequestPacket.OnResponse(net, this);
        }
    }
    //---------------------------------------------------------------------------------------------------------
    /// <summary>
    /// 低速循环时间事件
    /// </summary>
    public class LowUpdateEvent : BaseEvent
    {
        private float mWaitSecond = 0;

        public override void WaitTime(float fSecond)
        {
            mWaitSecond = fSecond;
            base.WaitTime(fSecond);
        }
        
        public override void OnTimeOver()
        {
            RequestPacket.LowProcess();
            WaitTime(mWaitSecond);
        }
    }
    //---------------------------------------------------------------------------------------------------------

}