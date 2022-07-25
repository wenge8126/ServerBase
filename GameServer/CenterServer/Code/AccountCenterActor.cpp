#include "AccountCenterActor.h"
#include "CenterThread.h"
#include "GenerateDBUser.h"
#include "GenerateCodeTool.h"

CenterThread* AccountCenterActor::GetLoginThread()
{
	Auto<CenterActorManager> mgr = GetMgr();
	return mgr->mpThread;
}

void AccountCenterActor::On(RQ_CreateDBTable &msg, RS_CreateDBTable &resp, UnitID senderID, int)
{
	LOG("Rev %s msg %s :\r\n%s", senderID.dump().c_str(), msg.GetMsgName(), msg.dump().c_str());

	for (auto it = msg.mDBConfigData->begin(); it; ++it)
	{
		AString info;
		AutoTable configTable;
		it.get().get(&configTable, typeid(AutoTable));
		if (configTable)
		{

			if (GetDBMgr()->CreateDBTable(it.key().c_str(), configTable, info))
			{
				info.Format("Succeed create table %s", it.key().c_str());
				
				
			}
			AutoTable t = GetDBMgr()->GetTable(it.key().c_str());
			AString compName = "DBUser_";
			compName += it.key();
			GenerateDBUser::generate(compName, t, "../GameServer/Common/", false);
		}
		else
		{
			info.Format("Restore config table %s fail", it.key().c_str());
		}
		resp.mResultInfo += info;
		resp.mResultInfo += "\r\n";
	}

	//auto &list = GetDBMgr()->GetTableList();

	//GenerateCodeTool::generateDBManager(list, msg.mExportCodePath, "DB");

}

void AccountDBComponent::OnLoadRecord(ARecord record)
{
	((AccountCenterActor*)mpActor)->OnLoadRecord(record);
}
