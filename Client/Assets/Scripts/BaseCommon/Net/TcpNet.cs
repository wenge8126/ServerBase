//#define  NET_HEART_RECEIVE_CHECK  // 进行网络消息心跳检查，在间隔时间内未收到任何消息，则断开连接
//#define USE_ZIP_MSG_PACKET

#if !UNITY_IPHONE               // 只有IOS不使用线程接收数据, IOS线程接收出现接收不到数据情况
#   define  USE_TCP_THREAD_RECEIVE // 使用后台异步接收数据
#endif

#if UNITY_ANDROID || UNITY_IPHONE || UNITY_EDITOR
//#   define DEFINE_ENCRYPT
#endif

using System.Collections;

using System;
using System.Collections.Generic;
using System.Text;

using System.Net.Sockets;
using System.Net;
using System.Threading;


namespace Logic
{
    enum NET_PACKET_ID
    {
        PACKET_CHECK_REQUEST = 1,
        PACKET_CHECK_RESPONSE = 2,
        PACKET_COMPRESS_EVENT = 3,
        PACKET_EVENT = 7,
        PACKET_TRANSFER_DATA = 8,
        eNotifyNetEventID = 9,
        eNotifyHeartBeat = 10,
        PACKET_MAX,
    };

    public enum TARGET_NET_FLAG
    {
        eNetFlagNone,
        eLOCAL,
        eDB_SERVER, // 发送到 t_player 表格记录所在的DBNODE
        eGS_SERVER, // GameServer服务器
        eCLIENT, // 指定客户端
        eDB_NODE, // DB节点
        eDB_DATANODE, // 发送到 t_playerdata 表格记录所在的DBNODE
        eNet_SafeCode = 10,
    };
    //---------------------------------------------------------------------------------------------------------

    public abstract class Packet
    {
        public abstract byte GetPacketID();
        public abstract uint GetState();
        public abstract void SetState(uint stateData);

        public abstract bool Read(DataBuffer iStream, uint packetSize);
        public abstract bool Write(ref DataBuffer oStream);
        public abstract void Execute(tNetTool net);

        public abstract Packet New();
        public abstract void InitData();
    }

    public delegate void ProcessFunction(tNetTool, NetPacket);

    public abstract class NetPacket : Packet
    {
        public NiceData mMsgData = new NiceData();
        public ProcessFunction mProcessFunction;
        
        public override uint GetState()
        {
            return 0;
        }

        public override void SetState(uint stateData)
        {
        }

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

    public class RequestPacket : NetPacket
    {
        public RequestFunction mRequestFunction;
        
        public uint mRequestID {
            set { mMsgData.set("mRequestID", value); }
            get { return (uint) mMsgData.get("mRequestID"); }
        }
    }

    public class ResponsePacket : NetPacket
    {
        public uint mRequestID = 0;
        
        public override bool Read(DataBuffer iStream, uint packetSize)
        {
            iStream.read(out mRequestID);
            return mMsgData.restore(ref iStream);
        }

        public override bool Write(ref DataBuffer oStream)
        {
            oStream.write(mRequestID);
            return mMsgData.serialize(ref oStream);
        }
    }
    
    public delegate ResponsePacket RequestFunction(tNetTool, RequestPacket);

    //---------------------------------------------------------------------------------------------------------
    public abstract class NetDataStream
    {
        public abstract int tryRead(ref byte[] data, int beginPos, int count);
        public abstract int tryWrite(byte[] data, int beginPos, int count);
    }

    //---------------------------------------------------------------------------------------------------------
    public abstract class tNetTool : NetDataStream
    {
        public abstract bool SendPacket(Packet msgPacket);
        public abstract bool Send(tEvent evt, int target);
        public abstract bool OnReceive(ref DataBuffer data, int target);

        public abstract int available();

        public abstract void Process(float delayTime);

        public abstract bool Connect(string strIp, int port);
        public abstract bool ConnectDNS(string urlDNS, int port);

        public abstract bool Connect(IPAddress addr, int port);

        public abstract void SetEventCenter(tEventCenter eventCenter);

