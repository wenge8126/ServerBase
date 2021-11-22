#include "SQLThread.h"



bool LoadSQLTask::WriteToThread(ThreadLoopData &targetData, StrLenType index)
{
	DSIZE len = mSQLString.length();
	if (len > SQL_STRING_MAX)
	{
		ERROR_LOG("SQL string %d > %d", len, SQL_STRING_MAX);
		return false;
	}

	TaskDataHead head;
	head.mIndex = index;
	head.mDataLength = len;
	head.mType = mLoadSQLType;

	targetData.Write((const char*)&head, sizeof(head));
	targetData.Write(mSQLString.c_str(), mSQLString.length());
	return true;
}

bool LoadSQLTask::AWait(SQLThread *pSQLThread)
{
	if (pSQLThread->AppendSQL(this))
	{
		mCoroID = CORO;
		PRINT("Wait SQL ...")
			YIELD
			PRINT("Finist SQL ... %s ", mbSucceed ? "OK" : "FAIL");
			return mbSucceed;
	}
	return false;
}
