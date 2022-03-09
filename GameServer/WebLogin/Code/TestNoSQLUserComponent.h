// Auto generate by table tool
#ifndef _TESTNOSQLUSERCOMPONENT_H_
#define _TESTNOSQLUSERCOMPONENT_H_

#include "NoSQLComponent.h" 
#include "NoSQLUserComponent.h"

class TestNoSQLUserComponent : public RecordNoSQLUserComponent
{
public:
	AString mKey;

public:
	TestNoSQLUserComponent(){}

	Data ID(/*INT*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data NAME(/*STRING*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TYPE(/*STRING*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	int nID() const { int v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szNAME() const { if (mDataRecord)  return  mDataRecord->getString(1); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strNAME() const { if (mDataRecord)  return  mDataRecord->getAString(1); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szTYPE() const { if (mDataRecord)  return  mDataRecord->getString(2); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strTYPE() const { if (mDataRecord)  return  mDataRecord->getAString(2); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }

	virtual bool InitRecord(ARecord record) override
	{
		AutoField f = record->getField();
		if (!f || !f->CheckSame(-726408713))
		{
			ERROR_LOG("No same check code of %s", record->GetTable()->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ID"; fieldNameList[1] = "NAME"; fieldNameList[2] = "TYPE"; 
		if (fieldNameList.size()!=f->getCount()) { ERROR_LOG("%s field count is error.",  record->GetTable()->GetTableName()); return false; }
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
			if (info==NULL || info->getName()!=fieldNameList[i]) { ERROR_LOG("%s %d field is not same %s",  record->GetTable()->GetTableName(), i, fieldNameList[i].c_str()); return false; }
		}
		return RecordNoSQLUserComponent::InitRecord(record);
	}

    static AutoTable CreateTable()
    {
        AutoTable table = MEM_NEW StructBaseTable();
        table->AppendField("ID", "INT");
        table->AppendField("NAME", "STRING");
        table->AppendField("TYPE", "STRING");
        return table;
    }

};

#endif
