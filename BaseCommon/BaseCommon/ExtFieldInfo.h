/********************************************************************
	created:	2011/09/21
	created:	21:9:2011   16:45
	filename: 	e:\01_Langrisser\third\DataTable\DataBase\ExtFieldInfo.h
	file path:	e:\01_Langrisser\third\DataTable\DataBase
	file base:	ExtFieldInfo
	file ext:	h
	author:		Wenge Yang
	description:
	purpose:	Ext type info, sample unsigned 64 bit int
*********************************************************************/

#ifndef _INCLUDE_EXTFIELD_H_
#define _INCLUDE_EXTFIELD_H_

#include "FieldInfo.h"
#include "FieldTypeDefine.h"
#include "AutoString.h"

#define CHAR_STRING_LENGTH		(33)
//---------------------------------------------------------------------------------------------------
//用于共享内存表格的字符串字段信息, 特点时,动态指定长度, 且使用固定长度进行保存, 创建后不能再改变
class CharStringFieldInfo : public BaseFieldInfo
{
public:
	struct CharString
	{
		const char *mString;
	};

protected:
	int mMaxLength;

public:
	CharStringFieldInfo()
		: mMaxLength(CHAR_STRING_LENGTH)
	{

	}
	virtual ~CharStringFieldInfo(){}

	virtual int getType() const { return FIELD_CHAR_STRING; }
	virtual const char* getTypeString() const { return TYPE_CHARSTRING_NAME; }
	virtual const type_info& getTypeinfo() const { return typeid(CharString); }
    virtual DSIZE getLength() const { return mMaxLength; }

	virtual void setMaxLength(int maxLen) override { mMaxLength = maxLen;  }
	virtual int getMaxLength() const override { return mMaxLength; }
	virtual int getDBDataLength() const override { return getMaxLength()+sizeof(DSIZE); }	// 加4个字节保存长度

	virtual AString getTypeParam() const{ return AString(STRING((int)mMaxLength)); }
	virtual void setTypeParam( const AString &typeParam ){ int len = TOINT(typeParam.c_str()); if (len>0)  mMaxLength = len; }

	virtual bool CanUseShareMemory() const override { return true; }

	virtual bool saveField(DataStream *destData) const override
	{
		BaseFieldInfo::saveField(destData);
		destData->write(getMaxLength());
		return true;
	}

	virtual bool restoreFromData(DataStream *scrData) override
	{
		if (BaseFieldInfo::restoreFromData(scrData))
		{
			if (scrData->read(mMaxLength))
			{				
				return true;
			}
			mMaxLength = CHAR_STRING_LENGTH;
		}
		return false;
	}	

public:
	virtual void init(void *data)
	{
		memset(_ptr(data), 0, getMaxLength());
	}

	virtual bool set(void *data, int nVal) 
	{
		return _setString(data, STRING(nVal ) );
	}

	virtual bool set(void *data, float fVal)
	{
		return _setString(data, STRING(fVal ) );
	}
	virtual bool set(void *data, bool bVal)
	{
		return _setString(data, bVal ? "1":"0" );
		return true;
	}
	virtual bool set(void *data, const char* szVal)
	{
		return _setString(data, szVal);		
	}

	virtual bool set(void *data, const AString &strVal)
	{
		return _setString(data, strVal.c_str() );
	}

	virtual bool setData(void *data, const Data *dataVal)
	{
		return _setString(data, dataVal->c_str() );
	}

	virtual bool get(void *data, int &nVal) const
	{
		nVal = atoi(_ptr_const(data));
		return true;
	}

	virtual bool get(void *data, float &fVal) const
	{
		fVal = (float)atof(_ptr_const(data));
		return true;
	}
	virtual bool get(void *data, bool &bVal) const
	{
		bVal = atoi(_ptr_const(data))!=0;
		return true;
	}
	
	virtual const char* getString(void* data) const { return _ptr_const(data); }

	virtual bool get(void *data, AString &strAString) const
	{		
		strAString = _ptr_const(data);
		return true;
	}

