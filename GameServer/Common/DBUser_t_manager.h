// Auto generate by table tool
#ifndef _DBUSER_T_MANAGER_H_
#define _DBUSER_T_MANAGER_H_

#include "DBUserComponent.h" 

class DBUser_t_manager : public DBUserComponent
{
public:
	DBUser_t_manager(){}

    virtual const char* GetTableName() const override { return "t_manager"; }

	Data ACCOUNT(/*STRING*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PHONE(/*INT64*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data REGISTER_TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data USERDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(4); return mDataRecord->get(4); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MD5(/*STRING*/) { if (mDataRecord) return mDataRecord->get(5); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data ACTIVE_TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data ACTIVE_IP(/*STRING*/) { if (mDataRecord) return mDataRecord->get(7); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data POWER_LEVEL(/*INT*/) { if (mDataRecord) return mDataRecord->get(8); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data LOCK(/*BOOL*/) { if (mDataRecord) return mDataRecord->get(9); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	const char* szACCOUNT() const { if (mDataRecord)  return  mDataRecord->getString(0); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strACCOUNT() const { if (mDataRecord)  return  mDataRecord->getAString(0); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wPHONE() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	UInt64 uREGISTER_TIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceUSERDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(4, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(4, v); } else mDataRecord->NotifyChanged(4); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szMD5() const { if (mDataRecord)  return  mDataRecord->getString(5); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strMD5() const { if (mDataRecord)  return  mDataRecord->getAString(5); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	UInt64 uACTIVE_TIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(6, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szACTIVE_IP() const { if (mDataRecord)  return  mDataRecord->getString(7); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strACTIVE_IP() const { if (mDataRecord)  return  mDataRecord->getAString(7); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nPOWER_LEVEL() const { int v = 0; if (mDataRecord) {  mDataRecord->get(8, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	bool bLOCK() const { int v = 0; if (mDataRecord) {  mDataRecord->get(9, v); return v!=0; } ERROR_LOG("No exist record %s", mKey.c_str()); return false; }

    bool SetDBID(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetPHONE(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetREGISTER_TIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, (Int64)nVal); }

    bool SetMD5(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(5, nVal); }
    bool SetACTIVE_TIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(6, (Int64)nVal); }
    bool SetACTIVE_IP(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(7, nVal); }
    bool SetPOWER_LEVEL(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(8, nVal); }
    bool SetLOCK(bool nVal) { if (!mDataRecord) return false; return mDataRecord->set(9, nVal?1:0); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-1667401318))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ACCOUNT"; fieldNameList[1] = "DBID"; fieldNameList[2] = "PHONE"; fieldNameList[3] = "REGISTER_TIME"; fieldNameList[4] = "USERDATA"; fieldNameList[5] = "MD5"; fieldNameList[6] = "ACTIVE_TIME"; fieldNameList[7] = "ACTIVE_IP"; fieldNameList[8] = "POWER_LEVEL"; fieldNameList[9] = "LOCK"; 
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