        public abstract void SetOk(bool bOk);
        public abstract bool IsOk();

        public abstract void Close(bool bRunCloseEvent);

        public abstract void ReConnect();

        public abstract void OnConnect();
        public abstract void OnClose();
        public abstract void OnConnectFail();

        public abstract void OnError(string errorInfo);

        public abstract void SetConnectEvent(tEvent evt);

        public virtual void InitReady()
        {
        }

        public virtual Packet CreatePacket(byte packetID)
        {
            return null;
        }
    }
    //---------------------------------------------------------------------------------------------------------

    public abstract class BaseNetTool : tNetTool
    {
        static public int RECEIVE_OVER_TIME = 30; // 接收数据超时，20秒内未收到任务消息包，则断线处理
        static public string TARGET_FLAG_NAME = "_T_N_FLAG_";
        static public string TARGET_ID_NAME = "_T_N_ID_";
        static public string TARGET_DB_FLAG = "_DB_";

        static public int CONFIG_PACKHEAD_LENGTH = 6;
        static public int ONCE_RECEIVE_PACKET_COUNT = 100;

        protected OutputBuffer mSendBuffer = new OutputBuffer(16 * 1024);
        protected InputBuffer mRevBuffer = new InputBuffer(16 * 1024);

        public tEventCenter mEventCenter;

        protected DataBuffer mDatabuffer = new DataBuffer(2048);
        protected DataBuffer mZipBuffer = new DataBuffer(2048);

        public tEvent mConnectEvent;
        public tEvent mNotifyConnectFinishEvent;
        public string mCloseEventName = "";

        protected float mLastReceiveTime = 0;

        protected object mReceiveDataLock = new object();

        Dictionary<byte, Packet> mNetPacketList = new Dictionary<byte, Packet>();

        //-----------------------------------------------------------------------------
        public BaseNetTool()
        {

        }

        public void RegisterPacket(NetPacket msgPacket, ProcessFunction processFunction)
        {
            msgPacket.mProcessFunction = processFunction;
            mNetPacketList[msgPacket.GetPacketID()] = msgPacket;
        }
        
        public void RegisterPacket(RequestPacket msgPacket, RequestFunction function)
        {
            msgPacket.mRequestFunction = function;
            mNetPacketList[msgPacket.GetPacketID()] = msgPacket;
        }
        
        public override Packet CreatePacket(byte packetID)
        {
            Packet pak;
            if (mNetPacketList.TryGetValue(packetID, out pak))
                return pak;
            Log("Error : No exist packet : "+packetID.ToString());
            return null;
        }

        public virtual string GetIP()
        {
            return "UNKNOW";
        }

        public virtual int GetPort()
        {
            return 0;
        }

        public virtual void InitClear()
        {
            mSendBuffer.Clear();
            mRevBuffer.Clear();
        }

        public override void InitReady()
        {
            mbStartNet = false;
        }

        public override void SetConnectEvent(tEvent evt)
        {
            if (mConnectEvent != null)
                mConnectEvent.Finish();

            mConnectEvent = evt;
        }

        public void Log(string info)
        {
            if (null != mEventCenter) LOG.log(info);
        }

        public override void SetEventCenter(tEventCenter eventCenter)
        {
            mEventCenter = eventCenter;
        }

        enum EEventPacketState
        {
            ePacketStateEncrypt = 1, // 需要加密
            ePacketStateZip = 1 << 1, // 是否压缩
        };

