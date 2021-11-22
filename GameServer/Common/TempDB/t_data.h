// Auto generate by table tool
#ifndef _T_DATA_H_
#define _T_DATA_H_

class t_data
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	t_data(){}

	t_data(int key)
	{
       AutoTable t = t_data::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_data");
	}

	t_data(Int64 key)
	{
       AutoTable t = t_data::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_data");
	}

	t_data(UInt64 key)
	{
       AutoTable t = t_data::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_data");
	}

	t_data(const char *key)
	{
       AutoTable t = t_data::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %s in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_data");
	}

	t_data(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }

   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }
   void GrowthNew() { mRecord = table->GrowthNewRecord(); }

	t_data(const AString &key)
	{
       AutoTable t = t_data::table;
		mKey = TOINT(key.c_str());
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %s in table %s", key.c_str(), t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of t_data");
	}

	t_data(ARecord record)
	{
       mRecord = record;
		mKey = record[0];
	}
	Data ID(/*INT64*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data NAME(/*CHARSTRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data LEVEL(/*INT64*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }


	Int64 ID() const { Int64 v = 0; if (mRecord) {  mRecord->get(0, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	const char* szNAME() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	Int64 LEVEL() const { Int64 v = 0; if (mRecord) {  mRecord->get(2, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }

		AutoNice getNice() { if (mRecord) return mRecord->getNice(DBNICE); ERROR_LOG("Reocrd is NULL");  return AutoNice(); }


		AutoData getData() { if (mRecord) return mRecord->getData(DBDATA); ERROR_LOG("Reocrd is NULL");  return AutoData(); }


		void niceChanged() { if (mRecord) mRecord->NotifyChanged(DBNICE); else ERROR_LOG("Reocrd is NULL"); }


		void dataChanged() { if (mRecord) mRecord->NotifyChanged(DBDATA); else ERROR_LOG("Reocrd is NULL"); }


	static bool InitCheck(AutoTable t)
	{
       table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(1562968148))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ID"; fieldNameList[1] = "NAME"; fieldNameList[2] = "LEVEL"; 
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
