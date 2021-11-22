//Auto generate from ExcelTool 1.00.00
//
#pragma once

enum EnumHttpCommand
{
    eUpdateGameNotice = 1000,    //后端更新游戏公告
    eGetGameNotice = 1001,    //获取游戏公告
    eDeleteGameNotice = 1002,    //删除游戏公告
    eSendSystemNotice = 1003,    //发送系统公告
    eHttpCmdCode = 1004,    //激活码废弃
    eHttpCmdServerMail = 1005,    //系统邮件
    eHttpCmdRecharge = 1006,    //充值
    eHttpCmdPlayerInfo = 1007,    //用户信息查询
    eHttpCmdFenghao = 1008,    //封号
    eSeachDBBack = 1009,    //查询DB备份begindate=2021-01-28&enddate2021-02-20
    eQuestProp = 1010,    //查询玩家属性
    eForbiddenChat = 1011,    //禁言
    eRemoveForbiddenChat = 1012,    //解除禁言
    eLoadChatRecord = 1013,    //获取指定玩家聊天信息
    eHttpCmdRechargeEx = 1014,    //虚拟充值
    eHttpCmdDeblocking = 1015,    //解封
    eRechargeInfo = 1017,    //查询充值信息
    eGetServerList = 1018,    //获取服务器信息
    eGetServerListEx = 1019,    //获取服务器信息Info
    eSearchSystemNotice = 1020,    //查询系统公告
    eDeleteSystemNotice = 1021,    //删除系统公告
    eVerificationRole = 1022,    //角色验证接口
    eBuyGift = 1023,    //直购礼包接口
    eCanBuyGift = 1024,    //判断直购礼包能否购买接口
    eCodeGM = 1025,    //GM命令
    eLoadLogicDBInfo = 1026,    //获取游戏区逻辑服务信息(Gate和WorkerID)
    eLoadRunFileInfo = 1027,    //获取运行数据信息
    eLoadRunFileData = 1028,    //获取运行数据块
    eLoadGlobalConfigData = 1029,    //获取全局配置文件数据(返回指定块数据)
    eCodeQuestPlayer = 1030,    //激活码角色信息查询接口
    eCodeSendAward = 1031,    //激活码发送奖励道具接口
    eHttpWXRecharge = 1032,    //微信充值
    eGetWebClientVersion = 2000,    //获取web客户端版本
};

enum EnumItemID
{
    eItemWuHuaGuo = 120001,    //无花果
    eItemLottery = 120006,    //直播券
};

enum EnumQuality
{
    eQuality0 = 0,    //普通
    eQuality1 = 1,    //优秀
    eQuality2 = 2,    //精致
    eQuality3 = 3,    //卓越
    eQuality4 = 4,    //完美
    eQuality10 = 10,    //初级
    eQuality11 = 11,    //中级
    eQuality12 = 12,    //高级
    eQuality13 = 13,    //专家
    eQuality14 = 14,    //权威
    eQuality20 = 20,    //普通
    eQuality21 = 21,    //优秀 
    eQuality22 = 22,    //精英 
    eQuality23 = 23,    //特级 
    eQuality24 = 24,    //学霸 
    eQuality30 = 30,    //初级
    eQuality31 = 31,    //中级
    eQuality32 = 32,    //高级
    eQuality33 = 33,    //助教
    eQuality34 = 34,    //教授
};

enum EnumQualityColor
{
    eQualityColor0 = 0,    //白色
    eQualityColor1 = 1,    //绿色
    eQualityColor2 = 2,    //绿色+
    eQualityColor3 = 3,    //蓝色
    eQualityColor4 = 4,    //蓝色+
    eQualityColor5 = 5,    //紫色
    eQualityColor6 = 6,    //紫色+
    eQualityColor7 = 7,    //橙色
    eQualityColor8 = 8,    //橙色+
    eQualityColor9 = 9,    //红色
    eQualityColor10 = 10,    //红色+
};

enum EnumEffectConfig
{
    eXS_HeiBan_KaiXin = 1,
    eXS_HeiBan_LingGan = 2,
    eXS_HeiBan_ZuoCuoTi = 3,
    eXS_KaoShi = 4,
    eXS_ZaHeiBan = 5,
    eHandUp_Bubble = 6,
    eToilet_Bubble = 7,
    eCommode_Bubble = 8,
    eZZ_ChuXian = 9,
    eXS_DianMing_Bubble = 10,
    eLS_DianMing_Bubble = 11,
    eLS_DianMingShangKe_Bubble = 12,
    eTalk_Bubble = 13,
    eNagging_Bubble = 14,
    eTimeCount = 15,
    eZS_ChuXian = 16,
};

enum EnumTaskType
{
    eAI = 0,
    eThread = 1,
    eTotal = 2,
    eEveryDay = 3,
    eActive = 4,
    eResearch = 5,
    eOptionalCourses = 6,
    eAcceptCourses = 7,
    eLevelUpTask = 8,
};

enum EnumTicketConfig
{
    eTicketTeaching = 0,    //快速教学
    eTicketDiscuss = 1,    //学术研讨
    eTicketCompetion = 2,    //个人竞赛
    eTicketDiamondShop = 3,    //钻石商店
    eTicketcademicShop = 4,    //学术商店
    eTicketchoolunionShop = 5,    //校盟商店
    eTicketducationShop = 6,    //进修商店
    eTicketonorShop = 7,    //荣誉商店
};

enum EnumAttributeDefine
{
    eSystemExtendDesk = 0,    //桌子
    eSystemExtendDecorate = 1,    //装饰物
    eSystemAllianceSkill = 2,    // 校盟技能属性
    eSystemTeacher = 3,    //老师属性
    eSystemAcademicExtra = 4,    // 学府之路精英版
    eSystemSecretary = 5,    //秘书系统
    eSystemPlayerMax = 6,    // 玩家分界线
    eStudentBase = 7,    // 学生基础
    eStudentLevel = 8,    // 学生等级
    eStudentCulture = 9,    //学生培养
    eStudentFetter = 10,    //学生羁绊
    eStudentEquip = 11,    //学生装备
    eStudentMax = 12,    // 学生分界线
};

