
public enum EActorType
{
    Actor_Login = 100,
    Actor_DB = 101,
    Actor_WebLogin = 103,  // 只用来提供帐号web http 获取服务器列表, 帐号web分配 的单元ID
    Actor_DBWorker = 104,
    Actor_ToolWorker = 105,
    Actor_ManagerTool = 1000,
    Actor_Account = 1001,
    Actor_GameServer = 1002,
    Actor_Player = 1003,
    Actor_Client = 1004,
    Actor_AccountCenter,
    Actor_GameCenter,
    ClientActor_Login,
}

