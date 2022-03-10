// Auto generate by table tool
#ifndef _ITEMDATACOMPONENT_H_
#define _ITEMDATACOMPONENT_H_

#include "NoSQLUserComponent.h" 

class ItemDataComponent : public RecordNoSQLUserComponent
{
public:
    AString mKey;

public:
	ItemDataComponent(){}

	Data KEY(/*STRING*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data ID(/*INT*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data NAME(/*STRING*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TYPE(/*INT*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data COUNT(/*INT*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	const char* szKEY() const { if (mDataRecord)  return  mDataRecord->getString(0); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strKEY() const { if (mDataRecord)  return  mDataRecord->getAString(0); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nID() const { int v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szNAME() const { if (mDataRecord)  return  mDataRecord->getString(2); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strNAME() const { if (mDataRecord)  return  mDataRecord->getAString(2); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nTYPE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nCOUNT() const { int v = 0; if (mDataRecord) {  mDataRecord->get(4, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

	virtual bool InitRecord(ARecord record) override
	{
		AutoField f = record->getField();
		if (!f || !f->CheckSame(-947006127))
		{
			ERROR_LOG("No same check code of %s", record->GetTable()->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "KEY"; fieldNameList[1] = "ID"; fieldNameList[2] = "NAME"; fieldNameList[3] = "TYPE"; fieldNameList[4] = "COUNT"; 
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
        table->AppendField("KEY", "STRING");
        table->AppendField("ID", "INT");
        table->AppendField("NAME", "STRING");
        table->AppendField("TYPE", "INT");
        table->AppendField("COUNT", "INT");
        return table;
    }

};

#endif