enum EnumEnumItemUse
{
    eSUToItem = 1,    //万能碎片转化学生碎片
    eItemToSU = 2,    //学生碎片转化万能碎片
    eBoxToItem = 3,    //万能宝箱
    eExperience = 4,    //获得n小时心得
    eUseItemTuition = 5,    //获得n小时学费
    eBoxToStudent = 6,    //特招函
};

enum EnumPlayerSexConfig
{
    eWoman = 1,    //女
    eMan = 2,    //男
};

enum EnumServerState
{
    eServerNormal = 0,    //正常
    eServerMaintain = 1,    //维护
    eServerHot = 2,    //火爆
    eServerRecommend = 3,    //推荐
    eServerFluent = 4,    //流畅
};

enum EnumActivityConfig
{
    eActSubjectWelcome = 1,    //学科迎新
    eActRechargeBack = 2,    //充值返利
    eActRankStudent = 3,    //学生榜
    eActRankTeacher = 4,    //老师榜
    eActRankAlliance = 5,    //校盟榜
    eActLimitTime2 = 7,    //限时活动-6元
    eActLimitTime3 = 8,    //限时活动-18元
    eActLimitTime4 = 9,    //限时活动-68元
    eActLimitTime5 = 10,    //限时活动-128元
};

enum EnumHurtTypeConfig
{
    eHurtHit = 1,    //命中
    eHurtCritical = 2,    //暴击
    eImmune = 3,    //免疫伤害
};

enum EnumTaskConfig
{
    eTask_Active1 = 40001,    //活跃度
    eTask_Active2 = 40002,    //活跃度
    eTask_Active3 = 40003,    //活跃度
    eTask_Active4 = 40004,    //活跃度
    eTask_Active5 = 40005,    //活跃度
    eTask_Research = 40006,    //教研3小时任务（数值秒,PARAM:产出随机次数）
    eRewardSayHello = 40013,    //校盟打招奖励-配置奖励及抛出打招呼事件,不实例任务,无类名
    eTask_Refresherclass = 40016,    //进修班任务
    eTask_DaTi = 40018,    //答题
    eTask_FaZhan = 40019,    //罚站
    eTask_GoWC = 40020,    //上厕所
    eTask_UnmannedDrone = 40021,    //无人机任务
    eTask_Research_5 = 40025,    //教研5分钟任务（数值秒,PARAM:产出随机次数）
};

enum EnumChatChannel
{
    eChatChannelNone = 0,    //无
    eChatChannelToPlayer = 1,    //发送给目标玩家
    eChatChannelToAlliance = 2,    //发送给同盟
    eChatChannelWorld = 3,    //全区广播
};

enum EnumEnumNoticeType
{
    ePop = 1,
    eTipWindow = 2,
};

enum EnumStudentAction
{
    eTingke = 0,    //听课
    eZhanLi = 1,    //站立
    eJuShou = 2,    //举手
    eKaiXin = 3,    //开心
    eCeSuo = 4,    //上厕所
    eShuiJiao = 5,    //睡觉
    eXieZi = 6,    //写字
    eDaTi = 7,    //答题
    eBeiZou = 8,    //背面走
    eCeZou = 9,    //左侧走
    eZhenZou = 10,    //正面走
};

enum EnumTalkType
{
    eAcademic = 1,    //副本
    eOther = 2,    //其他
};

enum EnumSecretaryAvatarPart
{
    ePartHair = 0,    //发型
    ePartBody = 1,    //上装
    ePartThigh = 2,    //下装
    ePartShoes = 3,    //鞋子
    ePartWear = 4,    //饰品
    ePartSuit = 5,    //套装
};

enum EnumMailConfig
{
    DiscussAwardDay = 1,    //五人竞赛奖励
    DiscussAwardQuarter = 2,    //五人竞赛赛季奖励
    CompetitionAwardDay = 3,    //单人竞赛奖励
    CompetitionAwardQuarter = 4,    //单人竞赛赛季奖励
    LevelAwardDay = 5,    //学校总分排行
    StudentAwardDay = 9,    //最佳学生排行
    RefuseJoinAllianceMail = 13,    //成功加入校盟
    PulloverRewardMail = 14,    //遗漏的奖学金
    KickOutAlliance = 15,    //踢出校盟通知
    CodeAward = 16,    //奖励发放
    AllianceWishRewardMail = 17,    //校盟许愿奖励
    BagFullMail = 18,    //储藏室已满
    eActOpenRankStudent = 19,    //学生榜
    eActOpenRankTeacher = 20,    //老师榜
    eActOpenRankAlliance = 21,    //校盟榜
    eReturnGameAward = 22,    //回家大礼包
    eReturnGameRechargeAward = 23,    //删档充值奖励
    eRechargeWx = 24,    //微信充值奖励
};

enum EnumSystemFunctionConfig
{
    eSystemExtend = 0,    //扩建系统
    eSystemShop = 1,    //小卖铺
    eSystemSpring = 2,    //春游系统
    eSystemFriend = 3,    //好友系统
    eSystemHelp = 4,    //外援系统
    eSystemFetter = 5,    //羁绊系统
    eSystemDiscuss = 6,    //学术研讨系统
    eSystemCompetition = 7,    //个人竞赛
    eSystemAcademic = 8,    //学府之路
    eSystemLeague = 9,    //学科联赛系统
    eSystemBoss = 10,    //限时boss系统
    eSystemAllianceFuben = 11,    //校盟副本
    eSystemAlliance = 12,    //校盟系统
    eSystemDrawCard = 13,    //抽卡系统
    eSystemTeacherLeacture = 14,    //名师讲座系统
    eSystemRefresherClass = 15,    //进修班系统
    eSystemLaboratory = 16,    //教研室
    eSystemChat = 17,    //聊天
    eSystemGift = 18,    //礼包
    eSystemSevenDay = 19,    //七日登录
    eSystemStoreroom = 20,    //储藏室
    eSystemFirstRecharge = 21,    //首充
    eSystemEmail = 22,    //邮件
    eSystemRank = 23,    //排行榜
    eSystemActivity = 24,    //活动
    eSystemTask = 25,    //任务
    eSystemSignIn = 26,    //签到
    eSystemPublicColumn = 27,    //公告栏
    eSystemSecret = 28,    //秘书
    eSystemOfficialAcounts = 29,    //公众号
    eSystemAutonym = 30,    //实名认证
    eResetSystem = 31,    //时光机
    eRushTop = 34,    //冲榜
    EquipDecompose = 35,    //分解学习用品
    eFirstRecharge = 36,    //首充
};

