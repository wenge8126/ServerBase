#include "CloudDBTable.h"
#include "mShareMemTableData.h"

#include "StringHashIndex.h"
#include "Int64MapIndex.h"
#include "MultipleInt64MapIndex.h"
#include "MultipleStringIndex.h"
#include "IntEasyHashIndex.h"
#include "Int64HashIndex.h"
#include "StringEasyHashIndex.h"
#include "DBDefine.h"
#include "mShareMemTableData.h"
#include "MultipleUInt64MapIndex.h"

namespace NetCloud
{

	void CloudDBTable::ApplyExt(AutoNice extParam)
	{
		//设定slot key
		AString slotData = extParam["SLOT"];
		ShareMemTableData::SetSlotData(slotData, mKeyHashSlotList);
	}


	AutoIndex CloudDBTable::NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey)
	{
		// DB 内存表的索引使用STL，目的快速
		switch (indexKeyType)
		{
			//case FIELD_FLOAT:
		case FIELD_CHAR_STRING:
		case FIELD_STRING:
			if (bMultKey)
			{
				return MEM_NEW STL_MultipleStringIndex();
			}
			if (bHash)
				return MEM_NEW StringEasyHashIndex();
			else
				return MEM_NEW StringHashIndex();

		case FIELD_BYTE:
		case FIELD_SHORT:
		case FIELD_INT:
			if (bMultKey)
			{
				return MEM_NEW MultipleInt64MapIndex();
			}
			else if (bHash)
				return MEM_NEW IntEasyHashIndex();			// NOTE: 非ID 名称字段时, 使用EasyHash<int, ARecord> 2017.6.20
			else
				return MEM_NEW Int64MapIndex();

		case FIELD_INT64:
		case FIELD_UINT64:
			if (bMultKey)
			{
				if (indexKeyType == FIELD_UINT64)
					return MEM_NEW MultipleUInt64MapIndex();
				else
					return MEM_NEW MultipleInt64MapIndex();
			}
			else if (bHash)
				return MEM_NEW Int64HashIndex();  //NOTE: 非ID 名称字段时, 使用EasyHash<Int64, ARecord>		2017.6.20
			else
				return MEM_NEW Int64MapIndex();

		default:
		{
			FieldInfo info = FieldInfoManager::getFieldInfo(indexKeyType);
			if (info != NULL)
			{
				DEBUG_LOG("ERROR: 未实现 [%s] 类型的索引", info->getTypeString());
			}
			else
			{
				DEBUG_LOG("ERROR: 不存在字符字段类型 [%d]", indexKeyType);
			}
		}
		}

		return AutoIndex();
	}

	ARecord CloudDBTable::GrowthNewRecord(DataStream *recordData /*= NULL*/)
	{
		// NOTE:  使用分布表格, 产生的KEY, 必须是对应的哈希槽
		Int64 newKey = mLastMaxKey + 1;
		if (!mKeyHashSlotList.empty())
		{
			short hashSlot = HASH_SLOT(newKey);
			if (!mKeyHashSlotList.exist(hashSlot))
			{
				// 分配到SLOT范围内
				// 获取说明, 找到比当前新的KEY哈希大的哈希, 即下一个值, 不存在, 则分配到第一个最小的哈希+哈希总数
				bool bHave = false;
				for (int i = 0; i < mKeyHashSlotList.size(); ++i)
				{
					if (mKeyHashSlotList.getKey(i) > hashSlot)
					{
						bHave = true;
						newKey += mKeyHashSlotList.getKey(i) - hashSlot;
						break;
					}
				}
				if (!bHave)
				{
					newKey += mKeyHashSlotList.getKey(0) + DB_HASH_SLOT_COUNT - hashSlot;
				}
			}
		}
		mLastMaxKey = newKey;
		return CreateRecord(newKey, true);
	}

}