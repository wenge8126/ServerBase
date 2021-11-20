#include "IDNiceData.h"

AData& IDNiceData::getOrCreate(const char* key)
{
	int nKey = ToKeyID(key);
	AData *dest = mDataMap.findPtr(nKey);

	if (NULL==dest)
	{
		mDataMap.insert(nKey, AData());
		return mDataMap.find(nKey);
	}
	else
	{
		return *dest;
	}
}

//-------------------------------------------------------------------------*/
#define AUTODATA_COUNT_TYPE		unsigned char
#define AUTODATA_COUNT_MAX		(0xff)

bool IDNiceData::serialize(DataStream *destData) const
{
	if (destData->IsLoopBuffer())
	{
		int saveCount = 0;

		for (auto it = mDataMap.begin(); it.have(); it.next())
		{
			const auto &val = it.Value();
			// 如果字段不存在,或不需要序列化, 一般用于保存特殊数据, 如脚本智能对象
			FieldInfo f = val.mVal._getField();
			if (!f || !f->needSaveToBuffer())
			{
				continue;
			}

			++saveCount;
		}

		if (saveCount > AUTODATA_COUNT_MAX)
		{
			ERROR_LOG("IDNiceData 数量[%llu]超过了 %d\r\n%s"
				, saveCount
				, (int)AUTODATA_COUNT_MAX
				, dump().c_str()
			);
			AssertEx(0, "NiceData %d count too most then %d", saveCount, AUTODATA_COUNT_MAX);
			return false;
		}

		//size_t sizePos = destData->tell();
		destData->write((AUTODATA_COUNT_TYPE)saveCount);

		for (auto it = mDataMap.begin(); it.have(); it.next())
		{
			const auto &val = it.Value();
			// 如果字段不存在,或不需要序列化, 一般用于保存特殊数据, 如脚本智能对象
			FieldInfo f = val.mVal._getField();
			if (!f || !f->needSaveToBuffer())
			{
				continue;
			}

			destData->write((int)val.mKey);
			if (!val.mVal.serialize(destData))
				return false;
		}
		return true;
	}

	if (count() > AUTODATA_COUNT_MAX)
	{
		ERROR_LOG("IDNiceData 数量[%llu]超过了 %d\r\n%s"
			, count()
			, (int)AUTODATA_COUNT_MAX
			, dump().c_str()
		);
		AssertEx(0, "NiceData %d count too most then %d", (int)count(), AUTODATA_COUNT_MAX);
		return false;
	}

	AUTODATA_COUNT_TYPE s = (AUTODATA_COUNT_TYPE)count();

	size_t sizePos = destData->tell();
	destData->write(s);

	AUTODATA_COUNT_TYPE saveCount = 0;

	for (auto it=mDataMap.begin(); it.have(); it.next())
	{
		const auto &val = it.Value();
		// 如果字段不存在,或不需要序列化, 一般用于保存特殊数据, 如脚本智能对象
		FieldInfo f = val.mVal._getField();
		if (!f || !f->needSaveToBuffer())
		{			
			continue;
		}

		++saveCount;
		
		destData->write((int)val.mKey);

		if (!val.mVal.serialize(destData))
			return false;
	}

	if (saveCount<s)
	{
		size_t nowPos = destData->tell();
		destData->seek(sizePos);
		destData->write(saveCount);
		destData->seek(nowPos);
	}

	return true;
}

bool IDNiceData::restore(DataStream *scrData)
{
	// 可以读取恢复 NiceData的数据
	//byte type = 0;
	//if (!scrData->read(type))
	//	return false;	

	//if (type!=ID_NICEDATA && type!=NICEDATA)
	//{
	//	ERROR_LOG("Type [%d]: IDNiceData 无法恢复, 错误的数据", type);
	//	return false;
	//}

	//if (type==NICEDATA)
	//{
	//	WARN_LOG("Type [%d]: IDNiceData 读取恢复 NiceData 的数据");
	//}	

	mDataMap.clear(false);

	AUTODATA_COUNT_TYPE s = 0;

	if (!scrData->read(s))
		return false;

	for (size_t i=0; i<s; ++i)
	{
		int k = 0;

		//if (type == ID_NICEDATA)
		//{
			if (!scrData->read(k) )
				return false;
		//}
		//else
		//{
		//	AString keyName;
		//	if (!scrData->readString(keyName))
		//		return false;

		//	k = MAKE_INDEX_ID(keyName.c_str());
		//}

		mDataMap.insert(k, AData());
		if (!mDataMap.find(k).restore(scrData))
			return false;
	}

	return true;
}

size_t IDNiceData::append(const tNiceData &scrData, bool bReplace)
{
    const IDNiceData *pIDNice = dynamic_cast<const IDNiceData*>(&scrData);
    if (pIDNice != NULL)
    {
        for (int i = 0; i < pIDNice->mDataMap.size(); ++i)
        {
            auto v = mDataMap.getValue(i);
            if (bReplace)
                mDataMap.erase(v.mKey);
            else if (!mDataMap.find(v.mKey).empty())
                continue;
            mDataMap.insert(v.mKey, v.mVal);
        }
        return mDataMap.size();
    }
    else if (scrData.getType() == NICEDATA)
    {
        size_t resultCount = 0;
        for (auto it = scrData.begin(); it.have(); it.next())
        {
            int key = ToKeyID(it.key().c_str());

            // 如果替换, 先直接销毁元素, 不替换但如果存在,则直接处理下一个
            if (bReplace)
                mDataMap.erase(key);
            else if (!mDataMap.find(key).empty())
                continue;

            const AData &d = it.get();
            mDataMap.insert(key, d);
            ++resultCount;
        }
        return resultCount;
    }

    ERROR_LOG("IDNiceData append 追加的源数据必须为 IDNiceData 或者 NiceData, now type [%d]", scrData.getType());
    return 0;
}
