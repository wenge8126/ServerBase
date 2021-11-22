#pragma once

#include "ServerEvent.h"

//-------------------------------------------------------------------------
// 用来处理工具辅助功能, 比如, 打包工具, 使用JSzip 压缩配置数据包
// CoCos Web 连接 工具角色 WebLogin, 打包工具通过Gate连接到WebLogin, 
// 然后, 由WebLogin请求Web返回结果, 回复给工具
//-------------------------------------------------------------------------

// 请求JSZIP压缩数据
class LC_RequestJSZipData : Logic::tServerEvent
{
public:
	virtual bool _DoEvent() override
	{
		WaitTime(20);
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent) override
	{
		NOTE_LOG("JS zip result > %s\r\n%s", respEvent["RESULT"].c_str(), respEvent->GetData().dump().c_str());

	}
};