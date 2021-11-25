#pragma once

#ifndef _INCLUDE_DBSHAREMEMMSGEVENT_H_
#define _INCLUDE_DBSHAREMEMMSGEVENT_H_

namespace NetCloud
{
	//-------------------------------------------------------------------------
	class tMemBaseEvent : public Logic::tBaseNetEvent
	{
	public:
		ShareMemCloudDBNode* GetDBNode()
		{
			if (mNetConnect)
			{
				Hand<ShareMemDBNet> net = mNetConnect->GetNetHandle();
				return net->mpDB;
			}
			return NULL;
		}
	};

	//-------------------------------------------------------------------------
		// 通知共享内存新建增加
	class MD_NotifyNewAppendShareMem : public tMemBaseEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			int key = get("SHARE_KEY");
			int index = get("ARRAY_INDEX");
			AString tableName = get("TABLE_NAME");
			Auto<ShareMemDBTable> table = GetDBNode()->GetTable(tableName);
			table->AppendShareMem(key, index);

			return true;
		}
	};
	//-------------------------------------------------------------------------

}

#endif //_INCLUDE_DBSHAREMEMMSGEVENT_H_
