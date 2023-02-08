//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

//  网络地址
class IPAddress : public tBaseMsg
{ 
public:
    AString ip;		//  IP地址,支持域名
    int port;		//  端口
    int saft_code;		//  安全码, 用于避免网络连接混乱及规避未知连接接入

public:
    IPAddress() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, ip);
        CheckGet(scrData, port);
        CheckGet(scrData, saft_code);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["ip"] = ip;
        destData["port"] = port;
        destData["saft_code"] = saft_code;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(ip, 4);
        SAVE_MSG_VALUE(port, 1);
        SAVE_MSG_VALUE(saft_code, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        ip.setNull();
        port = 0;
        saft_code = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("IPAddress", otherMsg.GetMsgName())!=0) { LOG("%s is not IPAddress", otherMsg.GetMsgName()); return; }; const IPAddress &other = *(const IPAddress*)(&otherMsg);
        ip = other.ip;
        port = other.port;
        saft_code = other.saft_code;
    }

    virtual const char* GetMsgName() const override { return "IPAddress"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "ip")==0) { AData value; value = ip; return value; }
        if (strcmp(szMember, "port")==0) { AData value; value = port; return value; }
        if (strcmp(szMember, "saft_code")==0) { AData value; value = saft_code; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "ip")==0) { ip = value; return true; };
        if (strcmp(szMember, "port")==0) { port = value; return true; };
        if (strcmp(szMember, "saft_code")==0) { saft_code = value; return true; };
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

//  GateNode 用于Gate之间组网
class GateNode : public tBaseMsg
{ 
public:
    IPAddress address;		//  GateNode地址 
    int fix_count;		//  固定Gate数量
    int gate_code;		//  编号(0~fix_count-1)

public:
    GateNode() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["address"];
            if (d) address.Full(d); else LOG("No exist data address");
        }
        CheckGet(scrData, fix_count);
        CheckGet(scrData, gate_code);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "IPAddress"; address.ToData(d);
            destData["address"] = d.getPtr();
        }
        destData["fix_count"] = fix_count;
        destData["gate_code"] = gate_code;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_STRUCT(address);
        SAVE_MSG_VALUE(fix_count, 1);
        SAVE_MSG_VALUE(gate_code, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        address.clear(false);
        fix_count = 0;
        gate_code = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GateNode", otherMsg.GetMsgName())!=0) { LOG("%s is not GateNode", otherMsg.GetMsgName()); return; }; const GateNode &other = *(const GateNode*)(&otherMsg);
        address.copy(other.address);
        fix_count = other.fix_count;
        gate_code = other.gate_code;
    }

    virtual const char* GetMsgName() const override { return "GateNode"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "fix_count")==0) { AData value; value = fix_count; return value; }
        if (strcmp(szMember, "gate_code")==0) { AData value; value = gate_code; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "fix_count")==0) { fix_count = value; return true; };
        if (strcmp(szMember, "gate_code")==0) { gate_code = value; return true; };
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

//  CloudeGate 程序配置
class GateServerConfig : public tBaseMsg
{ 
public:
    GateNode gate_node;		//  GateNode 用于Gate之间组网
    IPAddress gate_server;		//  Gate接受单元节点连接开放的服务地址
    IPAddress main_gate;		//  Gate接受单元节点连接开放的服务地址
    AString title;		//  程序运行的标题

public:
    GateServerConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["gate_node"];
            if (d) gate_node.Full(d); else LOG("No exist data gate_node");
        }
        {
            AutoNice d = (tNiceData*)scrData["gate_server"];
            if (d) gate_server.Full(d); else LOG("No exist data gate_server");
        }
        {
            AutoNice d = (tNiceData*)scrData["main_gate"];
            if (d) main_gate.Full(d); else LOG("No exist data main_gate");
        }
        CheckGet(scrData, title);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "GateNode"; gate_node.ToData(d);
            destData["gate_node"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "IPAddress"; gate_server.ToData(d);
            destData["gate_server"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "IPAddress"; main_gate.ToData(d);
            destData["main_gate"] = d.getPtr();
        }
        destData["title"] = title;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_STRUCT(gate_node);
        SAVE_MSG_STRUCT(gate_server);
        SAVE_MSG_STRUCT(main_gate);
        SAVE_MSG_VALUE(title, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        gate_node.clear(false);
        gate_server.clear(false);
        main_gate.clear(false);
        title.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GateServerConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not GateServerConfig", otherMsg.GetMsgName()); return; }; const GateServerConfig &other = *(const GateServerConfig*)(&otherMsg);
        gate_node.copy(other.gate_node);
        gate_server.copy(other.gate_server);
        main_gate.copy(other.main_gate);
        title = other.title;
    }

    virtual const char* GetMsgName() const override { return "GateServerConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "title")==0) { title = value; return true; };
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

//  共享内存参数
class ShareMem : public tBaseMsg
{ 
public:
    int key;		//  共享内存分配KEY,用于共享内存消息通道
    int size;		//  共享内存分配大小,推荐配置 2100000 约2M,收发各1M

public:
    ShareMem() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, key);
        CheckGet(scrData, size);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["key"] = key;
        destData["size"] = size;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(key, 1);
        SAVE_MSG_VALUE(size, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        key = 0;
        size = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ShareMem", otherMsg.GetMsgName())!=0) { LOG("%s is not ShareMem", otherMsg.GetMsgName()); return; }; const ShareMem &other = *(const ShareMem*)(&otherMsg);
        key = other.key;
        size = other.size;
    }

    virtual const char* GetMsgName() const override { return "ShareMem"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "key")==0) { AData value; value = key; return value; }
        if (strcmp(szMember, "size")==0) { AData value; value = size; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "key")==0) { key = value; return true; };
        if (strcmp(szMember, "size")==0) { size = value; return true; };
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

