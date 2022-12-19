// Auto generate by table tool
#ifndef _WINDOWCONFIG_H_
#define _WINDOWCONFIG_H_

class WindowConfig
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	WindowConfig(){}

	WindowConfig(int key)
	{
       AutoTable t = WindowConfig::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table WindowConfig");
	}

	WindowConfig(Int64 key)
	{
       AutoTable t = WindowConfig::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table WindowConfig");
	}

	WindowConfig(UInt64 key)
	{
       AutoTable t = WindowConfig::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table WindowConfig");
	}

	WindowConfig(const char *key)
	{
       AutoTable t = WindowConfig::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table WindowConfig");
	}

	WindowConfig& operator = (ARecord re)
	{
       if (re)
        {
           if (re->GetTable()==NULL)
            {
                ERROR("Reocrd table is NULL"); return *this;
            }
           if (re->getField()->GetCheckCode()!=682082858)
            {
                ERROR("Reocrd field check fail"); return *this;
            }
        }
       mRecord = re;
		if (mRecord) mKey = mRecord[0]; else mKey = 0;
		return *this;
	}

	WindowConfig(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	WindowConfig(const AString &key)
	{
       AutoTable t = WindowConfig::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table WindowConfig");
	}

	WindowConfig(ARecord record)
	{
       *this = record;
	}

    void Save()
	{
       if (mRecord) mRecord->SaveUpdate();
	}

	Data WIN_NAME(/*STRING*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data RES(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data CLOSE_DESTORY(/*BOOL*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data INFO(/*STRING*/) { if (mRecord) return mRecord->get(3); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ZINDEX(/*INT*/) { if (mRecord) return mRecord->get(4); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data CLICK_CLOSE(/*BOOL*/) { if (mRecord) return mRecord->get(5); ERROR_LOG("No exist record %d", mKey); return Data(); }


	const char* szWIN_NAME() const { if (mRecord)  return  mRecord->getString(0); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strWIN_NAME() const { if (mRecord)  return  mRecord->getAString(0); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szRES() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strRES() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	bool bCLOSE_DESTORY() const { int v = 0; if (mRecord) {  mRecord->get(2, v); return v!=0; } ERROR_LOG("No exist record %d", mKey); return false; }
	const char* szINFO() const { if (mRecord)  return  mRecord->getString(3); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strINFO() const { if (mRecord)  return  mRecord->getAString(3); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	int nZINDEX() const { int v = 0; if (mRecord) {  mRecord->get(4, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	bool bCLICK_CLOSE() const { int v = 0; if (mRecord) {  mRecord->get(5, v); return v!=0; } ERROR_LOG("No exist record %d", mKey); return false; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(682082858))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "WIN_NAME"; fieldNameList[1] = "RES"; fieldNameList[2] = "CLOSE_DESTORY"; fieldNameList[3] = "INFO"; fieldNameList[4] = "ZINDEX"; fieldNameList[5] = "CLICK_CLOSE"; 
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