enum EnumEnumSubject
{
    mHumanity = 1,    //人文
    mMathematical = 2,    //数理
    mArt = 3,    //艺术
    mSports = 4,    //体育
    mFamousTeacher = 5,    //名师
};

enum EnumEquipPosConfig
{
    eEquipPos0 = 0,    //学科用具
    eEquipPos1 = 1,    //理论用具
    eEquipPos2 = 2,    //课外物
    eEquipPos3 = 3,    //玩具
    eEquipPos4 = 4,    //专属1
    eEquipPos5 = 5,    //专属2
    eEquipPosMax = 6,    //装备的最大孔
};

enum EnumGamePropertyConfig
{
    eDiscussTime = 0,    //学术研讨赛季结束时间
    eCompetitionTime = 1,    //个人竞技赛季结束时间
    eCreateAlliance = 2,    //创建校盟
    eNewDay = 3,    //新的一天
};

enum EnumHelpStatusConfig
{
    eHelpNothing = 0,    //空闲中
    eHelpWork = 1,    //工作中
};

enum EnumMailStatusConfig
{
    eUnread = 0,
    eRead = 1,
    eReceived = 2,
    eDelete = 3,
};

enum EnumRankType
{
    eScoreRank = 0,    //学校总分榜
    eSchoolRank = 1,    //学校等级排行榜
    eClassRank = 2,    //班级排行榜
    eStudentRank = 3,    //优生榜
    eDiscussRank = 4,    //5人竞赛
    eCompetitionRank = 5,    //单人竞赛
    eLeagueRank1 = 6,    //学科联赛-人文
    eLeagueRank2 = 7,    //学科联赛-数理
    eLeagueRank3 = 8,    //学科联赛-艺术
    eLeagueRank4 = 9,    //学科联赛-体育
    eLeagueRank5 = 10,    //学科联赛-综合
    eAllianceDamageRank = 11,    //校盟副本伤害排行
    eTeacherRank = 12,    //老师榜
    eAcademicRank = 13,    //关卡榜
    eAllianceRank = 14,    //校盟榜
};

enum EnumStudentState
{
    eIdle = 0,    //待机
    eGoWC = 1,    //上厕所
    eAnswer = 2,    //答题
    eSleep = 3,    //睡觉罚站
    ePlay = 4,    //碎碎念
    eStateMax = 5,    //最大值
};

enum EnumTeacherAction
{
    eZhanBei = 0,    //背面待机
    eZhanZuo = 1,    //左侧面待机
    ZhanYou = 2,    //右侧面待机
    ZhanZheng = 3,    //正面待机
    eJiangBei = 4,    //背面讲课
    eJiangZuo = 5,    //左侧讲课
    eJiangYou = 6,    //右侧讲课
    eJiangZheng = 7,    //正面讲课
    eTeacherXieZi = 8,    //写字
    eZouBei = 9,    //背面走路
    eZouYou = 10,    //右侧走路
    eZouZuo = 11,    //左侧走路
    eZouZheng = 12,    //正面走路
    eZouJiangYou = 13,    //右侧走讲
    eZouJiangZuo = 14,    //左侧走讲
    eZouJiangZheng = 15,    //正面走讲
};

enum EnumAutonymConfig
{
    Autonym0 = 0,    //未实名
    Autonym1 = 1,    //未成年人
    Autonym2 = 2,    //已成年人
};

enum EnumEntityFirstType
{
    efStudent = 0,    //最高评分学生
    efTeacher = 1,    //最高评分老师
};

enum EnumEnumEntityQuality
{
    eEntityQuality0 = 0,
    eEntityQuality1 = 1,
    eEntityQuality2 = 2,
    eEntityQuality3 = 3,
    eEntityQuality4 = 4,
    eEntityQuality5 = 5,
    eEntityQuality6 = 6,
    eEntityQuality7 = 7,
    eEntityQuality8 = 8,
    eEntityQuality9 = 9,
    eEntityQuality10 = 10,
    eEntityQuality11 = 11,
    eEntityQuality12 = 12,
    eEntityQuality13 = 13,
    eEntityQuality14 = 14,
    eEntityQuality15 = 15,
    eEntityQuality16 = 16,
    eEntityQuality17 = 17,
    eEntityQuality18 = 18,
    eEntityQuality19 = 19,
    eEntityQuality20 = 20,
};

enum EnumAllianceContribute
{
    eSmallContribute = 1,    //小赠
    eContributeTuition = 2,    //普赠
    eBigContribute = 3,    //豪赠
    eContributeDiamond = 4,    //超赠
    eMakWish = 5,    //许愿
};

enum EnumShopTypeConfig
{
    DiamondShop = 0,    //钻石
    AcademicShop = 1,    //学术
    SchoolunionShop = 2,    //校盟
    EducationShop = 3,    //进修
    HonorShop = 4,    //荣誉
    WatchShop = 5,    //观影商店
};

enum EnumAttributeConfig
{
    eTuition = 1,    //学费
    eTuitionRate = 2,    //学费增产率
    eExp = 3,    //心得
    eExpRate = 4,    //心得增产率
    eHumanity = 11,    //人文
    eHumanityRate = 12,    //人文成长率
    eMathematical = 21,    //数理
    eMathematicalRate = 22,    //数理成长率
    eArt = 31,    //艺术
    eArtRate = 32,    //艺术成长率
    eSports = 41,    //体育
    eSportsRate = 42,    //体育成长率
    eAllSubjects = 51,    //全学科
    eAllSubjectsRate = 52,    //全学科成长率
    eOffLineExpRate = 61,    //离线心得收益
    eOffLineTuitionRate = 62,    //离线学费收益
    eTeachingExpRate = 63,    //顿悟心得收益
    eTeachingTuitionRate = 64,    //顿悟学费收益
    eDatiReducTime = 65,    //缩短答题时间
    eExtraTeachingExp = 66,    //额外教学经验
    eDodge = 71,    //忽视
    eHit = 72,    //命中
    eCritical = 81,    //会心
    eCriticalDefense = 82,    //会心抵御
    eCriticalDamage = 83,    //会心伤害
    eFinishDamage = 91,    //最终伤害率
    eFinishlDefense = 92,    //最终减伤率
    eHp = 93,    //精力值
    eAttack = 94,    //攻击值
    eAffect = 95,    //精力值是否影响供给值
    ePVEHurtUp = 101,    //伤害提升
    ePVEHurtUpRate = 102,    //伤害率
    ePVEHurtDown = 111,    //伤害下降
    ePVEHurtDownRate = 112,    //减伤率
    ePVPHurtUp = 201,    //伤害提升
    ePVPHurtUpRate = 202,    //伤害率
    ePVPHurtDown = 211,    //伤害下降
    ePVPHurtDownRate = 212,    //减伤率
};

