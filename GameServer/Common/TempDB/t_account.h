// Auto generate by table tool
#ifndef _T_ACCOUNT_H_
#define _T_ACCOUNT_H_

class t_account
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	t_account(){}

	t_account(int key)
	{
       AutoTable t = t_account::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_account");
	}

	t_account(Int64 key)
	{
       AutoTable t = t_account::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_account");
	}

	t_account(UInt64 key)
	{
       AutoTable t = t_account::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_account");
	}

	t_account(const char *key)
	{
       AutoTable t = t_account::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %s in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_account");
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
			if (!mRecord)	 ERROR_LOG("No exist record %s in table %s", key.c_str(), t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_account");
	}

	t_account(ARecord record)
	{
       mRecord = record;
		mKey = record[0];
	}
	Data ACCOUNT(/*CHARSTRING*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PASSWORD(/*CHARSTRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PASS_MD5(/*CHARSTRING*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ID(/*INT64*/) { if (mRecord) return mRecord->get(3); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data LOGIN_TIME(/*INT64*/) { if (mRecord) return mRecord->get(4); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data CREATE_TIME(/*INT64*/) { if (mRecord) return mRecord->get(5); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PHONE(/*INT64*/) { if (mRecord) return mRecord->get(6); ERROR_LOG("No exist record %d", mKey); return Data(); }


	const char* szACCOUNT() const { if (mRecord)  return  mRecord->getString(0); ERROR_LOG("No exist record %d", mKey); return ""; }
	const char* szPASSWORD() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const char* szPASS_MD5() const { if (mRecord)  return  mRecord->getString(2); ERROR_LOG("No exist record %d", mKey); return ""; }
	Int64 ID() const { Int64 v = 0; if (mRecord) {  mRecord->get(3, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	Int64 LOGIN_TIME() const { Int64 v = 0; if (mRecord) {  mRecord->get(4, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	Int64 CREATE_TIME() const { Int64 v = 0; if (mRecord) {  mRecord->get(5, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	Int64 PHONE() const { Int64 v = 0; if (mRecord) {  mRecord->get(6, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }

		AutoNice getNice() { if (mRecord) return mRecord->getNice(DBNICE); ERROR_LOG("Reocrd is NULL");  return AutoNice(); }


		AutoData getData() { if (mRecord) return mRecord->getData(DBDATA); ERROR_LOG("Reocrd is NULL");  return AutoData(); }


		void niceChanged() { if (mRecord) mRecord->NotifyChanged(DBNICE); else ERROR_LOG("Reocrd is NULL"); }


		void dataChanged() { if (mRecord) mRecord->NotifyChanged(DBDATA); else ERROR_LOG("Reocrd is NULL"); }


	static bool InitCheck(AutoTable t)
	{
       table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-917805))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ACCOUNT"; fieldNameList[1] = "PASSWORD"; fieldNameList[2] = "PASS_MD5"; fieldNameList[3] = "ID"; fieldNameList[4] = "LOGIN_TIME"; fieldNameList[5] = "CREATE_TIME"; fieldNameList[6] = "PHONE"; 
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
