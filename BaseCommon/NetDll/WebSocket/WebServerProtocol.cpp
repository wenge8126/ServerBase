#include "WebServerProtocol.h"
#include "Common.h"

bool WebConnect::SendMsgData(DataStream *pSendMsgData, int frameType)
{
    //mSendFramData.clear(false);
	Auto<WebSendPacket> p = GetNetHandle()->GetNetProtocol()->CreatePacket(DEFALUT_WEB_SEND_PACKET_ID);
	p->mData.clear();
    if (WebProtocol::wsEncodeFrame(pSendMsgData, &p->mData, (WS_FrameType)frameType)>0)
    {
        //if (!mSendData._write((void*)mSendFramData.data(), mSendFramData.dataSize()))
		if (!Send(DEFALUT_WEB_SEND_PACKET_ID, p.getPtr()))
		{
			ERROR_LOG("Write msg data fail, %d more then send buffer size %d", (int)p->mData.dataSize(), (int)mSendLoopData.size());
			return false;
		}
        return true;
    }
    ERROR_LOG("加密WEB消息数据失败");
    return false;
}

void WebConnect::_ProcessReceiveData()
{
    if (!mbReceiveData)
    {
        if (mReceiveLoopData.dataSize()<=0)
        {
            return;
        }
        // NOTE: HTTP   连接握手
        std::string request = mReceiveLoopData.data();
        std::string response;
        if (WebProtocol::wsHandshake(request, response)==WS_STATUS_CONNECT)
        {
			mReceiveLoopData.skip(request.length());
			mReceiveLoopData.SendData((char*)response.c_str(), response.length());
            //NOTE_LOG("%s", request.c_str());
            //NOTE_LOG("----------------------%d\r\n%s", (int)response.length(), response.c_str());
            mbReceiveData = true;
            //mbHaveRevData = false;
            mNet->OnAddConnect(this);
            OnConnected();
            //mRevFrameData.clear(false);
            //mRevFrameData.write((ushort)0xFFFF);
            //SendMsgData(&mRevFrameData, WS_BINARY_FRAME);
            return;
        }
//        else
//        {	
//#if DEVELOP_MODE
//            ERROR_LOG("WARN: [%s : %d] Connect request error , then now remove, request >%s", GetIp(), GetPort(), request.c_str());
//#else
//            DEBUG_LOG("WARN: [%s : %d] Connect request error , then now remove, request >%s", GetIp(), GetPort(), request.c_str());
//#endif                
//            OnConnectSafeCodeError(GetIp(), GetPort(), 0);
//            SetRemove(true);
//        }
    }
    else
    {
        if (mReceiveLoopData.dataSize()<=0)
                return;
		DSIZE nowDataSize = mReceiveLoopData.dataSize();
            if (mTempRevBuffer.size()< nowDataSize)
                mTempRevBuffer.resize(nowDataSize +128);
        mTempRevBuffer.clear(false);
        if (mReceiveLoopData.peek(mTempRevBuffer.data(), nowDataSize))
        {            
            mTempRevBuffer.setDataSize(nowDataSize);
            do
            {
                WS_FrameType frameType = WS_EMPTY_FRAME;
                bool bLastFrame = true;
                bool bMask = true;
                mRevFrameData.clear(false);
                int dataSize = mTempRevBuffer.dataSize();
                int msgUseLen = WebProtocol::wsDecodeFrame(&mTempRevBuffer, &mRevFrameData, frameType, bLastFrame, bMask);
                if (dataSize<msgUseLen)
                    break;
                if (frameType==WS_CLOSING_FRAME)
                {
                    mRevFrameData.clear(false);
                    SendMsgData(&mRevFrameData, WS_CLOSING_FRAME);
					mReceiveLoopData.skip(msgUseLen);
                  
                    //DEBUG_LOG("[%s:%d] Web client request close, now remove connect", GetIp(), GetPort());
                    SetRemove(true);
                    return;
                }
                if (msgUseLen>0)
                {
                    mTempRevBuffer.seek(mTempRevBuffer.tell()+msgUseLen);
                  
                    if (mReceiveLoopData.skip(msgUseLen)==FALSE)
                    {
                        ERROR_LOG(" Web skip seek fail >%d", msgUseLen);
                        break;
                    }
                    if (!bMask)
                    {
                        ERROR_LOG("[%s:%d] WEB server must receive mask msg", GetIp(), GetPort());        
                        SetRemove(true);
                        return;
                    }
                    if (!bLastFrame)
                    {
                        ERROR_LOG("Now can do part frame function");
                        continue;
                    }
                    //static int x = 0;
                    //NOTE_LOG(" *** Rev %d size %d, pos %d, msgLen %d, data size %d", ++x, mRevFrameData.dataSize(), mTempRevBuffer.tell()-msgUseLen, msgUseLen, dataSize);
                    OnReceiveFrame(&mRevFrameData, frameType, bLastFrame);
                }
                else
                    break;
            } while (mTempRevBuffer.lastDataSize()>0);
        }
    }
}