        public override bool Send(tEvent evt, int target)
        {
            return false;
//             if (!IsOk())
//             {
//                 if (mEventCenter != null)
//                     mEventCenter.OnSendEvent(evt);
//                 evt.Log("***** Error: net is not connect or not ready. >>> send fail >>>");
//                 evt.OnSendFail(SEND_RESULT.eNetDisconnect);
//                 return false;
//             }
//
//             bool bEncrypt = false;
// #if DEFINE_ENCRYPT
//             bEncrypt = evt.GetEventFactory().NeedEncrypt();
// #endif
//             mDatabuffer.seek(CONFIG_PACKHEAD_LENGTH);
//             int offset = CONFIG_PACKHEAD_LENGTH;
//             byte id = (byte)NET_PACKET_ID.PACKET_EVENT;
//             int targetFlag = evt.GetEventFactory().GetTargetFlag();
//             if (targetFlag > 0)
//             {
//                 id = (byte)NET_PACKET_ID.PACKET_TRANSFER_DATA;
//                 mDatabuffer.write((byte)targetFlag);
//                 mDatabuffer.write((int)1);
//                 mDatabuffer.write((byte)NET_PACKET_ID.PACKET_EVENT);
//                 offset += sizeof(byte)*2 + sizeof(int);
//             }
//             //else if (evt["_DATA_"])
//             //{
//             //    id = (byte)NET_PACKET_ID.PACKET_TRANSFER_DATA;
//             //    mDatabuffer.write((byte)TARGET_NET_FLAG.eDB_DATANODE);
//             //    mDatabuffer.write((int)evt["_TARGET_"]);
//             //    mDatabuffer.write((byte)NET_PACKET_ID.PACKET_EVENT);
//             //}
//
//             if (!mEventCenter.SerializeMsg(evt, ref mDatabuffer))
//             {
//                 Log("Event serialize fail >" + evt.GetEventName());
//                 return false;
//             }
//             if (mEventCenter != null)
//                 mEventCenter.OnSendEvent(evt);
//
//             int count = mDatabuffer.tell();
//             mDatabuffer.seek(0);
//
//             mDatabuffer.write(id);
//             uint infoData = 0;
//             if (bEncrypt)
//                 infoData = ((uint)EEventPacketState.ePacketStateEncrypt) << 24;
//             infoData += (uint)(count - CONFIG_PACKHEAD_LENGTH);
//             mDatabuffer.write(infoData);  // <--save data size
// #if DEFINE_ENCRYPT
//             if (bEncrypt)
//                 CommonParam.encrypt_common(mDatabuffer.getData(), count - offset, 0, offset);
// #endif
//             bool bSu = mSendBuffer.Write(mDatabuffer.getData(), 0, count); //<--save data to send buffer
//             if (!bSu)
//                 evt.OnSendFail(SEND_RESULT.eWriteSendDataFail);
//             return bSu;
        }

        public override bool SendPacket(Packet msgPacket)
        {
            bool bEncrypt = false;

            mDatabuffer.seek(CONFIG_PACKHEAD_LENGTH);
            int offset = CONFIG_PACKHEAD_LENGTH;
            byte id = (byte) msgPacket.GetPacketID();

            if (!msgPacket.Write(ref mDatabuffer))
            {
                Log("Packet Write data fail >" + id.ToString());
                return false;
            }

            int count = mDatabuffer.tell();

            mDatabuffer.seek(0);
            mDatabuffer.write(id);
            mDatabuffer.write((byte) 0);
            mDatabuffer.write((uint)count); // <--save data size
            // uint infoData = 0;
            // if (bEncrypt)
            //     infoData = ((uint) EEventPacketState.ePacketStateEncrypt) << 24;
            // infoData += (uint) (count - CONFIG_PACKHEAD_LENGTH);
            
            //mDatabuffer.write(infoData); // <--save data size

#if DEFINE_ENCRYPT
            if (bEncrypt)
                CommonParam.encrypt_common(mDatabuffer.getData(), count - offset, 0, offset);
#endif
            bool bSu = mSendBuffer.Write(mDatabuffer.getData(), 0, count); //<--save data to send buffer
            if (!bSu)
                LOG.logError("Send packet fail >" + id.ToString());
            return bSu;
        }