	virtual bool set(void *data, const void* obj, const type_info &info) 
	{
		return false; 
	}
	virtual bool get(void *data, void* obj, const type_info &info) const 
	{
		return false; 
	}

	virtual bool set(void *data, UInt64 uVal64)
	{
		StringTool strTool(uVal64);
		if (strlen(strTool.c_str()) < getMaxLength())
		{
			strcpy(_ptr(data), strTool.c_str());
			return true;
		}
		else
			ERROR_LOG("Restore string length %d > %d", strlen(strTool.c_str()), getMaxLength() - 1);
		return false;
	}
	virtual bool get(void *data, UInt64 &uVal64) const
	{
		uVal64 = TOUINT64(_ptr_const(data));
		//return sscanf(_ptr_const(data), "%llu", &uVal64 )==1;		
		return true;
	}

	virtual bool saveData(void *data, DataStream *destData) const 
	{
		destData->writeString(_ptr_const(data));
		return true;
	}
	virtual bool restoreData(void *data, DataStream *scrData)
	{
		_free(data);
		AString temp;
		if (scrData->readString(temp))
		{
			if (temp.length() <= getMaxLength() - 1)
				strcpy(_ptr(data), temp.c_str());
			else
				ERROR_LOG("Restore string length %d > %d", temp.length(), getMaxLength() - 1);
			//set(data, temp.c_str());
			return true;
		}
		return false;
	}

	virtual void free(void *data)
	{
		_free(data);
	}

	virtual int getIndexValue(void* data) const;

	virtual int getIndexValue(int nIndex) const;
	virtual int getIndexValue(const char* strIndex) const;

protected:	
	bool _setString(void *data, const char *szString)
	{
		if (strlen(szString) <= getMaxLength() - 1)
		{
			strcpy(_ptr(data), szString);
			return true;
		}
		else
			ERROR_LOG("Field [%s] set string %s length %d > %d", getName().c_str(), szString, strlen(szString), getMaxLength() - 1);
		return false;
	}

	char* _ptr(void *data){ return ((char*)data + mPosition); }

	const char* _ptr_const(void *data) const { return (const char*)data + mPosition; }

	void _free(void* data)
    {
        memset(_ptr(data), 0, getMaxLength());
    }
};

class CharStringFieldInfoFactory : public tFieldInfoFactory
{
public:
	virtual int getType() const{ return FIELD_CHAR_STRING; }
	virtual const char* getTypeName() const { return TYPE_CHARSTRING_NAME; }
	virtual tFieldInfo*	createFieldInfo()
	{
		return MEM_NEW CharStringFieldInfo();
	}
};
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//64位整数
class INT64FieldInfo : public BaseFieldInfo
{
public:
	virtual ~INT64FieldInfo() {}

	virtual int getType() const { return FIELD_INT64; }
	virtual const char* getTypeString() const { return TYPE_INT64_NAME; }
	virtual const type_info& getTypeinfo() const { return typeid(Int64); }
	virtual DSIZE getLength() const { return sizeof(Int64); }

	virtual bool CanUseShareMemory() const override { return true; }

	virtual bool set(void *data, int nVal)
	{
		*_ptr(data) = nVal;
		return true;
	}
	virtual bool set(void *data, float fVal)
	{
		*_ptr(data) = (int)fVal;
		return true;
	}
	virtual bool set(void *data, bool bVal)
	{
		*_ptr(data) = (int)(bVal ? 1 : 0);
		return true;
	}
	virtual bool set(void *data, const char* szVal)
	{
		UInt64 x = 0;
		if (sscanf(szVal, ("%llu"), &x) == 1)
		{
			*_ptr(data) = x;
			return true;
		}
		return false;
	}
	virtual bool set(void *data, const AString &strVal)
	{
		return set(data, strVal.c_str());
		return true;
	}

	virtual bool get(void *data, int &nVal) const
	{
		nVal = (int)((*_ptr_const(data)) & 0XFFFFFFFF);
		return true;
	}
	virtual bool get(void *data, float &fVal) const
	{
		fVal = (float)(*_ptr_const(data));
		return true;
	}
	virtual bool get(void *data, bool &bVal) const
	{
		bVal = *_ptr_const(data) != 0;
		return true;
	}

