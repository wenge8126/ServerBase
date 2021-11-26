#pragma once

#include "MemoryDBTable.h"
#include "Actor.h"
#include "ActorDBLib.h"

namespace NetCloud
{
	class ActorDBLib_Export TableActor : public Actor
	{
	public:
		TableActor()
		{
			
		}

	public:
		Auto<MemoryDBTable> GetTable() { return mDBTable; }

		void InitSQLTable();

		void InitShareSQL();

		void InitShareTable()
		{
			//mDBTable = ShareDBTable(this);
		}

	public:
		virtual void RegisterMsg(ActorManager *pActor);

	protected:
		Auto<MemoryDBTable> mDBTable;
	};

}
