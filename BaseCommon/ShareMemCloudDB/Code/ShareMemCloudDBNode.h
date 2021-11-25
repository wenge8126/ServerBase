#pragma once

/*/-------------------------------------------------------------------------
DBNode 优化
1 DB 之间, User与DB之前, 全部使用异步云请求操作方式,
2 去掉DB User 部分相关功能(DBNodeManage), 事件网络服务
3 去掉 DataSoure
4 去掉落地检测
5 冷数据, 在 MemSQL中处理, DB进程使用后, 设置标签, SQL检测后, 清除标签并重置等待时间
6 集成 冷处理与索引功能, 不再兼容其他表
//------------------------------------------------------------------------*/

#ifndef _SHAREMEMCLOUDDBNODE_H_
#define _SHAREMEMCLOUDDBNODE_H_


#include "ShareMemAPI.h"
#include "ShareMemNet.h"

#include "TaskSystem.h"
#include "ShareMemCloudDB.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
// 分布在网络节点上的DB
	任何允许的类型单元都可以快速方便访问DB, 有效提供网络云存储基础 (落地及缓存)
	1 在(type)
	选择一个节点, 创建表格索引服务, 将哈希KEY分布进行索引, 其他节点开启后, 能和加入到索引

	DBUint请求DB时, 如果索引信用不存在, 则先到 索引 TableTypeIndexUnit 请求信息, 然后再发送请求
//-------------------------------------------------------------------------*/
	class ShareCloudDB_Export ShareMemCloudDBNode : public Base<ShareMemCloudDBNode>
	{
	public:
		// 异步协程内回调执行
		virtual void OnDBReadySucceed() {}


		virtual AutoAny GetAttachData() { return AutoAny(); }


		AutoNice DBRunInfo();

	public:		
		virtual AutoTable CreateTable(const char *szIndex, const char *szType);

		void AsyncRequestDBInfo();

		// 异步请求共享内存消息事件
		AutoEvent AwaitRequestMemDB(const char *szRequestEventName, tNiceData *pParamData, int waitMilSecond);


		AutoEvent StartShareMemMsg(const char *szRequestEventName);

		bool SendToShareMemory(Logic::tEvent *pEvent);
	
	public:
		virtual bool InitDB(tNiceData &initParam) ;


		virtual size_t AvailMemory() { return  90000000; }
		virtual size_t NowUseMemory() { return 100; }
		virtual void OnNotifyMemoryWarn(size_t availSize) {}


		virtual void Process() ;
		virtual void LowProcess(int spaceTime) ;

		virtual void Close() ;

		virtual bool ReadyClose() { mbReadyClose = true; return true; }

		// NOTE: 需要在协程函数内使用
		virtual bool CreateDBTable(const char *szTableName, const char *szType, AutoTable table, AutoNice  extParam) ;

		/*/ 根据配置表格创建DB表		
		INT,STRING,STRING,INT,BOOL,STRING,STRING,BOOL
		ID,NAME,TYPE,MAX_LENGTH,IS_INDEX,INFO,SLOT,IS_ORDER
		-1,,,,,,0~99,1
		0,ID,INT64,,,主键,,
		1,NAME,FIELD_CHAR_STRING,127,1,,,
		*/
		virtual bool CreateDBTable(const char *szTableName, AutoTable configTable) ;


	public:
		ShareMemCloudDBNode(int dbCode, const char *szCloudNodeIp, int nCloudNodePort, int nCloudSafeCode, bool bRegisterBaseOperate, int initBackTaskThreadNum);
		~ShareMemCloudDBNode()
		{
			mShareMemNet._free();

		}

		virtual void RegisterShareMemMsg(Logic::tEventCenter *pCenter);

		AutoTable GetTable(const AString &tableName) { return mTableList.find(tableName); }

	public:
		bool					mbReadyClose;
		

	protected:
		// 共享内存
		AutoNet														mShareMemNet;
		NiceData														mInitParam;
		FastHash<AString, AutoTable>						mTableList;
		
	};

	class ShareMemDBNet : public ShareMemNet
	{
	public:
		ShareMemCloudDBNode *mpDB;
	};
}
#endif //_SHAREMEMCLOUDDBNODE_H_