// Auto generate by table tool
#ifndef _T_SERVERLIST_H_
#define _T_SERVERLIST_H_

class t_serverlist
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	t_serverlist(){}

	t_serverlist(int key)
	{
       AutoTable t = t_serverlist::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_serverlist");
	}

	t_serverlist(Int64 key)
	{
       AutoTable t = t_serverlist::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_serverlist");
	}

	t_serverlist(UInt64 key)
	{
       AutoTable t = t_serverlist::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_serverlist");
	}

	t_serverlist(const char *key)
	{
       AutoTable t = t_serverlist::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_serverlist");
	}

	t_serverlist& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=483498659)
            {
                ERROR("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	t_serverlist(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	t_serverlist(const AString &key)
	{
       AutoTable t = t_serverlist::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_serverlist");
	}

	t_serverlist(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data ID(/*INT*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data NICEDATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(1); return mRecord->get(1); } ERROR_LOG("No exist record %d", mKey); return Data(); }


	int nID() const { int v = 0; if (mRecord) {  mRecord->get(0, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceNICEDATA(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(1, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(1, v); } else mRecord->NotifyChanged(1); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(483498659))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ID"; fieldNameList[1] = "NICEDATA"; 
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
