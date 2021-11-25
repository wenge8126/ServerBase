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
		//�趨slot key
		AString slotData = extParam["SLOT"];
		ShareMemTableData::SetSlotData(slotData, mKeyHashSlotList);
	}


	AutoIndex CloudDBTable::NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey)
	{
		// DB �ڴ�������ʹ��STL��Ŀ�Ŀ���
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
				return MEM_NEW IntEasyHashIndex();			// NOTE: ��ID �����ֶ�ʱ, ʹ��EasyHash<int, ARecord> 2017.6.20
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
				return MEM_NEW Int64HashIndex();  //NOTE: ��ID �����ֶ�ʱ, ʹ��EasyHash<Int64, ARecord>		2017.6.20
			else
				return MEM_NEW Int64MapIndex();

		default:
		{
			FieldInfo info = FieldInfoManager::getFieldInfo(indexKeyType);
			if (info != NULL)
			{
				DEBUG_LOG("ERROR: δʵ�� [%s] ���͵�����", info->getTypeString());
			}
			else
			{
				DEBUG_LOG("ERROR: �������ַ��ֶ����� [%d]", indexKeyType);
			}
		}
		}

		return AutoIndex();
	}

	ARecord CloudDBTable::GrowthNewRecord(DataStream *recordData /*= NULL*/)
	{
		// NOTE:  ʹ�÷ֲ����, ������KEY, �����Ƕ�Ӧ�Ĺ�ϣ��
		Int64 newKey = mLastMaxKey + 1;
		if (!mKeyHashSlotList.empty())
		{
			short hashSlot = HASH_SLOT(newKey);
			if (!mKeyHashSlotList.exist(hashSlot))
			{
				// ���䵽SLOT��Χ��
				// ��ȡ˵��, �ҵ��ȵ�ǰ�µ�KEY��ϣ��Ĺ�ϣ, ����һ��ֵ, ������, ����䵽��һ����С�Ĺ�ϣ+��ϣ����
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