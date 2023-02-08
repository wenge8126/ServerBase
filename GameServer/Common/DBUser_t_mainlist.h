// Auto generate by table tool
#ifndef _DBUSER_T_MAINLIST_H_
#define _DBUSER_T_MAINLIST_H_

#include "DBUserComponent.h" 

class DBUser_t_mainlist : public DBUserComponent
{
public:
	DBUser_t_mainlist(){}

    virtual const char* GetTableName() const override { return "t_mainlist"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MANAGER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wMANAGER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	UInt64 uTIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetMANAGER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetTIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, (Int64)nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-1695914276))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "MANAGER"; fieldNameList[2] = "TIME"; 
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
