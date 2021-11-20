#pragma once

#ifndef _INCLUDE_ADATA_H_
#define _INCLUDE_ADATA_H_

#include "FieldTypeDefine.h"
#include "FieldInfo.h"
#include "Array.h"

#pragma pack(push)
#pragma pack(1)
//-------------------------------------------------------------------------*
class BaseCommon_Export AData : public MemBase
{
    friend class tNiceData;
    friend class NiceData;
    //friend class IDNiceData;
    friend class Array<AData>;

    typedef tFieldInfo*					FieldInfo;

public:
    AData()
    {
        memset(mData, 0, sizeof(mData));
    }


    ~AData()
    {
        free();
    }
    //----------------------------------------------------------------
    AData(const AData &other)
    {
        memset(mData, 0, sizeof(mData));
        *this = other;
    }
    //-------------------------------------------------------------------------
    bool empty() const { return mData[0] == FIELD_NULL; }
    //-------------------------------------------------------------------------
    AData& operator = (const AData &other)
    {
        int otherInfo = (int)other.mData[0];
        if (otherInfo != FIELD_NULL)
        {
            FieldInfo f = setType(otherInfo);
            Data d(&other);
            f->setData(_dataPtr(), &d);
            return *this;
        }
        else
            free();
        return *this;
    }
    //-------------------------------------------------------------------------
    AData& operator = (int nVal)
    {
        set(nVal);
        return *this;
    }
    AData& operator = (uint uVal)
    {
        set(uVal);
        return *this;
    }

    AData& operator = (float fVal)
    {
        set(fVal);
        return *this;
    }

    AData& operator = (const char* szVal)
    {
        set(szVal);
        return *this;
    }

    AData& operator = (UInt64 IVal)
    {
        set(IVal);
        return *this;
    }

    AData& operator = (Int64 nVal)
    {
        set(nVal);
        return *this;
    }

    AData& operator = (bool bVal)
    {
        set(bVal);
        return *this;
    }

    AData& operator = (const Data &dVal)
    {
        set(dVal);
        return *this;
    }

    AData& operator = (const AString &dVal)
    {
        set(dVal);
        return *this;
    }

    template<typename T>
    AData& operator = (const T &niceData)
    {
        set((void*)&niceData, typeid(T));
        return *this;
    }

	template<typename T>
	AData(const T &value)
	{
		memset(mData, 0, sizeof(mData));
		*this = value;
	}
    //-------------------------------------------------------------------------
    operator int() const
    {
        int val = 0;
        FieldInfo f = _getField();
        if (f)
            f->get(_dataPtr(), val);
        return val;
    }

	operator short() const
	{
		int val = 0;
		FieldInfo f = _getField();
		if (f)
			f->get(_dataPtr(), val);
		return val;
	}

	operator byte() const
	{
		int val = 0;
		FieldInfo f = _getField();
		if (f)
			f->get(_dataPtr(), val);
		return val;
	}

    operator float() const
    {
        float val = 0;
        FieldInfo f = _getField();
        if (f)
            f->get(_dataPtr(), val);
        return val;
    }


    operator UInt64 () const
    {
        UInt64 val = 0;
        FieldInfo f = _getField();
        if (f)
            f->get(_dataPtr(), val);
        return val;
    }

    operator Int64 () const
    {
        UInt64 val = 0;
        FieldInfo f = _getField();
        if (f)
            f->get(_dataPtr(), val);
        return (Int64)val;
    }

    operator bool() const
    {
        bool val = 0;
        FieldInfo f = _getField();
        if (f)
            f->get(_dataPtr(), val);
        return val;
    }

    operator AString () const
    {
        FieldInfo f = _getField();
        if (f)
        {
            const AString *pStr = f->getAString(_dataPtr());
            if (pStr != NULL)
                return *pStr;
            AString s;
            f->get(_dataPtr(), s);
            return s;
        }
        return AString();
    }

    operator tNiceData* () const;

