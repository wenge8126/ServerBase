// Auto generate by table tool
#ifndef _RESOURCEINDEX_H_
#define _RESOURCEINDEX_H_

class ResourceIndex
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	ResourceIndex(){}

	ResourceIndex(int key)
	{
       AutoTable t = ResourceIndex::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table ResourceIndex");
	}

	ResourceIndex(Int64 key)
	{
       AutoTable t = ResourceIndex::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table ResourceIndex");
	}

	ResourceIndex(UInt64 key)
	{
       AutoTable t = ResourceIndex::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table ResourceIndex");
	}

	ResourceIndex(const char *key)
	{
       AutoTable t = ResourceIndex::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table ResourceIndex");
	}

	ResourceIndex& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR_LOG("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=-1418165197)
            {
                ERROR_LOG("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	ResourceIndex(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	ResourceIndex(const AString &key)
	{
       AutoTable t = ResourceIndex::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table ResourceIndex");
	}

	ResourceIndex(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data INDEX(/*STRING*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data MD5(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data TYPE(/*INT*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }


	const char* szINDEX() const { if (mRecord)  return  mRecord->getString(0); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strINDEX() const { if (mRecord)  return  mRecord->getAString(0); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szMD5() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strMD5() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	int nTYPE() const { int v = 0; if (mRecord) {  mRecord->get(2, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(-1418165197))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "INDEX"; fieldNameList[1] = "MD5"; fieldNameList[2] = "TYPE"; 
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
