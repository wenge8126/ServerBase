// Auto generate by table tool
#ifndef _ITEMDATACOMPONENT_H_
#define _ITEMDATACOMPONENT_H_

#include "DBUserComponent.h" 

class ItemDataComponent : public DBUserComponent
{
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

    bool SetID(int nVal) { return mDataRecord->set(1, nVal); }
    bool SetNAME(const char *nVal) { return mDataRecord->set(2, nVal); }
    bool SetTYPE(int nVal) { return mDataRecord->set(3, nVal); }
    bool SetCOUNT(int nVal) { return mDataRecord->set(4, nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-947006127))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "KEY"; fieldNameList[1] = "ID"; fieldNameList[2] = "NAME"; fieldNameList[3] = "TYPE"; fieldNameList[4] = "COUNT"; 
		if (fieldNameList.size()!=f->getCount()) { ERROR_LOG("%s field count is error.",  t->GetTableName()); return false; }
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
			if (info==NULL || info->getName()!=fieldNameList[i]) { ERROR_LOG("%s %d field is not same %s",  t->GetTableName(), i, fieldNameList[i].c_str()); return false; }
		}
		return true;
	}
};

#endif
