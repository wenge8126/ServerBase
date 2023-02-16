// Auto generate by table tool
#ifndef _DBUSER_T_PERSON_H_
#define _DBUSER_T_PERSON_H_

#include "DBUserComponent.h" 

class DBUser_t_person : public DBUserComponent
{
public:
	DBUser_t_person(){}

    virtual const char* GetTableName() const override { return "t_person"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PHONE(/*INT64*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data VIP(/*INT*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data INVITE_CODE(/*STRING*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data INTRODUCER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data REGISTER_TIME(/*UINT64*/) { if (mDataRecord) return mDataRecord->get(5); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data USERDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(6); return mDataRecord->get(6); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SHOWDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(7); return mDataRecord->get(7); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data GET_GIFT(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(8); return mDataRecord->get(8); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data STATE(/*INT*/) { if (mDataRecord) return mDataRecord->get(9); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data LIKE(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(10); return mDataRecord->get(10); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data EXTDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(11); return mDataRecord->get(11); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TALK(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(12); return mDataRecord->get(12); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data VIDEO(/*INT64*/) { if (mDataRecord) return mDataRecord->get(13); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data BTJ_REWARD(/*INT*/) { if (mDataRecord) return mDataRecord->get(14); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TJ_REWARD(/*INT*/) { if (mDataRecord) return mDataRecord->get(15); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TOTAL_REWARD(/*INT*/) { if (mDataRecord) return mDataRecord->get(16); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data RMB(/*INT*/) { if (mDataRecord) return mDataRecord->get(17); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TOTAL_RMB(/*INT*/) { if (mDataRecord) return mDataRecord->get(18); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data NEWSDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(19); return mDataRecord->get(19); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SUCCEED(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(20); return mDataRecord->get(20); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data USERTYPE(/*INT*/) { if (mDataRecord) return mDataRecord->get(21); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data HONGNIANG(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(22); return mDataRecord->get(22); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data KEFU_DATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(23); return mDataRecord->get(23); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wPHONE() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(1, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nVIP() const { int v = 0; if (mDataRecord) {  mDataRecord->get(2, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szINVITE_CODE() const { if (mDataRecord)  return  mDataRecord->getString(3); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strINVITE_CODE() const { if (mDataRecord)  return  mDataRecord->getAString(3); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	Int64 wINTRODUCER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(4, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	UInt64 uREGISTER_TIME() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(5, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceUSERDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(6, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(6, v); } else mDataRecord->NotifyChanged(6); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceSHOWDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(7, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(7, v); } else mDataRecord->NotifyChanged(7); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceGET_GIFT(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(8, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(8, v); } else mDataRecord->NotifyChanged(8); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nSTATE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(9, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceLIKE(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(10, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(10, v); } else mDataRecord->NotifyChanged(10); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceEXTDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(11, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(11, v); } else mDataRecord->NotifyChanged(11); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceTALK(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(12, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(12, v); } else mDataRecord->NotifyChanged(12); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wVIDEO() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(13, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nBTJ_REWARD() const { int v = 0; if (mDataRecord) {  mDataRecord->get(14, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nTJ_REWARD() const { int v = 0; if (mDataRecord) {  mDataRecord->get(15, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nTOTAL_REWARD() const { int v = 0; if (mDataRecord) {  mDataRecord->get(16, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nRMB() const { int v = 0; if (mDataRecord) {  mDataRecord->get(17, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nTOTAL_RMB() const { int v = 0; if (mDataRecord) {  mDataRecord->get(18, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceNEWSDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(19, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(19, v); } else mDataRecord->NotifyChanged(19); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceSUCCEED(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(20, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(20, v); } else mDataRecord->NotifyChanged(20); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nUSERTYPE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(21, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceHONGNIANG(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(22, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(22, v); } else mDataRecord->NotifyChanged(22); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceKEFU_DATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(23, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(23, v); } else mDataRecord->NotifyChanged(23); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetPHONE(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetVIP(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetINVITE_CODE(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }
    bool SetINTRODUCER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, nVal); }
    bool SetREGISTER_TIME(UInt64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(5, (Int64)nVal); }



    bool SetSTATE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(9, nVal); }



    bool SetVIDEO(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(13, nVal); }
    bool SetBTJ_REWARD(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(14, nVal); }
    bool SetTJ_REWARD(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(15, nVal); }
    bool SetTOTAL_REWARD(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(16, nVal); }
    bool SetRMB(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(17, nVal); }
    bool SetTOTAL_RMB(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(18, nVal); }


    bool SetUSERTYPE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(21, nVal); }




	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-2124042787))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "PHONE"; fieldNameList[2] = "VIP"; fieldNameList[3] = "INVITE_CODE"; fieldNameList[4] = "INTRODUCER"; fieldNameList[5] = "REGISTER_TIME"; fieldNameList[6] = "USERDATA"; fieldNameList[7] = "SHOWDATA"; fieldNameList[8] = "GET_GIFT"; fieldNameList[9] = "STATE"; fieldNameList[10] = "LIKE"; fieldNameList[11] = "EXTDATA"; fieldNameList[12] = "TALK"; fieldNameList[13] = "VIDEO"; fieldNameList[14] = "BTJ_REWARD"; fieldNameList[15] = "TJ_REWARD"; fieldNameList[16] = "TOTAL_REWARD"; fieldNameList[17] = "RMB"; fieldNameList[18] = "TOTAL_RMB"; fieldNameList[19] = "NEWSDATA"; fieldNameList[20] = "SUCCEED"; fieldNameList[21] = "USERTYPE"; fieldNameList[22] = "HONGNIANG"; fieldNameList[23] = "KEFU_DATA"; 
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