        public override bool OnReceive(ref DataBuffer data, int target)
        {
            //Log("Receive data >" + data.size());

            tEvent evt = null;
            try
            {
                //evt = mEventCenter.RestoreMsg(ref data, 0, 0);
            }
            catch (Exception e)
            {
                LOG.logError("Error: restore packet data fail. may be event restore error. >>> \r\n" + e.ToString());
            }

            if (null != evt)
            {
#if DEVELOP_MODE
               EventCenter.Log(LOG_LEVEL.GENERAL, " > " + evt.GetEventInfo());
                
                    if (mEventCenter != null)
                        mEventCenter.OnReceiveEvent(evt);

                    bool re = evt.DoEvent();
                    evt.GetEventFactory().FreeEvent(evt);
                    return re;
#else
                try
                {
                    // mEventCenter.OnReceiveEvent(evt);
                    //
                    // bool re = evt.DoEvent();
                    //evt.GetEventFactory().FreeEvent(evt);
                    //return re;

                    return false;
                }
                catch (Exception e)
                {
                    LOG.logError("Error: run net event fail. >>> " + evt.GetEventName() + ">>>" + e.ToString());
                }
#endif
            }

            return false;
        }

        protected bool mbStartNet = false;

        //NOTE: USE_TCP_THREAD_RECEIVE 时被TCP类覆盖,不再执行这个过程
        public override void Process(float delayTime)
        {
            // receive data.
            int len = mRevBuffer.FillOnNet(this);
            if (len > 0)
                mLastReceiveTime = EventCenter.Instance.mTimeEventManager.GetNowTime();

            if (IsOk())
            {
                // send data.
                mSendBuffer._NetSend(this);
            }
            else if (len > 0)
            {
                SetOk(true);
            }

            _ReceivePacket();

        }

