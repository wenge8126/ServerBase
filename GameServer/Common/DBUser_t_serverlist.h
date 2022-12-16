// Auto generate by table tool
#ifndef _DBUSER_T_SERVERLIST_H_
#define _DBUSER_T_SERVERLIST_H_

#include "DBUserComponent.h" 

class DBUser_t_serverlist : public DBUserComponent
{
public:
	DBUser_t_serverlist(){}

    virtual const char* GetTableName() const override { return "t_serverlist"; }

	Data ID(/*INT*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data NICEDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(1); return mDataRecord->get(1); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	int nID() const { int v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceNICEDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(1, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(1, v); } else mDataRecord->NotifyChanged(1); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }




	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(483498659))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ID"; fieldNameList[1] = "NICEDATA"; 
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
