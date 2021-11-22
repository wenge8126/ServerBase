//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

#include "MsgStruct.h"

class g_register_login : public tBaseMsg
{ 
public:
    AString OPEN_TIME;		
    int SERVER_ID;		
    AString SERVER_NAME;		
    AString VERSION;		
    AString WSS_IP;		
    int WSS_PORT;		
    AString dbname;		
    AString dbname2;		
    int port;		
    int port2;		
    AString pwd;		
    AString pwd2;		
    AString url;		
    AString url2;		
    AString user;		
    AString user2;		

public:
    g_register_login() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, OPEN_TIME);
        CheckGet(scrData, SERVER_ID);
        CheckGet(scrData, SERVER_NAME);
        CheckGet(scrData, VERSION);
        CheckGet(scrData, WSS_IP);
        CheckGet(scrData, WSS_PORT);
        CheckGet(scrData, dbname);
        CheckGet(scrData, dbname2);
        CheckGet(scrData, port);
        CheckGet(scrData, port2);
        CheckGet(scrData, pwd);
        CheckGet(scrData, pwd2);
        CheckGet(scrData, url);
        CheckGet(scrData, url2);
        CheckGet(scrData, user);
        CheckGet(scrData, user2);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["OPEN_TIME"] = OPEN_TIME;
        destData["SERVER_ID"] = SERVER_ID;
        destData["SERVER_NAME"] = SERVER_NAME;
        destData["VERSION"] = VERSION;
        destData["WSS_IP"] = WSS_IP;
        destData["WSS_PORT"] = WSS_PORT;
        destData["dbname"] = dbname;
        destData["dbname2"] = dbname2;
        destData["port"] = port;
        destData["port2"] = port2;
        destData["pwd"] = pwd;
        destData["pwd2"] = pwd2;
        destData["url"] = url;
        destData["url2"] = url2;
        destData["user"] = user;
        destData["user2"] = user2;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)16);

        SAVE_MSG_VALUE(OPEN_TIME, 4);
        SAVE_MSG_VALUE(SERVER_ID, 1);
        SAVE_MSG_VALUE(SERVER_NAME, 4);
        SAVE_MSG_VALUE(VERSION, 4);
        SAVE_MSG_VALUE(WSS_IP, 4);
        SAVE_MSG_VALUE(WSS_PORT, 1);
        SAVE_MSG_VALUE(dbname, 4);
        SAVE_MSG_VALUE(dbname2, 4);
        SAVE_MSG_VALUE(port, 1);
        SAVE_MSG_VALUE(port2, 1);
        SAVE_MSG_VALUE(pwd, 4);
        SAVE_MSG_VALUE(pwd2, 4);
        SAVE_MSG_VALUE(url, 4);
        SAVE_MSG_VALUE(url2, 4);
        SAVE_MSG_VALUE(user, 4);
        SAVE_MSG_VALUE(user2, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        OPEN_TIME.setNull();
        SERVER_ID = 0;
        SERVER_NAME.setNull();
        VERSION.setNull();
        WSS_IP.setNull();
        WSS_PORT = 0;
        dbname.setNull();
        dbname2.setNull();
        port = 0;
        port2 = 0;
        pwd.setNull();
        pwd2.setNull();
        url.setNull();
        url2.setNull();
        user.setNull();
        user2.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("g_register_login", otherMsg.GetMsgName())!=0) { LOG("%s is not g_register_login", otherMsg.GetMsgName()); return; }; const g_register_login &other = *(const g_register_login*)(&otherMsg);
        OPEN_TIME = other.OPEN_TIME;
        SERVER_ID = other.SERVER_ID;
        SERVER_NAME = other.SERVER_NAME;
        VERSION = other.VERSION;
        WSS_IP = other.WSS_IP;
        WSS_PORT = other.WSS_PORT;
        dbname = other.dbname;
        dbname2 = other.dbname2;
        port = other.port;
        port2 = other.port2;
        pwd = other.pwd;
        pwd2 = other.pwd2;
        url = other.url;
        url2 = other.url2;
        user = other.user;
        user2 = other.user2;
    }

    virtual const char* GetMsgName() const override { return "g_register_login"; }

    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { AData value; value = OPEN_TIME; return value; }
        if (strcmp(szMember, "SERVER_ID")==0) { AData value; value = SERVER_ID; return value; }
        if (strcmp(szMember, "SERVER_NAME")==0) { AData value; value = SERVER_NAME; return value; }
        if (strcmp(szMember, "VERSION")==0) { AData value; value = VERSION; return value; }
        if (strcmp(szMember, "WSS_IP")==0) { AData value; value = WSS_IP; return value; }
        if (strcmp(szMember, "WSS_PORT")==0) { AData value; value = WSS_PORT; return value; }
        if (strcmp(szMember, "dbname")==0) { AData value; value = dbname; return value; }
        if (strcmp(szMember, "dbname2")==0) { AData value; value = dbname2; return value; }
        if (strcmp(szMember, "port")==0) { AData value; value = port; return value; }
        if (strcmp(szMember, "port2")==0) { AData value; value = port2; return value; }
        if (strcmp(szMember, "pwd")==0) { AData value; value = pwd; return value; }
        if (strcmp(szMember, "pwd2")==0) { AData value; value = pwd2; return value; }
        if (strcmp(szMember, "url")==0) { AData value; value = url; return value; }
        if (strcmp(szMember, "url2")==0) { AData value; value = url2; return value; }
        if (strcmp(szMember, "user")==0) { AData value; value = user; return value; }
        if (strcmp(szMember, "user2")==0) { AData value; value = user2; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { OPEN_TIME = value; return true; };
        if (strcmp(szMember, "SERVER_ID")==0) { SERVER_ID = value; return true; };
        if (strcmp(szMember, "SERVER_NAME")==0) { SERVER_NAME = value; return true; };
        if (strcmp(szMember, "VERSION")==0) { VERSION = value; return true; };
        if (strcmp(szMember, "WSS_IP")==0) { WSS_IP = value; return true; };
        if (strcmp(szMember, "WSS_PORT")==0) { WSS_PORT = value; return true; };
        if (strcmp(szMember, "dbname")==0) { dbname = value; return true; };
        if (strcmp(szMember, "dbname2")==0) { dbname2 = value; return true; };
        if (strcmp(szMember, "port")==0) { port = value; return true; };
        if (strcmp(szMember, "port2")==0) { port2 = value; return true; };
        if (strcmp(szMember, "pwd")==0) { pwd = value; return true; };
        if (strcmp(szMember, "pwd2")==0) { pwd2 = value; return true; };
        if (strcmp(szMember, "url")==0) { url = value; return true; };
        if (strcmp(szMember, "url2")==0) { url2 = value; return true; };
        if (strcmp(szMember, "user")==0) { user = value; return true; };
        if (strcmp(szMember, "user2")==0) { user2 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class r_register_login : public tBaseMsg
{ 
public:
    int mResult;		

public:
    r_register_login() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mResult);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mResult"] = mResult;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mResult, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mResult = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("r_register_login", otherMsg.GetMsgName())!=0) { LOG("%s is not r_register_login", otherMsg.GetMsgName()); return; }; const r_register_login &other = *(const r_register_login*)(&otherMsg);
        mResult = other.mResult;
    }

    virtual const char* GetMsgName() const override { return "r_register_login"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mResult")==0) { AData value; value = mResult; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mResult")==0) { mResult = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class g_notify_login_close : public tBaseMsg
{ 
public:
    AString OPEN_TIME;		
    int SERVER_ID;		
    AString SERVER_NAME;		
    AString VERSION;		
    AString WSS_IP;		
    int WSS_PORT;		

public:
    g_notify_login_close() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, OPEN_TIME);
        CheckGet(scrData, SERVER_ID);
        CheckGet(scrData, SERVER_NAME);
        CheckGet(scrData, VERSION);
        CheckGet(scrData, WSS_IP);
        CheckGet(scrData, WSS_PORT);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["OPEN_TIME"] = OPEN_TIME;
        destData["SERVER_ID"] = SERVER_ID;
        destData["SERVER_NAME"] = SERVER_NAME;
        destData["VERSION"] = VERSION;
        destData["WSS_IP"] = WSS_IP;
        destData["WSS_PORT"] = WSS_PORT;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_VALUE(OPEN_TIME, 4);
        SAVE_MSG_VALUE(SERVER_ID, 1);
        SAVE_MSG_VALUE(SERVER_NAME, 4);
        SAVE_MSG_VALUE(VERSION, 4);
        SAVE_MSG_VALUE(WSS_IP, 4);
        SAVE_MSG_VALUE(WSS_PORT, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        OPEN_TIME.setNull();
        SERVER_ID = 0;
        SERVER_NAME.setNull();
        VERSION.setNull();
        WSS_IP.setNull();
        WSS_PORT = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("g_notify_login_close", otherMsg.GetMsgName())!=0) { LOG("%s is not g_notify_login_close", otherMsg.GetMsgName()); return; }; const g_notify_login_close &other = *(const g_notify_login_close*)(&otherMsg);
        OPEN_TIME = other.OPEN_TIME;
        SERVER_ID = other.SERVER_ID;
        SERVER_NAME = other.SERVER_NAME;
        VERSION = other.VERSION;
        WSS_IP = other.WSS_IP;
        WSS_PORT = other.WSS_PORT;
    }

    virtual const char* GetMsgName() const override { return "g_notify_login_close"; }

    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { AData value; value = OPEN_TIME; return value; }
        if (strcmp(szMember, "SERVER_ID")==0) { AData value; value = SERVER_ID; return value; }
        if (strcmp(szMember, "SERVER_NAME")==0) { AData value; value = SERVER_NAME; return value; }
        if (strcmp(szMember, "VERSION")==0) { AData value; value = VERSION; return value; }
        if (strcmp(szMember, "WSS_IP")==0) { AData value; value = WSS_IP; return value; }
        if (strcmp(szMember, "WSS_PORT")==0) { AData value; value = WSS_PORT; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { OPEN_TIME = value; return true; };
        if (strcmp(szMember, "SERVER_ID")==0) { SERVER_ID = value; return true; };
        if (strcmp(szMember, "SERVER_NAME")==0) { SERVER_NAME = value; return true; };
        if (strcmp(szMember, "VERSION")==0) { VERSION = value; return true; };
        if (strcmp(szMember, "WSS_IP")==0) { WSS_IP = value; return true; };
        if (strcmp(szMember, "WSS_PORT")==0) { WSS_PORT = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class g_load_server_list : public tBaseMsg
{ 
public:

public:
    g_load_server_list() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
    }

    virtual void ToData(AutoNice &destData) override
    {
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)0);

        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("g_load_server_list", otherMsg.GetMsgName())!=0) { LOG("%s is not g_load_server_list", otherMsg.GetMsgName()); return; }; const g_load_server_list &other = *(const g_load_server_list*)(&otherMsg);
    }

    virtual const char* GetMsgName() const override { return "g_load_server_list"; }

    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;

    AData get(const char *szMember) const 
    {
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class ServerInfo : public tBaseMsg
{ 
public:
    AutoNice LOGIN_LIST;		
    AString NOTICE;		
    AString OPEN_TIME;		
    int SERVER_ID;		
    AString SERVER_NAME;		
    int STATE;		
    AString VERSION;		
    AString WSS_IP;		
    int WSS_PORT;		

public:
    ServerInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        LOGIN_LIST = (tNiceData*)scrData["LOGIN_LIST"];
        CheckGet(scrData, NOTICE);
        CheckGet(scrData, OPEN_TIME);
        CheckGet(scrData, SERVER_ID);
        CheckGet(scrData, SERVER_NAME);
        CheckGet(scrData, STATE);
        CheckGet(scrData, VERSION);
        CheckGet(scrData, WSS_IP);
        CheckGet(scrData, WSS_PORT);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["LOGIN_LIST"] = LOGIN_LIST.getPtr();
        destData["NOTICE"] = NOTICE;
        destData["OPEN_TIME"] = OPEN_TIME;
        destData["SERVER_ID"] = SERVER_ID;
        destData["SERVER_NAME"] = SERVER_NAME;
        destData["STATE"] = STATE;
        destData["VERSION"] = VERSION;
        destData["WSS_IP"] = WSS_IP;
        destData["WSS_PORT"] = WSS_PORT;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)9);

        SAVE_MSG_NICE(LOGIN_LIST);
        SAVE_MSG_VALUE(NOTICE, 4);
        SAVE_MSG_VALUE(OPEN_TIME, 4);
        SAVE_MSG_VALUE(SERVER_ID, 1);
        SAVE_MSG_VALUE(SERVER_NAME, 4);
        SAVE_MSG_VALUE(STATE, 1);
        SAVE_MSG_VALUE(VERSION, 4);
        SAVE_MSG_VALUE(WSS_IP, 4);
        SAVE_MSG_VALUE(WSS_PORT, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (LOGIN_LIST) LOGIN_LIST.setNull();
        NOTICE.setNull();
        OPEN_TIME.setNull();
        SERVER_ID = 0;
        SERVER_NAME.setNull();
        STATE = 0;
        VERSION.setNull();
        WSS_IP.setNull();
        WSS_PORT = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ServerInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not ServerInfo", otherMsg.GetMsgName()); return; }; const ServerInfo &other = *(const ServerInfo*)(&otherMsg);
        COPY_MSG_NICE(other.LOGIN_LIST, LOGIN_LIST);
        NOTICE = other.NOTICE;
        OPEN_TIME = other.OPEN_TIME;
        SERVER_ID = other.SERVER_ID;
        SERVER_NAME = other.SERVER_NAME;
        STATE = other.STATE;
        VERSION = other.VERSION;
        WSS_IP = other.WSS_IP;
        WSS_PORT = other.WSS_PORT;
    }

    virtual const char* GetMsgName() const override { return "ServerInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "NOTICE")==0) { AData value; value = NOTICE; return value; }
        if (strcmp(szMember, "OPEN_TIME")==0) { AData value; value = OPEN_TIME; return value; }
        if (strcmp(szMember, "SERVER_ID")==0) { AData value; value = SERVER_ID; return value; }
        if (strcmp(szMember, "SERVER_NAME")==0) { AData value; value = SERVER_NAME; return value; }
        if (strcmp(szMember, "STATE")==0) { AData value; value = STATE; return value; }
        if (strcmp(szMember, "VERSION")==0) { AData value; value = VERSION; return value; }
        if (strcmp(szMember, "WSS_IP")==0) { AData value; value = WSS_IP; return value; }
        if (strcmp(szMember, "WSS_PORT")==0) { AData value; value = WSS_PORT; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "NOTICE")==0) { NOTICE = value; return true; };
        if (strcmp(szMember, "OPEN_TIME")==0) { OPEN_TIME = value; return true; };
        if (strcmp(szMember, "SERVER_ID")==0) { SERVER_ID = value; return true; };
        if (strcmp(szMember, "SERVER_NAME")==0) { SERVER_NAME = value; return true; };
        if (strcmp(szMember, "STATE")==0) { STATE = value; return true; };
        if (strcmp(szMember, "VERSION")==0) { VERSION = value; return true; };
        if (strcmp(szMember, "WSS_IP")==0) { WSS_IP = value; return true; };
        if (strcmp(szMember, "WSS_PORT")==0) { WSS_PORT = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class r_server_list : public tBaseMsg
{ 
public:
    int mRecommendServerID;		//  推荐游戏区ID, 列表为空 为 -1
    ArrayList<ServerInfo> mServerList;	

public:
    r_server_list() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mRecommendServerID);
        FullStuctArray(scrData, mServerList, "mServerList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mRecommendServerID"] = mRecommendServerID;
        StuctArrayToData(destData, mServerList, "mServerList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mRecommendServerID, 1);
        SaveMsgArray("mServerList", mServerList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mRecommendServerID = 0;
        mServerList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("r_server_list", otherMsg.GetMsgName())!=0) { LOG("%s is not r_server_list", otherMsg.GetMsgName()); return; }; const r_server_list &other = *(const r_server_list*)(&otherMsg);
        mRecommendServerID = other.mRecommendServerID;
        CopyMsgArray(other.mServerList, mServerList);
    }

    virtual const char* GetMsgName() const override { return "r_server_list"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mRecommendServerID")==0) { AData value; value = mRecommendServerID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mRecommendServerID")==0) { mRecommendServerID = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class ld_load_server_info : public tBaseMsg
{ 
public:

public:
    ld_load_server_info() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
    }

    virtual void ToData(AutoNice &destData) override
    {
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)0);

        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ld_load_server_info", otherMsg.GetMsgName())!=0) { LOG("%s is not ld_load_server_info", otherMsg.GetMsgName()); return; }; const ld_load_server_info &other = *(const ld_load_server_info*)(&otherMsg);
    }

    virtual const char* GetMsgName() const override { return "ld_load_server_info"; }

    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;

    AData get(const char *szMember) const 
    {
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class r_load_server_info : public tBaseMsg
{ 
public:
    AString OPEN_TIME;		
    int SERVER_ID;		
    AString SERVER_NAME;		
    AString VERSION;		
    AString dbname;		
    AString dbname2;		
    int port;		
    int port2;		
    AString pwd;		
    AString pwd2;		
    AString url;		
    AString url2;		
    AString user;		
    AString user2;		

public:
    r_load_server_info() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, OPEN_TIME);
        CheckGet(scrData, SERVER_ID);
        CheckGet(scrData, SERVER_NAME);
        CheckGet(scrData, VERSION);
        CheckGet(scrData, dbname);
        CheckGet(scrData, dbname2);
        CheckGet(scrData, port);
        CheckGet(scrData, port2);
        CheckGet(scrData, pwd);
        CheckGet(scrData, pwd2);
        CheckGet(scrData, url);
        CheckGet(scrData, url2);
        CheckGet(scrData, user);
        CheckGet(scrData, user2);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["OPEN_TIME"] = OPEN_TIME;
        destData["SERVER_ID"] = SERVER_ID;
        destData["SERVER_NAME"] = SERVER_NAME;
        destData["VERSION"] = VERSION;
        destData["dbname"] = dbname;
        destData["dbname2"] = dbname2;
        destData["port"] = port;
        destData["port2"] = port2;
        destData["pwd"] = pwd;
        destData["pwd2"] = pwd2;
        destData["url"] = url;
        destData["url2"] = url2;
        destData["user"] = user;
        destData["user2"] = user2;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)14);

        SAVE_MSG_VALUE(OPEN_TIME, 4);
        SAVE_MSG_VALUE(SERVER_ID, 1);
        SAVE_MSG_VALUE(SERVER_NAME, 4);
        SAVE_MSG_VALUE(VERSION, 4);
        SAVE_MSG_VALUE(dbname, 4);
        SAVE_MSG_VALUE(dbname2, 4);
        SAVE_MSG_VALUE(port, 1);
        SAVE_MSG_VALUE(port2, 1);
        SAVE_MSG_VALUE(pwd, 4);
        SAVE_MSG_VALUE(pwd2, 4);
        SAVE_MSG_VALUE(url, 4);
        SAVE_MSG_VALUE(url2, 4);
        SAVE_MSG_VALUE(user, 4);
        SAVE_MSG_VALUE(user2, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        OPEN_TIME.setNull();
        SERVER_ID = 0;
        SERVER_NAME.setNull();
        VERSION.setNull();
        dbname.setNull();
        dbname2.setNull();
        port = 0;
        port2 = 0;
        pwd.setNull();
        pwd2.setNull();
        url.setNull();
        url2.setNull();
        user.setNull();
        user2.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("r_load_server_info", otherMsg.GetMsgName())!=0) { LOG("%s is not r_load_server_info", otherMsg.GetMsgName()); return; }; const r_load_server_info &other = *(const r_load_server_info*)(&otherMsg);
        OPEN_TIME = other.OPEN_TIME;
        SERVER_ID = other.SERVER_ID;
        SERVER_NAME = other.SERVER_NAME;
        VERSION = other.VERSION;
        dbname = other.dbname;
        dbname2 = other.dbname2;
        port = other.port;
        port2 = other.port2;
        pwd = other.pwd;
        pwd2 = other.pwd2;
        url = other.url;
        url2 = other.url2;
        user = other.user;
        user2 = other.user2;
    }

    virtual const char* GetMsgName() const override { return "r_load_server_info"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { AData value; value = OPEN_TIME; return value; }
        if (strcmp(szMember, "SERVER_ID")==0) { AData value; value = SERVER_ID; return value; }
        if (strcmp(szMember, "SERVER_NAME")==0) { AData value; value = SERVER_NAME; return value; }
        if (strcmp(szMember, "VERSION")==0) { AData value; value = VERSION; return value; }
        if (strcmp(szMember, "dbname")==0) { AData value; value = dbname; return value; }
        if (strcmp(szMember, "dbname2")==0) { AData value; value = dbname2; return value; }
        if (strcmp(szMember, "port")==0) { AData value; value = port; return value; }
        if (strcmp(szMember, "port2")==0) { AData value; value = port2; return value; }
        if (strcmp(szMember, "pwd")==0) { AData value; value = pwd; return value; }
        if (strcmp(szMember, "pwd2")==0) { AData value; value = pwd2; return value; }
        if (strcmp(szMember, "url")==0) { AData value; value = url; return value; }
        if (strcmp(szMember, "url2")==0) { AData value; value = url2; return value; }
        if (strcmp(szMember, "user")==0) { AData value; value = user; return value; }
        if (strcmp(szMember, "user2")==0) { AData value; value = user2; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { OPEN_TIME = value; return true; };
        if (strcmp(szMember, "SERVER_ID")==0) { SERVER_ID = value; return true; };
        if (strcmp(szMember, "SERVER_NAME")==0) { SERVER_NAME = value; return true; };
        if (strcmp(szMember, "VERSION")==0) { VERSION = value; return true; };
        if (strcmp(szMember, "dbname")==0) { dbname = value; return true; };
        if (strcmp(szMember, "dbname2")==0) { dbname2 = value; return true; };
        if (strcmp(szMember, "port")==0) { port = value; return true; };
        if (strcmp(szMember, "port2")==0) { port2 = value; return true; };
        if (strcmp(szMember, "pwd")==0) { pwd = value; return true; };
        if (strcmp(szMember, "pwd2")==0) { pwd2 = value; return true; };
        if (strcmp(szMember, "url")==0) { url = value; return true; };
        if (strcmp(szMember, "url2")==0) { url2 = value; return true; };
        if (strcmp(szMember, "user")==0) { user = value; return true; };
        if (strcmp(szMember, "user2")==0) { user2 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  系统公告推送消息
class LC_NotifySystemNotice : public tBaseMsg
{ 
public:
    int channel;		//  渠道
    AString content;		//  公告文本
    Int64 enddate;		//  结束时间
    int gap;		//  间隔多少秒重新显示

public:
    LC_NotifySystemNotice() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, channel);
        CheckGet(scrData, content);
        CheckGet(scrData, enddate);
        CheckGet(scrData, gap);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["channel"] = channel;
        destData["content"] = content;
        destData["enddate"] = enddate;
        destData["gap"] = gap;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(channel, 1);
        SAVE_MSG_VALUE(content, 4);
        SAVE_MSG_VALUE(enddate, 8);
        SAVE_MSG_VALUE(gap, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        channel = 0;
        content.setNull();
        enddate = 0;
        gap = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LC_NotifySystemNotice", otherMsg.GetMsgName())!=0) { LOG("%s is not LC_NotifySystemNotice", otherMsg.GetMsgName()); return; }; const LC_NotifySystemNotice &other = *(const LC_NotifySystemNotice*)(&otherMsg);
        channel = other.channel;
        content = other.content;
        enddate = other.enddate;
        gap = other.gap;
    }

    virtual const char* GetMsgName() const override { return "LC_NotifySystemNotice"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "channel")==0) { AData value; value = channel; return value; }
        if (strcmp(szMember, "content")==0) { AData value; value = content; return value; }
        if (strcmp(szMember, "enddate")==0) { AData value; value = enddate; return value; }
        if (strcmp(szMember, "gap")==0) { AData value; value = gap; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "channel")==0) { channel = value; return true; };
        if (strcmp(szMember, "content")==0) { content = value; return true; };
        if (strcmp(szMember, "enddate")==0) { enddate = value; return true; };
        if (strcmp(szMember, "gap")==0) { gap = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