        public bool _ReceivePacket()
        {
            //int receiveCount = 0;
            while (true)
            {
                uint packSize = 0;
                byte id = 0xFF;
                bool bEncrypt = false;

#if USE_TCP_THREAD_RECEIVE
                lock (mReceiveDataLock)
#endif
                {
                    if (mRevBuffer.DataSize() <
                        CONFIG_PACKHEAD_LENGTH) //receiveCount++ >= ONCE_RECEIVE_PACKET_COUNT || 
                        return true;

                    byte[] d = new byte[CONFIG_PACKHEAD_LENGTH];

                    if (!mRevBuffer.Peek(ref d, 0, CONFIG_PACKHEAD_LENGTH))
                        break;

                    //when use biffult restore packet.
                    //ushort id = BitConverter.ToUInt16(d, 0);
                    id = d[0];

                    // uint indexAndSize = BitConverter.ToUInt32(d, sizeof(byte));
                    // packSize = indexAndSize & 0xffffff;
                    //
                    // uint packState = indexAndSize >> 24;
                    //
                    // bEncrypt = (packState & (uint) EEventPacketState.ePacketStateEncrypt) != 0;

                    //if (packSize <= 0)
                    //    throw new Exception("Error: have packet size is zero, net error, please must close net.");
                    uint packSize = BitConverter.ToUInt32(d, sizeof(short));
                    if (mRevBuffer.DataSize() < CONFIG_PACKHEAD_LENGTH + packSize)
                        break;

                    if (!mRevBuffer.Skip(CONFIG_PACKHEAD_LENGTH))
                    {
                        Log(" *** XXX move receive data buffer fail. for packet head data");
                        return false;
                    }

                    if (id == (ushort) NET_PACKET_ID.eNotifyHeartBeat)
                    {
                        // 接收到心跳包
                        mLastReceiveTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
                        //LOG.log("* Get once heart beat");
                        continue;
                    }

                    if (packSize > mDatabuffer.size())
                        mDatabuffer._resize((int) packSize);

                    if (!mRevBuffer.Peek(ref mDatabuffer.mData, 0, (int) packSize))
                    {
                        Log(" *** XXX read receive data fail. packet data can not read");
                        return false;
                    }

                    if (!mRevBuffer.Skip((int) packSize))
                    {
                        Log(" *** XXX move receive data buffer fail. for packet data");
                        return false;
                    }

                    mDatabuffer.seek((int) packSize);
                }

#if DEFINE_ENCRYPT
                if (bEncrypt)
                    CommonParam.decrypt_common(mDatabuffer.getData(), mDatabuffer.tell(), 0);
#endif
                Packet packet = CreatePacket(id);
                if (packet != null)
                {
                    mDatabuffer.seek(0);
                    mDatabuffer._setZero(packSize);
                    if (packet.Read(mDatabuffer, packSize))
                        packet.Execute(this);
                    else
                    {
                        Log($"Packet {id.ToString()} read fail");

                    }
                }
                else
                {
                    Log("No exist packet : " + id.ToString());
                }

            }

            return true;
        }

//                 if (id == (ushort)NET_PACKET_ID.PACKET_COMPRESS_EVENT)
//                 {
// #if USE_ZIP_MSG_PACKET
//                     if (mZipBuffer.size() < packSize)
//                         mZipBuffer._resize((int)packSize);
//                     Array.Copy(mDatabuffer.mData, mZipBuffer.mData, packSize);
//                     // un compress
//                     if (packSize > sizeof(uint))
//                     {
//                         if (!mZipBuffer.seek(0))
//                             break;
//
//                         uint scrSize = 0;
//                         if (!mZipBuffer.read(out scrSize))
//                             break;
//
//                         if (mDatabuffer.size() < scrSize)
//                             mDatabuffer._resize((int)scrSize);
//                         int zipSize = (int)packSize - sizeof(uint);
//                         if (!ZipTool.UnGZip(mZipBuffer.mData, sizeof(uint), zipSize, ref mDatabuffer.mData, (int)scrSize))
//                         {
//                             Log("XXX >>> un compress zip data fail.");
//                             return false;
//                         }
//                         else
//                         {
//                             Log("*** Succeed to un zip data Zip size[" + (packSize - sizeof(uint)).ToString() + "], scr size[" + scrSize.ToString() + "]");
//                             mDatabuffer.seek((int)scrSize);
//                         }
//                     }
//                     else
//                     {
//                         Log("XXX >>> zip data is error, so small");
//                         return false;
//                     }
//
//                     mDatabuffer.seek(0);
//                     OnReceive(ref mDatabuffer, 0);
//                     continue;
// #else
//                     OnError("Error: now can not use zip net packet.");
//                     throw new Exception("Error: now can not use zip net packet.");
// #endif
//                 }
//                 else
//                 {
//                     if (id == (ushort)NET_PACKET_ID.PACKET_EVENT)
//                     {
//                         mDatabuffer.seek(0);
//                         OnReceive(ref mDatabuffer, 0);
//                         continue;
//                     }
//                     else if (id == (ushort)NET_PACKET_ID.eNotifyNetEventID)
//                     {
//                         //EventCenter.Instance.UpdateNetMsgIndex(ref mDatabuffer);
//                         continue;
//                     }                    
//                     else
//                     {
//                         Packet p;
//                         if (mNetPacketList.TryGetValue(id, out p))
//                         {
//                             p.InitData();
//                             mDatabuffer.seek(0);
//                             mDatabuffer._setZero(packSize);
//                             if (p.Read(mDatabuffer, packSize))
//                                 p.Execute(this);
//                             else
//                                 LOG.logError(id.ToString()+" >Packet read data fail");
//                         }
//                         else
//                             LOG.logError("Unkwon msg >" + id.ToString() + ", now give up msg data >" + packSize.ToString());
//                     }
//                 }
        //         //break;
        //     }                     
        //
        //     return true;
        // }

        public override void OnConnect()
        {
            Log("*** Net connect succeed. Ok *****************************");
            mbStartNet = true;
            mLastReceiveTime = EventCenter.Instance.mTimeEventManager.GetNowTime();

            if (mConnectEvent != null)
                mConnectEvent.Finish();

            if (mNotifyConnectFinishEvent != null)
            {
                mNotifyConnectFinishEvent.Do();
                object obj =
                    mNotifyConnectFinishEvent.getObject("NETAUTOFINISH");
                if (obj != null && obj.GetType() == typeof(bool))
                {
                    if ((bool) obj)
                    {
                        Log("Info: connect finish event set auto finish");
                        mNotifyConnectFinishEvent.Finish();
                        return;
                    }
                }
            }
            else
            {
                Log("not set connect finish event------------------");
            }
        }

