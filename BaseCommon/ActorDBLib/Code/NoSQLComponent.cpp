#include "NoSQLComponent.h"

#include "FieldIndex.h"

//bool NetCloud::NoSQLUserComponent::Load(bool bNeedLoadField)
//{
//	tDBData *pDBData = &mData;
//	SQL_LoadNoSQLData loadMsg;
//	loadMsg.mbNeedField = bNeedLoadField;
//	loadMsg.mKey = mData.mKey;
//	SQL_ResponseNoSQLData resp;
//	if (mpActor->Await(mData.GetNoSQLID(NOSQL_DB_TYPE, GetNoSQLCount()), loadMsg, resp, 10000))
//	{
//		AutoTable t = MEM_NEW NormalBaseTable(INIT_FIELD_MODE::eInitCreateField);
//		if (resp.mFieldData)
//		{
//			mData.mField = t->GetField(); // MEM_NEW FieldIndex(t.getPtr(), 0);
//			resp.mFieldData->seek(0);
//			if (!mData.mField->restoreFromData(resp.mFieldData.getPtr()))
//			{
//				ERROR_LOG("Restore field fail");
//				return false;
//			}
//			t->SetMainIndex(0, false, true);
//		}
//		//t->SetField(mData.mField);
//		mData.mDataRecord = t->CreateRecord(mData.mKey, true);
//		resp.mData->seek(0);
//		if (mData.mDataRecord->restoreData(resp.mData.getPtr()))
//			return true;
//		ERROR_LOG("Restore record fail");
//	}
//	return false;
//}

//bool NetCloud::NoSQLUserComponent::LoadNice()
//{
//	tDBData *pDBData = &mData;
//	SQL_LoadNoSQLData loadMsg;
//	loadMsg.mbNeedField = false;
//	loadMsg.mKey = mData.mKey;
//	SQL_ResponseNoSQLData resp;
//	if (mpActor->Await(mData.GetNoSQLID(NOSQL_DB_TYPE, GetNoSQLCount()), loadMsg, resp, 10000))
//	{
//		if (!pDBData->mNiceData)
//			pDBData->mNiceData = MEM_NEW NiceData();
//		resp.mData->seek(0);
//		if (mData.mNiceData->restore(resp.mData.getPtr()))
//			return true;
//		ERROR_LOG("Restore nicedata fail");
//	}
//	return false;
//}

bool NetCloud::RecordNoSQLUserComponent::Load(bool bNeedLoadField)
{
	SQL_LoadNoSQLData loadMsg;
	loadMsg.mbNeedField = bNeedLoadField;
	loadMsg.mKey = mKey;
	SQL_ResponseNoSQLData resp;
	if (mpActor->Await(GetNoSQLID(), loadMsg, resp, 10000))
	{
		if (bNeedLoadField)
		{
			AutoTable t = MEM_NEW NormalBaseTable(INIT_FIELD_MODE::eInitCreateField);
			if (resp.mFieldData)
			{
				mField = t->GetField(); // MEM_NEW FieldIndex(t.getPtr(), 0);
				resp.mFieldData->seek(0);
				if (!mField->restoreFromData(resp.mFieldData.getPtr()))
				{
					ERROR_LOG("Restore field fail");
					return false;
				}
				t->SetMainIndex(0, false, true);
			}

			//t->SetField(mData.mField);
			if (!mDataRecord)
				mDataRecord = t->CreateRecord(mKey, true);
		}
		resp.mData->seek(0);
		if (mDataRecord->restoreData(resp.mData.getPtr()))
			return true;
		ERROR_LOG("Restore record fail");
	}
	return false;
}