class SqlDB : public tBaseMsg
{ 
public:
    AString mDBBASE;		//  DB连接库, 可以为空, 表示连接 test
    AString mDBIP;		//  Mysql 地址
    AString mDBPASSWORD;		//  连接密码
    int mDBPORT;		//  Mysql 端口
    AString mDBUSER;		//  Mysql用户名
    AString mTABLE_LIST;		//  表格加载列表 (创建DB表格时, 自动生成)

public:
    SqlDB() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBBASE);
        CheckGet(scrData, mDBIP);
        CheckGet(scrData, mDBPASSWORD);
        CheckGet(scrData, mDBPORT);
        CheckGet(scrData, mDBUSER);
        CheckGet(scrData, mTABLE_LIST);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBBASE"] = mDBBASE;
        destData["mDBIP"] = mDBIP;
        destData["mDBPASSWORD"] = mDBPASSWORD;
        destData["mDBPORT"] = mDBPORT;
        destData["mDBUSER"] = mDBUSER;
        destData["mTABLE_LIST"] = mTABLE_LIST;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_VALUE(mDBBASE, 4);
        SAVE_MSG_VALUE(mDBIP, 4);
        SAVE_MSG_VALUE(mDBPASSWORD, 4);
        SAVE_MSG_VALUE(mDBPORT, 1);
        SAVE_MSG_VALUE(mDBUSER, 4);
        SAVE_MSG_VALUE(mTABLE_LIST, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBBASE.setNull();
        mDBIP.setNull();
        mDBPASSWORD.setNull();
        mDBPORT = 0;
        mDBUSER.setNull();
        mTABLE_LIST.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SqlDB", otherMsg.GetMsgName())!=0) { LOG("%s is not SqlDB", otherMsg.GetMsgName()); return; }; const SqlDB &other = *(const SqlDB*)(&otherMsg);
        mDBBASE = other.mDBBASE;
        mDBIP = other.mDBIP;
        mDBPASSWORD = other.mDBPASSWORD;
        mDBPORT = other.mDBPORT;
        mDBUSER = other.mDBUSER;
        mTABLE_LIST = other.mTABLE_LIST;
    }

    virtual const char* GetMsgName() const override { return "SqlDB"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBBASE")==0) { AData value; value = mDBBASE; return value; }
        if (strcmp(szMember, "mDBIP")==0) { AData value; value = mDBIP; return value; }
        if (strcmp(szMember, "mDBPASSWORD")==0) { AData value; value = mDBPASSWORD; return value; }
        if (strcmp(szMember, "mDBPORT")==0) { AData value; value = mDBPORT; return value; }
        if (strcmp(szMember, "mDBUSER")==0) { AData value; value = mDBUSER; return value; }
        if (strcmp(szMember, "mTABLE_LIST")==0) { AData value; value = mTABLE_LIST; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBBASE")==0) { mDBBASE = value; return true; };
        if (strcmp(szMember, "mDBIP")==0) { mDBIP = value; return true; };
        if (strcmp(szMember, "mDBPASSWORD")==0) { mDBPASSWORD = value; return true; };
        if (strcmp(szMember, "mDBPORT")==0) { mDBPORT = value; return true; };
        if (strcmp(szMember, "mDBUSER")==0) { mDBUSER = value; return true; };
        if (strcmp(szMember, "mTABLE_LIST")==0) { mTABLE_LIST = value; return true; };
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

//  共享内存进程配置
class ShareMemDBConfig : public tBaseMsg
{ 
public:
    ShareMem share_key;		//  共享内存配置
    SqlDB sql_back_db;		//  MYSQL备份库连接配置
    SqlDB sql_db;		//  MYSQL连接配置
    AString title;		//  程序运行的标题

public:
    ShareMemDBConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["share_key"];
            if (d) share_key.Full(d); else LOG("No exist data share_key");
        }
        {
            AutoNice d = (tNiceData*)scrData["sql_back_db"];
            if (d) sql_back_db.Full(d); else LOG("No exist data sql_back_db");
        }
        {
            AutoNice d = (tNiceData*)scrData["sql_db"];
            if (d) sql_db.Full(d); else LOG("No exist data sql_db");
        }
        CheckGet(scrData, title);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "ShareMem"; share_key.ToData(d);
            destData["share_key"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "SqlDB"; sql_back_db.ToData(d);
            destData["sql_back_db"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "SqlDB"; sql_db.ToData(d);
            destData["sql_db"] = d.getPtr();
        }
        destData["title"] = title;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_STRUCT(share_key);
        SAVE_MSG_STRUCT(sql_back_db);
        SAVE_MSG_STRUCT(sql_db);
        SAVE_MSG_VALUE(title, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        share_key.clear(false);
        sql_back_db.clear(false);
        sql_db.clear(false);
        title.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ShareMemDBConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not ShareMemDBConfig", otherMsg.GetMsgName()); return; }; const ShareMemDBConfig &other = *(const ShareMemDBConfig*)(&otherMsg);
        share_key.copy(other.share_key);
        sql_back_db.copy(other.sql_back_db);
        sql_db.copy(other.sql_db);
        title = other.title;
    }

    virtual const char* GetMsgName() const override { return "ShareMemDBConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "title")==0) { title = value; return true; };
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

//  云单元用户节点
class UserNode : public tBaseMsg
{ 
public:
    IPAddress gate;		//  连接的Gate服务地址
    IPAddress node;		//  用户节点当前分配地址

public:
    UserNode() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["gate"];
            if (d) gate.Full(d); else LOG("No exist data gate");
        }
        {
            AutoNice d = (tNiceData*)scrData["node"];
            if (d) node.Full(d); else LOG("No exist data node");
        }
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "IPAddress"; gate.ToData(d);
            destData["gate"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "IPAddress"; node.ToData(d);
            destData["node"] = d.getPtr();
        }
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_STRUCT(gate);
        SAVE_MSG_STRUCT(node);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        gate.clear(false);
        node.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("UserNode", otherMsg.GetMsgName())!=0) { LOG("%s is not UserNode", otherMsg.GetMsgName()); return; }; const UserNode &other = *(const UserNode*)(&otherMsg);
        gate.copy(other.gate);
        node.copy(other.node);
    }

    virtual const char* GetMsgName() const override { return "UserNode"; }

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

//  HttpWeb服务
class HttpWeb : public tBaseMsg
{ 
public:
    AString cert_file;		//  证书文件
    AString http_address;		//  开放HTTP地址(用于提供给连接进程)
    AString key_file;		//  证书key.pem文件
    AString password;		//  证书密码
    int port;		//  开放端口

public:
    HttpWeb() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, cert_file);
        CheckGet(scrData, http_address);
        CheckGet(scrData, key_file);
        CheckGet(scrData, password);
        CheckGet(scrData, port);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["cert_file"] = cert_file;
        destData["http_address"] = http_address;
        destData["key_file"] = key_file;
        destData["password"] = password;
        destData["port"] = port;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_VALUE(cert_file, 4);
        SAVE_MSG_VALUE(http_address, 4);
        SAVE_MSG_VALUE(key_file, 4);
        SAVE_MSG_VALUE(password, 4);
        SAVE_MSG_VALUE(port, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        cert_file.setNull();
        http_address.setNull();
        key_file.setNull();
        password.setNull();
        port = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("HttpWeb", otherMsg.GetMsgName())!=0) { LOG("%s is not HttpWeb", otherMsg.GetMsgName()); return; }; const HttpWeb &other = *(const HttpWeb*)(&otherMsg);
        cert_file = other.cert_file;
        http_address = other.http_address;
        key_file = other.key_file;
        password = other.password;
        port = other.port;
    }

    virtual const char* GetMsgName() const override { return "HttpWeb"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "cert_file")==0) { AData value; value = cert_file; return value; }
        if (strcmp(szMember, "http_address")==0) { AData value; value = http_address; return value; }
        if (strcmp(szMember, "key_file")==0) { AData value; value = key_file; return value; }
        if (strcmp(szMember, "password")==0) { AData value; value = password; return value; }
        if (strcmp(szMember, "port")==0) { AData value; value = port; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "cert_file")==0) { cert_file = value; return true; };
        if (strcmp(szMember, "http_address")==0) { http_address = value; return true; };
        if (strcmp(szMember, "key_file")==0) { key_file = value; return true; };
        if (strcmp(szMember, "password")==0) { password = value; return true; };
        if (strcmp(szMember, "port")==0) { port = value; return true; };
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

class DBConfig : public tBaseMsg
{ 
public:
    int db_code;		//  DB编号, 0时为主DB, 提供DB表格汇总服务
    UserNode db_node;		//  DB云节点分配地址(与其他连接在Gate上的节点互连)
    int record_update_milsecond;		//  记录检查更新落地的间隔毫秒时间, 用于调整DB整体性能, 此时间越大, 性能越高, 但安全落地风险越大 (推荐值: 1000~10000)
    ShareMem share_key;		//  DB数据库连接的共享内存配置, Mysql 信息通过消息从对应共享内存进程获取
    int worker_id;		//  辅助工具单元ID

public:
    DBConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, db_code);
        {
            AutoNice d = (tNiceData*)scrData["db_node"];
            if (d) db_node.Full(d); else LOG("No exist data db_node");
        }
        CheckGet(scrData, record_update_milsecond);
        {
            AutoNice d = (tNiceData*)scrData["share_key"];
            if (d) share_key.Full(d); else LOG("No exist data share_key");
        }
        CheckGet(scrData, worker_id);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["db_code"] = db_code;
        {
            AutoNice d = destData->NewNice(); d->msKey = "UserNode"; db_node.ToData(d);
            destData["db_node"] = d.getPtr();
        }
        destData["record_update_milsecond"] = record_update_milsecond;
        {
            AutoNice d = destData->NewNice(); d->msKey = "ShareMem"; share_key.ToData(d);
            destData["share_key"] = d.getPtr();
        }
        destData["worker_id"] = worker_id;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_VALUE(db_code, 1);
        SAVE_MSG_STRUCT(db_node);
        SAVE_MSG_VALUE(record_update_milsecond, 1);
        SAVE_MSG_STRUCT(share_key);
        SAVE_MSG_VALUE(worker_id, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        db_code = 0;
        db_node.clear(false);
        record_update_milsecond = 0;
        share_key.clear(false);
        worker_id = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not DBConfig", otherMsg.GetMsgName()); return; }; const DBConfig &other = *(const DBConfig*)(&otherMsg);
        db_code = other.db_code;
        db_node.copy(other.db_node);
        record_update_milsecond = other.record_update_milsecond;
        share_key.copy(other.share_key);
        worker_id = other.worker_id;
    }

    virtual const char* GetMsgName() const override { return "DBConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "db_code")==0) { AData value; value = db_code; return value; }
        if (strcmp(szMember, "record_update_milsecond")==0) { AData value; value = record_update_milsecond; return value; }
        if (strcmp(szMember, "worker_id")==0) { AData value; value = worker_id; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "db_code")==0) { db_code = value; return true; };
        if (strcmp(szMember, "record_update_milsecond")==0) { record_update_milsecond = value; return true; };
        if (strcmp(szMember, "worker_id")==0) { worker_id = value; return true; };
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

