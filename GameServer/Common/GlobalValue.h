// Auto generate by table tool
#ifndef _GLOBALVALUE_H_
#define _GLOBALVALUE_H_

class GlobalValue
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	GlobalValue(){}

	GlobalValue(int key)
	{
       AutoTable t = GlobalValue::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table GlobalValue");
	}

	GlobalValue(Int64 key)
	{
       AutoTable t = GlobalValue::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table GlobalValue");
	}

	GlobalValue(UInt64 key)
	{
       AutoTable t = GlobalValue::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table GlobalValue");
	}

	GlobalValue(const char *key)
	{
       AutoTable t = GlobalValue::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table GlobalValue");
	}

	GlobalValue& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR_LOG("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=609082294)
            {
                ERROR_LOG("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	GlobalValue(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	GlobalValue(const AString &key)
	{
       AutoTable t = GlobalValue::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table GlobalValue");
	}

	GlobalValue(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data INDEX(/*STRING*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data VALUE(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data INFO(/*STRING*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }


	const char* szINDEX() const { if (mRecord)  return  mRecord->getString(0); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strINDEX() const { if (mRecord)  return  mRecord->getAString(0); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szVALUE() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strVALUE() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szINFO() const { if (mRecord)  return  mRecord->getString(2); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strINFO() const { if (mRecord)  return  mRecord->getAString(2); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(609082294))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "INDEX"; fieldNameList[1] = "VALUE"; fieldNameList[2] = "INFO"; 
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
