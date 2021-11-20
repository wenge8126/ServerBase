#ifndef _INCLUDE_SQLDATAFIELD_H_
#define _INCLUDE_SQLDATAFIELD_H_

#include "FieldInfo.h"
#include "DBTool.h"
//-------------------------------------------------------------------------
// ��ʱDBSql���漰��ȡ����
// �ֶ���ϢΪ��������
// NOTE: ���ֶ�ֻ����ʹ����DB��¼��, ��ռ�ü�¼�ռ�
// ���ֶα�������Ϊ SetSkipUpdate, ������Ҫ���µ�DB
//-------------------------------------------------------------------------
class BaseCommon_Export_H SqlDataFieldInfo : public BaseFieldInfo
{
	typedef BaseFieldInfo base;

protected:
	int mMaxLength;
	AutoDBTool	mDBTool;
	AString mKeyFieldName;
	AString mTableName;
    AutoData mTempData;

public:
	SqlDataFieldInfo()
		: mMaxLength(DB_BLOB_FIELD_DEFAULT_LENGTH){}

    void Init(const char *tableName, const char *indexKeyName, AutoDBTool dbTool)
    {       
        mTableName = tableName;
        mKeyFieldName = indexKeyName;
        mDBTool = dbTool;
    }

public:
	virtual const char* getTypeString()const{ return TYPE_SQL_DATA; }
	virtual int getType() const { return FIELD_SQLDATA; }

	virtual const type_info& getTypeinfo() const { return typeid(AutoData); }
	virtual DSIZE getLength()const{ return 0; }

	virtual bool needSaveToBuffer(void) const { return false; }

	virtual void setMaxLength(int maxLen) override { mMaxLength = maxLen;  }
	virtual int getMaxLength() const override { return mMaxLength; }
	virtual int getDBDataLength() const override { return getMaxLength()+sizeof(DSIZE); }	// ��4���ֽڱ��泤��

	virtual AString getTypeParam() const{ return AString(STRING((int)mMaxLength)); }
	virtual void setTypeParam( const AString &typeParam )
	{ 
		int len = TOINT(typeParam.c_str());
		if (len>0)  
		{
			if (len<DB_BLOB_FIELD_DEFAULT_LENGTH)
			{
				WARN_LOG("=== SqlDataFieldInfo set param sql data max size %d, less default %d, now use default", len, (int)DB_BLOB_FIELD_DEFAULT_LENGTH);
				mMaxLength = DB_BLOB_FIELD_DEFAULT_LENGTH;
			}
			else
				mMaxLength = len; 
		}
	}

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
			mMaxLength = DB_BLOB_FIELD_DEFAULT_LENGTH;
		}
		return false;
	}	

public:
	virtual bool saveData(void *data, DataStream *destData) const
	{  
		AssertNote(0, "SqlDataFieldInfo ���ɱ�������");
		return  false;
	}

	virtual bool restoreData(void *data, DataStream *scrData)
	{
		AssertNote(0, "SqlDataFieldInfo ���ɻָ�����");
		return  false;
	}

	virtual bool set( void *data, const char* szVal )
	{ 
		WARN_LOG("SqlDataFieldInfo ��������ַ�");
		return false; 
	}
	virtual bool get( void *data, AString &resultString ) const
	{
		WARN_LOG("SqlDataFieldInfo ����ת��Ϊ�ַ�");
		return false;
	}	

		virtual bool set(void *data, int nVal) { return false; }

		virtual bool set(void *data, float fVal){ return false; }
		virtual bool set(void *data, bool bVal){ return false; }
		
		virtual bool set(void *data, const AString &strVal){ return false; }

		virtual bool get(void *data, int &nVal) const { return false; }
		virtual bool get(void *data, float &fVal) const { return false; }
		virtual bool get(void *data, bool &bVal) const{ return false; }

		virtual bool set(void *data, UInt64 uVal64){ return false; }
		virtual bool get(void *data, UInt64 &uVal64) const{  return false; }

		virtual bool get(void *data, Data &destStringData) const { return false; }
		

		virtual bool set(void *data, const void* obj, const type_info &info) 
		{
			if (typeid(AutoData)!=info)
			{
				WARN_LOG("SqlDataFieldInfo ֻ�ɱ��� AutoData");
				return false;
			}
			AutoData &d = *(AutoData*)obj;
			if (!d)
				d = MEM_NEW DataBuffer(1);
			AString whereStr;
			whereStr.Format("%s = %d", mKeyFieldName.c_str(), *(int*)data);
			if (mDBTool && mDBTool->SaveBlobData(mTableName.c_str(), getName().c_str(), whereStr.c_str(), d.getPtr(), false))
				return true;

			ERROR_LOG("SqlDataFieldInfo save table %s data %s fail where %s\r\nSQL: %s", mTableName.c_str(), getName().c_str(), whereStr.c_str(), mDBTool ? mDBTool->getErrorMsg():"DB tool is NULL");
			return false;
		}
		virtual bool get(void *data, void* obj, const type_info &info) const 
		{
			if (info!=typeid(AutoData))
				return false;

			AutoData d = MEM_NEW DataBuffer(getMaxLength()+1);
            ((SqlDataFieldInfo*)(this))->mTempData = d; //��ʱ����, ����Dataֻ����ָ�뱻�ͷź�����Ч
			AString whereStr;
			whereStr.Format("%s = %d", mKeyFieldName.c_str(), *(int*)data);
			tDBTool *p = (tDBTool*)mDBTool.getPtr();
			int re = -1;
			if (p!=NULL)
			{
				re = p->LoadBlobData(mTableName.c_str(), getName().c_str(), whereStr.c_str(), d.getPtr());
				if (re==SQL_SUCCEED)
				{				
					*(AutoData*)obj = d;
					return true;
				}
			}
			NOTE_LOG("Default blob size %d, now is %d, get %d", (int)DB_BLOB_FIELD_DEFAULT_LENGTH, (int)mMaxLength, getMaxLength());
			ERROR_LOG("SqlDataFieldInfo load table %s data %s fail where %s, Buffer size %d, ERROR [%d]: %s", mTableName.c_str(), getName().c_str(), whereStr.c_str(), (int)d->size(), re, p->getErrorMsg() );
			return false;			
		}

		virtual bool setData(void *data, const Data *dataVal)
		{
            int infoType = dataVal->getFieldType();
            if (infoType==FIELD_NULL)
                return true;
			if (infoType==FIELD_DATA || infoType==FIELD_SQLDATA)
			{
				AutoData	temp;
				if (dataVal->get(&temp, typeid(AutoData)))
					return set(data, &temp, typeid(AutoData));
			}
			return false; 
		}

		virtual void init(void *data)
		{
			
		}
		virtual void free(void *data)
		{
			
		}

		virtual int getIndexValue(void* data) const { return 0; }
		virtual int getIndexValue(int nIndex) const { return 0; }
		virtual int getIndexValue(const char* strIndex) const { return 0; }

};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class SqlDataFieldInfoFactory : public tFieldInfoFactory
{
public:
	SqlDataFieldInfoFactory(){ }

	virtual int getType() const { return FIELD_SQLDATA; }
	virtual const char* getTypeName() const { return TYPE_SQL_DATA; }
	virtual tFieldInfo*	createFieldInfo()
	{
		return MEM_NEW SqlDataFieldInfo();
	}
};
//-------------------------------------------------------------------------
#endif