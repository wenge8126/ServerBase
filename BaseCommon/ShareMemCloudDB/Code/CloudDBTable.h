#pragma once

#ifndef _INCLUDE_CLUDEDBTABLE_H_
#define _INCLUDE_CLUDEDBTABLE_H_

#include "ShareMemCloudDB.h"
#include "IndexBaseTable.h"
#include "DBDefine.h"

namespace NetCloud
{
	//-------------------------------------------------------------------------
	class ShareCloudDB_Export CloudDBTable : public SkipBaseTable
	{
		friend class ShareMemCloudDBNode;
		friend class CloudDBRecord;

	public:
		CloudDBTable(ShareMemCloudDBNode *pDB, INIT_FIELD_MODE initField = eInitNullField)
			: SkipBaseTable(initField)
			, mpDB(pDB)
			, mLastMaxKey(0)
			, mUpdateCount(0)
		{}

	public:
		virtual void LowProcess() {}

		virtual AutoIndex NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey) override;
		virtual ARecord GrowthNewRecord(DataStream *recordData /*= NULL*/);

	public:
		virtual void InitStart(tNiceData *pSQLParam) {}
		virtual bool RestoreFromShareMem(const Array<int> &sharePtrList) { return true;  }
		virtual void ApplyExt(AutoNice extParam);


		virtual AString RunInfo() { return GetTableType(); }

	protected:
		EasyMap<short, bool>			mKeyHashSlotList;					// 保存记录KEY对应的哈希SLOT
		ShareMemCloudDBNode		*mpDB;
		Int64										mLastMaxKey;

		UInt64									mUpdateCount;
	};
	//-------------------------------------------------------------------------
}

#endif  //_INCLUDE_CLUDEDBTABLE_H_