enum EnumCoursesFastConfig
{
    eCoursesLookADS = 8,
};

enum EnumPotentialConfig
{
};

enum EnumItemType
{
    mEquipment = 0,    //装备
    mMaterial = 1,    //材料
    mFragment = 2,    //碎片
    mProp = 3,    //道具
};

enum EnumNoticeConfig
{
    eTest = 103,    //<outline color = #91543C width = 2>%s</outline>
    eMailReadFail = 104,    //<outline color = #91543C width = 2>读取失败</outline>
    eMailReceiveFail = 105,    //<outline color = #91543C width = 2>领取失败</outline>
    eMailDeletedFail = 106,    //<outline color = #91543C width = 2>删除失败</outline>
    eMailDeletedFullFail = 107,    //<outline color = #91543C width = 2>储藏室已满,领取失败</outline>
    eArgsError = 108,    //<outline color = #91543C width = 2>请求参数错误</outline>
    eClassRoomRepeatSeat = 109,    //<outline color = #91543C width = 2>座位上已有学生</outline>
    eArgsRepeat = 110,    //<outline color = #91543C width = 2>参数重复</outline>
    eConfigError = 111,    //<outline color = #91543C width = 2>配置不存在</outline>
    eLevelNotEnough = 112,    //<outline color = #91543C width = 2>请先将学校等级提升到<color=#ff5f5f><outline color=#3a3030>%s</outline></color>级，再来试试吧~</outline>
    eTuitionNotEnough = 113,    //<outline color = #91543C width = 2>您的学费不足了呢~</outline>
    eItemNotEnough = 114,    //<outline color = #91543C width = 2>您的道具不足了呢~</outline>
    eLevelLimit = 115,    //<outline color = #91543C width = 2>提升品质后再来试试吧！</outline>
    eLevelLimitMax = 116,    //<outline color = #91543C width = 2>请先将学校等级提升到<color=#ff5f5f><outline color=#3a3030>%s</outline></color>级，再来试试吧~</outline>
    eExpNotEnough = 117,    //<outline color = #91543C width = 2>您的道具不足了呢~</outline>
    eLevelMax = 118,    //<outline color = #91543C width = 2>好像已经满级了</outline>
    ePatchNotEnough = 119,    //<outline color = #91543C width = 2>您的碎片不够了呢~</outline>
    eSystemError = 120,    //<outline color = #91543C width = 2>系统错误</outline>
    eCultureFull = 121,    //<outline color = #91543C width = 2>该学生培养次数已满，去试试其他学生吧~</outline>
    eDiamondsNotEnough = 122,    //<outline color = #91543C width = 2>您的钻石不足了呢</outline>
    eEquipMasterLimit = 123,    //<outline color = #91543C width = 2>这件学习用品精通已达上限，去试试其他学习用品吧~</outline>
    eTaskNoExist = 124,    //<outline color = #91543C width = 2>好像找不到这条任务了呢</outline>
    eTaskNoFinish = 125,    //<outline color = #91543C width = 2>该任务还未完成，请先完成任务再来试试哦~</outline>
    eChalkNotEnough = 126,    //<outline color = #91543C width = 2>您的加速答题时间不足了~</outline>
    eChalkLimit = 127,    //<outline color = #91543C width = 2>您今天已经无法购买灯泡了哦~</outline>
    eSUNotEnough = 128,    //<outline color = #91543C width = 2>您的万能碎片不足了呢~</outline>
    eLimitError = 129,    //<outline color = #91543C width = 2>好像已到达上限了</outline>
    eTravelError = 130,    //<outline color = #91543C width = 2>您可以去写游记了哦~</outline>
    eOilDrumNotEnough = 131,    //<outline color = #91543C width = 2>您的油桶库存好像已经空了~</outline>
    eShopNotEnough = 132,    //<outline color = #91543C width = 2>这件商品已经售罄了~</outline>
    eSystemNotOpen = 133,    //<outline color = #91543C width = 2>%s系统尚未开启，请耐心等待哦~</outline>
    eNotPlayer = 134,    //<outline color = #91543C width = 2>实在找不到该玩家呢~</outline>
    eFriendFull = 135,    //<outline color = #91543C width = 2>您的好友列表已经满了~</outline>
    eOtherFriendFull = 136,    //<outline color = #91543C width = 2>对方的好友列表已经满了~</outline>
    eOtherApply = 137,    //<outline color = #91543C width = 2>对方已收到消息，请耐心等待~</outline>
    eAlreadyFriend = 138,    //<outline color = #91543C width = 2>你们已是好友了</outline>
    eOutingError = 139,    //<outline color = #91543C width = 2>正在外出</outline>
    eSearchError = 140,    //<outline color = #91543C width = 2>无法查找自己哦~</outline>
    eMatchingError = 141,    //<outline color = #91543C width = 2>无法使用该学科的学习用品哦</outline>
    eNoFriendError = 142,    //<outline color = #91543C width = 2>该玩家还不是您的好友哦</outline>
    eHasRented = 143,    //<outline color = #91543C width = 2>您已租用过该好友了，去试试其他好友吧~</outline>
    eFriendlyNotEnough = 144,    //<outline color = #91543C width = 2>您的友情点不足了呢</outline>
    eAwardAlready = 145,    //<outline color = #91543C width = 2>该奖励无法重复领取哦</outline>
    eResearchDesktopNoexist = 146,    //<outline color = #91543C width = 2>教研席位已满</outline>
    eBattleStudent = 147,    //<outline color = #91543C width = 2>请先上阵学生</outline>
    eNotEnough = 148,    //<outline color = #91543C width = 2>%s不足</outline>
    eRevengeNotEnough = 149,    //<outline color = #91543C width = 2>次数不足，明日再战</outline>
    eTeachingNotEnough = 150,    //<outline color = #91543C width = 2>您的教研币不足了呢</outline>
    eNameRepeat = 151,    //<outline color = #91543C width = 2>该名称已有玩家使用了</outline>
    eChallengeSelf = 152,    //<outline color = #91543C width = 2>无法挑战自己哦</outline>
    eMatchedFail = 153,    //<outline color = #91543C width = 2>匹配系统好像出了点问题，再试试吧</outline>
    eCanNotChanllenge = 154,    //<outline color = #91543C width = 2>您现在无法挑战任何人哦</outline>
    eActivated = 155,    //<outline color = #91543C width = 2>已激活</outline>
    eConditionNotSatisfied = 156,    //<outline color = #91543C width = 2>暂未达成条件</outline>
    eNeedSeating = 157,    //<outline color = #91543C width = 2>请编排座位后再来试试吧</outline>
    eAllianceNoExist = 158,    //<outline color = #91543C width = 2>未找到该校盟哦</outline>
    eStudentNoExist = 159,    //<outline color = #91543C width = 2>未找到该学生</outline>
    eLotteryNotEnough = 160,    //<outline color = #91543C width = 2>您的直播券储备不足哦</outline>
    eAllianceNotSame = 161,    //<outline color = #91543C width = 2>您和该玩家不属于同一校盟</outline>
    eTeacherNoExist = 162,    //<outline color = #91543C width = 2>未找到该老师</outline>
    eTeacherAlreadyLecture = 163,    //<outline color = #91543C width = 2>该老师今天已经开启过讲座，正在休息中</outline>
    eWuHuaGuoNotEnough = 164,    //<outline color = #91543C width = 2>您的无花果不足了呢</outline>
    eChapterPass = 165,    //<outline color = #91543C width = 2>当前章节已通关</outline>
    eLayerPass = 166,    //<outline color = #91543C width = 2>当前关卡已通关</outline>
    eFubenPass = 167,    //<outline color = #91543C width = 2>副本已完全通关</outline>
    eAlreadyJoinAlliance = 168,    //<outline color = #91543C width = 2>已向会长发送申请，请耐心等待哦</outline>
    eNotIsAllianceLeader = 169,    //<outline color = #91543C width = 2>只有盟主才能进行该操作哦</outline>
    eApplyJoinRecordNoExist = 170,    //<outline color = #91543C width = 2>没有查找到该条请求记录哦</outline>
    eJoinAllianceNoOneDay = 171,    //<outline color = #91543C width = 2>加入校盟未满一天</outline>
    eSayHelloThreeTime = 172,    //<outline color = #91543C width = 2>今日打招呼次数已用完</outline>
    eLectureFinish = 173,    //<outline color = #91543C width = 2>讲座已结束</outline>
    eAllianceLevelNotEnough = 174,    //<outline color = #91543C width = 2>校盟等级不足</outline>
    eDingHao = 175,    //<outline color = #91543C width = 2>你的账号已在其他地方登陆</outline>
    ePreserve = 176,    //<outline color = #91543C width = 2>游戏正在维护中</outline>
    eEquipLevelUpSuccess = 177,    //强化成功，等级+1
    eEquipLevelUpCrit = 178,    //强化暴击，等级+2
    eEquipLevelUpSuperCrit = 179,    //强化大暴击，等级+3
    eEquipMasterUpSuccess = 180,    //精通成功
    eRechargeFail = 181,    //<outline color = #91543C width = 2>充值失败</outline>
    eTimesNotEnough = 182,    //<outline color = #91543C width = 2>剩余次数不足</outline>
    eNoExistAllianceMember = 183,    //<outline color = #91543C width = 2>未找到该校盟成员</outline>
    eEquipPosUnlock = 184,    //%s已解锁
    eFetterOutOfDate = 185,    //%s租借到期已返还给%s
    eTodayAlreadyContribute = 186,    //今日已捐赠
    eNoFetterActorToSelect = 187,    //没有羁绊者
    eNoAssistPos = 188,    //噫呜呜~人满了呢
    eCodeError0 = 189,    //<outline color = #91543C width = 2>%s</outline>
    eCodeError1 = 190,    //<outline color = #91543C width = 2>请输入有效的激活码</outline>
    eCodeError2 = 191,    //<outline color = #91543C width = 2>领取失败,激活码错误</outline>
    eCodeError3 = 192,    //<outline color = #91543C width = 2>领取失败,激活码已失效</outline>
    eCodeError4 = 193,    //<outline color = #91543C width = 2>领取失败,您已领取过该奖励</outline>
    eCodeError5 = 194,    //<outline color = #91543C width = 2>领取失败,该记号码已被使用</outline>
    eServerError = 195,    //服务器维护, 请选择其他游戏区重试
    eNetError = 196,    //网络错误, 请检查网络
    eAccountCheckFail = 197,    //帐号检测失败
    eAutonymError0 = 198,    //未实名限制
    eAutonymError1 = 199,    //未成年限制
    eStudentAlreadyListenLecture = 200,    //该学生今日已听讲
    eStudentNowListening = 201,    //该学生正在听讲
    eAllianceNameRepeat = 202,    //校盟名称重复
    ePleaseLogin = 203,    //<outline color = #91543C width = 2>请重新登陆</outline>
    eAllianceNameMoreLong = 204,    //校盟名称不能超过10个文字
    eFengHao = 205,    //<outline color = #91543C width = 2>很抱歉你账号被封了</outline>
    eEquipLevelLock = 206,    //强化%s级解锁
    eEquipMasterLock = 207,    //精通%s级解锁
    eFetterActiveSuccess = 208,    //成功激活
    eFetterRentSuccess = 209,    //成功借助
    ePlayerAlreadyHasAlliance = 210,    //对方已加入其他校盟
    eLectruePosExistStudeng = 211,    //听讲位置上已经坐上了学生
    eAutoEquipBusy = 212,    //正在一键换装
    eAutoEquipNonsense = 213,    //学习用品已达到最强了
    eJoinAllianceSecondDay = 214,    //次日才能重新加入校盟
    eAtupEquipNoEquip = 215,    //没有可使用的学习用品了
    eTodayAlreadyWishFinish = 216,    //今日许愿次数已用完
    eAcademicExtraError1 = 217,    //<outline color = #91543C width = 2>请先通关该章节额外boss</outline>
    eAcademicExtraError2 = 218,    //<outline color = #91543C width = 2>不可扫荡</outline>
    eAcademicExtraError3 = 219,    //<outline color = #91543C width = 2>请先通关%d星</outline>
    eAcademicExtraError4 = 220,    //<outline color = #91543C width = 2>没有剩余次数</outline>
    eAcademicExtraError5 = 221,    //<outline color = #91543C width = 2>请先通关精英关卡</outline>
    eAcademicExtraError6 = 222,    //<outline color = #91543C width = 2>没有扫荡次数</outline>
    eAcademicExtraError7 = 223,    //<outline color = #91543C width = 2>请先通关该章节额外boss</outline>
    eAcademicExtraError8 = 224,    //<outline color = #91543C width = 2>请先通关该章节额外boss</outline>
    eActivityError0 = 225,    //%s活动尚未开启
    eExtendError0 = 226,    //已解锁
    eExtendError1 = 227,    //未解锁
    eExtendError2 = 228,    //请订阅
    eNoUnlockChest = 229,    //未解锁
    eNoexistChest = 230,    //宝箱不存在
    eChestNoOverTime = 231,    //宝箱开启时间未达成
    eSchoolWorkNoExist = 232,    //校务不存在
    eChestAlreadyPlace = 233,    //宝箱已经放置
    eSchoolAllianceFull = 234,    //校盟人数已满
    eAlreadyStartLiveClass = 235,    //你已开启了1场直播课
    eLiveClassGiftNoexist = 236,    //礼物不存在
    eLiveClassGiftNoEnough = 237,    //礼物不足
    eLiveClassNoExist = 238,    //直播课不存在
    eLiveClassNoFinish = 239,    //直播课还未结束
    eNoLookADSFinish = 240,    //广告视频没有看完
    eAlreadyFinishDayCourses = 241,    //今天已完成全部进修
    eNowCoursesTaskFull = 242,    //同时进修任务已满
    eBagFullError = 243,    //存储库已满
    eBagConfigError = 244,    //配置错误
    eBagUseLimitError = 245,    //<outline color = #91543C width = 2>超过使用上限</outline>
    eAccountNoInWhiteList = 246,    //帐号不在白单内
    eAccountInBlackList = 247,    //帐号在黑名单内
    eLogicServerBuy = 248,    //服务器太过火爆
    eNeedLineUpLogin = 249,    //登陆玩家太多,需要排队等候
    eLogicDBMaintain = 250,    //游戏服务器维护中…
    eNoRechargeRMB = 251,    //还未充值
    eRechargeSuccess = 252,    //%s充值成功!
};

