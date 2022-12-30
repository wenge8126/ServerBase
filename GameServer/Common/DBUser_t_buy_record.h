// Auto generate by table tool
#ifndef _DBUSER_T_BUY_RECORD_H_
#define _DBUSER_T_BUY_RECORD_H_

#include "DBUserComponent.h" 

class DBUser_t_buy_record : public DBUserComponent
{
public:
	DBUser_t_buy_record(){}

    virtual const char* GetTableName() const override { return "t_buy_record"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data BUYER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data GOODS_ID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data GOODS_OWNER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PRICE(/*INT*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data AMOUNT(/*INT64*/) { if (mDataRecord) return mDataRecord->get(5); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data COUNT(/*INT64*/) { if (mDataRecord) return mDataRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data STATE(/*INT*/) { if (mDataRecord) return mDataRecord->get(7); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data LOCK(/*BOOL*/) { if (mDataRecord) return mDataRecord->get(8); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data CLOSE(/*BOOL*/) { if (mDataRecord) return mDataRecord->get(9); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data EXDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(10); return mDataRecord->get(10); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wBUYER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wGOODS_ID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wGOODS_OWNER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(3, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nPRICE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(4, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wAMOUNT() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(5, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wCOUNT() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(6, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nSTATE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(7, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	bool bLOCK() const { int v = 0; if (mDataRecord) {  mDataRecord->get(8, v); return v!=0; } ERROR_LOG("No exist record %s", mKey.c_str()); return false; }
	bool bCLOSE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(9, v); return v!=0; } ERROR_LOG("No exist record %s", mKey.c_str()); return false; }
	AutoNice niceEXDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(10, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(10, v); } else mDataRecord->NotifyChanged(10); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetBUYER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetGOODS_ID(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetGOODS_OWNER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }
    bool SetPRICE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, nVal); }
    bool SetAMOUNT(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(5, nVal); }
    bool SetCOUNT(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(6, nVal); }
    bool SetSTATE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(7, nVal); }
    bool SetLOCK(bool nVal) { if (!mDataRecord) return false; return mDataRecord->set(8, nVal?1:0); }
    bool SetCLOSE(bool nVal) { if (!mDataRecord) return false; return mDataRecord->set(9, nVal?1:0); }



	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-319871195))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "BUYER"; fieldNameList[2] = "GOODS_ID"; fieldNameList[3] = "GOODS_OWNER"; fieldNameList[4] = "PRICE"; fieldNameList[5] = "AMOUNT"; fieldNameList[6] = "COUNT"; fieldNameList[7] = "STATE"; fieldNameList[8] = "LOCK"; fieldNameList[9] = "CLOSE"; fieldNameList[10] = "EXDATA"; 
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
