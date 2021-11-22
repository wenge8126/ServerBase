// Auto generate by table tool
#ifndef _T_ACCOUNT_H_
#define _T_ACCOUNT_H_

class t_account
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	t_account(){}

	t_account(int key)
	{
       AutoTable t = t_account::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_account");
	}

	t_account(Int64 key)
	{
       AutoTable t = t_account::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_account");
	}

	t_account(UInt64 key)
	{
       AutoTable t = t_account::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_account");
	}

	t_account(const char *key)
	{
       AutoTable t = t_account::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_account");
	}

	t_account& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=809214277)
            {
                ERROR("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	t_account(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	t_account(const AString &key)
	{
       AutoTable t = t_account::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_account");
	}

	t_account(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data ACCOUNT(/*CHARSTRING*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PASSWORD(/*CHARSTRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PASS_MD5(/*CHARSTRING*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data LOGIN_TIME(/*INT64*/) { if (mRecord) return mRecord->get(3); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data CREATE_TIME(/*INT64*/) { if (mRecord) return mRecord->get(4); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PHONE(/*INT64*/) { if (mRecord) return mRecord->get(5); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data UNITID(/*INT*/) { if (mRecord) return mRecord->get(6); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data DBID_LIST(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(7); return mRecord->get(7); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data EXT(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(8); return mRecord->get(8); } ERROR_LOG("No exist record %d", mKey); return Data(); }


	const char* szACCOUNT() const { if (mRecord)  return  mRecord->getString(0); ERROR_LOG("No exist record %d", mKey); return ""; }
	const char* szPASSWORD() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const char* szPASS_MD5() const { if (mRecord)  return  mRecord->getString(2); ERROR_LOG("No exist record %d", mKey); return ""; }
	Int64 wLOGIN_TIME() const { Int64 v = 0; if (mRecord) {  mRecord->get(3, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	Int64 wCREATE_TIME() const { Int64 v = 0; if (mRecord) {  mRecord->get(4, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	Int64 wPHONE() const { Int64 v = 0; if (mRecord) {  mRecord->get(5, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	int nUNITID() const { int v = 0; if (mRecord) {  mRecord->get(6, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceDBID_LIST(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(7, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(7, v); } else mRecord->NotifyChanged(7); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceEXT(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(8, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(8, v); } else mRecord->NotifyChanged(8); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(809214277))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ACCOUNT"; fieldNameList[1] = "PASSWORD"; fieldNameList[2] = "PASS_MD5"; fieldNameList[3] = "LOGIN_TIME"; fieldNameList[4] = "CREATE_TIME"; fieldNameList[5] = "PHONE"; fieldNameList[6] = "UNITID"; fieldNameList[7] = "DBID_LIST"; fieldNameList[8] = "EXT"; 
		if (fieldNameList.size()!=f->getCount()) { ERROR_LOG("%s field count is error.", t->GetTableName()); return false; }
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
			if (info==NULL || info->getName()!=fieldNameList[i]) { ERROR_LOG("%s %d field is not same %s", t->GetTableName(), i, fieldNameList[i].c_str()); return false; }
		}
		return true;
	}
public:
	ARecord	mRecord;
	Int64			mKey;
};

#endif