enum EnumPropertyConfig
{
    ePlayerTuition = 0,    //学费是基础货币，【学校升级】和【扩建】都需要它
    ePlayerExperience = 1,    //容纳了多位老师的心得体会，主要用于【学生升级】
    ePlayerDiamonds = 2,    //很重要的货币，可以买各种好东西，主要来源就是氪金，你懂的
    ePlayerFriendly = 3,    //通过【好友赠送】可以获得友情点，借助【外援】时需要消耗友情点
    ePlayerScience = 4,    //在【学术研讨】中通过验证学生的成长，而获得的奖励，可以在【学术商店】进行消费
    ePlayerOilDrumCount = 5,    //满满的一桶汽油，是【春游】的必需品，可以在【荣誉商店】进行购买
    ePlayerSchoolUnion = 6,    //主要来自【校盟捐献】的奖励，可以在【校盟商店】购买一些学生和老师的碎片
    ePlayerHonour = 7,    //【单人竞赛】挑战后获得的荣誉点，可以在【荣誉商店】用来购买一些稀有道具
    ePlayerTeaching = 8,    //【单人竞赛】后获得的奖励，让老师们在【教研室】进行教研，需要支付他们教研币
    ePlayerEngage = 9,    //在【进修班】里获得的银币，可以用它在【进修商店】里购买道具
    ePlayerContribute = 10,    //校盟声望用于【校盟升级】，每天进行【校盟捐献】可获得声望
    ePlayerStrengExp = 11,    //学习知识具象化的产物，用于强化学习用品。
    ePlayerVipLv = 20,    //VIP等级
    ePlayerOnLine = 21,    //玩家本次登陆时间
    ePlayerLoginOut = 23,    //玩家上次离线时间
    ePlayerOffLineTime = 24,    //离线时间单位为分钟
    ePlayerLevel = 25,    //学校等级
    ePlayerChalkFree = 26,    //免费的粉笔次数
    ePlayerChalkCount = 27,    //购买的粉笔次数
    ePlayerChalkLimit = 28,    //粉笔已购买次数
    ePlayerQuicken = 29,    //快速教学次数
    ePlayerScore = 30,    //总成绩
    ePlayerEquipLevelRate = 31,    //装备强化大暴击概率
    ePlayerEquipLevelExp = 32,    //装备强化未消耗经验
    ePlayerIcon = 33,    //头像id
    ePlayerIconFrame = 34,    //头像框id
    ePlayerSystemFlag = 35,    //系统功能解锁字段
    ePlayerCreateTime = 36,    //创建时间
    ePlayerRecharge = 37,    //充值总金额
    ePlayerCostDiamonds = 38,    //消费钻石记录
    ePlayerIntegralDiscuess = 39,    //学术研讨积分
    ePlayerIntegralCompetition = 40,    //个人竞赛积分
    ePlayerSendFriendly = 41,    //累计赠送友情点
    ePlayerResearchDesktop = 42,    //教研室的桌子数量
    ePlayerSchoolUnionId = 43,    //校盟ID
    ePlayerLevelExp = 45,    //学校升级所需任务列表
    ePlayerSpringOutingCount = 46,    //春游事件累计次数
    ePlayerBossDamage = 47,    //限时boss累计伤害
    ePlayerVipExp = 48,    //vip经验
    ePlayerLeagueLayer1 = 49,    //学科联赛人文
    ePlayerLeagueLayer2 = 50,    //学科联赛数理
    ePlayerLeagueLayer3 = 51,    //学科联赛艺术
    ePlayerLeagueLayer4 = 52,    //学科联赛体育
    ePlayerLeagueLayer5 = 53,    //学科联赛综合
    ePlayerAllianceJob = 54,    //校盟职位
    ePlayerAllianceDamage = 55,    //校盟副本累计伤害
    ePlayerVipAwardIndex = 56,    //vip奖励二进制
    ePlayerJoinAllianceTime = 57,    //加入到校盟的时间
    ePlayerCoursesLevel = 59,    //进修班等级
    ePlayerStudentUpCount = 60,    //学生升级次数
    ePlayerTodayOnlineTime = 61,    //统计玩家当天时间
    ePlayerTotalSelfCoursesTaskCount = 62,    //进修班升级累计自己的任务数
    ePlayerAllianceSayHelloCount = 64,    //校盟打招呼当天次数(最后时间不是当天则为0)
    ePlayerSayHelloLastTime = 65,    //校盟打招呼最后时间(用于统计当天打招呼次数)
    ePlayerDayCount = 66,    //登陆天数
    ePlayerContributeLastTime = 67,    //校盟捐款的最后时间(用于限制一天只能捐款一次)
    ePlayerClassScore = 68,    //班级分数
    ePlayerStudentScore = 69,    //学生分数
    ePlayerDrawCount = 70,    //抽卡累计次数
    ePlayerRechargeDiamond = 71,    //累计充值钻石
    ePlayerCultureCount = 72,    //学生培养累计次数
    ePlayerSex = 73,    //性别
    ePlayerCompetitionTime = 76,    //个人竞赛赛季结束时间
    ePlayerDiscussTime = 77,    //学术研讨赛季结束时间
    ePlayerAcademicLayer = 78,    //学府之路最高层
    ePlayerBestStudentQuality = 79,    //废弃
    ePlayerForbiddenChatTime = 80,    //被禁言解放时间(0表示无禁言, -1永久禁言)
    ePlayerQuitAllianceTime = 81,
    ePlayerWishReward = 82,    //许愿获取的奖励
    ePlayerTeachingExtra = 83,    //狂欢额外收益值
    ePlayerSecretaryExp = 84,    //秘书好感度,提升秘书等级
    ePlayerSecretaryLevel = 85,    //秘书等级
    ePlayerPower = 86,    //体力
    ePlayerPowerTime = 87,    //体力更新时间
    ePlayerPowerCount = 88,    //购买体力的上限次数
    mSchoolAlliaExp = 89,    //校盟的经验,可以升级校盟
    ePlayerDeblockingTime = 90,    //解封时间
    ePlayerGMLevel = 91,    //gm等级
    ePlayerRealRecharge = 92,    //真实充值
    ePlayerbReceiveFirstRechargeReward = 93,    //是否领取了首充奖励
    ePlayerDayAddTuition = 204,    //每日获得学费
    ePlayerDaySubTuition = 205,    //每日消耗学费
    ePlayerDayAddExperience = 206,    //每日获得心得
    ePlayerDaySubExperience = 207,    //每日消耗心得
    ePlayerDayAddDiamonds = 208,    //每日充值钻石
    ePlayerDaySubDiamonds = 209,    //每日消费钻石
    ePlayerDayDonateTuition = 210,    //每日捐献学费
    ePlayerDayAddBossChallenge = 211,    //每日挑战限时boss次数
    ePlayerDaySpringOut = 212,    //每日春游次数
    ePlayerDayTeachingCount = 213,    //每日获得教研币
    ePlayerDayAllianceFubenCount = 214,    //每日校盟副本挑战次数
    ePlayerDayCompetitionCount = 215,    //每日个人竞赛挑战次数
    ePlayerDayDiscussCount = 216,    //每日学术研讨挑战次数
    ePlayerDayLeagueCount = 217,    //每日学科联赛挑战次数
    ePlayerContributeTimeCount = 218,    //每日校盟捐款次数
    ePlayerDayWishCount = 219,    //每日许愿的次数
    ePlayerDayDaTiCount = 220,    //每日累计答题次数(用于统计答题多次后可以使用剩余的)
    ePlayerDayUnmannedDroneCount = 221,
    ePlayerDayFinishRefresherCount = 222,    //完成进修任务的数量,用于限制每日一次
    ePlayerDayTaskActiveValue = 223,    //每日任务活跃度
    ePlayerDayActiveFinishInfo = 224,    //每日活跃度领取信息(二进制开关)
    ePlayerDayRecharge = 225,    //每日累计充值
    ePlayerFriendlyCount = 400,    //累计获得友情点
    ePlayerScienceCount = 401,    //累计获得学术币
    ePlayerOilDrumNum = 402,    //累计获得油桶
    ePlayerSchoolUnionCount = 403,    //累计获得校盟币
    ePlayerHonourCount = 404,    //累计获得荣誉点
    ePlayerTeachingCount = 405,    //累计获得教研币
    ePlayerEngageCount = 406,    //累计获得进修币
    ePlayerContributeCount = 407,    //累计获得声望贡献
    ePlayerBossChallengeCount = 408,    //累计挑战限时boss次数
    ePlayerContributeTuition = 409,    //累计捐献学费
    ePlayerExclusiveEquipCount = 410,    //累计学科资料(专属学习用品)
    ePlayerAllianceFubenCount = 411,    //累计校盟副本挑战次数
    ePlayerCompetitionCount = 412,    //累计个人竞赛次数
    ePlayerRevengeCompetitionCount = 413,    //累计个人复赛竞赛次数
    ePlayerDiscussCount = 414,    //累计学术研讨挑战次数
    ePlayerLeagueCount = 415,    //累计学科联赛挑战次数
    ePlayerFetterCount = 416,    //累计羁绊关系数量
    ePlayerServerLoginCount = 417,    //累计7日签到
    ePlayerEquipLvUp = 420,    //累计装备强化次数
    ePlayerShopBuyCount = 421,    //累计购物次数
    ePlayerTrashCount = 422,    //累计垃圾数量
    ePlayerTotalAllianceContributeCount = 423,    //统计校盟捐献次数
    ePlayerSpringCount = 424,    //春游累计次数
    ePlayerSpringLeiFengCount = 425,    //春游学雷锋累计次数
};

