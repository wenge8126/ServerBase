#pragma once

#ifndef _NCLUDE_MEMORYDBTABLE_2_H_
#define _NCLUDE_MEMORYDBTABLE_2_H_

#include "IndexBaseTable.h"
#include "SQLComponect.h"
#include "ActorDBLib.h"

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// 具有落地功能的内存表
	// 支持自动冷处理
	// 记录一旦变化, 立即落地处理
	// 低速时, 进行一定数据的冷处理检测
	//-------------------------------------------------------------------------
	class TableActor;
	class ActorDBLib_Export MemoryDBTable : public SkipBaseTable
	{
	public:
		MemoryDBTable(TableActor *pActor);

		void InitSQL(Hand<SQLComponect> sqlComp)
		{			
			mSQLComp = sqlComp;
		}



	protected:
		TableActor *mpActor;
		Hand<SQLComponect>		mSQLComp;
	};
	//-------------------------------------------------------------------------

}
#endif //_NCLUDE_MEMORYDBTABLE_2_H_
