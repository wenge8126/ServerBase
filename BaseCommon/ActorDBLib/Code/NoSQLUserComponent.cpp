#include "NoSQLUserComponent.h"



//-------------------------------------------------------------------------
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
			AutoTable t = MEM_NEW StructBaseTable(INIT_FIELD_MODE::eInitCreateField);
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

//-------------------------------------------------------------------------