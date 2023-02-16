// Auto generate by table tool
#ifndef _USERTYPE_H_
#define _USERTYPE_H_

class UserType
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	UserType(){}

	UserType(int key)
	{
       AutoTable t = UserType::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table UserType");
	}

	UserType(Int64 key)
	{
       AutoTable t = UserType::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table UserType");
	}

	UserType(UInt64 key)
	{
       AutoTable t = UserType::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table UserType");
	}

	UserType(const char *key)
	{
       AutoTable t = UserType::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table UserType");
	}

	UserType& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR_LOG("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=638212089)
            {
                ERROR_LOG("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	UserType(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	UserType(const AString &key)
	{
       AutoTable t = UserType::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table UserType");
	}

	UserType(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data VALUE(/*INT*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ENUM(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data DESC(/*STRING*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }


	int nVALUE() const { int v = 0; if (mRecord) {  mRecord->get(0, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	const char* szENUM() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strENUM() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szDESC() const { if (mRecord)  return  mRecord->getString(2); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strDESC() const { if (mRecord)  return  mRecord->getAString(2); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(638212089))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "VALUE"; fieldNameList[1] = "ENUM"; fieldNameList[2] = "DESC"; 
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
