// Auto generate by table tool
#ifndef _DBUSER_T_TEMPLATE_H_
#define _DBUSER_T_TEMPLATE_H_

#include "DBUserComponent.h" 

class DBUser_t_template : public DBUserComponent
{
public:
	DBUser_t_template(){}

    virtual const char* GetTableName() const override { return "t_template"; }

	Data DBID(/*INT*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TEMPLATE(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(1); return mDataRecord->get(1); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data INFO(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(2); return mDataRecord->get(2); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MANAGER(/*STRING*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	int nDBID() const { int v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceTEMPLATE(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(1, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(1, v); } else mDataRecord->NotifyChanged(1); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceINFO(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(2, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(2, v); } else mDataRecord->NotifyChanged(2); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szMANAGER() const { if (mDataRecord)  return  mDataRecord->getString(3); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strMANAGER() const { if (mDataRecord)  return  mDataRecord->getAString(3); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }



    bool SetMANAGER(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-1541656421))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "TEMPLATE"; fieldNameList[2] = "INFO"; fieldNameList[3] = "MANAGER"; 
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