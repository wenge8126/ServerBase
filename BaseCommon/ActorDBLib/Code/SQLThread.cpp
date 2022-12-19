//#include "SQLThread.h"
//
//
//
//bool LoadSQLTask::WriteToThread(ThreadLoopData &targetData, StrLenType index)
//{
//	DSIZE len = mSQLString.length()+1;
//
//
//	TaskDataHead head;
//	head.mIndex = index;  // �ر�ע��,  ��ȡʱ, mIndex Ϊִ���б��е�����, ���ڷ��ؽ��
//	head.mDataLength = len; // �ر�ע��,  ��ȡʱ, mDataLength Ϊ��ѯ�ַ����ĳ���
//	head.mType = mLoadSQLType;
//
//	if (len  > SQL_STRING_MAX)
//	{
//		ERROR_LOG("SQL string %d > %d", len, SQL_STRING_MAX);
//		return false;
//	}
//
//	targetData.Write((const char*)&head, sizeof(head));
//	targetData.Write(mSQLString.c_str(), mSQLString.length()+1);
//	return true;
//}
//
//bool LoadSQLTask::AWait(SQLThread *pSQLThread)
//{
//	if (CORO == 0)
//	{
//		ERROR_LOG("Not in coroutine");
//		return false;
//	}
//
//	if (pSQLThread->AppendSQL(this))
//	{		
//		mCoroID = CORO;
//		PRINT("Wait SQL ...")
//			YIELD
//			PRINT("Finist SQL ... %s ", mbSucceed ? "OK" : "FAIL");
//			return mbSucceed;
//	}
//	return false;
//}
