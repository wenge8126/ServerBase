#include "NoSQLUserComponent.h"



//-------------------------------------------------------------------------
bool NetCloud::RecordNoSQLUserComponent::Load(const AString &key, bool bNeedLoadField)
{
	SQL_LoadNoSQLData loadMsg;
	loadMsg.mbNeedField = bNeedLoadField;
	loadMsg.mKey = key;
	SQL_ResponseNoSQLData resp;
	if (mpActor->Await(GetNoSQLID(), loadMsg, resp, 10000))
	{
		if (bNeedLoadField)
		{
			if (!resp.mFieldData)
			{
				ERROR_LOG("Response field data is null");
				return false;
			}

			AutoTable t = NewTable();
			AutoField field = t->GetField(); // MEM_NEW FieldIndex(t.getPtr(), 0);
			resp.mFieldData->seek(0);
			if (!field->restoreFromData(resp.mFieldData.getPtr()))
			{
				ERROR_LOG("Restore field fail");
				return false;
			}
			mDataRecord = t->CreateRecord(key, true);
		}

		resp.mData->seek(0);
		if (mDataRecord && mDataRecord->restoreData(resp.mData.getPtr()))
			return true;
		ERROR_LOG("Restore record fail");
	}
	return false;
}

//-------------------------------------------------------------------------