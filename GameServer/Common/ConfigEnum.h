//Auto generate from ExcelTool 2.00.00
//
#pragma once

enum EnumGender
{
    eGenderNone = 0,
    eGenderMan = 1,    //男
    eGenderWoman = 2,    //女
};

enum EnumPower
{
    ePowerPublic = 0,    //公用权限
    ePowerNormalUser = 1,    //正常用户
    ePowerManager1 = 2,    //1级管理员
    ePowerManager2 = 3,    //2级管理员
    ePowerManager3 = 4,    //3级管理员
};

enum EnumErrorCode
{
    eError_None = 0,    //无
    eError_Unkwon = 1,    //未知
    eError_ProgramError = 2,    //代码错误
    eError_ExistAccount = 3,    //帐号已存在
    eError_AccountError = 4,    //帐号不符合要求
    eError_PasswordError = 5,    //密码错误
    eError_Resource_NoExist = 6,    //资源不存在
    eError_Resource_TooLarge = 7,    //资源太大了
    eError_ResourceCheckFail = 8,    //资源验证失败
    eError_ActorNoexsit = 9,    //Actor不存在
    eError_RecordNoExist = 10,    //记录不存在
    eError_GoodsNoExist = 11,    //商品不存在
    eError_TableNoExist = 12,    //表格不存在
    eError_RecordExist = 13,    //记录已经存在
    eError_CreateRecordFail = 14,    //记录新建失败
    eError_NetError = 15,    //网络错误
    eError_GiftNoExist = 16,    //礼物类型错误
    eError_GiftCountError = 17,    //礼物数量不正确
    eError_GoldNotEnough = 18,    //金币不够
    eError_PowerLevelTooLow = 19,    //权限不够
    eError_PhoneNumberError = 20,    //电话号码不正确
    eError_CheckCodeSendFail = 21,    //验证码发送失败
    eError_CheckCodeSendOk = 22,    //验证码发送成功
    eError_CheckCodeError = 23,    //验证码错误
    eError_CheckCodeResendTimeError = 24,    //稍候再次重新发送验证码
    eError_TJUserNoexistOrPhoneError = 25,    //推荐人不存在,检验号码是否正确
    eError_TJRewardAlreadyGive = 26,    //推荐奖励已经领取
    eError_ZipFail = 27,    //压缩数据失败
};

enum EnumUserState
{
    eUserStateInit = 0,    //初始状态
    eUserStateNormal = 1,    //正常
    eUserStateNeedCheck = 2,    //需要检验
    eUserStateNoPass = 3,    //检验不通过
    eUserStateLock = 4,    //禁停
};

enum EnumFunction
{
    eFunctionNone = 0,    //空功能
    eFunctionGift = 1,    //礼物
    eFunctionTalk = 2,    //聊天
};