enum EnumActorType
{
    eStudent = 1,    //学生
    eTeacher = 2,    //老师
    eUav = 3,    //无人机
    eMgr = 4,    //管理者
    eNPC = 5,    //NPC
};

enum EnumCoursesType
{
    eCoursesTeam = 1,    //团队任务(一直一个)
    eCoursesDay1 = 2,    //今日6分钟(固定任务)
    eCoursesDay2 = 3,    //今日30分钟(固定任务)
    eCoursesDay3 = 4,    //今日60分钟(固定任务)
    eCoursesTime1 = 5,    //时段1
    eCoursesTime2 = 6,    //时段2
    eCoursesTypeMax = 7,    //类型最大值
};

enum EnumGuideInfo
{
    mMainWindowGuide = 0,    //主界面新手引导
    mDrawCardWindowGuide = 1,    //抽卡新手引导
    mDrawReawardWindowGuide = 2,    //点击翻卡引导
    mSeatChartWindowGuide = 3,    //座位表新手引导
    mExtensionWindowGuide = 4,    //扩建新手引导
    mExtensionContentGuide = 5,    //扩建新手引导
    mSchoolUplevelWindowGuide = 6,    //学校升级新手引导
    mStudentWindowGuide = 11,    //学生界面新手引导
    mStudentDetailWindowGuide = 12,    //学生升级新手引导
    mLaboratoryWindowGuide = 13,    //教研室新手引导
    mLaboratoryInfoWindowGuide = 14,    //教研新手引导
    mTeacherWindowGuide = 15,    //老师引导
    mTeacherDetailWindowGuide = 16,    //老师引导
    mSubLeagueStudentWindowGuide = 17,    //学府之路引导
    mIntanceLevelWindowGuide = 18,    //学府之路引导
};

