// Auto generate by table tool
#ifndef _DBUSER_T_GOODS_H_
#define _DBUSER_T_GOODS_H_

#include "DBUserComponent.h" 

class DBUser_t_goods : public DBUserComponent
{
public:
	DBUser_t_goods(){}

    virtual const char* GetTableName() const override { return "t_goods"; }

	Data DBID(/*INT64*/) { if (mDataRecord) return mDataRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data NAME(/*STRING*/) { if (mDataRecord) return mDataRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data DETAILS(/*STRING*/) { if (mDataRecord) return mDataRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data TITLE(/*STRING*/) { if (mDataRecord) return mDataRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data URL(/*STRING*/) { if (mDataRecord) return mDataRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data PRICE(/*INT*/) { if (mDataRecord) return mDataRecord->get(5); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data COST(/*INT*/) { if (mDataRecord) return mDataRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SPECS(/*STRING*/) { if (mDataRecord) return mDataRecord->get(7); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SPECS_1(/*STRING*/) { if (mDataRecord) return mDataRecord->get(8); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SPECS_2(/*STRING*/) { if (mDataRecord) return mDataRecord->get(9); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data SPECS_3(/*STRING*/) { if (mDataRecord) return mDataRecord->get(10); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data WIDTH(/*INT*/) { if (mDataRecord) return mDataRecord->get(11); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data HEIGHT(/*INT*/) { if (mDataRecord) return mDataRecord->get(12); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data LENGTH(/*INT*/) { if (mDataRecord) return mDataRecord->get(13); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data OWNER(/*INT64*/) { if (mDataRecord) return mDataRecord->get(14); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	Data NICEDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(15); return mDataRecord->get(15); } ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(0, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szNAME() const { if (mDataRecord)  return  mDataRecord->getString(1); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strNAME() const { if (mDataRecord)  return  mDataRecord->getAString(1); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szDETAILS() const { if (mDataRecord)  return  mDataRecord->getString(2); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strDETAILS() const { if (mDataRecord)  return  mDataRecord->getAString(2); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szTITLE() const { if (mDataRecord)  return  mDataRecord->getString(3); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strTITLE() const { if (mDataRecord)  return  mDataRecord->getAString(3); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szURL() const { if (mDataRecord)  return  mDataRecord->getString(4); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strURL() const { if (mDataRecord)  return  mDataRecord->getAString(4); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nPRICE() const { int v = 0; if (mDataRecord) {  mDataRecord->get(5, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nCOST() const { int v = 0; if (mDataRecord) {  mDataRecord->get(6, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	const char* szSPECS() const { if (mDataRecord)  return  mDataRecord->getString(7); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strSPECS() const { if (mDataRecord)  return  mDataRecord->getAString(7); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szSPECS_1() const { if (mDataRecord)  return  mDataRecord->getString(8); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strSPECS_1() const { if (mDataRecord)  return  mDataRecord->getAString(8); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szSPECS_2() const { if (mDataRecord)  return  mDataRecord->getString(9); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strSPECS_2() const { if (mDataRecord)  return  mDataRecord->getAString(9); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	const char* szSPECS_3() const { if (mDataRecord)  return  mDataRecord->getString(10); ERROR_LOG("No exist record %s", mKey.c_str()); return ""; }
	const AString& strSPECS_3() const { if (mDataRecord)  return  mDataRecord->getAString(10); ERROR_LOG("No exist record %s", mKey.c_str()); static AString s; return s; }
	int nWIDTH() const { int v = 0; if (mDataRecord) {  mDataRecord->get(11, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nHEIGHT() const { int v = 0; if (mDataRecord) {  mDataRecord->get(12, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	int nLENGTH() const { int v = 0; if (mDataRecord) {  mDataRecord->get(13, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	Int64 wOWNER() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(14, v); return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }
	AutoNice niceNICEDATA(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(15, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(15, v); } else mDataRecord->NotifyChanged(15); } return v; } ERROR_LOG("No exist record %s", mKey.c_str()); return v; }

    bool SetNAME(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(1, nVal); }
    bool SetDETAILS(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(2, nVal); }
    bool SetTITLE(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(3, nVal); }
    bool SetURL(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(4, nVal); }
    bool SetPRICE(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(5, nVal); }
    bool SetCOST(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(6, nVal); }
    bool SetSPECS(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(7, nVal); }
    bool SetSPECS_1(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(8, nVal); }
    bool SetSPECS_2(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(9, nVal); }
    bool SetSPECS_3(const char *nVal) { if (!mDataRecord) return false; return mDataRecord->set(10, nVal); }
    bool SetWIDTH(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(11, nVal); }
    bool SetHEIGHT(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(12, nVal); }
    bool SetLENGTH(int nVal) { if (!mDataRecord) return false; return mDataRecord->set(13, nVal); }
    bool SetOWNER(Int64 nVal) { if (!mDataRecord) return false; return mDataRecord->set(14, nVal); }



	virtual bool CheckTable(AutoTable t) override
	{
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-826674277))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "NAME"; fieldNameList[2] = "DETAILS"; fieldNameList[3] = "TITLE"; fieldNameList[4] = "URL"; fieldNameList[5] = "PRICE"; fieldNameList[6] = "COST"; fieldNameList[7] = "SPECS"; fieldNameList[8] = "SPECS_1"; fieldNameList[9] = "SPECS_2"; fieldNameList[10] = "SPECS_3"; fieldNameList[11] = "WIDTH"; fieldNameList[12] = "HEIGHT"; fieldNameList[13] = "LENGTH"; fieldNameList[14] = "OWNER"; fieldNameList[15] = "NICEDATA"; 
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