	virtual bool set(void *data, Int64 uVal64) override
	{
		*_ptr(data) = uVal64;
		return true;
	}

	virtual bool set(void *data, UInt64 uVal64)
	{
		*_ptr(data) = (Int64)uVal64;
		return true;
	}

	virtual bool get(void *data, UInt64 &uVal64) const
	{
		uVal64 = (Int64)(*_ptr_const(data));
		return true;
	}

	virtual bool get(void *data, AString &strAString) const
	{
		strAString.setNull();
		strAString += *_ptr_const(data);
		return true;
	}

	virtual bool set(void *data, const void* obj, const type_info &info)
	{
		if (info == typeid(UInt64))
		{
			*_ptr(data) = (Int64)*((UInt64*)obj);
			return true;
		}
		else if (info == typeid(Int64))
		{
			*_ptr(data) = *((Int64*)obj);
			return true;
		}
		return false;
	}
	virtual bool get(void *data, void* obj, const type_info &info) const
	{
		if (info == typeid(UInt64))
		{
			*((UInt64*)obj) = (UInt64)*_ptr_const(data);
			return true;
		}
		else if (info == typeid(Int64))
		{
			*((Int64*)obj) = *_ptr_const(data);
			return true;
		}

		return false;
	}

	virtual bool setData(void *data, const Data *dataVal)
	{
		(*_ptr(data)) = (*dataVal);
		return true;
	}

	virtual bool saveData(void *data, DataStream *destData) const
	{
		destData->write(*_ptr_const(data));
		return true;
	}
	virtual bool restoreData(void *data, DataStream *scrData)
	{
		return scrData->read(*_ptr(data));
	}

	virtual void init(void *data) { *_ptr(data) = 0; }

	virtual void free(void *data)
	{
		*_ptr(data) = 0;
	}

	virtual int getIndexValue(void* data) const { return (int)(*_ptr_const(data)); }
	virtual int getIndexValue(int nIndex) const { return nIndex; }
	virtual int getIndexValue(const char* strIndex) const { return atoi(strIndex); }

protected:
	Int64* _ptr(void *data) { return (Int64*)((char*)data + mPosition); }

	const Int64* _ptr_const(void *data) const { return (Int64*)((char*)data + mPosition); }
};
//---------------------------------------------------------------------------------------------------
class INT64FieldInfoFactory : public tFieldInfoFactory
{
public:
	virtual int getType() const { return FIELD_INT64; }
	virtual const char* getTypeName() const { return TYPE_INT64_NAME; }
	virtual tFieldInfo*	createFieldInfo()
	{
		return MEM_NEW INT64FieldInfo();
	}
};
//-------------------------------------------------------------------------
// 用于KeyValueDB, 保存NiceData > UInt64 key
//-------------------------------------------------------------------------
//无符号64位整数
class UINT64FieldInfo : public BaseFieldInfo
{
public:
	virtual ~UINT64FieldInfo(){}

	virtual int getType() const { return FIELD_UINT64; }
	virtual const char* getTypeString() const{ return TYPE_UINT64_NAME; }
	virtual const type_info& getTypeinfo() const { return typeid(UInt64); }
	virtual DSIZE getLength() const { return sizeof(UInt64); }

	virtual bool CanUseShareMemory() const override { return true; }

	virtual bool set(void *data, int nVal) 
	{
		*_ptr(data) = nVal;
		return true;
	}
	virtual bool set(void *data, float fVal)
	{
		*_ptr(data) = (int)fVal;
		return true;
	}
	virtual bool set(void *data, bool bVal)
	{
		*_ptr(data) = (int)(bVal ? 1 : 0);
		return true;
	}
	virtual bool set(void *data, const char* szVal)
	{
		*_ptr(data) = TOUINT64(szVal);		
		return true;
	}
	virtual bool set(void *data, const AString &strVal)
	{		
		return set(data, strVal.c_str());
		return true;
	}

