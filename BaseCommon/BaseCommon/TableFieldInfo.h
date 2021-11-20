#ifndef _INCLUDE_TABLEFIELDINFO_H_
#define _INCLUDE_TABLEFIELDINFO_H_

#include "FieldInfo.h"
#include "AutoObjectFieldInfo.h"
//#include "NiceTable.h"

#include "BaseTable.h"
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//class BaseCommon_Export TableFieldInfo : public AutoObjFieldInfo<AutoTable, FIELD_TABLE>
//{
//	typedef AutoObjFieldInfo<AutoTable, FIELD_TABLE> base;
//
//public:
//	virtual const char* getTypeString()const{ return TYPE_TABLE_NAME; }
//
//public:
//	virtual bool saveData(void *data, DataStream *destData) const;
//
//	virtual bool restoreData(void *data, DataStream *scrData);
//
//	virtual bool set( void *data, const char* szVal );
//	virtual bool set(void *data, const AString &strVal);
//
//	virtual bool get(void *data, AString &resultString) const;
//
//	virtual AString getTypeParam() const;
//	virtual void setTypeParam(const AString &typeParam);
//
//	virtual void CopyData(const tFieldInfo *pScrFieldInfo);
//
//public:
//	AutoField	mSubTableField;
//};
////----------------------------------------------------------------------------------------------------------------------
//
//class TableFieldInfoFactory : public tFieldInfoFactory
//{
//public:
//	TableFieldInfoFactory(){ }
//
//	virtual int getType() const { return FIELD_TABLE; }
//	virtual const char* getTypeName() const { return TYPE_TABLE_NAME; }
//	virtual tFieldInfo*	createFieldInfo()
//	{
//		return MEM_NEW TableFieldInfo();
//	}
//};
enum 
{
	_SubTableNull = 0, 
	_SubTableDefault = 1,
};
//----------------------------------------------------------------------------------------------------------------------
// �����ڴ� DB
// �ֶ���Ϣֻ����У����
// NOTE: �˱���ֶ�, ���б仯��������, ���ȫ����������, ��ֱ��ȫ����Ϊ�仯״̬
//		 ���һ����¼δ�ڱ����, ��Ĭ�ϴ�����¼����
//		 ɾ���ӱ��¼����ͨ����������ɾ��
//-------------------------------------------------------------------------
class BaseCommon_Export DBTableFieldInfo : public AutoObjFieldInfo<AutoTable, FIELD_TABLE>
{
	typedef AutoObjFieldInfo<AutoTable, FIELD_TABLE> base;

protected:
	int mMaxLength;

public:
	DBTableFieldInfo()
		: mMaxLength(DB_BLOB_FIELD_DEFAULT_LENGTH){}

public:
	virtual const char* getTypeString()const{ return TYPE_TABLE_NAME; }

	virtual void setMaxLength(int maxLen) override { mMaxLength = maxLen;  }
	virtual int getMaxLength() const override { return mMaxLength; }
	virtual int getDBDataLength() const override { return getMaxLength()+sizeof(DSIZE); }	// ��4���ֽڱ��泤��

	virtual AString getTypeParam() const;
	virtual void setTypeParam( const AString &typeParam );

	virtual bool saveField( DataStream *destData ) const;
	virtual bool restoreFromData( DataStream *scrData );

	virtual AutoTable CreateFieldTable();

public:
	virtual bool saveData(void *data, DataStream *destData) const;

	virtual bool restoreData(void *data, DataStream *scrData);

	virtual bool set( void *data, const char* szVal );
	virtual bool get( void *data, AString &resultString ) const;

	void CopyData( const tFieldInfo *pScrFieldInfo );

    virtual bool SaveDataToDB(tBaseTable *pScrTable, DataStream *pDBData) override
    {
        int checkCode = pScrTable->GetField()->GetCheckCode();
        pDBData->write(checkCode);
        if (!pScrTable->SaveData(pDBData))
        {
            ERROR_LOG("��¼�еı�񱣴浽����ʧ��");
            return false;
        }
        return true;
    }

    virtual bool RestoreFromDB(DataStream *pScrDBData, tBaseTable *destTable) override
    {
        pScrDBData->seek(0);

        int checkCode = 0;
        READ(pScrDBData, checkCode);

        if (checkCode!=destTable->GetField()->GetCheckCode())
        {
            ERROR_LOG("��¼�ӱ���ֶμ���ʧ��> �ֶ�[%s] record code %d != %d", getName().c_str(), 
                checkCode, destTable->GetField()->GetCheckCode() );
            return false;
        }
        if (!destTable->LoadFromData(pScrDBData))
        {
            ERROR_LOG("��¼�ӱ�ָ���ԭ> �ֶ�[%s] table [UNKNOW]", getName().c_str());
            destTable->ClearAll();
            return false;
        }
        return true;
    }

    virtual bool CheckField() 
    {
        if (mSubTableField )            
        {
            if (!mSubTableField->check())
            {
                ERROR_LOG("�ӱ���ֶμ��ʧ�� [%s]", getName().c_str());
                return false;
            }
            return true;
        }
        else
            ERROR_LOG("�ӱ���ֶ�%sΪ��", getName().c_str());
        return false;
    }

public:
	AutoField	mSubTableField;
};
//----------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------
class DB_TableFieldInfoFactory : public tFieldInfoFactory
{
public:
	DB_TableFieldInfoFactory(){ }

	virtual int getType() const { return FIELD_TABLE; }
	virtual const char* getTypeName() const { return TYPE_TABLE_NAME; }
	virtual tFieldInfo*	createFieldInfo()
	{
		return MEM_NEW DBTableFieldInfo();
	}
};
//-------------------------------------------------------------------------
#endif