        public override void OnClose()
        {
            if (mConnectEvent != null)
                mConnectEvent.Finish();

            if (mEventCenter != null)
            {
                if (mCloseEventName != "")
                {
                    tEvent evt = mEventCenter.StartEvent(mCloseEventName);
                    if (evt != null)
                    {
                        evt.set("IP", GetIP());
                        evt.set("PORT", GetPort());
                        evt.Do();
                    }
                    else
                        EventCenter.Log(LOG_LEVEL.ERROR, "No register event >" + mCloseEventName);
                }
            }

            Log("XXX Connect close ===================");
        }

        public override void OnConnectFail()
        {
            Log("***XXX Net connect fail. ***************");
            if (mConnectEvent != null)
                mConnectEvent.Finish();
        }

        public override void OnError(string errorInfo)
        {
            Log("XXX Net Error: " + errorInfo);
            if (mEventCenter != null)
            {
                tEvent evt = mEventCenter.StartEvent("NET_ERROR", false);
                if (evt != null)
                {
                    evt.set("ERROR", errorInfo);
                    evt.Do();

                }
            }
        }

    }

    //---------------------------------------------------------------------------------------------------------
    // use TcpClient connect to server
    public class TcpClientNet : BaseNetTool
    {
        static public int msReceiveBufferSize = 128 * 1024;
        static public float WAIT_CONNECT_OVER_TIME = 10.0f;
        static private ManualResetEvent connectDone = new ManualResetEvent(false);


        TcpClient mTcpClient;
        NetworkStream mNetStream;


        protected IPAddress mIP = null;
        protected int mPort;
        bool mIsOk = false;

#if USE_TCP_THREAD_RECEIVE
        byte[] mThreadReceiveBuffer = new byte[msReceiveBufferSize];
#endif
        object mStreamLock = new object();

        public struct SyncTool
        {
            public TcpClientNet net;
            public NetworkStream stream;
        }
        //-------------------------------------------------------------------------

        public TcpClientNet()
        {
            mTcpClient = new TcpClient(AddressFamily.InterNetwork); //IPv6  AddressFamily.InterNetworkV6
            mTcpClient.NoDelay = false;
            mTcpClient.ReceiveBufferSize = 1024 * 64;
            mTcpClient.ReceiveTimeout = 20;
            mTcpClient.SendBufferSize = 1024 * 64;
            mTcpClient.SendTimeout = 100;

        }

        public override string GetIP()
        {
            return mIP.ToString();
        }

        public override int GetPort()
        {
            return mPort;
        }

        public EndPoint GetLocalPoint()
        {
            if (mTcpClient != null) return mTcpClient.Client.LocalEndPoint;
            return null;
        }

        public void SetPortReuse(bool bReuse)
        {
            if (mTcpClient != null)
                mTcpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, bReuse);
        }

        public override bool Connect(IPAddress addr, int port)
        {
            mIP = addr;
            mPort = port;

            _ReConnect();
            return true;
        }

        public override void InitClear()
        {
#if USE_TCP_THREAD_RECEIVE
            lock (mStreamLock)
            {
                if (mNetStream != null)
                {
                    NetworkStream s = mNetStream;
                    mNetStream = null;
                    s.Close();
                }
            }
#endif
            if (mTcpClient != null)
            {
                mTcpClient.Close();
                mTcpClient = null;
            }

            base.InitClear();
            SetOk(false);
        }

        public override void SetOk(bool bOk)
        {
            bool bRunConnect = false;
            if (mIsOk != bOk && bOk)
                bRunConnect = true;

            mIsOk = bOk;
            if (bRunConnect)
            {
                OnConnect();
            }
        }

        public override bool IsOk()
        {
            return mIsOk;
        }