	virtual bool get(void *data, int &nVal) const
	{
		nVal = (int)((*_ptr_const(data)) & 0XFFFFFFFF);
		return true;
	}
	virtual bool get(void *data, float &fVal) const
	{
		fVal = (float)(*_ptr_const(data));
		return true;
	}
	virtual bool get(void *data, bool &bVal) const
	{
		bVal = *_ptr_const(data) != 0;
		return true;
	}

	virtual bool set(void *data, UInt64 uVal64)
	{
		*_ptr(data) = uVal64;
		return true;
	}

	virtual bool get(void *data, UInt64 &uVal64) const
	{
		uVal64 = (*_ptr_const(data));
		return true;
	}

	virtual bool get(void *data, AString &strAString) const
	{
		strAString.setNull();
		strAString += *_ptr_const(data);
		return true;
	}

	virtual bool set(void *data, const void* obj, const type_info &info)
	{
		if (info==typeid(UInt64))
		{
			*_ptr(data) = *((UInt64*)obj);
			return true;
		}
		return false;
	}
	virtual bool get(void *data, void* obj, const type_info &info) const
	{
		if (info==typeid(UInt64))
		{
			*((UInt64*)obj) = *_ptr_const(data);
			return true;
		}
		return false;
	}

	virtual bool setData(void *data, const Data *dataVal)
	{
		(*_ptr(data)) = (*dataVal);
		return true;
	}

	virtual bool saveData(void *data, DataStream *destData) const 
	{
		destData->write(*_ptr_const(data));
		return true;
	}
	virtual bool restoreData(void *data, DataStream *scrData)
	{
		return scrData->read(*_ptr(data));
	}		

	virtual void init(void *data){ *_ptr(data)= 0; }

	virtual void free(void *data)
	{
		*_ptr(data)= 0;
	}

	virtual int getIndexValue(void* data) const { return (int)(*_ptr_const(data)); }
	virtual int getIndexValue(int nIndex) const { return nIndex; }
	virtual int getIndexValue(const char* strIndex) const { return atoi(strIndex); }

protected:
	UInt64* _ptr(void *data){ return (UInt64*)((char*)data + mPosition); }

	const UInt64* _ptr_const(void *data) const { return (UInt64*)((char*)data + mPosition); }
};
//---------------------------------------------------------------------------------------------------
class UINT64FieldInfoFactory : public tFieldInfoFactory
{
public:
	virtual int getType() const { return FIELD_UINT64; }
	virtual const char* getTypeName() const { return TYPE_UINT64_NAME; }
	virtual tFieldInfo*	createFieldInfo()
	{
		return MEM_NEW UINT64FieldInfo();
	}
};
//-------------------------------------------------------------------------
// 用于KeyValueDB, 保存NiceData > UInt64 key
//-------------------------------------------------------------------------
class CloudDBNiceKeyFieldInfo : public UINT64FieldInfo
{
public:
	virtual ~CloudDBNiceKeyFieldInfo() {}

	virtual int getType() const { return FIELD_CLOUD_NICE; }
	virtual const char* getTypeString() const { return TYPE_DBNICEKEY_NAME; }

};
//---------------------------------------------------------------------------------------------------
class CloudDBNiceKeyFieldInfoFactory : public tFieldInfoFactory
{
public:
	virtual int getType() const { return FIELD_CLOUD_NICE; }
	virtual const char* getTypeName() const { return TYPE_DBNICEKEY_NAME; }
	virtual tFieldInfo*	createFieldInfo()
	{
		return MEM_NEW CloudDBNiceKeyFieldInfo();
	}
};
//---------------------------------------------------------------------------------------------------
// 二进制数据字段, 固定长度
//---------------------------------------------------------------------------------------------------

