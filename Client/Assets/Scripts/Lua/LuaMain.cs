
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using Logic;
using UnityEngine;
using XLua;


using LuaAPI = XLua.LuaDLL.Lua;
//using Cysharp.Threading.Tasks;
//using Ryu.Framework;
using System.Threading.Tasks;

public class LuaMain
{
    static public LuaEnv msLuaState;


    public delegate Task AsyncCFun();

    public class AsyncRunData
    {
        public AsyncCFun mCFun;
        public Action<bool> onFinished;

        public async Task Run()
        {
            await mCFun();
            onFinished(true);
        }
    }

    // 用于LUA异步调用C#异步函数
    static public void LuaCallCFun(AsyncCFun fun, Action<bool> onFinished)
    {
        LOG.log("Async call : " + fun.ToString());
        var runData = new AsyncRunData();
        runData.mCFun = fun;
        runData.onFinished = onFinished;
        runData.Run();    
        
    }

    static public async Task AsyncTest()
    {
        LOG.log("===== AsyncTest start =====");
        await Task.Delay(2000);
        LOG.log("========= AsyncTest end ========");
    }

    static public void StartLua()
    {
        if (msLuaState != null)
            CloseLua();

        try
        {            
            //new LuaScriptMgr();
            msLuaState = new LuaEnv(); // LuaScriptMgr.Instance.lua;
            
            //LuaScriptMgr.Instance.Start();
            //LuaScriptMgr.Instance.Init();
            msLuaState.DoString(msBaseLuaString, "_BaseScript_");
            //msLuaState = new LuaState();            
        }
        catch (System.Exception e)
        {
            LOG.logError("LUA: Error: " + e.ToString());
            msLuaState = new LuaEnv();
        }


        // 脚本内部加载方法 require
        msLuaState.AddLoader(new LuaEnv.CustomLoader(_luaLoader));


        //??? LuaAPI.luaopen_pb(msLuaState.L);
#if USE_LIST_TABLE
        NiceTable luaList = TableManager.GetTable("LuaList");
        if (luaList != null)
        {
            foreach (KeyValuePair<int, DataRecord> vRe in luaList.GetAllRecord())
            {
                //LOG.log("Load lua >" + (string)vRe.Value["LUA"]);
                try
                {
                    LoadLua(vRe.Value["LUA"]);
                }
                catch (System.Exception e)
                {
                    LOG.logError("LUA: " + (string)vRe.Value["LUA"] + "> Error: " + e.ToString());
                }
            }
        }
#else
        LoadLua("Load");
#endif
        // 发布版本时, LUA >print 什么都不做, 你懂得~~
#if !UNITY_EDITOR
        if (GameCommon.LogToFile)
            msLuaState.DoString("function print(info) CS.LOG.log(info); end");
#else
        msLuaState.DoString("function print(info) CS.LOG.log(info); end");
#endif
    }

   
    static public void CloseLua()
    {
        EventCenter.Instance.ClearFactory(typeof(LuaEventFactory));
        EventCenter.Instance.ClearFactory(typeof(LuaButtonEventFactory));

        //if (LuaScriptMgr.Instance!=null)
        //    LuaScriptMgr.Instance.Destroy();
        if (msLuaState != null)
        {
            msLuaState.Dispose();
            msLuaState = null;
        }
     
    }

    // 脚本内部加载方法, NOTE: 文件名不包含扩展名.lua

    static public byte[] _luaLoader(ref string filepath)
    {
#if UNITY_EDITOR
        LOG.log("Load lua : "+filepath);
        var d = UpdateResourceMgr.LoadFileData(Application.dataPath + "/LuaCode/" + filepath + ".lua");
#else
        var d = UpdateResourceMgr.LoadUpdateFileData(LocalResourcePath.LuaCodePath, filepath);
#endif
        if (d != null)
            return d;
        LOG.logError("*** Load lua file data fail >" + filepath);
        return null;
    }

    // NOTE: 参数脚本资源名,需要包含.lua

    static public void LoadLua(string resName)
    {
        var d = _luaLoader(ref resName);
        if (d != null)
        {
            try
            {
                msLuaState.DoString(d, resName+".lua");
            }
            catch (System.Exception e)
            {
                
                LOG.logError("LUA: " + resName + "> Error: " + e.ToString());
            }
        }
        else
            LOG.logError("No exist lua >" + resName );
    }



