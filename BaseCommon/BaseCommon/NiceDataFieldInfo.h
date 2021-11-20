#pragma once

#ifndef _INCLUDE_NICEDATAFIELDINFO_H_
#define _INCLUDE_NICEDATAFIELDINFO_H_

//----------------------------------------------------------------------------------------------------------------------
// 保存AutoNice字段
//----------------------------------------------------------------------------------------------------------------------
class AutoNiceFieldInfo : public AutoObjFieldInfo<AutoNice, FIELD_NICEDATA>
{
    typedef AutoObjFieldInfo<AutoNice, FIELD_NICEDATA> Base;
protected:
    AString     mTypeParam;
    int		mMaxLength;

public:
    AutoNiceFieldInfo()
        : mMaxLength(16 * 1024)
    {}

    virtual bool CanUseShareMemory() const override { return false; }

public:
    virtual const char* getTypeString()const { return TYPE_NICEDATA_NAME; }
    virtual void setMaxLength(int maxLen) override { mMaxLength = maxLen; }
    virtual int getMaxLength()const override { return mMaxLength; }
    virtual int getDBDataLength() const override { return getMaxLength() + sizeof(DSIZE); }	// 加4个字节保存长度

    virtual AString getTypeParam() const { return mTypeParam; }
    virtual void setTypeParam(const AString &paramData) { mTypeParam = paramData; }

    virtual bool saveField(DataStream *destData) const override
    {
        Base::saveField(destData);
        destData->write(mMaxLength);
        destData->writeString(mTypeParam);
        return true;
    }
    virtual bool restoreFromData(DataStream *scrData) override
    {
        if (Base::restoreFromData(scrData))
        {
            scrData->read(mMaxLength);
            return scrData->readString(mTypeParam);
        }
        return false;
    }

    virtual bool saveData(void *data, DataStream *destData) const
    {
        AutoNice temp;
        if (Base::get(data, &temp, typeid(AutoNice)))
        {
            byte dataType = (byte)(temp ? temp->getType() : NULL_NICEDATA);
            //!!! 使用ID方式序列速度要慢于NiceData，所以不在转换成ID存储
            //if (dataType==NICEDATA)
            //	dataType = (byte)ID_NICEDATA;
            destData->write(dataType);
            if (temp)
                return temp->serialize(destData);
            return true;
        }
        return false;
    }

    virtual bool restoreData(void *data, DataStream *scrData)
    {
        byte dataType = NULL_NICEDATA;
        if (scrData->read(dataType))
        {
            if (dataType != NULL_NICEDATA)
            {
                AutoNice tempNice;
                //if (dataType==ID_NICEDATA)

                //	tempNice = MEM_NEW IDNiceData();
                //else 
				if (dataType == ARRAY_NICEDATA)
					tempNice = MEM_NEW ArrayNiceData();
				else if (dataType == ID_NICEDATA)
					tempNice = MEM_NEW IDNiceData();
				else
                    tempNice = MEM_NEW NiceData();
                bool b = tempNice->restore(scrData);
                if (b)
                    Base::set(data, &tempNice, typeid(tempNice));
                return b;
            }
            return true;
        }
        return false;
    }


    virtual bool set(void *data, const char* szVal)
    {
        AutoNice d;
        Base::get(data, &d, typeid(AutoNice));
        if (!d)
            d = MEM_NEW NiceData();
        if (d->FullJSON(szVal))
            return Base::set(data, &d, typeid(AutoNice));
        return false;
    }
    virtual bool get(void *data, AString &strAString) const
    {
        AutoNice dV;
        Base::get(data, &dV, typeid(AutoNice));
        if (dV)
            strAString = dV->ToJSON();
        else
            strAString = "";
        return true;
    }

    virtual tNiceData* getNiceData(void *data) override 
    { 
        AutoNice d;
        if (Base::get(data, &d, typeid(AutoNice)))
        {
            if (!d)
            {
                d = MEM_NEW NiceData();
                Base::set(data, &d, typeid(AutoNice));
                return d.getPtr();
            }
        }
        return NULL; 
    }
};
//----------------------------------------------------------------------------------------------------------------------
class AutoNiceFieldInfoFactory : public tFieldInfoFactory
{
public:
    AutoNiceFieldInfoFactory() { }

