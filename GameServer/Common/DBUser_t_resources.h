// Auto generate by table tool
#ifndef _DBUSER_T_RESOURCES_H_
#define _DBUSER_T_RESOURCES_H_

#include "DBUserComponent.h" 

class DBUser_t_resources : public DBUserComponent
{
public:
	DBUser_t_resources(){}

    virtual const char* GetTableName() const override { return "t_resources"; }

	Data NAME(/*STRING*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MD5(/*STRING*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data DATA(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(2); return mDataRecord->get(2); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	const char* szNAME() const { if (mDataRecord)  return  mDataRecord->getString(0); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strNAME() const { if (mDataRecord)  return  mDataRecord->getAString(0); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szMD5() const { if (mDataRecord)  return  mDataRecord->getString(1); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strMD5() const { if (mDataRecord)  return  mDataRecord->getAString(1); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	AutoData dataDATA(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(2, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(2, v); } else mDataRecord->NotifyChanged(2); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	UInt64 uTIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetMD5(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }

    bool SetTIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, (Int64)nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-1340828054))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "NAME"; fieldNameList[1] = "MD5"; fieldNameList[2] = "DATA"; fieldNameList[3] = "TIME"; 
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