    static string msBaseLuaString = @"local _class={}
 
function class(super)
	local class_type={}
	class_type.ctor=false
	class_type.super=super
	class_type.new=function(...) 
			local obj={}
			do
				local create
				create = function(c,...)
					if c.super then
						create(c.super,...)
					end
					if c.ctor then
						c.ctor(obj,...)
					end
				end
 
				create(class_type,...)
			end
			setmetatable(obj,{ __index=_class[class_type] })
			return obj
		end
	local vtbl={}
	_class[class_type]=vtbl
 
	setmetatable(class_type,{__newindex=
		function(t,k,v)
			vtbl[k]=v
		end
	})
 
	if super then
		setmetatable(vtbl,{__index=
			function(t,k)
				local ret=_class[super][k]
				vtbl[k]=ret
				return ret
			end
		})
	end
 
	return class_type
end

-- Base event

base_event = class();

function base_event:ctor()	


end
 
--function base_event:_DoEvent(evt)	
--	print('WARN: Do nothing when _DoEvent');
--end
 
--function base_event:_OnResp(resp, evt)	
--	print('WARN: Respone nothing, when _OnResp');
--end

function base_event:_OnTimeOver(evt)
	print('WARN: over time nothing, when _OnTimeOver');
end

math.randomseed(os.time());
";
}
//-------------------------------------------------------------------------
// LUA中的C#对象, 对应Lua 中的 base_event, 如果 base_event:_DoEvent(evt) evt参数即是此对象
//-------------------------------------------------------------------------
public class LuaEvent : BaseEvent
{
    public LuaTable mLuaTable;

    LuaFunction mDoFun;
    LuaFunction mOverTimeFun;
        LuaFunction mOnFinishFun;
        LuaFunction mUpdateFun;

    public LuaEvent()
    {
        mData = new NiceData();
    }

    public  static void TryGetFun(LuaTable t, string  funName, out LuaFunction fun)
    {
        try
        {
            if (t.ContainsKey<string>(funName))
                t.Get<string, LuaFunction>(funName, out fun);
            else
                fun = null;
        }catch (Exception e)
        {
            fun = null;
        }
    }

    public void InitLua()
    {
        TryGetFun(mLuaTable, "Do", out mDoFun);
        TryGetFun(mLuaTable, "OnTimeOver", out mOverTimeFun);
        TryGetFun(mLuaTable, "OnFinish", out mOnFinishFun);
        TryGetFun(mLuaTable, "Update", out mUpdateFun);
    }
    
    public override void Do()
    {       
        if (mDoFun != null)
            mDoFun.Call(mLuaTable, this);
      
    }

    
    public override void Update(float t)
    {
        if (mUpdateFun != null)
        {
            mUpdateFun.Call(mLuaTable, this, t);
        }
        else
        {
            LOG.logWarn("LUA event no exist Update function");
            StopUpdate();
        }
      
    }

    public override void  OnTimeOver()
    {
        if (mOverTimeFun != null)
            mOverTimeFun.Call(mLuaTable, this);
    }

    public override void OnFinish()
    {
        if (mOnFinishFun != null)
            mOnFinishFun.Call(mLuaTable, this);

      
    }

    public override void Finish()
    {
        base.Finish();
        mLuaTable["event"] = null;
        mLuaTable = null;
    }
}


public class LuaEventFactory : BaseFactory
{      

    LuaFunction mNewFun;
    LuaTable mLuaTable;

    public LuaEventFactory(string luaEventName)
    {
        LuaTable luaEvent = LuaMain.msLuaState.Global.GetInPath<LuaTable>(luaEventName);
        if (luaEvent == null)
        {
            LOG.logError("Lua event >" + luaEventName + " no exist");
            return;
        }
        mLuaTable = luaEvent;

        LuaFunction newFun = null;
        luaEvent.Get<string, LuaFunction>("new", out newFun);
        if (newFun == null)
        {
            LOG.logError("Register lua event fail, lua event not have new");
            return;
        }

        mNewFun = newFun;
       
    }

    public override tEvent NewEvent()
    {
        LuaEvent evt;      
            evt = new LuaEvent();
        evt.mLuaTable = mNewFun.Call(mLuaTable)[0] as LuaTable;
        evt.mLuaTable["event"] = evt;

        evt.InitLua();

        return evt;
    }

    public override bool NeedLog() { return true; }
}


