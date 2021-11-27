#pragma once

#include "MemoryDBTable.h"
#include "Actor.h"
#include "ActorDBLib.h"
#include "ActorManager.h"

#define TableActorType			(-10000)

namespace NetCloud
{
	enum EActorTableType
	{
		eATable_MYSQL,
		eATable_ShareSQL,
		eATable_ShareMem,
	};

	class ActorDBLib_Export TableActor : public Actor
	{
	public:
		TableActor()
		{
			
		}

	public:
		struct InitParam
		{
			Auto<ActorManager> mgr;
			AutoNice	mSQLParam;
		};
		static void InitDB(Auto<ActorManager> mgr, AutoNice sqlParam)
		{
			InitParam p = { mgr, sqlParam };
			CoroutineTool::AsyncCall(&TableActor::_AsyncInitDB, p);
		}

		static void _AsyncInitDB(InitParam p);

	public:
		Auto<MemoryDBTable> GetTable() { return mDBTable; }

		void StartDB(EActorTableType type, AutoNice param)
		{
			switch (type)
			{
			case NetCloud::eATable_MYSQL:
				InitSQLTable(param);
				break;
			case NetCloud::eATable_ShareSQL:
				InitShareSQL(param);
				break;
			case NetCloud::eATable_ShareMem:
				InitShareTable(param);
				break;
			default:
				ERROR_LOG("No exist type %d", type);
				break;
			}
		}

		void InitSQLTable(AutoNice param);

		void InitShareSQL(AutoNice param);

		void InitShareTable(AutoNice param)
		{
			//mDBTable = ShareDBTable(this);
		}

	public:
		virtual void RegisterMsg(ActorManager *pActor);

	protected:
		Auto<MemoryDBTable> mDBTable;
	};

	//-------------------------------------------------------------------------
	struct TableActorID
	{
		union
		{
			struct {
				Int64				id : 32;
				Int64				hash : 16;
				Int64				type : 16;
			};
			UInt64			value;
		};
	};
	/* 使用如下
	NetCloud::TableActorID  x;
	x.id = 123456789;
	x.type = 222;
	x.hash = 55;

	UnitID z(33, 888);
	z.id = x.value;

	NetCloud::TableActorID cc;
	cc.value = z.id;
	*/
	//-------------------------------------------------------------------------
}
