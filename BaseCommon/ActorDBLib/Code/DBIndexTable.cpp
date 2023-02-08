#include "DBIndexTable.h"
#include "DBUserComponent.h"


bool NetCloud::DBIndexTable::LoadAllRecord(DBUserComponent *pComponent)
{
	AString sql;

	sql.Format("SELECT * from `%s`", GetTableName());

	if (!mDBDataLoadSQL->mMainThreadSQLTool.exeSql(sql, true))
	{
		ERROR_LOG("Exe sql %s error : %s", sql.c_str(), mDBDataLoadSQL->mMainThreadSQLTool.getErrorMsg());
		return false;
	}

	MySqlDBTool &tool = mDBDataLoadSQL->mMainThreadSQLTool;

	while (true)
	{
		ARecord r = NewRecord();
		r->_alloctData(0);
		if (tool.LoadRecord(r))
		{
			mRecordMap.insert(RecordMap::value_type(r[0], r));
			if (pComponent != NULL)
				pComponent->OnLoadRecord(r);
		}
		else
			break;
	}
	return true;
}

bool NetCloud::DBIndexTable::DeleteRecord(ARecord record)
{
	Int64 wKey = record[0];
	auto result = mRecordMap.erase(wKey);
	if (result>0)
	{
		AString sql;

		sql.Format("DELETE FROM `%s` WHERE `%s` = %s", GetTableName(), mKeyFieldName.c_str(), STRING(wKey));

		int affectLine = 0;
		if (!mDBDataLoadSQL->mMainThreadSQLTool.exeSql(sql, false, &affectLine))
		{
			ERROR_LOG("Exe sql %s error : %s", sql.c_str(), mDBDataLoadSQL->mMainThreadSQLTool.getErrorMsg());
			return false;
		}
		if (affectLine <= 0)
			ERROR_LOG("Delete %s fail from DB %s, affect line <= 0", GetTableName(), STRING(wKey));

		return true;
	}
	ERROR_LOG("Delete fail : %s, not find in record list", record[0].c_str());
	return false;
}
