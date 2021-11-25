#pragma once
#ifndef _INCLUDE_SHAREMEMDBTABLE_H_
#define _INCLUDE_SHAREMEMDBTABLE_H_

#include "ShareMemCloudDB.h"
#include "IndexBaseTable.h"
#include "IndexDBRecord.h"
#include "CloudDBTable.h"
#include "DBDefine.h"
#include "LoopList.h"
#include "PoolLoop.h"

//-------------------------------------------------------------------------
// 共享内存DB表 (2021/4/11 停止使用, 进行修改如下)
// NOTE: 难点与共享进程修改标识同步问题 (去掉记录修改锁方案)
// 1 脏标识还是使用共享内存, 业务进程新加状态数据
// 2 修改后,同时修改业务进程与共享内存的状态, Timer 计时 10秒后,再集中更新消息发送到共享进程(只发达业务层的状态数据)
// 3 状态数据中增加4字节更新次数, 用于判断是否完全更新落地
// 4 业务进程关闭后, 共享进程关闭前, 再根据更新次数和脏标识检查落地
// 5 共享层去掉更新落地线程
// 优势: 1 更新消息不必发送记录数据 2 更新次数减少 3 加强数据完全落地
//-------------------------------------------------------------------------

class ShareMemAO;

namespace NetCloud
{

	class ShareMemCloudDBNode;

	class ShareCloudDB_Export ShareMemDBTable : public CloudDBTable
	{
		friend class ShareMemRecord;
		friend class DBDataIndexRecord;
	public:
		ShareMemDBTable(ShareMemCloudDBNode *pDB);

		virtual void ClearAll() override;

		virtual const char* GetTableType() { return SHARE_MEMORY_TABLE; }

		virtual void Process() override {}
		virtual void LowProcess();

		virtual void InitStart(tNiceData *pSQLParam) {}

	public:
		// 创建记录必须在协程函数中
		virtual ARecord GrowthNewRecord(DataStream *recordData = NULL) override;
		virtual ARecord CreateRecord(Int64 nIndex, bool bReplace) override;

		virtual ARecord CreateRecord(const char* szIndex, bool bReplace) override;

		virtual ARecord NewRecord() override;
		virtual ARecord _NewRecord() override;

		// 从DB彻底删除记录, NOTE: 需要运行在协程函数里
		virtual bool DeleteRecord(ARecord record) override;

	public:
		// 直接从共享内存池恢复所有记录
		virtual bool RestoreFromShareMem(const Array<int> &sharePtrList);

		bool _RestoreRecord(char *pRecordMemPtr);

		void* GetShareRecordMem(SHARE_PTR recordPtr);

		virtual ARecord _ReqeustMemDBCreateRecord(const AString &key, bool bGrowth = false);

		void AppendShareMem(int key, int arrayIndex);

	protected:
		Array<ShareMemAO*>			mShareMemList;

	public:
		PoolLoop<ARecord>				mUpdateRecordList;				// 需要Update的记录列表, 可能重复
		UInt64										mUpdateCount;
		UInt64										mAlreadyUpdateCount;		
	};

	//-------------------------------------------------------------------------
	// 从共享内存恢复的DB记录
	//-------------------------------------------------------------------------
	class ShareCloudDB_Export ShareMemRecord : public IndexDBRecord
	{
		friend class ShareMemDBTable;
	public:
		ShareMemRecord( AutoTable hOwnerTable )
			: IndexDBRecord(hOwnerTable)
		{}
		~ShareMemRecord()
		{
			mRecordData = NULL;
		}

	public:
		void SetShareMem(char *pMemPtr)
		{
			mRecordData = pMemPtr;
			mUpdateFlagBuffer.resize(IndexDBRecord::_getStateDataSize());
		}
		// 在新建时, 就已经获取到共享内存地址
		virtual void _alloctData(int extData) override
		{
			mUpdateFlagBuffer.resize(IndexDBRecord::_getStateDataSize());
		}

		virtual void _freeData() override
		{
			mRecordData = NULL;
		}

		virtual char* _getData() const { return mRecordData; }

		virtual Data _getData(const char *szKey) const override { return get(szKey); }
		virtual Data _getData(int field) const override { return get(field); }

		virtual DSIZE _getStateDataSize() override
		{
			return IndexDBRecord::_getStateDataSize() + sizeof(ShareUpdateInfo);
		}

		virtual ShareUpdateInfo *getUpdateInfo()
		{
			return (ShareUpdateInfo*)(_getStateData() + _getStateDataSize() - sizeof(ShareUpdateInfo));
		}

		virtual void FullAllUpdate(bool bNeed) override
		{
			IndexDBRecord::FullAllUpdate(bNeed);
			memcpy(mUpdateFlagBuffer.data(), _getStateData(), IndexDBRecord::_getStateDataSize());
			if (bNeed)
			{
				Auto<ShareMemDBTable> t = GetTable();
				t->mUpdateRecordList.insert(this);
				t->mUpdateCount++;
				getUpdateInfo()->mShareUpdateCount++;
			}
		}

		virtual bool _setNeedUpdate(int col, bool bNeed = true) override
		{			
			if (bNeed)
			{
				mUpdateFlagBuffer.openState(col);
				Auto<ShareMemDBTable> t = GetTable();
				t->mUpdateRecordList.insert(this);
				t->mUpdateCount++;
				getUpdateInfo()->mShareUpdateCount++;
			}
			else
				mUpdateFlagBuffer.closeState(col);

			return IndexDBRecord::_setNeedUpdate(col, bNeed);
		}

		bool ExistUpdateFlag()
		{
			char *sz = mUpdateFlagBuffer.data();
			for (int i = 0; i < mUpdateFlagBuffer.size(); ++i)
			{
				if (sz[i] != 0)
					return true;
			}
			return false;
		}


	public:
		//NOTE: 在此处检查, 必须保证每次修改后, 都要打一次脏标识! 自动加锁，开锁 (引用计数增加为2 时进行加锁处理)
		virtual void OnUseChanged(int useCount) override;


	public:
		// 用于业务进程标识更新状态数据
		Buffer		mUpdateFlagBuffer;
	};

	//-------------------------------------------------------------------------
	// 只用于从共享内存中恢复记录数据的字段索引, 用于DB进程
	// NOTE: DB进程不可分配新建记录的内存, 新建时协程异步请求等待
	//-------------------------------------------------------------------------
	class ShareMemFieldIndex : public BaseFieldIndex
	{
	public:
		ShareMemFieldIndex()
			:BaseFieldIndex() {}

		virtual INIT_FIELD_MODE getFieldMode() const override { return eShareMemoryUseField; }
	public:
		virtual void* _allotRecordData(uint size) override { /*ERROR_LOG("Can not user _allotRecordData");  AssertEx(0, "Can not user _allotRecordData");*/  return NULL; }

		virtual void _freeRecordData(void *pRecordData) override { /*ERROR_LOG("Can not user _freeRecordData");  AssertEx(0, "Can not user _freeRecordData");*/ }


		virtual DSIZE _getStateDataSize()
		{
			return (getCount() + 7) / 8 + sizeof(ShareUpdateInfo);			
		}

		virtual DSIZE _getRecordMemSize()
		{
			return getDataLength() + _getStateDataSize() + 1;
		}
	};



}
#endif //_INCLUDE_SHAREMEMDBTABLE_H_