//class BaseCommon_Export BinaryDataFieldInfo : public BaseFieldInfo
//{
//protected:
//	DSIZE	mLength;
//
//public:
//	BinaryDataFieldInfo()		
//		: mLength(8)
//	{
//
//	}
//	virtual ~BinaryDataFieldInfo(){}
//
//	virtual int getType() const { return FIELD_BINARY; }
//	virtual const char* getTypeString()const{ return FIELD_BINARY_NAME; }
//	virtual const type_info& getTypeinfo() const { return typeid(BinaryDataFieldInfo); }
//
//	virtual DSIZE getLength() const { return mLength; }
//	virtual void setLength(DSIZE length){ mLength = length; }
//
//	bool saveField( DataStream *destData ) const
//	{
//		BaseFieldInfo::saveField(destData);
//		destData->write(mLength);
//		return true;
//	}
//
//	bool restoreFromData( DataStream *scrData )
//	{
//		if (BaseFieldInfo::restoreFromData(scrData))
//			return scrData->read(mLength);
//		return false;
//	}
//
//
//	virtual bool set(void *data, int nVal) { return false; }
//
//	virtual bool set(void *data, float fVal){ return false; }
//	virtual bool set(void *data, bool bVal){ return false; }
//	virtual bool set(void *data, const char* szVal){ return false; }
//	virtual bool set(void *data, const AString &strVal){ return false; }
//
//	virtual bool get(void *data, int &nVal) const { return false; }
//	virtual bool get(void *data, float &fVal) const { return false; }
//	virtual bool get(void *data, bool &bVal) const{ return false; }
//
//	virtual bool set(void *data, UInt64 uVal64){ return false; }
//	virtual bool get(void *data, UInt64 &uVal64) const{  return false; }
//
//	virtual bool get(void *data, Data &destStringData) const { return false; }
//	virtual bool get(void *data, AString &strAString) const;
//
//	// only use function to set data
//	virtual bool set(void *data, const void* obj, const type_info &info);
//
//	virtual bool get(void *data, void* obj, const type_info &info) const ;
//
//
//	virtual bool setData(void *data, const Data *dataVal);
//
//	virtual bool saveData(void *data, DataStream *destData) const 
//	{		
//		destData->write(mLength);
//		destData->_write((void*)_ptr_const(data), getLength());
//		return true; 
//	}
//	virtual bool restoreData(void *data, DataStream *scrData)
//	{
//		DSIZE len = 0;
//		if (scrData->read(len) && len==getLength())
//		{
//			return scrData->_read(_ptr(data), getLength());
//		}
//		return false; 
//	}
//
//	virtual void free(void *data)
//	{
//		_free(data);
//	}
//
//	virtual int getIndexValue(void* data) const { return 0; }
//	virtual int getIndexValue(int nIndex) const { return 0; }
//	virtual int getIndexValue(const char* strIndex) const { return 0; }
//
//protected:
//	char* _ptr(void *data){ return ((char*)data + mPosition); }
//
//	const char* _ptr_const(void *data) const { return ((char*)data + mPosition); }
//
//	bool _saveData(void *thisData, void* scrData, size_t len)
//	{
//		if (scrData!=NULL && (DSIZE)len<=getLength())
//		{
//			memcpy(_ptr(thisData), scrData, len);
//			memset(_ptr(thisData)+len, 0, getLength()-len);
//			return true;
//		}
//		return false;
//	}
//
//	bool _loadData(void *thisData, void *destData, size_t destLen)
//	{
//		if (destData!=NULL && (DSIZE)destLen>=getLength())
//		{
//			memcpy(destData, _ptr(thisData), getLength());
//			memset( (char*)destData+getLength(), 0, destLen-getLength() );
//			return true;
//		}
//		return false;
//	}
//
//	virtual void init(void *data){ memset(_ptr(data), 0, getLength());   }
//
//	void _free(void* data)
//	{ 
//#if DEVELOP_MODE
//		// 配合错误检测, 可以不进行处理
//		memset(_ptr(data), 0, getLength()); 
//#endif
//	}
//
//
//};
////---------------------------------------------------------------------------------------------------
//class BinaryDataFieldInfoFactory : public tFieldInfoFactory
//{
//public:
//	virtual int getType() const { return FIELD_BINARY; }
//	virtual const char* getTypeName() const { return FIELD_BINARY_NAME; }
//	virtual tFieldInfo*	createFieldInfo()
//	{
//		return MEM_NEW BinaryDataFieldInfo();
//	}
//};
////---------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------

#endif //_INCLUDE_EXTFIELD_H_