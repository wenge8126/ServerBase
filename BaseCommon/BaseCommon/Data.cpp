
#include "Data.h"

#include "FieldInfo.h"
#include "NiceData.h"
#include "BaseTable.h"


Data::operator ArrayNiceData*() const
{
    AutoArrayData d;
    get(&d, typeid(d));
    if (d)
        return d.getPtr();

    AutoNice n;
    get(&n, typeid(AutoNice));
    return dynamic_cast<ArrayNiceData*>(n.getPtr());
}

bool Data::get(int &nVal) const
{
	if (!empty())
		return _getFieldInfo()->get(_getData(), nVal);
	return false;
}

bool Data::get( float &fVal ) const
{
	if (!empty())
		return _getFieldInfo()->get(_getData(), fVal);
	return false;
}

bool Data::get(UInt64 &uResult) const 
{ 
	if (_getFieldInfo() && _getData()) return _getFieldInfo()->get(_getData(), uResult); return false; 
}

bool Data::get( bool &bVal ) const
{
	if (!empty())
		return _getFieldInfo()->get(_getData(), bVal);
	return false;
}

bool Data::get( void *obj, const type_info &typeInfo ) const
{
	if (empty())
	{
		// ERROR_LOG("Error: get data [%s] fail, now empty", typeInfo.name());
		return false;
	}
	if (_isAData())
	{
		return _getFieldInfo()->get(_getData(), obj, typeInfo);
	}
	else
	{
		// 为了支持 扩展DBData
		bool bResult = _getRecord()->get(mFieldCol, obj, typeInfo);
		if (bResult)
			return true;

		if (_getFieldInfo() != NULL)
			ERROR_LOG("Error: get data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str())
		else
			ERROR_LOG("Error: get data fail: Record col %d", mFieldCol);

		return false;
	}
	return false;
}

bool Data::get( unsigned char &bVal ) const
{
	if (!empty())
	{
		int nV = 0;
		bool b = get(nV);
		bVal = nV;
		return b;
	}
	return false;
}

Data & Data::operator=(const Data &other)
{
    if (mData==NULL && mFieldCol==-1)
    {
        if (!other.empty())
            ERROR_LOG("Set Data target is empty");
        return *this;
    }
    if (!setData(other))
        ERROR_LOG("Data = fail");
    return *this;
}

Data::operator const AString& () const
{
	//AString str;
	//if (!empty())
	//	_getFieldInfo()->get(_getData(), str);
	return string();
}

bool Data::comType( tFieldInfo *info ) const
{
    if (_getFieldInfo()==NULL)
        return info==NULL;
    if (info==NULL)
        return false;
	return info->getType()==_getFieldInfo()->getType();
}

int Data::getFieldType() const
{
    tFieldInfo *pInfo = _getFieldInfo();
    if (pInfo!=NULL)
        return pInfo->getType();
    return FIELD_NULL;
}

bool Data::isInt( void ) const
{
	if (_getFieldInfo()) return _getFieldInfo()->getType()==FIELD_INT; return false;
}

bool Data::isString( void ) const
{
	if (_getFieldInfo()) return _getFieldInfo()->getType()==FIELD_STRING; return false;
}

bool Data::isFloat( void ) const
{
	if (_getFieldInfo()) return _getFieldInfo()->getType()==FIELD_FLOAT; return false;
}

bool Data::isBool( void ) const
{
	if (_getFieldInfo()) return _getFieldInfo()->getType()==FIELD_BOOL; return false;
}

const AString& Data::string() const
{	
	 if (empty())
	 {
		 return mString;
	 }

	 //const char *szStr = _getFieldInfo()->getString(_getData());

	 //if (szStr!=NULL)
	//	 return szStr;
	 if (_getFieldInfo() != NULL && _getData() != NULL)
	 {
		 const AString *szStr = _getFieldInfo()->getAString(_getData());
		 if (szStr != NULL)
			 return *szStr;

		 if (!_getFieldInfo()->get(_getData(), mString))
			 ERROR_LOG("Get string fail");
		 return mString;
	 }
	 else
		 ERROR_LOG("字段信息为空或数据空间为空");

	 return mString;

	//if (empty() || !mString.empty())
	//	return mString.c_str();
	//StringData temp;
	//_getFieldInfo()->get(_getData(), temp);
	//if (temp.mCharString)
	//	return temp.mCharString;
	//*((AString*)(&mString)) = temp.mString;
	//
	//return mString.c_str();
}

const char* Data::c_str() const
{
	if (_getFieldInfo() != NULL && _getData() != NULL)
	{
        const char *szStr = _getFieldInfo()->getString(_getData());
        if (szStr != NULL)
            return szStr;
		if (!_getFieldInfo()->get(_getData(), mString))
			ERROR_LOG("获取字符失败");
	}
	else
		ERROR_LOG("字段信息为空或数据空间为空");

	return mString.c_str();
}

bool Data::set( void *pValue, const type_info &valueType )
{
	if (_isAData())
	{
		// mFieldCol<0 _getData() 保存的是 AData的指针
		AData *p = (AData*)mData;
		return p->set(pValue, valueType);		
	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, pValue, valueType);	
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return bResult;
	}
	else
		ERROR_LOG("Error: set data fail > [%s]", valueType.name());

	return false;
}

