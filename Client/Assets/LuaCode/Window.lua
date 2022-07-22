
CS.LOG.log("Load Init window lua"); 

local util = require 'util';

GameObject = CS.UnityEngine.GameObject;


EventCenter = CS.Logic.EventCenter;
LuaEventFactory = CS.LuaEventFactory;


NiceTable = CS.Logic.NiceTable;
DataRecord = CS.Logic.DataRecord;
TableIt = CS.Logic.TableIt;


TableManager = CS.TableManager;
DataCenter = CS.DataCenter;

GameCommon = CS.GameCommon;
UIManager = CS.UIManager;
ObjectManager = CS.ObjectManager;

LuaEvent = CS.LuaEvent;

LuaWindowFactory = CS.LuaWindowFactory;
Window = CS.Window;
Net = CS.Net;
LOG = CS.LOG;
--AI = luanet.import_type("AI");

--OpenFunction = luanet.import_type("OpenFunction");
CommonParam = CS.CommonParam;
Time = CS.UnityEngine.Time;

LuaProtoMsg = CS.LuaProtoMsg;

LuaFunctionEventFactory = CS.LuaFunctionEventFactory;
LuaButtonEventFactory = CS.LuaButtonEventFactory; 
--
function print(info)
	LOG.log("LUA:"..info);
end
--

function _PrintTable(t, k)	
	local strInfo = "____________ "..k.." __________\r\n";
	for key, value in pairs(t) do      
		if (type(value)=="table") then
			strInfo = strInfo.._PrintTable(value, key);
		else
	    	strInfo = strInfo..key.." : "..tostring(value).."\r\n";
		end	
	end  
	strInfo = strInfo .. "_______________________________\r\n"
	return strInfo;
end

function exPrintTable(t)	
	if (type(t)~="table") then
		print(string(t));
	else	
		print(_PrintTable(t, ""));
	end	
end

function PrintTable ( t )  
	local strInfo = "";
    local print_r_cache={}
    local function sub_print_r(t,indent)
		local info = "";
        if (print_r_cache[tostring(t)]) then
            info = info..(indent.."*"..tostring(t))
			info = info.."\r\n";
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        info = info..(indent.."["..pos.."] => "..tostring(t).." {")
						info = info.."\r\n";
                        info = info.. sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
						info = info.."\r\n";
                        info = info..(indent..string.rep(" ",string.len(pos)+6).."}")
						info = info.."\r\n";
                    elseif (type(val)=="string") then
                        info = info..(indent.."["..pos..'] => "'..val..'"')
						info = info.."\r\n";
                    else
                        info = info..(indent.."["..pos.."] => "..tostring(val))
						info = info.."\r\n";
                    end
                end
            else
                info= info..(indent..tostring(t))
				info = info.."\r\n";
            end
        end
		return info;
    end
    if (type(t)=="table") then
        strInfo = strInfo..(tostring(t).." {").."\r\n";
        strInfo = strInfo.. sub_print_r(t,"  ");
        strInfo = strInfo..("}")
    else
        strInfo = strInfo.. sub_print_r(t,"  ");
    end
    print(strInfo);
end
----------------------------------------------------------------------------------


base_window = class(base_event);

function base_window:ctor()	
	--print("base_window ctor")

end

function base_window:getPrefab()
	return "LUA_NONE";
end
 
function base_window:Init(win)	
	--print("WARN: Do nothing when Init window");
end

function base_window:InitUI(win)	
	--print("WARN: Do nothing when Init window");
	return true;
end
 
function base_window:Open(win)	
	--print("WARN: Respone nothing, when window"); 
	--win:Open(null, null);
end

function base_window:OnOpen(win)
end

function base_window:Close()
	self.win:Close();
end

function base_window:OnEvent(win, evt)
	win:OnEvent(evt);
end	

function base_window:GetSub(subName)
	self.win:GetSub(subName);
end

function base_window:SetText(lableName, text)
	self.win:SetText(lableName, text);
end
function base_window:SetUIText( parentName , lableName,  text)
	self.win:SetUIText( parentName , lableName,  text);
end
function base_window:SetUISprite( parentName , spriteObject,  atlas,  atlasSpriteName) 
	self.win:SetUISprite( parentName , spriteObject,  atlas,  atlasSpriteName) ;
end

function base_window:Register(windowEventName, windowFunName)
	local LuaButtonEventFactory = CS.LuaButtonEventFactory; 
	EventCenter.StaticRegister(windowEventName, LuaButtonEventFactory(self, windowFunName));
end

function base_window:RegisterFunction(windowEventName, windowFun)
	local LuaFunctionEventFactory = CS.LuaFunctionEventFactory;
	EventCenter.StaticRegister(windowEventName, LuaFunctionEventFactory(self, windowFun));
end

function base_window : RegisterButton(buttonName, windowFunName)
	local winName = self:getPrefab();
	local buttonEventName = "Button_"+winName+"_"+buttonName;
	EventCenter.StaticRegister(buttonEventName, LuaButtonEventFactory(self, windowFunName));
end

function base_window:GetText(lableName)
	return self.win:GetText(lableName);
end

function base_window:GetInputText(inputName)
	return self.win:GetInputText(inputName);
end

function base_window:GetMGameObjUI()
	return self.win:GetMGameObjUI();
end
function base_window:SetVisible(subObject,bVisible)
	self.win:SetVisible(subObject,bVisible);
