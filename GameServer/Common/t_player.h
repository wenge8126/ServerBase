// Auto generate by table tool
#ifndef _T_PLAYER_H_
#define _T_PLAYER_H_

class t_player
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	t_player(){}

	t_player(int key)
	{
       AutoTable t = t_player::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_player");
	}

	t_player(Int64 key)
	{
       AutoTable t = t_player::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_player");
	}

	t_player(UInt64 key)
	{
       AutoTable t = t_player::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_player");
	}

	t_player(const char *key)
	{
       AutoTable t = t_player::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_player");
	}

	t_player& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=975180949)
            {
                ERROR("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	t_player(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	t_player(const AString &key)
	{
       AutoTable t = t_player::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_player");
	}

	t_player(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data ID(/*INT64*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ACCOUNT(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data NAME(/*STRING*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data PROPERTY(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(3); return mRecord->get(3); } ERROR_LOG("No exist record %d", mKey); return Data(); }


	Int64 wID() const { Int64 v = 0; if (mRecord) {  mRecord->get(0, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	const char* szACCOUNT() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strACCOUNT() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szNAME() const { if (mRecord)  return  mRecord->getString(2); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strNAME() const { if (mRecord)  return  mRecord->getAString(2); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	AutoNice nicePROPERTY(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(3, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(3, v); } else mRecord->NotifyChanged(3); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(975180949))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ID"; fieldNameList[1] = "ACCOUNT"; fieldNameList[2] = "NAME"; fieldNameList[3] = "PROPERTY"; 
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
