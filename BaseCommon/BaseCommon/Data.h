/********************************************************************
	created:	2011/07/11
	created:	11:7:2011   23:24
	filename: 	e:\NewTable__0709\NewTable\Common\DataBase\Data.h
	file path:	e:\NewTable__0709\NewTable\Common\DataBase
	file base:	Data
	file ext:	h
	author:		Yang Wenge
	
	purpose:	
*********************************************************************/

#ifndef _INCLUDE_DATA_H_
#define _INCLUDE_DATA_H_

#include "AutoString.h"
#include "FieldTypeDefine.h"

#define CSTR	string().c_str()

#if __LINUX__
#include <typeinfo>
using namespace std;
#endif

//-------------------------------------------------------------------------*

class tFieldInfo;
class tBaseTable;
class DataStream;
class tNiceData;
class NiceData;
class ArrayNiceData;
class BaseRecord;
class AData;
//------------------------------------------------------------------------------------
// 方便容器取值,设置值的中间结构
// NOTE: 注意 = 操作为引用类型方式, 即对左值 Data = 操作会使用后面Data的值设置到目标Data中
// 为避免错误, = 左值检查为空时报错
//------------------------------------------------------------------------------------
struct BaseCommon_Export Data : public MemBase
{
	friend class AData;
	friend class DataRecord;
	friend class tNiceData;
	friend class NiceData;
	friend class BaseRecord;
	friend class BinaryDataFieldInfo;
	friend class DataTool;
    friend class DBNiceFieldInfo;

public:
	Data()		
		: mData(NULL)
		, mFieldCol(-1)
	{
	}
	
	~Data()
	{
		
	}

	bool empty() const;	

	Data(const Data &other)
		: mData(NULL)
		, mFieldCol(-1)
	{	
        mFieldCol = other.mFieldCol;
        mData = ((Data*)&other)->mData;
	}

    Data(const AData *d)
    {
        mFieldCol = -2;
        mData = (void*)d;
    }

	Data(void *pDataValue, int fieldCol)
		: mData(pDataValue)
		, mFieldCol(fieldCol)
	{

	}

    //NOTE:  = 操作为引用类型方式
	Data & operator = (const Data &other);	

public:
	template<typename T>
	Data& operator = (const T &value)
	{
		set((void*)&value, typeid(T));
		return *this;
	}
    //!!! NOTE: 对与索引DB记录, 不可以使用此功能修改数据, 否则不会触发索引记录变化
	bool setData(const Data &other);
	bool set(void *pValue, const type_info &valueType);

	Data& operator = (const int value);
	Data& operator = (const short value);
	Data& operator = (const ushort value){ return *this=(int)value; }
	Data& operator = (const uint value) { return *this = (UInt64)value; }
	Data& operator = (const byte value) { return *this = (ushort)value; }
	Data& operator = (const char value) { return *this = (short)value; }
	Data& operator = (const bool value);
	Data& operator = (const float value);
	Data& operator = (const Int64 value);
	Data& operator = (const UInt64 value);
	Data& operator = (const char *szValue);
	Data& operator = (const AString &szValue);

	Data& operator = (DataStream *value);
	Data& operator = (tNiceData *value);
	Data& operator = (tBaseTable *value);
	
public:
	operator int () const
	{
		int n = 0;
		get(n);
		return n;
	}

	operator float () const
	{
		float fVal = 0.0f;
		get(fVal);
		return fVal;
	}

	//operator const char* () const
	//{
	//	return c_str();
	//}

	operator const AString& () const;

	operator bool () const
	{
		bool b = false;
		get(b);
		return b;
	}

	operator unsigned char () const
	{
		int val = 0;
		get(val);
		return (unsigned char)val;
	}

	operator short () const
	{
		int x = 0;
		get(x);
		return (short)x;
	}

	operator Int64 () const { UInt64 uN = 0; get(uN); return (Int64)uN;  }

	operator UInt64 () const
	{ 
		UInt64 uN = 0; get(uN); return uN; 
	}

	operator DataStream* () const;

	operator tBaseTable* () const;

	operator tNiceData*() const;

	operator NiceData* () const
	{
		return (NiceData*)(tNiceData*)(*this);
	}

    operator ArrayNiceData* () const;

	bool get(int &nVal) const;

	bool get(UInt64 &uResult) const ;

	bool get(float &fVal) const;

	bool get(bool &bVal) const;

	bool get(unsigned char &bVal) const;

	const AString& string() const;
	const char* c_str() const;

	bool get(void *obj, const type_info &typeInfo) const;

	bool isInt(void) const;
	bool isString(void) const;
	bool isFloat(void) const;
	bool isBool(void) const;

	bool comType(tFieldInfo *info) const;
    int getFieldType() const;

protected:
	tFieldInfo* _getFieldInfo() const;
	BaseRecord* _getRecord() const;
	void* _getData() const;

	bool _isAData() const { return mFieldCol==-2 && mData!=NULL; }

protected:
	int				mFieldCol;	// ==-2 表示为AData, >=0 表示记录, ==-1 表示为空
	void				*mData;	
	mutable		AString			mString;
};



#endif //_INCLUDE_DATA_H_