        void __ReadySocket()
        {
            connectDone.Reset();
            SetOk(false);

            InitClear();

            if (mTcpClient == null)
            {
                mTcpClient = new TcpClient(AddressFamily.InterNetwork); //IPv6  AddressFamily.InterNetworkV6
                mTcpClient.NoDelay = false;
                mTcpClient.ReceiveBufferSize = 1024 * 64;
                mTcpClient.ReceiveTimeout = 20;
                mTcpClient.SendBufferSize = 1024 * 64;
                mTcpClient.SendTimeout = 100;

                //mTcpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            }
        }

        public override void Close(bool bRunCloseEvent)
        {
            SetOk(false);

            InitClear();
            if (bRunCloseEvent)
                OnClose();
            else
            {
                Log("=========== Net close and not need run close event. ============");
            }
        }

        public override void ReConnect()
        {
            _ReConnect();

            //throw new NotImplementedException ();
        }

        public void _ReConnect()
        {
#if USE_TCP_THREAD_RECEIVE
            lock (mReceiveDataLock)
            {
                mbDisConnect = false;
            }
#endif
            if (null == mIP)
            {
                Log("ip or url is error");
                return;
            }

            __ReadySocket();

            try
            {
                mTcpClient.BeginConnect(mIP, mPort,
                    new AsyncCallback(ConnectCallback), mTcpClient);
            }
            catch (Exception e)
            {
                Log(e.ToString());
                Console.WriteLine(e.ToString());
            }
        }


        private static void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the socket from the state object.
                TcpClient client = (TcpClient) ar.AsyncState;

                // Complete the connection.
                client.EndConnect(ar);

                //Console.WriteLine("Socket connected to {0}",
                //    client.RemoteEndPoint.ToString());

                // Signal that the connection has been made.
                connectDone.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public override bool Connect(string strIp, int port)
        {
            Log("Begin connect to ip >" + strIp + ">Port>" + port.ToString());

            mPort = port;

            if (!IPAddress.TryParse(strIp, out mIP))
            {
                Log("IP is error >" + strIp);
                return false;
            }
            //IPEndPoint mIPInfo = new IPEndPoint(mIP, port);

            _ReConnect();

            return true;
        }

        public override bool ConnectDNS(string urlDNS, int port)
        {
            mPort = port;
            IPAddress[] ipInfo = Dns.GetHostAddresses(urlDNS);
            if (ipInfo.Length <= 0)
                return false;

            mIP = ipInfo[0];

            _ReConnect();

            return true;
        }

        float mTempLastTime = 0;

        public override void Process(float delayTime)
        {
            if (IsOk())
            {
#if USE_TCP_THREAD_RECEIVE
                // send data.
                mSendBuffer._NetSend(this);

                bool bHave = false;
                lock (mReceiveDataLock)
                {
                    if (mbDisConnect)
                    {
                        //LOG.log("*** Connect disconnect, now close");
                        Close(true);
                        mbDisConnect = false;
                        return;
                    }
                    else
                        bHave = !mRevBuffer.empty();
                }

                if (bHave)
                {
                    mLastReceiveTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
                    _ReceivePacket();
                }
#else
                base.Process(delayTime);
#endif
            }
            else if (mTcpClient != null && (mTcpClient.Available > 0 || mTcpClient.Connected))
            {
                try
                {
                    mNetStream = mTcpClient.GetStream();

                    int safeCode = (int) TARGET_NET_FLAG.eNet_SafeCode;
                    if (tryWrite(BitConverter.GetBytes(safeCode), 0, 4) == 4)
                    {
                        Log("send check code succeed");
                        SetOk(true);
#if USE_TCP_THREAD_RECEIVE
                        StartThreadReceive();
#endif
                    }
                    else
                        Log("NET_ERROR: send check code fail");

                }
                catch (Exception e)
                {
                    OnError("Error: get stream >>> " + e.ToString());
                    OnConnectFail();
                }
            }

            // 如果当前帧或与上次检查时间之隔大于2秒，则重置, 如果阻塞在当前帧时，执行到这里耗时已经很长了
            if (delayTime > 2 || EventCenter.Instance.mTimeEventManager.GetNowTime() - mTempLastTime > 2)
            {
                mLastReceiveTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
            }
            else if (mbStartNet)
            {
#if NET_HEART_RECEIVE_CHECK && !UNITY_EDITOR
                if (EventCenter.Instance.mTimeEventManager.GetNowTime() - mLastReceiveTime > RECEIVE_OVER_TIME)
                {
                    LOG.logError("NET_ERROR : HeartBeat time over >"+mLastReceiveTime.ToString()+", Over >" + (EventCenter.Instance.mTimeEventManager.GetNowTime() - mLastReceiveTime).ToString());
                    mLastReceiveTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
                    // 服务器隔10秒发一个心跳包，如果20秒内未收到任务数据包，则断线处理                    
                    Close(true);
                    return;
                }
#endif
            }

            mTempLastTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
        }

#if USE_TCP_THREAD_RECEIVE
        AsyncCallback mCallBack = null;

        void StartThreadReceive()
        {
            if (mCallBack == null)
                mCallBack = new AsyncCallback(AsyncReceiveBack);
            SyncTool t;
            t.net = this;
            t.stream = mNetStream;
            mNetStream.BeginRead(mThreadReceiveBuffer, 0, msReceiveBufferSize, mCallBack, t);
        }

        int mReceiveDataLength = 0;
        bool mbDisConnect = false;

        public static void AsyncReceiveBack(IAsyncResult result)
        {
            SyncTool t = (SyncTool) result.AsyncState;
            TcpClientNet net = t.net; //(TcpClientNet)result.AsyncState;
            lock (net.mStreamLock)
            {
                if (net.mNetStream != t.stream)
                    return;

                net.mReceiveDataLength = net.mNetStream.EndRead(result);
                if (net.mReceiveDataLength > 0)
                {
                    net.mReadPos = 0;
                    lock (net.mReceiveDataLock)
                    {
                        while (net.mRevBuffer.FillOnNet(net) > 0)
                        {
                        }
                        //LOG.log("*** receive size >" + net.mRevBuffer.DataSize().ToString());
                    }

                    net.StartThreadReceive();
                }
                else
                {
                    // 断开连接状态设置
                    lock (net.mReceiveDataLock)
                    {
                        net.mbDisConnect = true;
                    }
                }
            }
        }

        // Noto: 已经完全在后台线程使用
        int mReadPos = 0;

        public override int tryRead(ref byte[] data, int beginPos, int count)
        {
            if (count < 0)
            {
                LOG.logError(" *** XXX Code error. read net data count is lower zero");
                return 0;
            }

            if (mReadPos >= mReceiveDataLength)
                return 0;

            int x = mReceiveDataLength - mReadPos;
            if (x > count)
                x = count;

            Array.Copy(mThreadReceiveBuffer, mReadPos, data, beginPos, x);

            mReadPos += x;

            return x;
        }

        public override int available()
        {
            return mReceiveDataLength - mReadPos;
        }
#else
                // receive
        public override int tryRead(ref byte[] data, int beginPos, int count)
        {
            if (count < 0)
            {
                Log(" *** XXX Code error. read net data count is lower zero");
                return 0;
            }
            int x = 0;
            try
            {
                if (mNetStream!=null && mNetStream.DataAvailable)
                    x = mNetStream.Read(data, beginPos, count);
            }
            catch (Exception e)
            {
                LOG.log(e.ToString());
                SetOk(false);
                OnClose();
            }
            return x;

        }

        public override int available() 
        {
            if (mTcpClient!=null)
                return mTcpClient.Available;

            return 0;
        }
#endif

        // Noto: this is not same of NetTool. read is receive and write is send
        public override int tryWrite(byte[] data, int beginPos, int count)
        {
            if (count < 0)
            {
                Log(" *** XXX Code error. write net data count is lower zero");
                return 0;
            }

            if (mNetStream.CanWrite)
            {
                try
                {
                    mNetStream.Write(data, beginPos, count);
                    return count;
                }
                catch (Exception e)
                {
                    Log(e.ToString());
                    // close event
                    Close(true);
                    //Close();
                }
            }

            return 0;
        }
    }
}
//---------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------

    