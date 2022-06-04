#pragma once

#ifndef _INCLUDE_DBUSERCOMPONENT_H_
#define _INCLUDE_DBUSERCOMPONENT_H_

#include "Actor.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
	ʹ��Actor DB�������
	1 ���Բ�����DBActor, Ҫ�����ʹ��DBActoryManager
	2 �ɽṹ��񵼳��м����ݱ�ṹ��, ����ֱ�Ӳ���DB��¼
	//-------------------------------------------------------------------------*/

	class ActorDBLib_Export_H DBUserComponent : public Component
	{
	public:
		virtual bool CheckTable(AutoTable t) = 0;

	public:
		bool InitTable(const char *szTableName)
		{
			AutoTable t = GetDBMgr()->GetTable(szTableName);
			if (CheckTable(t))
			{
				mDBTable = t;
				return true;
			}
			return false;
		}

		AutoDBManager GetDBMgr()
		{
			return mpActor->GetDBMgr();
		}

		ARecord LoadRecord(const char *szKey)
		{
			if (!mDBTable)
			{
				ERROR_LOG("Must init table");
				return ARecord();
			}

			ARecord re = mDBTable->GetRecord(szKey);
			if (re)
			{
				mKey = szKey;
				mDataRecord = re;
				return re;
			}
			ERROR_LOG("No exist %s in table %s", szKey, mDBTable->GetTableName());

			return ARecord();
		}

		ARecord LoadRecord(Int64 nKey)
		{
			if (!mDBTable)
			{
				ERROR_LOG("Must init table");
				return ARecord();
			}
			ARecord re = mDBTable->GetRecord(nKey);
			if (re)
			{
				mKey = STRING(nKey);
				mDataRecord = re;
				return re;
			}
			ERROR_LOG("No exist %s in table %s", STRING(nKey), mDBTable->GetTableName());

			return ARecord();
		}

	public:
		virtual void LowUpdate() override
		{
			if (mDataRecord)
				mDataRecord->SaveUpdate();
		}

	protected:
		AString		mKey;
		ARecord	mDataRecord;
		AutoTable mDBTable;
	};

}

#endif //_INCLUDE_DBUSERCOMPONENT_H_