    virtual int getType() const { return FIELD_NICEDATA; }
    virtual const char* getTypeName() const { return TYPE_NICEDATA_NAME; }
    virtual tFieldInfo*	createFieldInfo()
    {
        return MEM_NEW AutoNiceFieldInfo();
    }
};

//----------------------------------------------------------------------------------------------------------------------
// 保存IDNiceData字段, 主要用于DB Record 中
// 优点: KEY为4个字节的整数, 及 紧凑EasyMap空间, 占用空间最小
// 缺点: 取值相对于NiceData较慢, 需要计算HASH 字符串KEY
//----------------------------------------------------------------------------------------------------------------------
class DBNiceFieldInfo : public AutoObjFieldInfo<IDNiceData, FIELD_DBNICE>
{
    typedef AutoObjFieldInfo<IDNiceData, FIELD_DBNICE> Base;

protected:
    AString     mTypeParam;
    int		mMaxLength;

public:
    DBNiceFieldInfo()
        : mMaxLength(16 * 1024)
    {}

    virtual bool CanUseShareMemory() const override { return false; }
    virtual bool DebugCheckFree() const override { return false; }

public:
    virtual const char* getTypeString()const { return TYPE_DBNICE_NAME; }
    virtual void setMaxLength(int maxLen) override { mMaxLength = maxLen; }
    virtual int getMaxLength()const override { return mMaxLength; }
    virtual int getDBDataLength() const override { return getMaxLength() + sizeof(DSIZE); }	// 加4个字节保存长度

    virtual AString getTypeParam() const { return mTypeParam; }
    virtual void setTypeParam(const AString &paramData) { mTypeParam = paramData; }

    virtual bool saveField(DataStream *destData) const override
    {
        Base::saveField(destData);
        destData->write(mMaxLength);
        destData->writeString(mTypeParam);
        return true;
    }
    virtual bool restoreFromData(DataStream *scrData) override
    {
        if (Base::restoreFromData(scrData))
        {
            scrData->read(mMaxLength);
            return scrData->readString(mTypeParam);
        }
        return false;
    }

    virtual bool saveData(void *data, DataStream *destData) const
    {
        auto *pDBNice = _ptr_const(data);
        return pDBNice->serialize(destData);

    }

    virtual bool restoreData(void *data, DataStream *scrData)
    {
        return _ptr(data)->restore(scrData);
    }

    virtual bool setData(void *data, const Data *dataVal) override
    {
        tNiceData *pNice = dataVal->_getFieldInfo()->getNiceData(dataVal->_getData());
        if (pNice!=NULL)
        {
            getNiceData(data)->clear(false);
            getNiceData(data)->append(*pNice, true);
        }
        return false;
    }

    virtual tNiceData* getNiceData(void *data) override
    {
        return _ptr(data);
    }

    virtual bool get(void *data, void* obj, const type_info &info) const override
    {
        ERROR_LOG("[%s] field type %s can not use get(void *data, void* obj, const type_info &info)", getName().c_str(), getTypeString());
        return false;
    }

    virtual bool set(void *data, const void* obj, const type_info &info) override
    {
        ERROR_LOG("*[%s] field type %s can not use set(void *data, void* obj, const type_info &info)", getName().c_str(), getTypeString());
        return false;
    }

    virtual bool set(void *data, const char* szVal)
    {
        ERROR_LOG("DBNice can not use set ( string )");
       
        return false;
    }

    virtual bool get(void *data, AString &strAString) const
    {
        ERROR_LOG("DBNice can not use get ( string )");

        return false;
        
    }
};
//----------------------------------------------------------------------------------------------------------------------
class DBNiceFieldInfoFactory : public tFieldInfoFactory
{
public:
    DBNiceFieldInfoFactory() { }

    virtual int getType() const { return FIELD_DBNICE; }
    virtual const char* getTypeName() const { return TYPE_DBNICE_NAME; }
    virtual tFieldInfo*	createFieldInfo()
    {
        return MEM_NEW  DBNiceFieldInfo();
    }
};

#endif //_INCLUDE_NICEDATAFIELDINFO_H_