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
                ERROR_LOG("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=1270315684)
            {
			   ERROR_LOG("Reocrd field check fail"); return *this;
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

	Data DBID(/*INT64*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ACCOUNT(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data INFO(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(2); return mRecord->get(2); } ERROR_LOG("No exist record %d", mKey); return Data(); }


	Int64 wDBID() const { Int64 v = 0; if (mRecord) {  mRecord->get(0, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	const char* szACCOUNT() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strACCOUNT() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	AutoNice niceINFO(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(2, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(2, v); } else mRecord->NotifyChanged(2); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(1270315684))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "DBID"; fieldNameList[1] = "ACCOUNT"; fieldNameList[2] = "INFO"; 
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
