/********************************************************************
	created:	2011/07/10
	created:	10:7:2011   0:39
	filename: 	d:\NewTable__0709\NewTable\Common\DataBase\FieldIndex.h
	file path:	d:\NewTable__0709\NewTable\Common\DataBase
	file base:	FieldIndex
	file ext:	h
	author:		Yang Wenge
	
	purpose:	�ر�ע��: �˶��󲻿������޸�, ԭ���ǵ��޸ĺ�,ʹ�õ��˶���ļ�¼�ͷ�ʱ, �п��ܻ���������ڴ�й©
*********************************************************************/

#ifndef _INCLUDE_FIELDINDEX_H_
#define _INCLUDE_FIELDINDEX_H_

#include "FieldInfo.h"
#include "Array.h"
#include "EasyMap.h"
#include "EasyHash.h"

#include "AutoPtr.h"

#define FIELD_USE_HASH_INDEX		1				// ʹ�ÿ���HASH


#define FIELD_MAX_COUNT				256
//------------------------------------------------------------------------------------
enum INIT_FIELD_MODE
{
	eInitCreateField,
	eInitPoolField,
	eInitNullField,
	eInitShareMemoryPoolField,
	eShareMemoryCreateField,
	eShareMemoryUseField,
};
//------------------------------------------------------------------------------------
class tBaseTable;
class DataStream;
class tBaseTable;
class BaseRecord;
//------------------------------------------------------------------------------------
//�ֶ������ṹ
class BaseCommon_Export FieldIndex : public AutoBase
{
	friend class BaseRecord;
public:
	typedef Array<FieldInfo, 8, unsigned short>				FieldVec;

#if FIELD_USE_HASH_INDEX
	typedef EasyHash<AString, short, 4>		FieldMap;
#else
	typedef EasyMap<AString, short>		FieldMap;
#endif

private:
	FieldVec	mFieldVec;
	FieldMap	mFieldMap;
	size_t		mDataLength;

public:
	// Ĭ��Ϊ1����ʾ����״̬��0��ʾ�����£���״̬������� NOTE: ��״̬���μӸ���
	Buffer		mUpdateState;			

public:
	void _appendField(const char* fieldName, int col);

	// �Ż��ֶ����б�,����߲�ѯ�ٶ� 
	void _optimizeNameHash(float rate)
	{ 
#if FIELD_USE_HASH_INDEX
		mFieldMap.OptimizeSpeed(rate);
#endif
	}

	virtual INIT_FIELD_MODE getFieldMode() const { return eInitCreateField; }
    virtual SHARE_PTR getShareMemoryPtr(const char *szRecordDataPtr) { return SHARE_PTR(); }

	virtual void* _allotRecordData(uint size) { return  ALLOCT_NEW(size); }
	virtual void _freeRecordData(void *pRecordData) {ALLOCT_FREE(pRecordData); }

	virtual BaseRecord* TryGetFreeRecord() { return NULL; }

public:
	friend class DataRecord;
	friend class tBaseTable;

	FieldIndex( tBaseTable *pTable, size_t num );
	~FieldIndex();

public:
	int getCount() const { return (int)mFieldVec.size(); }

	const FieldVec& getFieldInfoList() const { return mFieldVec; }
	const FieldMap&	getFieldMap() const { return mFieldMap; }

	//�õ�һ���ֶ��������е�λ��(��),����������򷵻�-1;
	FieldInfo getFieldInfo( const char *name ) const;
	FieldInfo getFieldInfo( int col ) const;

	int getFieldCol( const char *name ) const;
	bool existField( const char *name ) const;

	int getFieldType(int col);

	bool operator ==( const FieldIndex &other ) const;
	FieldInfo operator [] (int col) const
	{
		if (col>=0 && col<(int)mFieldVec.size())
			return mFieldVec[col];

		return NULL;
	}

	//��������������Ƿ�һһ��Ӧ
	bool check() const;

	// ����DB���
	virtual bool needCheck() const { return false; }
	virtual bool CheckSame(int checkCode) const { return true; }
	virtual int GetCheckCode() const { return 0; }

	//����
	FieldIndex& operator = ( const FieldIndex& other );

	// ����ֶ����Ƿ����Ҫ��
	bool checkFieldName(const AString &fieldNameList) const;

	size_t getDataLength(void) const
	{
		return mDataLength;
	}

	bool saveToData(DataStream *destData) const;
	bool restoreFromData(DataStream *scrData);

public:
	//�����ֶ�
	FieldInfo setField( const char *name, int type, int col );
	FieldInfo setField( const char *name, const char* strType, int col );

	//ɾ�����һ���ֶ���Ϣ,һ��˵,���ɾ���м��ֶ���Ϣ,��Ҫ��������,���߰Ѻ�����ֶ���Ϣȫ��ǰ�ƶ�
	void deleBackField( );

	virtual void _updateInfo();

	// for Redis save load
	AString ToString() const;

	bool FullFromString(const AString &scrString);

	virtual bool SetSkipUpdate(int nFieldCol, bool bSkip)
	{ 
        if (bSkip)
		    return mUpdateState.closeState(nFieldCol);		
        else
            return mUpdateState.openState(nFieldCol);
	}
	virtual bool IsSkipUpdate(int nFieldCol){ return !mUpdateState.isOpenState(nFieldCol); }

protected:
	virtual void FreeRecord(BaseRecord *pRecord);

public:
	static FieldInfo createFiledInfo(const char* strType);
	static FieldInfo createFiledInfo(int type);
	static FieldInfo createFieldInfo(int type);
	static FieldInfo createFieldInfo(const char* typeName);
};
//------------------------------------------------------------------------------------
typedef Auto<FieldIndex>	AutoField;

//------------------------------------------------------------------------------------


#endif //_INCLUDE_FIELDINDEX_H_