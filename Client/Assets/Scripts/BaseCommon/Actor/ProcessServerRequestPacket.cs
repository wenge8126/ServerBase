namespace Logic
{
        // 接收请求处理的消息(回复请求处理过程)
    public class ProcessRequestPacket : NetPacket
    {
        public uint mRequestID = 0;
        public System.UInt64 mClientActorID = 0;
        public string mRequestName = "";
        public DataBuffer mRequestMsgData = new DataBuffer();

        public override byte GetPacketID()
        {
            return (byte)NET_PACKET_ID.PACKET_REQUEST_CLIENTACTOR;
        }
        
        public override bool Read(DataBuffer iStream, uint packetSize)
        {
            iStream.read(out mRequestID);
            iStream.read(out mClientActorID);
            iStream.readString(out mRequestName);
            return iStream.readData(ref mRequestMsgData);
        }

        public override bool Write(ref DataBuffer oStream)
        {
            oStream.write(mRequestID);
            oStream.write(mClientActorID);
            oStream.writeString(mRequestName);
            return oStream.writeData(ref mRequestMsgData);
        }
        
        public override async void Execute(tNetTool net)
        {
            var actor = ActorManager.Instance.FindActor(mClientActorID);
            if (actor != null)
            {
                var msgData = new NiceData();
                mRequestMsgData.seek(0);
                if (!msgData.restore(ref mRequestMsgData))
                {
                    LOG.logError("No restore request msg data");
                    return;
                }
                NiceData resp = await actor.OnRequestMsg(mRequestName, msgData);
                if (resp != null)
                {
                    var respPak = new ResponsePacket();
                    respPak.mRequestID = mRequestID;
                    if (resp.serialize(ref respPak.mResponseData))
                        net.SendPacket(respPak);
                    else
                    {
                        LOG.logError("Save response data fail");
                    }
                }
                else
                {
                    LOG.logError("No response data");
                }
            }
            else
            {
                LOG.logError("No exist actor : "+new UnitID(mClientActorID).dump());
            }
        }
        
        public override void InitData()
        {
            mRequestID = 0;
            mClientActorID = 0;
            mRequestMsgData.clear(true);
        }
    }
}