end
function base_window:SetUIVisiable(parentName,uiName,bVisible)
	self.win:SetUIVisiable(parentName,uiName,bVisible);
end
function base_window:GetUIGrid( parentObj,  gridName)
	return self.win:GetUIGrid( parentObj,  gridName);
end
function base_window:GetUITweenPosition( parentName,  tweenName)
	return self.win:GetUITweenPosition( parentName,  tweenName);
end
function base_window:GetUITable(tableName)
	return self.win:GetUITable(tableName);
end
function base_window:GetUISprite( parentName,  spriteName)
	return self.win:GetUISprite( parentName,  spriteName);
end
function base_window:SetBtnData( parentName,  buttonName,  dataKey,  data)
	return self.win:SetBtnData( parentName,  buttonName,  dataKey,  data);
end

function base_window : GetWindowName()
	return self.win:GetEventName();
end

function base_window : CheckRegisterButton(buttonName)	
	local evtName = "Button_"..self:getPrefab().."_"..buttonName;
	if (not EventCenter.Instance:ExistEventFactory(evtName)) then
			if ( self[buttonName]~=nil) then
				EventCenter.StaticRegister(evtName, LuaButtonEventFactory(self, buttonName));
			else
				LOG.logError(self:GetWindowName() .. " : No exist button function "..buttonName);
			end                        
	end                
end

--
base_componet = class();

function base_componet:ctor()
end

function base_componet:Start(cComponet)
	--print("Run lua Start()");
end

--Need Update then use
--function base_componet:Update(cComponet)
	-- return true; -- return false then if stop update
--end

function RegisterWindow(winName)
	LuaWindowFactory(winName);
end	

function RegisterEvent(luaEventName)
	EventCenter.StaticRegister(luaEventName, LuaEventFactory(luaEventName));
end

function Start(eventName)
    local evt = EventCenter.Start(eventName);
    if (evt~=nil) then
	    return evt.mLuaTable;
    end
    LOG.log("No exist event : "..eventName);
    return nil;
end

-------------------------------------------------------
BaseEvent = class(LuaEvent)

function BaseEvent:GetName()
	return self.event:GetEventName();
end


function BaseEvent:getData()
	return self.event:getData();
end

function BaseEvent:Do(  )
    LOG.log(" *** Ready async : "..self:GetName());
	 local co = coroutine.create(self.Async)
    coroutine.resume(co, self);
end

function BaseEvent:Async( )
	
end

function BaseEvent:Finish()
	self.event:Finish();
end


function BaseEvent:WaitTime(t )
	self.event:WaitTime(t);
end

function BaseEvent:getObject( key )
	return self.event:getData():getObject(key);
end

function BaseEvent:set(key, value)
	self.event:getData():set(key, value);
end



function BaseEvent:AsyncRun(fun, ...)
    local co = coroutine.create(fun)
    coroutine.resume(co, self, ...);
end
--------------------------------------------------------------
function Await(milSecond)
	local evt = Start("TimeEvent");  
    evt:Await(milSecond);
end

function Async(fun, ...)
    local co = coroutine.create(fun)
    coroutine.resume(co, ...);
end

-- 异步循环执行(如: 检查逻辑循环) onceSec: 循环间隔时间, overSec: 超时时间, checkFun: 循环执行的函数,返回true, 退出循环
function AsyncLoop(onceSec, overSec, checkFun, ...)
    local et = CS.System.Diagnostics.Stopwatch.StartNew();
    local re = false;
	while true do    
        Await(onceSec);

        if (checkFun(...)) then
            LOG.log("Finish loop : "..tostring(checkFun).. " : "..tostring(overSec));
            re = true;
            break;
        end
        if (overSec>0 and et.ElapsedMilliseconds*0.001>=overSec) then        
            LOG.log("Over loop : "..tostring(checkFun).. " : "..tostring(overSec));
            re = false;
            break;
        end
        
    end
    
    return re;
end


------------------------------------------------------------
-- 异步等待时间
TimeEvent = class(BaseEvent);

function _WaitTime( timerEvent, second, onFun )
    timerEvent.mOnTimeFun = onFun;    
	timerEvent:WaitTime(second);
end

function TimeEvent:Await(second)
	local  fun = util.async_to_sync(_WaitTime);
	return fun(self, second);
end


function TimeEvent:OnTimeOver()
    if (self.mOnTimeFun~=nil) then
        self.mOnTimeFun(true);
    end
end

RegisterEvent("TimeEvent");
-------------------------------------------------------------
-- 准备调用C#Unity组件异步函数 如    public static IEnumerator TestC()
function InitCoroutineRunner()
local gameobject = CS.UnityEngine.GameObject('Coroutine_Runner')
CS.UnityEngine.Object.DontDestroyOnLoad(gameobject)
local cs_coroutine_runner = gameobject:AddComponent(typeof(CS.Coroutine_Runner))

local function async_yield_return(to_yield, cb)
    cs_coroutine_runner:YieldAndCallback(to_yield, cb)
end

-- LUA 异步等待C#函数
yield_return = util.async_to_sync(async_yield_return)
end
-- 先不使用此功能
--InitCoroutineRunner
-----------------------------------------------------------
--调用C# async 异步函数
luaCallCFun = util.async_to_sync(CS.LuaMain.LuaCallCFun);
function AwaitFunc(cFun)
	luaCallCFun(cFun);
end

json = require('Json')


LOG.log("=== Load window.lua succeed ===");