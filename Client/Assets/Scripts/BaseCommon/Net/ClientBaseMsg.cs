using System.Collections;
using System.Collections.Generic;
using  System;

using  Logic;

public enum CS_MSG_ID
{
    eMsg_ClientRequestServer = 101,
    eMsg_ServerRequestClient = 102,
};

// public class CS_ClientRequest: RequestPacket
// {
//     public override byte GetPacketID()
//     {
//         return (byte)CS_MSG_ID.eMsg_ClientRequestServer;
//         
//     }
//     
//     public UInt64 mTargetActorID
//     {
//         set { mMsgData.set("mTargetActorID", value); }
//         get { return (UInt64)mMsgData.get("mTargetActorID"); }
//     }
//     
//     public string mRequestMsgName
//     {
//         set { mMsgData.set("mRequestMsgName", value); }
//         get { return (string)mMsgData.get("mRequestMsgName"); }
//     }
//     
//     public DataBuffer mRequestMsgData
//     {
//         set { mMsgData.set("mRequestMsgData", value); }
//         get { return mMsgData.getObject("mRequestMsgData") as DataBuffer; }
//     }
// }
//
// public class SC_ResponseMsg : BasePacket
// {
//     public NiceData mResponseData
//     {
//         set { mMsgData.set("mResponseData;", value); }
//         get { return mMsgData.getObject("mResponseData;") as NiceData; }
//     }
// }