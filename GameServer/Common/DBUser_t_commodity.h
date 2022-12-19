// Auto generate by table tool
#ifndef _DBUSER_T_COMMODITY_H_
#define _DBUSER_T_COMMODITY_H_

#include "DBUserComponent.h" 

class DBUser_t_commodity : public DBUserComponent
{
public:
	DBUser_t_commodity(){}

    virtual const char* GetTableName() const override { return "t_commodity"; }

	Data PAGE_NAME(/*STRING*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PAGE_DATA(/*STRING*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MANAGER(/*STRING*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	const char* szPAGE_NAME() const { if (mDataRecord)  return  mDataRecord->getString(0); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strPAGE_NAME() const { if (mDataRecord)  return  mDataRecord->getAString(0); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szPAGE_DATA() const { if (mDataRecord)  return  mDataRecord->getString(1); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strPAGE_DATA() const { if (mDataRecord)  return  mDataRecord->getAString(1); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	UInt64 uTIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szMANAGER() const { if (mDataRecord)  return  mDataRecord->getString(3); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strMANAGER() const { if (mDataRecord)  return  mDataRecord->getAString(3); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }

    bool SetPAGE_DATA(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetTIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, (Int64)nVal); }
    bool SetMANAGER(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-108879340))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "PAGE_NAME"; fieldNameList[1] = "PAGE_DATA"; fieldNameList[2] = "TIME"; fieldNameList[3] = "MANAGER"; 
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
