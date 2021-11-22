// Auto generate by table tool
#ifndef _T_DATA_H_
#define _T_DATA_H_

class t_data
{
public:
	static AutoTable table;

	inline operator bool () const   {  return mRecord; }

	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }

	t_data(){}

	t_data(int key)
	{
       AutoTable t = t_data::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_data");
	}

	t_data(Int64 key)
	{
       AutoTable t = t_data::table;
		mKey = key;
		if ( t )
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 DEBUG_LOG("No exist record %d in table %s", key, t->GetTableName());
		}
		else
			ERROR_LOG("No exist table t_data");
	}

	t_data(UInt64 key)
	{
       AutoTable t = t_data::table;
		mKey = (Int64)(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_data");
	}

	t_data(const char *key)
	{
       AutoTable t = t_data::table;
		mKey = TOINT64(key);
		if ( t )
		{
			mRecord = t->GetRecord(key);
		}
		else
			ERROR_LOG("No exist table t_data");
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
		}
		else
			ERROR_LOG("No exist table t_data");
	}

	t_data(ARecord record)
	{
       mRecord = record;
		mKey = record[0];
	}
	Data ID(/*INT64*/) { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ACCOUNT(/*STRING*/) { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data NAME(/*STRING*/) { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data JUHE_USER(/*STRING*/) { if (mRecord) return mRecord->get(3); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data DATA(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(4); return mRecord->get(4); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data FETTER(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(5); return mRecord->get(5); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data FETTERTEACHER(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(6); return mRecord->get(6); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data DISCUSS(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(7); return mRecord->get(7); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ACADEMIC(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(8); return mRecord->get(8); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data COMPETITION(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(9); return mRecord->get(9); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data LEAGUE(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(10); return mRecord->get(10); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data BOSS(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(11); return mRecord->get(11); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ALLIANCEFUBEN(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(12); return mRecord->get(12); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data MONTHCARD(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(13); return mRecord->get(13); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ALLIANC_SKILL(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(14); return mRecord->get(14); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data GIFT(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(15); return mRecord->get(15); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SEVEN_LOGIN(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(16); return mRecord->get(16); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ICON(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(17); return mRecord->get(17); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ESC(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(18); return mRecord->get(18); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data TICKET(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(19); return mRecord->get(19); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data FUND(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(20); return mRecord->get(20); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data RANK(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(21); return mRecord->get(21); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ITEMSHOP(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(22); return mRecord->get(22); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data STATUS(/*INT*/) { if (mRecord) return mRecord->get(23); ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data ACTIVITY(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(24); return mRecord->get(24); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data BASEPROPERTY(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(25); return mRecord->get(25); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data STUDENT(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(26); return mRecord->get(26); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data TEACHER(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(27); return mRecord->get(27); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(28); return mRecord->get(28); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM1(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(29); return mRecord->get(29); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM2(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(30); return mRecord->get(30); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM3(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(31); return mRecord->get(31); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM4(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(32); return mRecord->get(32); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM5(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(33); return mRecord->get(33); } ERROR_LOG("No exist record %d", mKey); return Data(); }
	Data SYSTEM6(bool bSetUpdate=false) /*NICEDATA*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(34); return mRecord->get(34); } ERROR_LOG("No exist record %d", mKey); return Data(); }


	Int64 wID() const { Int64 v = 0; if (mRecord) {  mRecord->get(0, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	const char* szACCOUNT() const { if (mRecord)  return  mRecord->getString(1); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strACCOUNT() const { if (mRecord)  return  mRecord->getAString(1); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szNAME() const { if (mRecord)  return  mRecord->getString(2); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strNAME() const { if (mRecord)  return  mRecord->getAString(2); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	const char* szJUHE_USER() const { if (mRecord)  return  mRecord->getString(3); ERROR_LOG("No exist record %d", mKey); return ""; }
	const AString& strJUHE_USER() const { if (mRecord)  return  mRecord->getAString(3); ERROR_LOG("No exist record %d", mKey); static AString s; return s; }
	AutoNice niceDATA(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(4, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(4, v); } else mRecord->NotifyChanged(4); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceFETTER(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(5, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(5, v); } else mRecord->NotifyChanged(5); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceFETTERTEACHER(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(6, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(6, v); } else mRecord->NotifyChanged(6); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceDISCUSS(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(7, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(7, v); } else mRecord->NotifyChanged(7); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceACADEMIC(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(8, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(8, v); } else mRecord->NotifyChanged(8); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceCOMPETITION(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(9, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(9, v); } else mRecord->NotifyChanged(9); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceLEAGUE(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(10, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(10, v); } else mRecord->NotifyChanged(10); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceBOSS(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(11, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(11, v); } else mRecord->NotifyChanged(11); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceALLIANCEFUBEN(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(12, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(12, v); } else mRecord->NotifyChanged(12); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceMONTHCARD(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(13, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(13, v); } else mRecord->NotifyChanged(13); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceALLIANC_SKILL(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(14, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(14, v); } else mRecord->NotifyChanged(14); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceGIFT(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(15, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(15, v); } else mRecord->NotifyChanged(15); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSEVEN_LOGIN(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(16, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(16, v); } else mRecord->NotifyChanged(16); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceICON(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(17, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(17, v); } else mRecord->NotifyChanged(17); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceESC(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(18, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(18, v); } else mRecord->NotifyChanged(18); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceTICKET(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(19, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(19, v); } else mRecord->NotifyChanged(19); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceFUND(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(20, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(20, v); } else mRecord->NotifyChanged(20); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceRANK(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(21, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(21, v); } else mRecord->NotifyChanged(21); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceITEMSHOP(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(22, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(22, v); } else mRecord->NotifyChanged(22); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	int nSTATUS() const { int v = 0; if (mRecord) {  mRecord->get(23, v); return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceACTIVITY(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(24, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(24, v); } else mRecord->NotifyChanged(24); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceBASEPROPERTY(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(25, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(25, v); } else mRecord->NotifyChanged(25); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSTUDENT(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(26, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(26, v); } else mRecord->NotifyChanged(26); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceTEACHER(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(27, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(27, v); } else mRecord->NotifyChanged(27); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(28, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(28, v); } else mRecord->NotifyChanged(28); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM1(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(29, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(29, v); } else mRecord->NotifyChanged(29); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM2(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(30, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(30, v); } else mRecord->NotifyChanged(30); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM3(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(31, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(31, v); } else mRecord->NotifyChanged(31); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM4(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(32, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(32, v); } else mRecord->NotifyChanged(32); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM5(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(33, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(33, v); } else mRecord->NotifyChanged(33); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }
	AutoNice niceSYSTEM6(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(34, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(34, v); } else mRecord->NotifyChanged(34); } return v; } ERROR_LOG("No exist record %d", mKey); return v; }

	static bool InitCheck(AutoTable t, bool bInitSet)
	{
       if (bInitSet) table = t;
		AutoField f = t->GetField();
		if (!f || !f->CheckSame(2005973233))
		{
			ERROR_LOG("No same check code of %s", t->GetTableName());
           return false;
       }

		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "ID"; fieldNameList[1] = "ACCOUNT"; fieldNameList[2] = "NAME"; fieldNameList[3] = "JUHE_USER"; fieldNameList[4] = "DATA"; fieldNameList[5] = "FETTER"; fieldNameList[6] = "FETTERTEACHER"; fieldNameList[7] = "DISCUSS"; fieldNameList[8] = "ACADEMIC"; fieldNameList[9] = "COMPETITION"; fieldNameList[10] = "LEAGUE"; fieldNameList[11] = "BOSS"; fieldNameList[12] = "ALLIANCEFUBEN"; fieldNameList[13] = "MONTHCARD"; fieldNameList[14] = "ALLIANC_SKILL"; fieldNameList[15] = "GIFT"; fieldNameList[16] = "SEVEN_LOGIN"; fieldNameList[17] = "ICON"; fieldNameList[18] = "ESC"; fieldNameList[19] = "TICKET"; fieldNameList[20] = "FUND"; fieldNameList[21] = "RANK"; fieldNameList[22] = "ITEMSHOP"; fieldNameList[23] = "STATUS"; fieldNameList[24] = "ACTIVITY"; fieldNameList[25] = "BASEPROPERTY"; fieldNameList[26] = "STUDENT"; fieldNameList[27] = "TEACHER"; fieldNameList[28] = "SYSTEM"; fieldNameList[29] = "SYSTEM1"; fieldNameList[30] = "SYSTEM2"; fieldNameList[31] = "SYSTEM3"; fieldNameList[32] = "SYSTEM4"; fieldNameList[33] = "SYSTEM5"; fieldNameList[34] = "SYSTEM6"; 
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