enum EnumActivityType
{
    eActNotOpen = 0,    //活动未开启
    eActOpening = 1,    //活动开启中
    eActAwait = 2,    //活动等待
    eActEnd = 3,    //活动结束
};

enum EnumUnlockConfig
{
    eResearchOneGet = 0,    //教研室一键领取功能
    eResearchOneTeaching = 1,    //教研室一键教研功能
    eRefresherVIP = 2,    //进修班一键领取
    eStudentUpgrade = 3,    //学生升品
    eTeacherUpgrade = 4,    //老师升品
    eStudentDevelop = 5,    //学生学科培养
    eStudentCompose = 6,    //学科培养道具合成
    eDailyTask = 7,    //每日任务
    eTotalTask = 8,    //累计任务
    eAssist = 9,    //外援系统
    eBattleSkip = 10,    //战斗跳过动画功能
    eEquipUpgrade = 11,    //装备强化
    eEquipMaster = 12,    //装备精通
    eLeagueHumanity = 13,    //学科联赛人文
    eLeagueMathematical = 14,    //学科联赛数理
    eLeagueArt = 15,    //学科联赛艺术
    eLeagueSport = 16,    //学科联赛体育
    eClassroomCarnival = 17,    //教室加速答题狂欢
    eShopDiamond = 18,    //小卖部1_钻石商店
    eShopScience = 19,    //小卖部2_学术商店（对应系统的开放等级）
    eShopAlliance = 20,    //小卖部3_校盟商店（对应系统的开放等级）
    eShopFurther = 21,    //小卖部4_进修商店（对应系统的开放等级）
    eShopHonor = 22,    //小卖部5_荣誉商店（对应系统的开放等级）
    eSecretaryChest = 23,    //秘书宝箱功能
};

