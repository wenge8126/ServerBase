
#include "AData.h"
#include "Common.h"
#include "NiceData.h"

AData::operator tNiceData* () const
{
    AutoNice d;
    get(&d, typeid(AutoNice));
    return d.getPtr();
}

//----------------------------------------------------------------
bool AData::set(void *obj, const type_info &typeInfo)
{
    FieldInfo mTypeInfo = _getField();
    if (mTypeInfo)
    {
        if (mTypeInfo->set(_dataPtr(), obj, typeInfo))
            return true;
        // 当前类型很可能保存不了很的类型, 所以下面的错误提示可能不需要
        //ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", mTypeInfo->getTypeString(), mTypeInfo->getName().c_str());
    }

    tFieldInfo *info = FieldInfoManager::getFieldInfo(typeInfo);
    if (info != NULL)
    {
        setType(info->getType());
        bool bResult = info->set(_dataPtr(), obj, typeInfo);
        if (!bResult)
            ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", info->getTypeString(), info->getName().c_str());

        return bResult;
    }

    const char *szName = typeInfo.name();
    if (strlen(szName) >= 4 && memcmp(szName, "enum", 4) == 0)
        return set(*(int*)obj);

    ERROR_LOG("Error: set data fail: type>[%s]", typeInfo.name());

    return false;
}

bool AData::set(const Data &kVal)
{
    FieldInfo info = kVal._getFieldInfo();
    if (info == NULL)
        return true;

    int t = info->getType();
    if (t == FIELD_NULL)
    {
        free();
        return true;
    }
    FieldInfo f = setType(t);
    if (f != NULL)
        return f->setData(_dataPtr(), &kVal);
    return false;
}


bool AData::serialize(DataStream *destData) const
{
    FieldInfo mTypeInfo = _getField();
    if (mTypeInfo)
    {
        unsigned char type = (unsigned char)(mTypeInfo->getType() & 0xFF);
        destData->write(type);
        return mTypeInfo->saveData(_dataPtr(), destData);
    }
    if (getType() == FIELD_NULL)
    {
        destData->write((byte)0);
        return true;
    }
    return false;
}

bool AData::restore(DataStream *scrData)
{
    unsigned char type = 0;
    if (!scrData->read(type))
    {
        free();
        return false;
    }
    FieldInfo f = setType((int)type);
    if (f)
    {
        return f->restoreData(_dataPtr(), scrData);
    }

    free();
    return type == FIELD_NULL;
}

void* AData::_dataPtr() const
{
	FieldInfo field = _getField();
	if (field)
	{
		if (field->getLength() > _getThisSpace())
		{

			return (void*)*(size_t*)&mData[1];
		}
		else
			return (void*)&mData[1];
	}
	return NULL;
}

AData & AData::getStatic()
{
	thread_local static AData ms;
	return ms;
}

FieldInfo AData::setType(int type)
{
    if (type == FIELD_NULL)
    {
        return NULL;
    }

    switch (type)
    {
        case FIELD_CHAR_STRING:
            type = FIELD_STRING;
            break;
        case FIELD_SQLDATA:
            type = FIELD_DATA;
            break;

        case FIELD_DB_TABLE:
            NOTE_LOG("WARN: AData 保存 DB_TABLE类型");
            type = FIELD_TABLE;
            //return NULL;
            //case FIELD_BINARY:
            //	AssertEx(0, "不支持保存类型 > FIELD_BINARY");
            //	return false;
    }

    FieldInfo typeInfo = _getField();
    if (typeInfo && type == typeInfo->getType())
        return typeInfo;

    free();
    FieldInfo f = FieldInfoManager::getFieldInfo((FIELD_TYPE)type);
    if (f != NULL)
    {
        mData[0] = (byte)type;
        if (f->getLength() > _getThisSpace())
        {
			void *p = ALLOCT_NEW(f->getLength());
			*(size_t*)&mData[1] = (size_t)p;
			// 必须初始
			memset(p, 0, f->getLength());
			f->init(p);
        }
        return f;
    }
    else
        ERROR_LOG("AData 不支持类型 %d", type);
    return NULL;
}

//bool AData::_resetTypeLength( size_t length )
//{
//	if (NULL!=mTypeInfo)
//	{
//		if ( mTypeInfo->getLength()>4 )
//		{
//			if (mData!=NULL)
//				Allot::freePtr(mData, mTypeInfo->getLength());
//			mData = NULL;
//		}
//		mTypeInfo->setLength(length);
//		_createData();
//		return true;
//	}
//	return false;
//}

//-------------------------------------------------------------------------*
