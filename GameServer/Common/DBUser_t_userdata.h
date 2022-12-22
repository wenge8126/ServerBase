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
	Data COLLECT(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(1); return mDataRecord->get(1); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data GOODS_CART(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(2); return mDataRecord->get(2); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PAY_RECORD(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(3); return mDataRecord->get(3); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data BUY_RECORD(/*INT*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data GOODS(bool bSetUpdate=false) /*DATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(5); return mDataRecord->get(5); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data COMMISSION(/*INT64*/) { if (mDataRecord) return mDataRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data CAN_CASH(/*INT64*/) { if (mDataRecord) return mDataRecord->get(7); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data CAN_PROCESS(/*FLOAT*/) { if (mDataRecord) return mDataRecord->get(8); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data CASH_OUT(/*INT64*/) { if (mDataRecord) return mDataRecord->get(9); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data REVENYE(/*INT64*/) { if (mDataRecord) return mDataRecord->get(10); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data REVENYE_CANCAST(/*INT64*/) { if (mDataRecord) return mDataRecord->get(11); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data REVENYE_CAST(/*INT64*/) { if (mDataRecord) return mDataRecord->get(12); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data EX_DATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(13); return mDataRecord->get(13); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoData dataCOLLECT(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(1, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(1, v); } else mDataRecord->NotifyChanged(1); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoData dataGOODS_CART(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(2, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(2, v); } else mDataRecord->NotifyChanged(2); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoData dataPAY_RECORD(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(3, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(3, v); } else mDataRecord->NotifyChanged(3); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nBUY_RECORD() const { int v = 0; if (mDataRecord) {  mDataRecord->get(4, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoData dataGOODS(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(5, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(5, v); } else mDataRecord->NotifyChanged(5); }   return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wCOMMISSION() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(6, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wCAN_CASH() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(7, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	float fCAN_PROCESS() const { float v = 0; if (mDataRecord) {  mDataRecord->get(8, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wCASH_OUT() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(9, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wREVENYE() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(10, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wREVENYE_CANCAST() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(11, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wREVENYE_CAST() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(12, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceEX_DATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(13, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(13, v); } else mDataRecord->NotifyChanged(13); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }




    bool SetBUY_RECORD(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, nVal); }

    bool SetCOMMISSION(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(6, nVal); }
    bool SetCAN_CASH(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(7, nVal); }
    bool SetCAN_PROCESS(float nVal) { if (!mDataRecord) return false; return mDataRecord->set(8, nVal); }
    bool SetCASH_OUT(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(9, nVal); }
    bool SetREVENYE(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(10, nVal); }
    bool SetREVENYE_CANCAST(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(11, nVal); }
    bool SetREVENYE_CAST(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(12, nVal); }



	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-841948855))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "COLLECT"; fieldNameList[2] = "GOODS_CART"; fieldNameList[3] = "PAY_RECORD"; fieldNameList[4] = "BUY_RECORD"; fieldNameList[5] = "GOODS"; fieldNameList[6] = "COMMISSION"; fieldNameList[7] = "CAN_CASH"; fieldNameList[8] = "CAN_PROCESS"; fieldNameList[9] = "CASH_OUT"; fieldNameList[10] = "REVENYE"; fieldNameList[11] = "REVENYE_CANCAST"; fieldNameList[12] = "REVENYE_CAST"; fieldNameList[13] = "EX_DATA"; 
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
