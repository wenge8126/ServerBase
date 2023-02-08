#include "HttpPostNet.h"

#include "DataBuffer.h"
#include "PacketFactory.h"
#include "EventProtocol.h"
#include "BaseMsg.h"
#include "ServerToolMsg.h"

#include "curl.h"
#include <string>
#include "Common.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")

static size_t OnWriteBytesData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	DataStream *responseData = (DataStream*)lpVoid;
	if (responseData == NULL || NULL == buffer)
	{
		return -1;
	}

	responseData->_write(buffer, size*nmemb);

	return nmemb;
}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if (itype == CURLINFO_TEXT)
	{
		//PRINT("[TEXT]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_IN)
	{
		PRINT("[HEADER_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_OUT)
	{
		PRINT("[HEADER_OUT]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_IN)
	{
		PRINT("[DATA_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_OUT)
	{
		PRINT("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

int PostBytes(const char *strUrl, DataStream *postData, DataStream *responseData, int connectOverSecond, int overSecond, const char * pCaPath /*= NULL*/)
{
	responseData->clear(false);

	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (false)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData->dataSize());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData->data());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteBytesData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)responseData);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectOverSecond);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, overSecond);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	AString* str = dynamic_cast<AString*>((AString *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->set(pData, size*nmemb);
	return nmemb;
}

int Gets(const char *strUrl, AString &strResponse, bool bDebug, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}


int Posts(const char *strUrl, const AString & strPost, AString & strResponse, bool bDebug, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}



class TaskRequestString : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		int re = 0;
		if (mbPost)
			re = Posts(mUrl.c_str(), mReuestParam, mResponseString, false, NULL);
		else
		{
			AString str = mUrl;
			str += "/?";
			str += mReuestParam;
			re = Gets(str.c_str(), mResponseString, false, NULL);
		}
		 
		mbSucceed = re == 0 || re == 56;
		if (!mbSucceed)
			DEBUG_LOG("Http reqeust %s fail %d> %s > %s ", mbPost ? "POST":"GET",  re, mUrl.c_str(), mReuestParam.c_str());
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{
		
	}

	virtual void InitFree() override
	{
		mUrl.setNull();
		mReuestParam.setNull();
		mResponseString.setNull();
		mbSucceed = false;
		mbPost = false;
	}

	TaskRequestString()
		: mbSucceed(false) {}

public:
	AString			mUrl;
	AString			mReuestParam;
	AString			mResponseString;
	bool					mbSucceed;
	bool				mbPost = false;
};


class TaskRequestBytes : public Task
{
public:
	virtual void Execute(TaskManager *pTaskMgr)
	{
		int re = PostBytes(mUrl.c_str(), &mRequestData, &mResponseData, mConnectOverSecond, mOverSecond, NULL);
		mbSucceed = re == 0 || re == 56;
		if (!mbSucceed)
			DEBUG_LOG("Http post bytes fail %d> %s ", re, mUrl.c_str());
	}
	virtual void OnFinished(TaskManager *pTaskMgr)
	{

	}

	virtual void InitFree() override
	{
		mUrl.setNull();
		mRequestData.clear();
		mResponseData.clear();
		mbSucceed = false;		
	}

	TaskRequestBytes()
		: mbSucceed(false) {}

public:
	AString			mUrl;
	DataBuffer	mRequestData;
	DataBuffer	mResponseData;
	bool				mbSucceed;
	int					mConnectOverSecond = 6;
	int					mOverSecond = 30;
};


HttpPostNet::HttpPostNet()
{
	RegisterTask(MEM_NEW DefineTaskFactory<TaskRequestString, eTaskRequestString>);
	RegisterTask(MEM_NEW DefineTaskFactory<TaskRequestBytes, eTaskReqeustPostBytesMsg>);
}


AutoNice HttpPostNet::AwaitRequest(AutoData token, const char *szUrl, int actorType, Int64 actorID, tBaseMsg *packet, int connectOverSecond, int overSecond, const char *pCaPath)
{
	Auto<TaskRequestBytes> task = StartTask(eTaskReqeustPostBytesMsg);
	task->mRequestData.clear();

	Auto<HttpReqeustActorMsg> msg = MEM_NEW HttpReqeustActorMsg();
	msg->mActorID = actorID;
	msg->mActorType = actorType;
	msg->mToken = token;

	msg->mMsgName = packet->GetMsgName();
	msg->mRequestMsgData = MEM_NEW DataBuffer();

	AutoData sendData = MEM_NEW DataBuffer();


	if (packet->Write(*msg->mRequestMsgData) == FALSE)
	{
		ERROR_LOG("%s packet write data fail", packet->GetMsgName());
		return AutoNice();
	}

	EventNetProtocol  pro;
	if (!pro.WritePacket(eMsg_ClientRequestServer, msg.getPtr(), &task->mRequestData))
	{
		ERROR_LOG("Event net protoco %s packet write to net stream fail", packet->GetMsgName());
		return AutoNice();
	}

	task->mUrl = szUrl;
	task->mConnectOverSecond = connectOverSecond;
	task->mOverSecond = overSecond;

	task->AWait();
	if (task->mbSucceed)
	{
		AutoNice d = MEM_NEW NiceData();
		task->mResponseData.seek(0);
		if (d->restore(&task->mResponseData))
			return d;
		ERROR_LOG("Reqeust bytes data restore fail : %s", szUrl);
	}
	return AutoNice();
}

AString HttpPostNet::AwaitRequestGet(const char *szUrl, const AString &paramString, int connectOverSecond, int overSecond, const char *pCaPath)
{
	Auto<TaskRequestString> task = StartTask(eTaskRequestString);

	task->mUrl = szUrl;
	task->mReuestParam = paramString;
	//task->mConnectOverSecond = connectOverSecond;
	//task->mOverSecond = overSecond;
	task->mbPost = false;

	task->AWait();
	if (task->mbSucceed)
	{
		return task->mResponseString;
	}
	return AString();
}
