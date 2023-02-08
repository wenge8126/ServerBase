// Auto generate by table tool
#ifndef _DBUSER_T_PAY_RECORD_H_
#define _DBUSER_T_PAY_RECORD_H_

#include "DBUserComponent.h" 

class DBUser_t_pay_record : public DBUserComponent
{
public:
	DBUser_t_pay_record(){}

    virtual const char* GetTableName() const override { return "t_pay_record"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data BUY_ID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data AMOUNT(/*INT64*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data STATE(/*INT*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PAY_ID(/*STRING*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PAY_MODE(/*INT*/) { if (mDataRecord) return mDataRecord->get(5); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data EX_DATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(6); return mDataRecord->get(6); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wBUY_ID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wAMOUNT() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nSTATE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szPAY_ID() const { if (mDataRecord)  return  mDataRecord->getString(4); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strPAY_ID() const { if (mDataRecord)  return  mDataRecord->getAString(4); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nPAY_MODE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(5, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceEX_DATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(6, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(6, v); } else mDataRecord->NotifyChanged(6); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetBUY_ID(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetAMOUNT(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetSTATE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }
    bool SetPAY_ID(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, nVal); }
    bool SetPAY_MODE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(5, nVal); }



	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-477946976))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "BUY_ID"; fieldNameList[2] = "AMOUNT"; fieldNameList[3] = "STATE"; fieldNameList[4] = "PAY_ID"; fieldNameList[5] = "PAY_MODE"; fieldNameList[6] = "EX_DATA"; 
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