enum EnumEnumAward
{
    eAwardItem = 0,    //道具类型: id=道具id, type=0, count=道具数量
    eAwardStudent = 1,    //学生卡类型: id=学生id, type=1
    eAwardTeacher = 2,    //老师卡类型: id=老师id, type=2
    eAwardProperty = 3,    //添加玩家数值: id=属性id, type=3, count=添加值
    eAwardTeacherExp = 4,    //添加玩家数值: id=老师id, type=4, count=教学经验
    eAwardSecretaryDress = 5,    //秘书装饰
    eAwardAlliancePrestige = 6,    //校盟声望
};

enum EnumClassParam
{
    eClassTime = 0,    //上课时长
    eChangeNameCost = 1,    //修改学校名称的费用
};

enum EnumResearchFastConfig
{
    eLookADS = 7,
};

enum EnumSchoolAlliance
{
    eAllianceNone = -1,
    eAllianceID = 0,    //主键
    eAllianceName = 1,    //校盟名称
    eAlliancePlayerList = 2,    //玩家列表
    eAllianceIcon = 3,    //校盟徽章
    eAllianceLevel = 4,    //等级
    eAlliancePrestige = 5,    //声望
    eAllianceLeader = 6,    //会长
    eAlliancelDues = 7,    //会费
    eAlliancelFuben = 8,    //校盟副本
    eAllianceContribute = 9,    //捐赠记录
    eAllianceApplyJion = 10,    //请求加入列表
    eLectureTaskList = 11,    //名师讲座列表
    eLectureFund = 12,    //奖学金
    eAllianceNotice = 13,    //校盟公告
    eContributDiamonds = 14,    //捐献钻石总数
    eWishDiamonds = 15,    //许愿钻石总数
    eWishList = 16,    //许愿玩家ID列表
    eScore = 17,    //校盟积分
    eExtend1 = 18,    //扩展数据1
    eExtend2 = 19,    //扩展数据2
    eExtend3 = 20,    //扩展数据3
    eExtend4 = 21,    //扩展数据4
};

enum EnumUnlockLimit
{
    eSchoolLevel = 1,    //学校等级
};