Data& Data::operator=( const char *szValue )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(szValue);		

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, szValue);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%s]", szValue);

	return *this;
}

Data& Data::operator=( const int value )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(value);		
	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, value);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%d]", value);

	return *this;
}

Data& Data::operator=( const short value )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(value);	

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, value);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%d]", value);

	return *this;
}

Data& Data::operator=( const bool value )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(value);	

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, value);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%d]", value ? 1:0);

	return *this;
}

Data& Data::operator=( const float value )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(value);	

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, value);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%f]", value);

	return *this;
}

Data& Data::operator=(const Int64 value)
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(value);

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, value);
		if (!bResult)
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > ["INT64_FORMAT"]", value);

	return *this;
}

Data& Data::operator=( const UInt64 value )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(value);	

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, (Int64)value);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%llu]", value);

	return *this;
}

Data& Data::operator=( DataStream *value )
{
	AutoData  d(value);
	set(&d, typeid(d));
	return *this;
}

Data& Data::operator=( tNiceData *value )
{
	AutoNice  d(value);
	set(&d, typeid(d));
	return *this;
}

Data& Data::operator=( tBaseTable *value )
{
	AutoTable  d(value);
	set(&d, typeid(d));
	return *this;
}

Data& Data::operator=( const AString &szValue )
{
	if (_isAData())
	{
		AData *p = (AData*)mData;
		p->set(szValue);		

	}
	else if (!empty())
	{
		bool bResult = _getRecord()->set(mFieldCol, szValue);		
		if (!bResult)		
			ERROR_LOG("Error: set data fail: TYPE>[%s], Name>[%s]", _getFieldInfo()->getTypeString(), _getFieldInfo()->getName().c_str());		

		return *this;
	}
	else
		ERROR_LOG("Error: set data fail > [%s]", szValue.c_str());

	return *this;
}

Data::operator DataStream*() const
{
	AutoData d;
	get(&d, typeid(AutoData));
	return d.getPtr();
}

Data::operator tBaseTable*() const
{
	BaseRecord *r = _getRecord();
	if (r!=NULL)
		return r->GetFieldTable(mFieldCol);

	AutoTable t;
	get(&t, typeid(AutoTable));
	return t.getPtr();
}

Data::operator tNiceData*() const
{
	AutoNice n;
	get(&n, typeid(AutoNice));
	return n.getPtr();
}

tFieldInfo* Data::_getFieldInfo() const
{
	if (mData!=NULL)
	{		
		if (mFieldCol>=0)
		{
			BaseRecord *p = (BaseRecord*)mData;
			return p->getFieldInfo(mFieldCol);
		}
		else
		{
			AData *p = (AData*)mData;
			return p->_getField();
		}
	}
	return NULL;
}

void* Data::_getData() const
{
	if (mData!=NULL)
	{		
		if (mFieldCol>=0)
		{
			BaseRecord *p = (BaseRecord*)mData;
			return p->_getData();
		}
		else
		{
			AData *p = (AData*)mData;
			return p->_dataPtr();
		}
	}
	return NULL;
}

BaseRecord* Data::_getRecord() const
{
	if (mFieldCol!=-2)
		return (BaseRecord*)mData;

	return NULL;
}

bool Data::empty() const
{
	if (mData==NULL)
		return true;
	
	if (_isAData())
	{
		AData *p = (AData*)mData;
		return p->empty();		
	}
	//!!! NOTE: 记录时, 有可能使用负的列, 表示记录的扩展数据 DBData
	//else if (mFieldCol>=0 )
	//{
	//	BaseRecord *p = (BaseRecord*)mData;
	//	return p->getFieldInfo(mFieldCol)==NULL;
	//}

	return false;
}

bool Data::setData( const Data &other )
{    
    if (other.mData==NULL)
        return true;

	if (_isAData())
	{
		// mFieldCol<0 _getData() 保存的是 AData的指针
		AData *p = (AData*)mData;
		return p->set(other);
	}
	else if (!empty())
	{
		BaseRecord *pRecord = _getRecord();
		FieldInfo info = pRecord->getFieldInfo(mFieldCol);
		if (info!=NULL)
		{
			pRecord->NotifyChanged(mFieldCol);

            if (other._getFieldInfo()==NULL)
            {
                // NOTE: 如果参数字段为空, 则使用原字段类型的空AData进行设置, 恢复到默认值
                AData defaultValue;           
                defaultValue.setType(info->getType());            
				Data d(&defaultValue);
                return info->setData(pRecord->_getData(), &d);	
            }

			return info->setData(pRecord->_getData(), &other);	
		}
		else
			ERROR_LOG("Record no exist col %d", mFieldCol);
	}
    else
        ERROR_LOG("空的数据设置数据>%s", other.string().c_str());
	return false;
}