    //-------------------------------------------------------------------------
public:
    bool set(int nVal)
    {
        FieldInfo mTypeInfo = _getField();
        if (!mTypeInfo || FIELD_INT != mTypeInfo->getType())
        {
            mTypeInfo = setType(FIELD_INT);
            if (mTypeInfo == NULL)
                return false;
        }

        return mTypeInfo->set(_dataPtr(), nVal);
    }

    bool set(uint uVal) { return set((int)uVal); }

    bool set(float fVal)
    {
        FieldInfo mTypeInfo = _getField();
        if (!mTypeInfo || FIELD_FLOAT != mTypeInfo->getType())
        {
            mTypeInfo = setType(FIELD_FLOAT);
            if (!mTypeInfo)
                return false;
        }

        return mTypeInfo->set(_dataPtr(), fVal);
    }

    bool set(bool bVal)
    {
        FieldInfo mTypeInfo = _getField();
        if (!mTypeInfo || FIELD_BOOL != mTypeInfo->getType())
        {
            mTypeInfo = setType(FIELD_BOOL);
            if (!mTypeInfo)
                return false;
        }

        return mTypeInfo->set(_dataPtr(), bVal);
    }
    bool set(unsigned char val)
    {
        FieldInfo mTypeInfo = _getField();
        if (!mTypeInfo || FIELD_BYTE != mTypeInfo->getType())
        {
            mTypeInfo = setType(FIELD_BYTE);
            if (!mTypeInfo)
                return false;
        }

        return mTypeInfo->set(_dataPtr(), val);
    }

    bool set(const char* szVal)
    {
        FieldInfo mTypeInfo = _getField();
        if (!mTypeInfo || FIELD_STRING != mTypeInfo->getType())
        {
            mTypeInfo = setType(FIELD_STRING);
            if (!mTypeInfo)
                return false;
        }

        return mTypeInfo->set(_dataPtr(), szVal);
    }

	bool set(Int64 szVal)
	{
		FieldInfo mTypeInfo = _getField();
		if (!mTypeInfo || FIELD_INT64 != mTypeInfo->getType())
		{
			mTypeInfo = setType(FIELD_INT64);
			if (!mTypeInfo)
				return false;
		}

		return mTypeInfo->set(_dataPtr(), szVal);
	}

    bool set(UInt64 szVal)
    {
        FieldInfo mTypeInfo = _getField();
        if (!mTypeInfo || FIELD_UINT64 != mTypeInfo->getType())
        {
            mTypeInfo = setType(FIELD_UINT64);
            if (!mTypeInfo)
                return false;
        }

        return mTypeInfo->set(_dataPtr(), szVal);
    }

    bool set(const Data &kVal);

    bool set(const AString &str)
    {
        setType(FIELD_STRING);
        return _getField()->set(_dataPtr(), str);
    }
 
    bool set(void *obj, const type_info &typeInfo);

    bool get(void *obj, const type_info &typeInfo) const
    {
        FieldInfo mTypeInfo = _getField();
        if (mTypeInfo)
        {
            return mTypeInfo->get(_dataPtr(), obj, typeInfo);
        }
        return false;
    }


    //----------------------------------------------------------------
    FieldInfo setType(int type);

    void init()
    {
        FieldInfo type = _getField();
        if (type)
        {
            type->free(_dataPtr());
        }
    }

    void free()
    {
        init();
        if (!_isThisSpace())
        {
            Allot::freePtr(_dataPtr());
        }
        memset(mData, 0, sizeof(mData));
    }

    bool serialize(DataStream *destData) const;
    bool restore(DataStream *scrData);

    void* _dataPtr() const;

    int _getThisSpace() const { static const int s = sizeof(AString); return s; }
    FieldInfo _getField() const
    {
        return FieldInfoManager::getFieldInfo((int)(byte)mData[0]);
    }
    bool _isThisSpace() const
    {
        FieldInfo type = _getField();
        if (type)
            return type->getLength() <= _getThisSpace();
        return true;
    }

    FIELD_TYPE getType() const
    {
        return (FIELD_TYPE)mData[0];
    }

	static AData & getStatic();

protected:
    char					mData[sizeof(AString) + 1];

};
//----------------------------------------------------------------
#pragma pack(pop)

#endif //_INCLUDE_ADATA_H_