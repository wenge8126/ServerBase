// Auto generate by table tool
#ifndef _DBUSER_T_USERDATA_H_
#define _DBUSER_T_USERDATA_H_

#include "DBUserComponent.h" 

class DBUser_t_userdata : public DBUserComponent
{
public:
	DBUser_t_userdata(){}

    virtual const char* GetTableName() const override { return "t_userdata"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PHONE(/*INT64*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data INVITE_CODE(/*STRING*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data INTRODUCER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data REGISTER_TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data USERDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(5); return mDataRecord->get(5); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MD5(/*STRING*/) { if (mDataRecord) return mDataRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wPHONE() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szINVITE_CODE() const { if (mDataRecord)  return  mDataRecord->getString(2); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strINVITE_CODE() const { if (mDataRecord)  return  mDataRecord->getAString(2); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	Int64 wINTRODUCER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	UInt64 uREGISTER_TIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(4, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceUSERDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(5, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(5, v); } else mDataRecord->NotifyChanged(5); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szMD5() const { if (mDataRecord)  return  mDataRecord->getString(6); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strMD5() const { if (mDataRecord)  return  mDataRecord->getAString(6); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }

    bool SetPHONE(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetINVITE_CODE(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetINTRODUCER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }
    bool SetREGISTER_TIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, (Int64)nVal); }

    bool SetMD5(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(6, nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(2114474147))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "PHONE"; fieldNameList[2] = "INVITE_CODE"; fieldNameList[3] = "INTRODUCER"; fieldNameList[4] = "REGISTER_TIME"; fieldNameList[5] = "USERDATA"; fieldNameList[6] = "MD5"; 
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
