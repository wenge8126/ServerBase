// Auto generate by table tool
#ifndef _DBUSER_T_TALK_H_
#define _DBUSER_T_TALK_H_

#include "DBUserComponent.h" 

class DBUser_t_talk : public DBUserComponent
{
public:
	DBUser_t_talk(){}

    virtual const char* GetTableName() const override { return "t_talk"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TEXT_(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(1); return mDataRecord->get(1); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SENDER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TARGET(/*INT64*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceTEXT(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(1, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(1, v); } else mDataRecord->NotifyChanged(1); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wSENDER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wTARGET() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }


    bool SetSENDER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetTARGET(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(1443884107))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "TEXT"; fieldNameList[2] = "SENDER"; fieldNameList[3] = "TARGET"; 
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
