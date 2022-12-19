// Auto generate by table tool
#ifndef _DBUSER_T_COMMODITY_DATA2_H_
#define _DBUSER_T_COMMODITY_DATA2_H_

#include "DBUserComponent.h" 

class DBUser_t_commodity_data2 : public DBUserComponent
{
public:
	DBUser_t_commodity_data2(){}

    virtual const char* GetTableName() const override { return "t_commodity_data2"; }

	Data ITEM_INDEX(/*INT*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PIC_KEY(/*STRING*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TITLE(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(2); return mDataRecord->get(2); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data INFO(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(3); return mDataRecord->get(3); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data DATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(5); return mDataRecord->get(5); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data MANAGER(/*STRING*/) { if (mDataRecord) return mDataRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data STATE(/*BYTE*/) { if (mDataRecord) return mDataRecord->get(7); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	int nITEM_INDEX() const { int v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szPIC_KEY() const { if (mDataRecord)  return  mDataRecord->getString(1); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strPIC_KEY() const { if (mDataRecord)  return  mDataRecord->getAString(1); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	AutoData dataTITLE(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(2, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(2, v); } else mDataRecord->NotifyChanged(2); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoData dataINFO(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(3, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(3, v); } else mDataRecord->NotifyChanged(3); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	UInt64 uTIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(4, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(5, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(5, v); } else mDataRecord->NotifyChanged(5); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szMANAGER() const { if (mDataRecord)  return  mDataRecord->getString(6); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strMANAGER() const { if (mDataRecord)  return  mDataRecord->getAString(6); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nSTATE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(7, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetPIC_KEY(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }


    bool SetTIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, (Int64)nVal); }

    bool SetMANAGER(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(6, nVal); }
    bool SetSTATE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(7, nVal); }


	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(1964466161))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ITEM_INDEX"; fieldNameList[1] = "PIC_KEY"; fieldNameList[2] = "TITLE"; fieldNameList[3] = "INFO"; fieldNameList[4] = "TIME"; fieldNameList[5] = "DATA"; fieldNameList[6] = "MANAGER"; fieldNameList[7] = "STATE"; 
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