//  AccountDB 帐号DB进程配置
class AccountServerConfig : public tBaseMsg
{ 
public:
    int account_id;		//  帐号服务器编号, 用于多帐号服务器负载
    UserNode actor_node;		
    AString title;		//  程序运行的标题	
    HttpWeb web_net;		//  WebHttpNet开放给客户端使用http请求创建或验证帐号
    HttpWeb wss_address;		//  Wss 网络地址

public:
    AccountServerConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, account_id);
        {
            AutoNice d = (tNiceData*)scrData["actor_node"];
            if (d) actor_node.Full(d); else LOG("No exist data actor_node");
        }
        CheckGet(scrData, title);
        {
            AutoNice d = (tNiceData*)scrData["web_net"];
            if (d) web_net.Full(d); else LOG("No exist data web_net");
        }
        {
            AutoNice d = (tNiceData*)scrData["wss_address"];
            if (d) wss_address.Full(d); else LOG("No exist data wss_address");
        }
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["account_id"] = account_id;
        {
            AutoNice d = destData->NewNice(); d->msKey = "UserNode"; actor_node.ToData(d);
            destData["actor_node"] = d.getPtr();
        }
        destData["title"] = title;
        {
            AutoNice d = destData->NewNice(); d->msKey = "HttpWeb"; web_net.ToData(d);
            destData["web_net"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "HttpWeb"; wss_address.ToData(d);
            destData["wss_address"] = d.getPtr();
        }
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_VALUE(account_id, 1);
        SAVE_MSG_STRUCT(actor_node);
        SAVE_MSG_VALUE(title, 4);
        SAVE_MSG_STRUCT(web_net);
        SAVE_MSG_STRUCT(wss_address);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        account_id = 0;
        actor_node.clear(false);
        title.setNull();
        web_net.clear(false);
        wss_address.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AccountServerConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not AccountServerConfig", otherMsg.GetMsgName()); return; }; const AccountServerConfig &other = *(const AccountServerConfig*)(&otherMsg);
        account_id = other.account_id;
        actor_node.copy(other.actor_node);
        title = other.title;
        web_net.copy(other.web_net);
        wss_address.copy(other.wss_address);
    }

    virtual const char* GetMsgName() const override { return "AccountServerConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "account_id")==0) { AData value; value = account_id; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "account_id")==0) { account_id = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
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

//  GameServer 进程配置
class GameServerConfig : public tBaseMsg
{ 
public:
    AString OPEN_TIME;		//  开放时间
    int SERVER_ID;		//  游戏区ID
    AString SERVER_NAME;		//  游戏区名称
    int SERVER_TYPE;		//  服务器类型
    DBConfig db_config;		//  Logic DB 配置	
    SqlDB log_sql;		//  日志服配置
    AString manager_csv_file;		//  管理员CSV配置列表文件
    UserNode server_node;		
    AString title;		//  程序运行的标题

public:
    GameServerConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, OPEN_TIME);
        CheckGet(scrData, SERVER_ID);
        CheckGet(scrData, SERVER_NAME);
        CheckGet(scrData, SERVER_TYPE);
        {
            AutoNice d = (tNiceData*)scrData["db_config"];
            if (d) db_config.Full(d); else LOG("No exist data db_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["log_sql"];
            if (d) log_sql.Full(d); else LOG("No exist data log_sql");
        }
        CheckGet(scrData, manager_csv_file);
        {
            AutoNice d = (tNiceData*)scrData["server_node"];
            if (d) server_node.Full(d); else LOG("No exist data server_node");
        }
        CheckGet(scrData, title);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["OPEN_TIME"] = OPEN_TIME;
        destData["SERVER_ID"] = SERVER_ID;
        destData["SERVER_NAME"] = SERVER_NAME;
        destData["SERVER_TYPE"] = SERVER_TYPE;
        {
            AutoNice d = destData->NewNice(); d->msKey = "DBConfig"; db_config.ToData(d);
            destData["db_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "SqlDB"; log_sql.ToData(d);
            destData["log_sql"] = d.getPtr();
        }
        destData["manager_csv_file"] = manager_csv_file;
        {
            AutoNice d = destData->NewNice(); d->msKey = "UserNode"; server_node.ToData(d);
            destData["server_node"] = d.getPtr();
        }
        destData["title"] = title;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)9);

        SAVE_MSG_VALUE(OPEN_TIME, 4);
        SAVE_MSG_VALUE(SERVER_ID, 1);
        SAVE_MSG_VALUE(SERVER_NAME, 4);
        SAVE_MSG_VALUE(SERVER_TYPE, 1);
        SAVE_MSG_STRUCT(db_config);
        SAVE_MSG_STRUCT(log_sql);
        SAVE_MSG_VALUE(manager_csv_file, 4);
        SAVE_MSG_STRUCT(server_node);
        SAVE_MSG_VALUE(title, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        OPEN_TIME.setNull();
        SERVER_ID = 0;
        SERVER_NAME.setNull();
        SERVER_TYPE = 0;
        db_config.clear(false);
        log_sql.clear(false);
        manager_csv_file.setNull();
        server_node.clear(false);
        title.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GameServerConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not GameServerConfig", otherMsg.GetMsgName()); return; }; const GameServerConfig &other = *(const GameServerConfig*)(&otherMsg);
        OPEN_TIME = other.OPEN_TIME;
        SERVER_ID = other.SERVER_ID;
        SERVER_NAME = other.SERVER_NAME;
        SERVER_TYPE = other.SERVER_TYPE;
        db_config.copy(other.db_config);
        log_sql.copy(other.log_sql);
        manager_csv_file = other.manager_csv_file;
        server_node.copy(other.server_node);
        title = other.title;
    }

    virtual const char* GetMsgName() const override { return "GameServerConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { AData value; value = OPEN_TIME; return value; }
        if (strcmp(szMember, "SERVER_ID")==0) { AData value; value = SERVER_ID; return value; }
        if (strcmp(szMember, "SERVER_NAME")==0) { AData value; value = SERVER_NAME; return value; }
        if (strcmp(szMember, "SERVER_TYPE")==0) { AData value; value = SERVER_TYPE; return value; }
        if (strcmp(szMember, "manager_csv_file")==0) { AData value; value = manager_csv_file; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "OPEN_TIME")==0) { OPEN_TIME = value; return true; };
        if (strcmp(szMember, "SERVER_ID")==0) { SERVER_ID = value; return true; };
        if (strcmp(szMember, "SERVER_NAME")==0) { SERVER_NAME = value; return true; };
        if (strcmp(szMember, "SERVER_TYPE")==0) { SERVER_TYPE = value; return true; };
        if (strcmp(szMember, "manager_csv_file")==0) { manager_csv_file = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
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

//  CenterServer配置
class CenterServerConfig : public tBaseMsg
{ 
public:
    SqlDB account_db;		//  帐号DB数据源
    UserNode center_node;		//  连接帐号DB节点配置	
    int share_server_code;		//  共享落地SQL安全码
    int share_server_port;		//  共享落地服务网络端口
    AString title;		//  程序运行的标题

public:
    CenterServerConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["account_db"];
            if (d) account_db.Full(d); else LOG("No exist data account_db");
        }
        {
            AutoNice d = (tNiceData*)scrData["center_node"];
            if (d) center_node.Full(d); else LOG("No exist data center_node");
        }
        CheckGet(scrData, share_server_code);
        CheckGet(scrData, share_server_port);
        CheckGet(scrData, title);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "SqlDB"; account_db.ToData(d);
            destData["account_db"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "UserNode"; center_node.ToData(d);
            destData["center_node"] = d.getPtr();
        }
        destData["share_server_code"] = share_server_code;
        destData["share_server_port"] = share_server_port;
        destData["title"] = title;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_STRUCT(account_db);
        SAVE_MSG_STRUCT(center_node);
        SAVE_MSG_VALUE(share_server_code, 1);
        SAVE_MSG_VALUE(share_server_port, 1);
        SAVE_MSG_VALUE(title, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        account_db.clear(false);
        center_node.clear(false);
        share_server_code = 0;
        share_server_port = 0;
        title.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CenterServerConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not CenterServerConfig", otherMsg.GetMsgName()); return; }; const CenterServerConfig &other = *(const CenterServerConfig*)(&otherMsg);
        account_db.copy(other.account_db);
        center_node.copy(other.center_node);
        share_server_code = other.share_server_code;
        share_server_port = other.share_server_port;
        title = other.title;
    }

    virtual const char* GetMsgName() const override { return "CenterServerConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "share_server_code")==0) { AData value; value = share_server_code; return value; }
        if (strcmp(szMember, "share_server_port")==0) { AData value; value = share_server_port; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "share_server_code")==0) { share_server_code = value; return true; };
        if (strcmp(szMember, "share_server_port")==0) { share_server_port = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
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

//  从库备份进程
class DBBackConfig : public tBaseMsg
{ 
public:
    AString cert_file;		//  pem证书文件
    AString key_file;		//  ssl Key 证书
    AString password;		//  密码
    SqlDB sql_config;		//  MYSQL配置
    AString title;		//  程序运行的标题	
    int ws_port;		//  中转ws端口

public:
    DBBackConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, cert_file);
        CheckGet(scrData, key_file);
        CheckGet(scrData, password);
        {
            AutoNice d = (tNiceData*)scrData["sql_config"];
            if (d) sql_config.Full(d); else LOG("No exist data sql_config");
        }
        CheckGet(scrData, title);
        CheckGet(scrData, ws_port);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["cert_file"] = cert_file;
        destData["key_file"] = key_file;
        destData["password"] = password;
        {
            AutoNice d = destData->NewNice(); d->msKey = "SqlDB"; sql_config.ToData(d);
            destData["sql_config"] = d.getPtr();
        }
        destData["title"] = title;
        destData["ws_port"] = ws_port;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_VALUE(cert_file, 4);
        SAVE_MSG_VALUE(key_file, 4);
        SAVE_MSG_VALUE(password, 4);
        SAVE_MSG_STRUCT(sql_config);
        SAVE_MSG_VALUE(title, 4);
        SAVE_MSG_VALUE(ws_port, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        cert_file.setNull();
        key_file.setNull();
        password.setNull();
        sql_config.clear(false);
        title.setNull();
        ws_port = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBBackConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not DBBackConfig", otherMsg.GetMsgName()); return; }; const DBBackConfig &other = *(const DBBackConfig*)(&otherMsg);
        cert_file = other.cert_file;
        key_file = other.key_file;
        password = other.password;
        sql_config.copy(other.sql_config);
        title = other.title;
        ws_port = other.ws_port;
    }

    virtual const char* GetMsgName() const override { return "DBBackConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "cert_file")==0) { AData value; value = cert_file; return value; }
        if (strcmp(szMember, "key_file")==0) { AData value; value = key_file; return value; }
        if (strcmp(szMember, "password")==0) { AData value; value = password; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        if (strcmp(szMember, "ws_port")==0) { AData value; value = ws_port; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "cert_file")==0) { cert_file = value; return true; };
        if (strcmp(szMember, "key_file")==0) { key_file = value; return true; };
        if (strcmp(szMember, "password")==0) { password = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
        if (strcmp(szMember, "ws_port")==0) { ws_port = value; return true; };
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

//  日志服
class LogServerConfig : public tBaseMsg
{ 
public:
    AString AUTHORIZATION1;		//  开放权限
    AString AUTHORIZATION2;		//  
    AString cert_file;		//  pem证书文件
    AString key_file;		//  ssl Key 证书
    AString mDBIP;		//  Mysql 地址
    AString mDBNAME;		//  库名
    AString mDBPASSWORD;		//  连接密码
    int mDBPORT;		//  Mysql 端口
    AString mDBUSER;		//  Mysql用户名
    AString mIp;		//  UDP接收地址
    int mPort;		//  UDP接收端口
    AString password;		//  密码
    AString title;		//  程序运行的标题
    AString web_addr;		//  开放ws地址
    int web_port;		//  开放ws端口

public:
    LogServerConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, AUTHORIZATION1);
        CheckGet(scrData, AUTHORIZATION2);
        CheckGet(scrData, cert_file);
        CheckGet(scrData, key_file);
        CheckGet(scrData, mDBIP);
        CheckGet(scrData, mDBNAME);
        CheckGet(scrData, mDBPASSWORD);
        CheckGet(scrData, mDBPORT);
        CheckGet(scrData, mDBUSER);
        CheckGet(scrData, mIp);
        CheckGet(scrData, mPort);
        CheckGet(scrData, password);
        CheckGet(scrData, title);
        CheckGet(scrData, web_addr);
        CheckGet(scrData, web_port);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["AUTHORIZATION1"] = AUTHORIZATION1;
        destData["AUTHORIZATION2"] = AUTHORIZATION2;
        destData["cert_file"] = cert_file;
        destData["key_file"] = key_file;
        destData["mDBIP"] = mDBIP;
        destData["mDBNAME"] = mDBNAME;
        destData["mDBPASSWORD"] = mDBPASSWORD;
        destData["mDBPORT"] = mDBPORT;
        destData["mDBUSER"] = mDBUSER;
        destData["mIp"] = mIp;
        destData["mPort"] = mPort;
        destData["password"] = password;
        destData["title"] = title;
        destData["web_addr"] = web_addr;
        destData["web_port"] = web_port;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)15);

        SAVE_MSG_VALUE(AUTHORIZATION1, 4);
        SAVE_MSG_VALUE(AUTHORIZATION2, 4);
        SAVE_MSG_VALUE(cert_file, 4);
        SAVE_MSG_VALUE(key_file, 4);
        SAVE_MSG_VALUE(mDBIP, 4);
        SAVE_MSG_VALUE(mDBNAME, 4);
        SAVE_MSG_VALUE(mDBPASSWORD, 4);
        SAVE_MSG_VALUE(mDBPORT, 1);
        SAVE_MSG_VALUE(mDBUSER, 4);
        SAVE_MSG_VALUE(mIp, 4);
        SAVE_MSG_VALUE(mPort, 1);
        SAVE_MSG_VALUE(password, 4);
        SAVE_MSG_VALUE(title, 4);
        SAVE_MSG_VALUE(web_addr, 4);
        SAVE_MSG_VALUE(web_port, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        AUTHORIZATION1.setNull();
        AUTHORIZATION2.setNull();
        cert_file.setNull();
        key_file.setNull();
        mDBIP.setNull();
        mDBNAME.setNull();
        mDBPASSWORD.setNull();
        mDBPORT = 0;
        mDBUSER.setNull();
        mIp.setNull();
        mPort = 0;
        password.setNull();
        title.setNull();
        web_addr.setNull();
        web_port = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LogServerConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not LogServerConfig", otherMsg.GetMsgName()); return; }; const LogServerConfig &other = *(const LogServerConfig*)(&otherMsg);
        AUTHORIZATION1 = other.AUTHORIZATION1;
        AUTHORIZATION2 = other.AUTHORIZATION2;
        cert_file = other.cert_file;
        key_file = other.key_file;
        mDBIP = other.mDBIP;
        mDBNAME = other.mDBNAME;
        mDBPASSWORD = other.mDBPASSWORD;
        mDBPORT = other.mDBPORT;
        mDBUSER = other.mDBUSER;
        mIp = other.mIp;
        mPort = other.mPort;
        password = other.password;
        title = other.title;
        web_addr = other.web_addr;
        web_port = other.web_port;
    }

    virtual const char* GetMsgName() const override { return "LogServerConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "AUTHORIZATION1")==0) { AData value; value = AUTHORIZATION1; return value; }
        if (strcmp(szMember, "AUTHORIZATION2")==0) { AData value; value = AUTHORIZATION2; return value; }
        if (strcmp(szMember, "cert_file")==0) { AData value; value = cert_file; return value; }
        if (strcmp(szMember, "key_file")==0) { AData value; value = key_file; return value; }
        if (strcmp(szMember, "mDBIP")==0) { AData value; value = mDBIP; return value; }
        if (strcmp(szMember, "mDBNAME")==0) { AData value; value = mDBNAME; return value; }
        if (strcmp(szMember, "mDBPASSWORD")==0) { AData value; value = mDBPASSWORD; return value; }
        if (strcmp(szMember, "mDBPORT")==0) { AData value; value = mDBPORT; return value; }
        if (strcmp(szMember, "mDBUSER")==0) { AData value; value = mDBUSER; return value; }
        if (strcmp(szMember, "mIp")==0) { AData value; value = mIp; return value; }
        if (strcmp(szMember, "mPort")==0) { AData value; value = mPort; return value; }
        if (strcmp(szMember, "password")==0) { AData value; value = password; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        if (strcmp(szMember, "web_addr")==0) { AData value; value = web_addr; return value; }
        if (strcmp(szMember, "web_port")==0) { AData value; value = web_port; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "AUTHORIZATION1")==0) { AUTHORIZATION1 = value; return true; };
        if (strcmp(szMember, "AUTHORIZATION2")==0) { AUTHORIZATION2 = value; return true; };
        if (strcmp(szMember, "cert_file")==0) { cert_file = value; return true; };
        if (strcmp(szMember, "key_file")==0) { key_file = value; return true; };
        if (strcmp(szMember, "mDBIP")==0) { mDBIP = value; return true; };
        if (strcmp(szMember, "mDBNAME")==0) { mDBNAME = value; return true; };
        if (strcmp(szMember, "mDBPASSWORD")==0) { mDBPASSWORD = value; return true; };
        if (strcmp(szMember, "mDBPORT")==0) { mDBPORT = value; return true; };
        if (strcmp(szMember, "mDBUSER")==0) { mDBUSER = value; return true; };
        if (strcmp(szMember, "mIp")==0) { mIp = value; return true; };
        if (strcmp(szMember, "mPort")==0) { mPort = value; return true; };
        if (strcmp(szMember, "password")==0) { password = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
        if (strcmp(szMember, "web_addr")==0) { web_addr = value; return true; };
        if (strcmp(szMember, "web_port")==0) { web_port = value; return true; };
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

//  通用DB共享落地服务器
class ShareDBConfig : public tBaseMsg
{ 
public:
    IPAddress server_address;		//  开放给逻辑使用进程的网络服务	
    AString title;		//  程序运行的标题

public:
    ShareDBConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["server_address"];
            if (d) server_address.Full(d); else LOG("No exist data server_address");
        }
        CheckGet(scrData, title);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "IPAddress"; server_address.ToData(d);
            destData["server_address"] = d.getPtr();
        }
        destData["title"] = title;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_STRUCT(server_address);
        SAVE_MSG_VALUE(title, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        server_address.clear(false);
        title.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ShareDBConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not ShareDBConfig", otherMsg.GetMsgName()); return; }; const ShareDBConfig &other = *(const ShareDBConfig*)(&otherMsg);
        server_address.copy(other.server_address);
        title = other.title;
    }

    virtual const char* GetMsgName() const override { return "ShareDBConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "title")==0) { title = value; return true; };
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

//  逻辑Actor服务器配置
class LogicActorDBConfig : public tBaseMsg
{ 
public:
    UserNode data_db_node;		//  连接Gate节点配置	
    int share_server_code;		//  共享落地SQL安全码
    int share_server_port;		//  共享落地服务网络IP
    SqlDB sql_db;		//  MYSQL数据库配置
    AString title;		//  程序运行的标题
    int worker_id;		//  线程内Unit对象分配的ID

public:
    LogicActorDBConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["data_db_node"];
            if (d) data_db_node.Full(d); else LOG("No exist data data_db_node");
        }
        CheckGet(scrData, share_server_code);
        CheckGet(scrData, share_server_port);
        {
            AutoNice d = (tNiceData*)scrData["sql_db"];
            if (d) sql_db.Full(d); else LOG("No exist data sql_db");
        }
        CheckGet(scrData, title);
        CheckGet(scrData, worker_id);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "UserNode"; data_db_node.ToData(d);
            destData["data_db_node"] = d.getPtr();
        }
        destData["share_server_code"] = share_server_code;
        destData["share_server_port"] = share_server_port;
        {
            AutoNice d = destData->NewNice(); d->msKey = "SqlDB"; sql_db.ToData(d);
            destData["sql_db"] = d.getPtr();
        }
        destData["title"] = title;
        destData["worker_id"] = worker_id;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_STRUCT(data_db_node);
        SAVE_MSG_VALUE(share_server_code, 1);
        SAVE_MSG_VALUE(share_server_port, 1);
        SAVE_MSG_STRUCT(sql_db);
        SAVE_MSG_VALUE(title, 4);
        SAVE_MSG_VALUE(worker_id, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        data_db_node.clear(false);
        share_server_code = 0;
        share_server_port = 0;
        sql_db.clear(false);
        title.setNull();
        worker_id = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LogicActorDBConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not LogicActorDBConfig", otherMsg.GetMsgName()); return; }; const LogicActorDBConfig &other = *(const LogicActorDBConfig*)(&otherMsg);
        data_db_node.copy(other.data_db_node);
        share_server_code = other.share_server_code;
        share_server_port = other.share_server_port;
        sql_db.copy(other.sql_db);
        title = other.title;
        worker_id = other.worker_id;
    }

    virtual const char* GetMsgName() const override { return "LogicActorDBConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "share_server_code")==0) { AData value; value = share_server_code; return value; }
        if (strcmp(szMember, "share_server_port")==0) { AData value; value = share_server_port; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        if (strcmp(szMember, "worker_id")==0) { AData value; value = worker_id; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "share_server_code")==0) { share_server_code = value; return true; };
        if (strcmp(szMember, "share_server_port")==0) { share_server_port = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
        if (strcmp(szMember, "worker_id")==0) { worker_id = value; return true; };
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

//  LoginConfig 进程配置
class LoginConfig : public tBaseMsg
{ 
public:
    AString cert_file;		//  pem证书文件
    AString key_file;		//  ssl Key 证书
    int login_id;		//  连接DB单元的ID  1000 2000 为区号, 尾数为登陆服务器的ID
    UserNode login_node;		//  连接 Data DB	
    AString password;		//  密码
    AString title;		//  程序运行的标题
    AString ws_ip;		//  开放ws地址
    int ws_port;		//  开放ws端口

public:
    LoginConfig() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, cert_file);
        CheckGet(scrData, key_file);
        CheckGet(scrData, login_id);
        {
            AutoNice d = (tNiceData*)scrData["login_node"];
            if (d) login_node.Full(d); else LOG("No exist data login_node");
        }
        CheckGet(scrData, password);
        CheckGet(scrData, title);
        CheckGet(scrData, ws_ip);
        CheckGet(scrData, ws_port);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["cert_file"] = cert_file;
        destData["key_file"] = key_file;
        destData["login_id"] = login_id;
        {
            AutoNice d = destData->NewNice(); d->msKey = "UserNode"; login_node.ToData(d);
            destData["login_node"] = d.getPtr();
        }
        destData["password"] = password;
        destData["title"] = title;
        destData["ws_ip"] = ws_ip;
        destData["ws_port"] = ws_port;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)8);

        SAVE_MSG_VALUE(cert_file, 4);
        SAVE_MSG_VALUE(key_file, 4);
        SAVE_MSG_VALUE(login_id, 1);
        SAVE_MSG_STRUCT(login_node);
        SAVE_MSG_VALUE(password, 4);
        SAVE_MSG_VALUE(title, 4);
        SAVE_MSG_VALUE(ws_ip, 4);
        SAVE_MSG_VALUE(ws_port, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        cert_file.setNull();
        key_file.setNull();
        login_id = 0;
        login_node.clear(false);
        password.setNull();
        title.setNull();
        ws_ip.setNull();
        ws_port = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LoginConfig", otherMsg.GetMsgName())!=0) { LOG("%s is not LoginConfig", otherMsg.GetMsgName()); return; }; const LoginConfig &other = *(const LoginConfig*)(&otherMsg);
        cert_file = other.cert_file;
        key_file = other.key_file;
        login_id = other.login_id;
        login_node.copy(other.login_node);
        password = other.password;
        title = other.title;
        ws_ip = other.ws_ip;
        ws_port = other.ws_port;
    }

    virtual const char* GetMsgName() const override { return "LoginConfig"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "cert_file")==0) { AData value; value = cert_file; return value; }
        if (strcmp(szMember, "key_file")==0) { AData value; value = key_file; return value; }
        if (strcmp(szMember, "login_id")==0) { AData value; value = login_id; return value; }
        if (strcmp(szMember, "password")==0) { AData value; value = password; return value; }
        if (strcmp(szMember, "title")==0) { AData value; value = title; return value; }
        if (strcmp(szMember, "ws_ip")==0) { AData value; value = ws_ip; return value; }
        if (strcmp(szMember, "ws_port")==0) { AData value; value = ws_port; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "cert_file")==0) { cert_file = value; return true; };
        if (strcmp(szMember, "key_file")==0) { key_file = value; return true; };
        if (strcmp(szMember, "login_id")==0) { login_id = value; return true; };
        if (strcmp(szMember, "password")==0) { password = value; return true; };
        if (strcmp(szMember, "title")==0) { title = value; return true; };
        if (strcmp(szMember, "ws_ip")==0) { ws_ip = value; return true; };
        if (strcmp(szMember, "ws_port")==0) { ws_port = value; return true; };
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

//  全部程序运行配置
class RunConfigData : public tBaseMsg
{ 
public:
    AccountServerConfig account_config;		//  帐号服务器配置
    CenterServerConfig center_config;		//  世界中心服务器配置
    GameServerConfig game_config;		//  游戏服务器配置	
    GateServerConfig gate_config;		//  Gata配置	
    LogicActorDBConfig logic_actor_config;		//  逻辑Actor服务器配置
    LoginConfig login_config;		//  登陆进程配置
    ShareDBConfig share_db_config;		//  通用DB共享落地服务器

public:
    RunConfigData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["account_config"];
            if (d) account_config.Full(d); else LOG("No exist data account_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["center_config"];
            if (d) center_config.Full(d); else LOG("No exist data center_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["game_config"];
            if (d) game_config.Full(d); else LOG("No exist data game_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["gate_config"];
            if (d) gate_config.Full(d); else LOG("No exist data gate_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["logic_actor_config"];
            if (d) logic_actor_config.Full(d); else LOG("No exist data logic_actor_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["login_config"];
            if (d) login_config.Full(d); else LOG("No exist data login_config");
        }
        {
            AutoNice d = (tNiceData*)scrData["share_db_config"];
            if (d) share_db_config.Full(d); else LOG("No exist data share_db_config");
        }
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "AccountServerConfig"; account_config.ToData(d);
            destData["account_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "CenterServerConfig"; center_config.ToData(d);
            destData["center_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "GameServerConfig"; game_config.ToData(d);
            destData["game_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "GateServerConfig"; gate_config.ToData(d);
            destData["gate_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "LogicActorDBConfig"; logic_actor_config.ToData(d);
            destData["logic_actor_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "LoginConfig"; login_config.ToData(d);
            destData["login_config"] = d.getPtr();
        }
        {
            AutoNice d = destData->NewNice(); d->msKey = "ShareDBConfig"; share_db_config.ToData(d);
            destData["share_db_config"] = d.getPtr();
        }
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)7);

        SAVE_MSG_STRUCT(account_config);
        SAVE_MSG_STRUCT(center_config);
        SAVE_MSG_STRUCT(game_config);
        SAVE_MSG_STRUCT(gate_config);
        SAVE_MSG_STRUCT(logic_actor_config);
        SAVE_MSG_STRUCT(login_config);
        SAVE_MSG_STRUCT(share_db_config);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        account_config.clear(false);
        center_config.clear(false);
        game_config.clear(false);
        gate_config.clear(false);
        logic_actor_config.clear(false);
        login_config.clear(false);
        share_db_config.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RunConfigData", otherMsg.GetMsgName())!=0) { LOG("%s is not RunConfigData", otherMsg.GetMsgName()); return; }; const RunConfigData &other = *(const RunConfigData*)(&otherMsg);
        account_config.copy(other.account_config);
        center_config.copy(other.center_config);
        game_config.copy(other.game_config);
        gate_config.copy(other.gate_config);
        logic_actor_config.copy(other.logic_actor_config);
        login_config.copy(other.login_config);
        share_db_config.copy(other.share_db_config);
    }

    virtual const char* GetMsgName() const override { return "RunConfigData"; }

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

class _comment {
public:
    static const char* CommentString()
     {
        static const char *gCommentString = 
                "ewogICAiQWNjb3VudFNlcnZlckNvbmZpZyIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImFjY291bnRfaWQiIDogIiDluJDlj7fmnI3liqHlmajnvJblj7csIOeUqOS6juWkmuW4kOWPt+acjeWKoeWZqOi0n+i9vSIsCiAgICAgICJhY3Rvcl9ub2RlIiA6ICIiLAogICAgICAiYWN0b3Jfbm9kZV9fdHlwZV9fIiA6ICJVc2VyTm9kZSIsCiAgICAgICJ0aXRsZSIgOiAiIOeoi+W6j+i/kOihjOeahOagh+mimFx0IiwKICAgICAgIndlYl9uZXQiIDogIiBXZWJIdHRwTmV05byA5pS+57uZ5a6i5oi356uv5L2/55SoaHR0cOivt+axguWIm+W7uuaIlumqjOivgeW4kOWPtyIsCiAgICAgICJ3ZWJfbmV0X190eXBlX18iIDogIkh0dHBXZWIiLAogICAgICAid3NzX2FkZHJlc3MiIDogIiBXc3Mg572R57uc5Zyw5Z2AIiwKICAgICAgIndzc19hZGRyZXNzX190eXBlX18iIDogIkh0dHBXZWIiCiAgIH0sCiAgICJBY2NvdW50U2VydmVyQ29uZmlnX19saXN0X18iIDogWyAidGl0bGUiLCAid2ViX25ldCIsICJ3c3NfYWRkcmVzcyIsICJhY3Rvcl9ub2RlIiwgImFjY291bnRfaWQiIF0sCiAgICJBY2NvdW50U2VydmVyQ29uZmlnX2luZm9fIiA6ICIgQWNjb3VudERCIOW4kOWPt0RC6L+b56iL6YWN572uIiwKICAgIkNlbnRlclNlcnZlckNvbmZpZyIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImFjY291bnRfZGIiIDogIiDluJDlj7dEQuaVsOaNrua6kCIsCiAgICAgICJhY2NvdW50X2RiX190eXBlX18iIDogIlNxbERCIiwKICAgICAgImNlbnRlcl9ub2RlIiA6ICIg6L+e5o6l5biQ5Y+3RELoioLngrnphY3nva5cdCIsCiAgICAgICJjZW50ZXJfbm9kZV9fdHlwZV9fIiA6ICJVc2VyTm9kZSIsCiAgICAgICJzaGFyZV9zZXJ2ZXJfY29kZSIgOiAiIOWFseS6q+iQveWcsFNRTOWuieWFqOeggSIsCiAgICAgICJzaGFyZV9zZXJ2ZXJfcG9ydCIgOiAiIOWFseS6q+iQveWcsOacjeWKoee9kee7nOerr+WPoyIsCiAgICAgICJ0aXRsZSIgOiAiIOeoi+W6j+i/kOihjOeahOagh+mimCIKICAgfSwKICAgIkNlbnRlclNlcnZlckNvbmZpZ19fbGlzdF9fIiA6IFsKICAgICAgInRpdGxlIiwKICAgICAgImNlbnRlcl9ub2RlIiwKICAgICAgImFjY291bnRfZGIiLAogICAgICAic2hhcmVfc2VydmVyX3BvcnQiLAogICAgICAic2hhcmVfc2VydmVyX2NvZGUiCiAgIF0sCiAgICJDZW50ZXJTZXJ2ZXJDb25maWdfaW5mb18iIDogIiBDZW50ZXJTZXJ2ZXLphY3nva4iLAogICAiREJCYWNrQ29uZmlnIiA6IHsKICAgICAgIl9zdHJ1Y3RfIiA6ICIiLAogICAgICAiY2VydF9maWxlIiA6ICIgcGVt6K+B5Lmm5paH5Lu2IiwKICAgICAgImtleV9maWxlIiA6ICIgc3NsIEtleSDor4HkuaYiLAogICAgICAicGFzc3dvcmQiIDogIiDlr4bnoIEiLAogICAgICAic3FsX2NvbmZpZyIgOiAiIE1ZU1FM6YWN572uIiwKICAgICAgInNxbF9jb25maWdfX3R5cGVfXyIgOiAiU3FsREIiLAogICAgICAidGl0bGUiIDogIiDnqIvluo/ov5DooYznmoTmoIfpophcdCIsCiAgICAgICJ3c19wb3J0IiA6ICIg5Lit6L2sd3Pnq6/lj6MiCiAgIH0sCiAgICJEQkJhY2tDb25maWdfX2xpc3RfXyIgOiBbICJ0aXRsZSIsICJzcWxfY29uZmlnIiwgIndzX3BvcnQiLCAia2V5X2ZpbGUiLCAiY2VydF9maWxlIiwgInBhc3N3b3JkIiBdLAogICAiREJCYWNrQ29uZmlnX2luZm9fIiA6ICIg5LuO5bqT5aSH5Lu96L+b56iLIiwKICAgIkRCQ29uZmlnIiA6IHsKICAgICAgIl9zdHJ1Y3RfIiA6ICIiLAogICAgICAiZGJfY29kZSIgOiAiIERC57yW5Y+3LCAw5pe25Li65Li7REIsIOaPkOS+m0RC6KGo5qC85rGH5oC75pyN5YqhIiwKICAgICAgImRiX25vZGUiIDogIiBEQuS6keiKgueCueWIhumFjeWcsOWdgCjkuI7lhbbku5bov57mjqXlnKhHYXRl5LiK55qE6IqC54K55LqS6L+eKSIsCiAgICAgICJkYl9ub2RlX190eXBlX18iIDogIlVzZXJOb2RlIiwKICAgICAgInJlY29yZF91cGRhdGVfbWlsc2Vjb25kIiA6ICIg6K6w5b2V5qOA5p+l5pu05paw6JC95Zyw55qE6Ze06ZqU5q+r56eS5pe26Ze0LCDnlKjkuo7osIPmlbREQuaVtOS9k+aAp+iDvSwg5q2k5pe26Ze06LaK5aSnLCDmgKfog73otorpq5gsIOS9huWuieWFqOiQveWcsOmjjumZqei2iuWkpyAo5o6o6I2Q5YC8OiAxMDAwfjEwMDAwKSIsCiAgICAgICJzaGFyZV9rZXkiIDogIiBEQuaVsOaNruW6k+i/nuaOpeeahOWFseS6q+WGheWtmOmFjee9riwgTXlzcWwg5L+h5oGv6YCa6L+H5raI5oGv5LuO5a+55bqU5YWx5Lqr5YaF5a2Y6L+b56iL6I635Y+WIiwKICAgICAgInNoYXJlX2tleV9fdHlwZV9fIiA6ICJTaGFyZU1lbSIsCiAgICAgICJ3b3JrZXJfaWQiIDogIiDovoXliqnlt6XlhbfljZXlhYNJRCIKICAgfSwKICAgIkRCQ29uZmlnX19saXN0X18iIDogWwogICAgICAiZGJfY29kZSIsCiAgICAgICJkYl9ub2RlIiwKICAgICAgInNoYXJlX2tleSIsCiAgICAgICJ3b3JrZXJfaWQiLAogICAgICAicmVjb3JkX3VwZGF0ZV9taWxzZWNvbmQiCiAgIF0sCiAgICJEQkNvbmZpZ19pbmZvXyIgOiAiIiwKICAgIkdhbWVTZXJ2ZXJDb25maWciIDogewogICAgICAiT1BFTl9USU1FIiA6ICIg5byA5pS+5pe26Ze0IiwKICAgICAgIlNFUlZFUl9JRCIgOiAiIOa4uOaIj+WMuklEIiwKICAgICAgIlNFUlZFUl9OQU1FIiA6ICIg5ri45oiP5Yy65ZCN56ewIiwKICAgICAgIlNFUlZFUl9UWVBFIiA6ICIg5pyN5Yqh5Zmo57G75Z6LIiwKICAgICAgIl9zdHJ1Y3RfIiA6ICIiLAogICAgICAiZGJfY29uZmlnIiA6ICIgTG9naWMgREIg6YWN572uXHQiLAogICAgICAiZGJfY29uZmlnX190eXBlX18iIDogIkRCQ29uZmlnIiwKICAgICAgImxvZ19zcWwiIDogIiDml6Xlv5fmnI3phY3nva4iLAogICAgICAibG9nX3NxbF9fdHlwZV9fIiA6ICJTcWxEQiIsCiAgICAgICJtYW5hZ2VyX2Nzdl9maWxlIiA6ICIg566h55CG5ZGYQ1NW6YWN572u5YiX6KGo5paH5Lu2IiwKICAgICAgInNlcnZlcl9ub2RlIiA6ICIiLAogICAgICAic2VydmVyX25vZGVfX3R5cGVfXyIgOiAiVXNlck5vZGUiLAogICAgICAidGl0bGUiIDogIiDnqIvluo/ov5DooYznmoTmoIfpopgiCiAgIH0sCiAgICJHYW1lU2VydmVyQ29uZmlnX19saXN0X18iIDogWwogICAgICAidGl0bGUiLAogICAgICAic2VydmVyX25vZGUiLAogICAgICAiZGJfY29uZmlnIiwKICAgICAgImxvZ19zcWwiLAogICAgICAiU0VSVkVSX0lEIiwKICAgICAgIlNFUlZFUl9OQU1FIiwKICAgICAgIk9QRU5fVElNRSIsCiAgICAgICJtYW5hZ2VyX2Nzdl9maWxlIiwKICAgICAgIlNFUlZFUl9UWVBFIgogICBdLAogICAiR2FtZVNlcnZlckNvbmZpZ19pbmZvXyIgOiAiIEdhbWVTZXJ2ZXIg6L+b56iL6YWN572uIiwKICAgIkdhdGVOb2RlIiA6IHsKICAgICAgIl9zdHJ1Y3RfIiA6ICIiLAogICAgICAiYWRkcmVzcyIgOiAiIEdhdGVOb2Rl5Zyw5Z2AICIsCiAgICAgICJhZGRyZXNzX190eXBlX18iIDogIklQQWRkcmVzcyIsCiAgICAgICJmaXhfY291bnQiIDogIiDlm7rlrppHYXRl5pWw6YePIiwKICAgICAgImdhdGVfY29kZSIgOiAiIOe8luWPtygwfmZpeF9jb3VudC0xKSIKICAgfSwKICAgIkdhdGVOb2RlX19saXN0X18iIDogWyAiZml4X2NvdW50IiwgImdhdGVfY29kZSIsICJhZGRyZXNzIiBdLAogICAiR2F0ZU5vZGVfaW5mb18iIDogIiBHYXRlTm9kZSDnlKjkuo5HYXRl5LmL6Ze057uE572RIiwKICAgIkdhdGVTZXJ2ZXJDb25maWciIDogewogICAgICAiX3N0cnVjdF8iIDogIiIsCiAgICAgICJnYXRlX25vZGUiIDogIiBHYXRlTm9kZSDnlKjkuo5HYXRl5LmL6Ze057uE572RIiwKICAgICAgImdhdGVfbm9kZV9fdHlwZV9fIiA6ICJHYXRlTm9kZSIsCiAgICAgICJnYXRlX3NlcnZlciIgOiAiIEdhdGXmjqXlj5fljZXlhYPoioLngrnov57mjqXlvIDmlL7nmoTmnI3liqHlnLDlnYAiLAogICAgICAiZ2F0ZV9zZXJ2ZXJfX3R5cGVfXyIgOiAiSVBBZGRyZXNzIiwKICAgICAgIm1haW5fZ2F0ZSIgOiAiIEdhdGXmjqXlj5fljZXlhYPoioLngrnov57mjqXlvIDmlL7nmoTmnI3liqHlnLDlnYAiLAogICAgICAibWFpbl9nYXRlX190eXBlX18iIDogIklQQWRkcmVzcyIsCiAgICAgICJ0aXRsZSIgOiAiIOeoi+W6j+i/kOihjOeahOagh+mimCIKICAgfSwKICAgIkdhdGVTZXJ2ZXJDb25maWdfX2xpc3RfXyIgOiBbICJ0aXRsZSIsICJnYXRlX25vZGUiLCAiZ2F0ZV9zZXJ2ZXIiLCAibWFpbl9nYXRlIiBdLAogICAiR2F0ZVNlcnZlckNvbmZpZ19pbmZvXyIgOiAiIENsb3VkZUdhdGUg56iL5bqP6YWN572uIiwKICAgIkh0dHBXZWIiIDogewogICAgICAiX3N0cnVjdF8iIDogIiIsCiAgICAgICJjZXJ0X2ZpbGUiIDogIiDor4Hkuabmlofku7YiLAogICAgICAiaHR0cF9hZGRyZXNzIiA6ICIg5byA5pS+SFRUUOWcsOWdgCjnlKjkuo7mj5Dkvpvnu5nov57mjqXov5vnqIspIiwKICAgICAgImtleV9maWxlIiA6ICIg6K+B5Lmma2V5LnBlbeaWh+S7tiIsCiAgICAgICJwYXNzd29yZCIgOiAiIOivgeS5puWvhueggSIsCiAgICAgICJwb3J0IiA6ICIg5byA5pS+56uv5Y+jIgogICB9LAogICAiSHR0cFdlYl9fbGlzdF9fIiA6IFsgImh0dHBfYWRkcmVzcyIsICJwb3J0IiwgImtleV9maWxlIiwgImNlcnRfZmlsZSIsICJwYXNzd29yZCIgXSwKICAgIkh0dHBXZWJfaW5mb18iIDogIiBIdHRwV2Vi5pyN5YqhIiwKICAgIklQQWRkcmVzcyIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImlwIiA6ICIgSVDlnLDlnYAs5pSv5oyB5Z+f5ZCNIiwKICAgICAgInBvcnQiIDogIiDnq6/lj6MiLAogICAgICAic2FmdF9jb2RlIiA6ICIg5a6J5YWo56CBLCDnlKjkuo7pgb/lhY3nvZHnu5zov57mjqXmt7fkubHlj4rop4Tpgb/mnKrnn6Xov57mjqXmjqXlhaUiCiAgIH0sCiAgICJJUEFkZHJlc3NfX2xpc3RfXyIgOiBbICJpcCIsICJwb3J0IiwgInNhZnRfY29kZSIgXSwKICAgIklQQWRkcmVzc19pbmZvXyIgOiAiIOe9kee7nOWcsOWdgCIsCiAgICJMb2dTZXJ2ZXJDb25maWciIDogewogICAgICAiQVVUSE9SSVpBVElPTjEiIDogIiDlvIDmlL7mnYPpmZAiLAogICAgICAiQVVUSE9SSVpBVElPTjIiIDogIiAiLAogICAgICAiX3N0cnVjdF8iIDogIiIsCiAgICAgICJjZXJ0X2ZpbGUiIDogIiBwZW3or4Hkuabmlofku7YiLAogICAgICAia2V5X2ZpbGUiIDogIiBzc2wgS2V5IOivgeS5piIsCiAgICAgICJtREJJUCIgOiAiIE15c3FsIOWcsOWdgCIsCiAgICAgICJtREJOQU1FIiA6ICIg5bqT5ZCNIiwKICAgICAgIm1EQlBBU1NXT1JEIiA6ICIg6L+e5o6l5a+G56CBIiwKICAgICAgIm1EQlBPUlQiIDogIiBNeXNxbCDnq6/lj6MiLAogICAgICAibURCVVNFUiIgOiAiIE15c3Fs55So5oi35ZCNIiwKICAgICAgIm1JcCIgOiAiIFVEUOaOpeaUtuWcsOWdgCIsCiAgICAgICJtUG9ydCIgOiAiIFVEUOaOpeaUtuerr+WPoyIsCiAgICAgICJwYXNzd29yZCIgOiAiIOWvhueggSIsCiAgICAgICJ0aXRsZSIgOiAiIOeoi+W6j+i/kOihjOeahOagh+mimCIsCiAgICAgICJ3ZWJfYWRkciIgOiAiIOW8gOaUvndz5Zyw5Z2AIiwKICAgICAgIndlYl9wb3J0IiA6ICIg5byA5pS+d3Pnq6/lj6MiCiAgIH0sCiAgICJMb2dTZXJ2ZXJDb25maWdfX2xpc3RfXyIgOiBbCiAgICAgICJ0aXRsZSIsCiAgICAgICJtSXAiLAogICAgICAibVBvcnQiLAogICAgICAibURCTkFNRSIsCiAgICAgICJtREJJUCIsCiAgICAgICJtREJQT1JUIiwKICAgICAgIm1EQlVTRVIiLAogICAgICAibURCUEFTU1dPUkQiLAogICAgICAiQVVUSE9SSVpBVElPTjEiLAogICAgICAiQVVUSE9SSVpBVElPTjIiLAogICAgICAid2ViX2FkZHIiLAogICAgICAid2ViX3BvcnQiLAogICAgICAia2V5X2ZpbGUiLAogICAgICAiY2VydF9maWxlIiwKICAgICAgInBhc3N3b3JkIgogICBdLAogICAiTG9nU2VydmVyQ29uZmlnX2luZm9fIiA6ICIg5pel5b+X5pyNIiwKICAgIkxvZ2ljQWN0b3JEQkNvbmZpZyIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImRhdGFfZGJfbm9kZSIgOiAiIOi/nuaOpUdhdGXoioLngrnphY3nva5cdCIsCiAgICAgICJkYXRhX2RiX25vZGVfX3R5cGVfXyIgOiAiVXNlck5vZGUiLAogICAgICAic2hhcmVfc2VydmVyX2NvZGUiIDogIiDlhbHkuqvokL3lnLBTUUzlronlhajnoIEiLAogICAgICAic2hhcmVfc2VydmVyX3BvcnQiIDogIiDlhbHkuqvokL3lnLDmnI3liqHnvZHnu5xJUCIsCiAgICAgICJzcWxfZGIiIDogIiBNWVNRTOaVsOaNruW6k+mFjee9riIsCiAgICAgICJzcWxfZGJfX3R5cGVfXyIgOiAiU3FsREIiLAogICAgICAidGl0bGUiIDogIiDnqIvluo/ov5DooYznmoTmoIfpopgiLAogICAgICAid29ya2VyX2lkIiA6ICIg57q/56iL5YaFVW5pdOWvueixoeWIhumFjeeahElEIgogICB9LAogICAiTG9naWNBY3RvckRCQ29uZmlnX19saXN0X18iIDogWwogICAgICAidGl0bGUiLAogICAgICAic3FsX2RiIiwKICAgICAgInNoYXJlX3NlcnZlcl9wb3J0IiwKICAgICAgInNoYXJlX3NlcnZlcl9jb2RlIiwKICAgICAgImRhdGFfZGJfbm9kZSIsCiAgICAgICJ3b3JrZXJfaWQiCiAgIF0sCiAgICJMb2dpY0FjdG9yREJDb25maWdfaW5mb18iIDogIiDpgLvovpFBY3RvcuacjeWKoeWZqOmFjee9riIsCiAgICJMb2dpbkNvbmZpZyIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImNlcnRfZmlsZSIgOiAiIHBlbeivgeS5puaWh+S7tiIsCiAgICAgICJrZXlfZmlsZSIgOiAiIHNzbCBLZXkg6K+B5LmmIiwKICAgICAgImxvZ2luX2lkIiA6ICIg6L+e5o6lRELljZXlhYPnmoRJRCAgMTAwMCAyMDAwIOS4uuWMuuWPtywg5bC+5pWw5Li655m76ZmG5pyN5Yqh5Zmo55qESUQiLAogICAgICAibG9naW5fbm9kZSIgOiAiIOi/nuaOpSBEYXRhIERCXHQiLAogICAgICAibG9naW5fbm9kZV9fdHlwZV9fIiA6ICJVc2VyTm9kZSIsCiAgICAgICJwYXNzd29yZCIgOiAiIOWvhueggSIsCiAgICAgICJ0aXRsZSIgOiAiIOeoi+W6j+i/kOihjOeahOagh+mimCIsCiAgICAgICJ3c19pcCIgOiAiIOW8gOaUvndz5Zyw5Z2AIiwKICAgICAgIndzX3BvcnQiIDogIiDlvIDmlL53c+err+WPoyIKICAgfSwKICAgIkxvZ2luQ29uZmlnX19saXN0X18iIDogWwogICAgICAidGl0bGUiLAogICAgICAibG9naW5fbm9kZSIsCiAgICAgICJsb2dpbl9pZCIsCiAgICAgICJ3c19pcCIsCiAgICAgICJ3c19wb3J0IiwKICAgICAgImtleV9maWxlIiwKICAgICAgImNlcnRfZmlsZSIsCiAgICAgICJwYXNzd29yZCIKICAgXSwKICAgIkxvZ2luQ29uZmlnX2luZm9fIiA6ICIgTG9naW5Db25maWcg6L+b56iL6YWN572uIiwKICAgIlJ1bkNvbmZpZ0RhdGEiIDogewogICAgICAiX3N0cnVjdF8iIDogIiIsCiAgICAgICJhY2NvdW50X2NvbmZpZyIgOiAiIOW4kOWPt+acjeWKoeWZqOmFjee9riIsCiAgICAgICJhY2NvdW50X2NvbmZpZ19fdHlwZV9fIiA6ICJBY2NvdW50U2VydmVyQ29uZmlnIiwKICAgICAgImNlbnRlcl9jb25maWciIDogIiDkuJbnlYzkuK3lv4PmnI3liqHlmajphY3nva4iLAogICAgICAiY2VudGVyX2NvbmZpZ19fdHlwZV9fIiA6ICJDZW50ZXJTZXJ2ZXJDb25maWciLAogICAgICAiZ2FtZV9jb25maWciIDogIiDmuLjmiI/mnI3liqHlmajphY3nva5cdCIsCiAgICAgICJnYW1lX2NvbmZpZ19fdHlwZV9fIiA6ICJHYW1lU2VydmVyQ29uZmlnIiwKICAgICAgImdhdGVfY29uZmlnIiA6ICIgR2F0YemFjee9rlx0IiwKICAgICAgImdhdGVfY29uZmlnX190eXBlX18iIDogIkdhdGVTZXJ2ZXJDb25maWciLAogICAgICAibG9naWNfYWN0b3JfY29uZmlnIiA6ICIg6YC76L6RQWN0b3LmnI3liqHlmajphY3nva4iLAogICAgICAibG9naWNfYWN0b3JfY29uZmlnX190eXBlX18iIDogIkxvZ2ljQWN0b3JEQkNvbmZpZyIsCiAgICAgICJsb2dpbl9jb25maWciIDogIiDnmbvpmYbov5vnqIvphY3nva4iLAogICAgICAibG9naW5fY29uZmlnX190eXBlX18iIDogIkxvZ2luQ29uZmlnIiwKICAgICAgInNoYXJlX2RiX2NvbmZpZyIgOiAiIOmAmueUqERC5YWx5Lqr6JC95Zyw5pyN5Yqh5ZmoIiwKICAgICAgInNoYXJlX2RiX2NvbmZpZ19fdHlwZV9fIiA6ICJTaGFyZURCQ29uZmlnIgogICB9LAogICAiUnVuQ29uZmlnRGF0YV9fbGlzdF9fIiA6IFsKICAgICAgInNoYXJlX2RiX2NvbmZpZyIsCiAgICAgICJsb2dpY19hY3Rvcl9jb25maWciLAogICAgICAiZ2F0ZV9jb25maWciLAogICAgICAibG9naW5fY29uZmlnIiwKICAgICAgImFjY291bnRfY29uZmlnIiwKICAgICAgImNlbnRlcl9jb25maWciLAogICAgICAiZ2FtZV9jb25maWciCiAgIF0sCiAgICJSdW5Db25maWdEYXRhX2luZm9fIiA6ICIg5YWo6YOo56iL5bqP6L+Q6KGM6YWN572uIiwKICAgIlNoYXJlREJDb25maWciIDogewogICAgICAiX3N0cnVjdF8iIDogIiIsCiAgICAgICJzZXJ2ZXJfYWRkcmVzcyIgOiAiIOW8gOaUvue7memAu+i+keS9v+eUqOi/m+eoi+eahOe9kee7nOacjeWKoVx0IiwKICAgICAgInNlcnZlcl9hZGRyZXNzX190eXBlX18iIDogIklQQWRkcmVzcyIsCiAgICAgICJ0aXRsZSIgOiAiIOeoi+W6j+i/kOihjOeahOagh+mimCIKICAgfSwKICAgIlNoYXJlREJDb25maWdfX2xpc3RfXyIgOiBbICJ0aXRsZSIsICJzZXJ2ZXJfYWRkcmVzcyIgXSwKICAgIlNoYXJlREJDb25maWdfaW5mb18iIDogIiDpgJrnlKhEQuWFseS6q+iQveWcsOacjeWKoeWZqCIsCiAgICJTaGFyZU1lbSIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImtleSIgOiAiIOWFseS6q+WGheWtmOWIhumFjUtFWSznlKjkuo7lhbHkuqvlhoXlrZjmtojmga/pgJrpgZMiLAogICAgICAic2l6ZSIgOiAiIOWFseS6q+WGheWtmOWIhumFjeWkp+WwjyzmjqjojZDphY3nva4gMjEwMDAwMCDnuqYyTSzmlLblj5HlkIQxTSIKICAgfSwKICAgIlNoYXJlTWVtREJDb25maWciIDogewogICAgICAiX3N0cnVjdF8iIDogIiIsCiAgICAgICJzaGFyZV9rZXkiIDogIiDlhbHkuqvlhoXlrZjphY3nva4iLAogICAgICAic2hhcmVfa2V5X190eXBlX18iIDogIlNoYXJlTWVtIiwKICAgICAgInNxbF9iYWNrX2RiIiA6ICIgTVlTUUzlpIfku73lupPov57mjqXphY3nva4iLAogICAgICAic3FsX2JhY2tfZGJfX3R5cGVfXyIgOiAiU3FsREIiLAogICAgICAic3FsX2RiIiA6ICIgTVlTUUzov57mjqXphY3nva4iLAogICAgICAic3FsX2RiX190eXBlX18iIDogIlNxbERCIiwKICAgICAgInRpdGxlIiA6ICIg56iL5bqP6L+Q6KGM55qE5qCH6aKYIgogICB9LAogICAiU2hhcmVNZW1EQkNvbmZpZ19fbGlzdF9fIiA6IFsgInRpdGxlIiwgInNoYXJlX2tleSIsICJzcWxfZGIiLCAic3FsX2JhY2tfZGIiIF0sCiAgICJTaGFyZU1lbURCQ29uZmlnX2luZm9fIiA6ICIg5YWx5Lqr5YaF5a2Y6L+b56iL6YWN572uIiwKICAgIlNoYXJlTWVtX19saXN0X18iIDogWyAia2V5IiwgInNpemUiIF0sCiAgICJTaGFyZU1lbV9pbmZvXyIgOiAiIOWFseS6q+WGheWtmOWPguaVsCIsCiAgICJTcWxEQiIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgIm1EQkJBU0UiIDogIiBEQui/nuaOpeW6kywg5Y+v5Lul5Li656m6LCDooajnpLrov57mjqUgdGVzdCIsCiAgICAgICJtREJJUCIgOiAiIE15c3FsIOWcsOWdgCIsCiAgICAgICJtREJQQVNTV09SRCIgOiAiIOi/nuaOpeWvhueggSIsCiAgICAgICJtREJQT1JUIiA6ICIgTXlzcWwg56uv5Y+jIiwKICAgICAgIm1EQlVTRVIiIDogIiBNeXNxbOeUqOaIt+WQjSIsCiAgICAgICJtVEFCTEVfTElTVCIgOiAiIOihqOagvOWKoOi9veWIl+ihqCAo5Yib5bu6RELooajmoLzml7YsIOiHquWKqOeUn+aIkCkiCiAgIH0sCiAgICJTcWxEQl9fbGlzdF9fIiA6IFsKICAgICAgIm1EQkJBU0UiLAogICAgICAibURCSVAiLAogICAgICAibURCUE9SVCIsCiAgICAgICJtREJVU0VSIiwKICAgICAgIm1EQlBBU1NXT1JEIiwKICAgICAgIm1UQUJMRV9MSVNUIgogICBdLAogICAiU3FsREJfaW5mb18iIDogIiIsCiAgICJVc2VyTm9kZSIgOiB7CiAgICAgICJfc3RydWN0XyIgOiAiIiwKICAgICAgImdhdGUiIDogIiDov57mjqXnmoRHYXRl5pyN5Yqh5Zyw5Z2AIiwKICAgICAgImdhdGVfX3R5cGVfXyIgOiAiSVBBZGRyZXNzIiwKICAgICAgIm5vZGUiIDogIiDnlKjmiLfoioLngrnlvZPliY3liIbphY3lnLDlnYAiLAogICAgICAibm9kZV9fdHlwZV9fIiA6ICJJUEFkZHJlc3MiCiAgIH0sCiAgICJVc2VyTm9kZV9fbGlzdF9fIiA6IFsgIm5vZGUiLCAiZ2F0ZSIgXSwKICAgIlVzZXJOb2RlX2luZm9fIiA6ICIg5LqR5Y2V5YWD55So5oi36IqC54K5Igp9Cg==";
        return gCommentString; 
    }
 };

