using UnityEngine;
using System.Collections;
using XLua;
using Logic;
using System;

//-------------------------------------------------------------------------
// Lua 中的窗口C#对象，对应Lua中 base_window 中的 self:win
// 由于窗口已经继承事件, 所以需要各种事件接口处理
//-------------------------------------------------------------------------
public class LuaWindow : tWindow
{
	public LuaTable	mLuaWin;

    public float mNowTime = 0;
    public int mRunCount = 1;
    public float mSpaceTime = 0.03f;

	public LuaWindow(LuaTable luaWin)
	{
		mLuaWin = luaWin;
		if (luaWin==null)
			LOG.logError("Lua window init fail, Lua window is null");
	}

    void RunLuaFun(string funName)
    {
        try
        {
            LuaFunction fun;
            mLuaWin.Get(funName, out fun);
            if (fun != null)
                fun.Call(mLuaWin, this);
        }
        catch (System.Exception e)
        {
            LOG.logError(GetEventName() + " Lua Error : "+funName + " > " + e.ToString());
        }

    }

    public override string getPrefabName()
    {
        LuaFunction fun = null;     
        mLuaWin.Get("getPrefab", out fun);
        try
        {
            if (fun != null)
            {
                object[] re = fun.Call(mLuaWin, this);
                if (re.Length > 0)
                    return (string)re[0];
            }
            else
                LOG.logError("Lua no exist  getPrefab at : "+GetEventName());
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }
        return "None";
    }

    public override void Init()
	{
		LuaFunction fun;
        mLuaWin.Get("Init", out fun);
        try
        {
            if (fun != null)
                fun.Call(mLuaWin, this);
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

	}

    public override void OnInit()
    {
        RunLuaFun("OnInit");
    }

    public override bool InitUI()
    {
        LuaFunction fun;
        mLuaWin.Get("InitUI", out fun);
        try
        {
            if (fun != null)
            {
                object[] re = fun.Call(mLuaWin, this);
                if (re.Length > 0)
                    return (bool)re[0];
            }
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }
        return false;
    }

    public override void Open(object param, GameObject parentRoot)
    {
        base.Open(param, parentRoot);
        try{
            LuaFunction fun;
            mLuaWin.Get("Open", out fun);
            if (fun != null)
                fun.Call(mLuaWin, param, this);
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

    }

    public override void OnOpen()
    {
        //base.OnOpen(param);
        LuaFunction fun;
        mLuaWin.Get("OnOpen", out fun);
        try
        {
            if (fun != null)
                fun.Call(mLuaWin, this);
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

    }

    public override void OnClose()
    {
        RunLuaFun("OnClose");
    }

    public override void OnTimeOver()
    {
        try
        {
            LuaFunction fun;
            mLuaWin.Get("OnTimeOver", out fun);
            if (fun != null)
                fun.Call(mLuaWin, this);
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

    }

    public override void WaitUpdate(float spaceTime)
    {
        mNowTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
        mSpaceTime = spaceTime;
        mRunCount = 1;
        StartUpdate();
    }

    public override void Update(float t)
    {
        try
        {
            LuaFunction fun = null;
            mLuaWin.Get("Update", out fun);
            if (fun != null)
            {
                if (EventCenter.Instance.mTimeEventManager.GetNowTime() - mNowTime >= mRunCount * mSpaceTime)
                {
                    ++mRunCount;
                                       
                   fun.Call(mLuaWin, mSpaceTime, this);
                 
                }
                return;
           }
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

        StopUpdate();
    }

    public object[] RunLuaFun(string luaFun, object param)
    {
        try
        {
            LuaFunction fun = null;
            mLuaWin.Get(luaFun, out fun);
            if (fun != null)
            {
                object[] r = fun.Call(mLuaWin, param, this);
                return r;
            }
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

        return null;
    }
}

public class LuaWindowFactory : BaseFactory
{
    public tWindow mOwnerWindow;

    public override bool NeedLog() { return true; }
    public override tEvent NewEvent()
    {
        return mOwnerWindow;
    }

    public LuaWindowFactory(string windowName)
    {
        SetEventName(windowName);
        LuaTable luaWin = LuaMain.msLuaState.Global.GetInPath<LuaTable>(windowName);
        if (luaWin == null)
        {
            LOG.logError("Lua window >" + windowName + " no exist");
            return;
        }
        LuaFunction newFun = luaWin.Get<LuaFunction>("new");
        if (newFun==null)
        {
            LOG.log("Lua class no use class : " + luaWin.ToString());
            mOwnerWindow = new LuaWindow(luaWin);
            return;
        }
        LuaTable newLuaWindow = newFun.Call(luaWin)[0] as LuaTable;
        mOwnerWindow = new LuaWindow(newLuaWindow);
        newLuaWindow.Set("win", mOwnerWindow);
        mOwnerWindow.SetEventFactory(this);
        mOwnerWindow.Create();        
        EventCenter.StaticRegister(windowName, this);
    }
}



public class LuaButtonEvent : BaseEvent
{
    public override void Do()
    {
        LOG.logError("Do function is null");
    }

    public override void Start(object go)
    {
        LuaButtonEventFactory f = mFactory as LuaButtonEventFactory;
        try
        {
            if (f != null)
            {
                if (f.mDoFun != null)
                    f.mDoFun.Call(f.mLuaWinTable, go, f.mOwnerWindow);
                else
                    LOG.logError("Window function is null");
            }
            else
                LOG.logError("Event factory must is LuaButtonEventFactory");
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

       
    }
}

public class LuaButtonEventFactory : BaseFactory
{
    public LuaTable mLuaWinTable;
    public LuaFunction mDoFun;

    public tWindow mOwnerWindow;

    public override bool NeedLog() { return true; }
    public override tEvent NewEvent() { return new LuaButtonEvent(); }

    public LuaButtonEventFactory(LuaTable luaWin, string funName)
    {
        luaWin.Get("win", out mOwnerWindow); // = luaWin["win"] as tWindow;
        mLuaWinTable = luaWin;
        if (mLuaWinTable != null)
        {
            mLuaWinTable.Get(funName, out mDoFun); // = mLuaWinTable[funName] as LuaFunction;
            if (mDoFun == null)
            {
                LOG.logError("No exist lua function>" + funName);
            }
        }
        else
            LOG.logError("Lua window is null");
    }
}

public class LuaFunctionEvent : BaseEvent
{
    public override void Do()
    {
        LuaFunctionEventFactory f = mFactory as LuaFunctionEventFactory;
        try
        {
            if (f != null)
            {
                if (f.mDoFun != null)
                    f.mDoFun.Call(f.mLuaWinTable, getData(), f.mOwnerWindow);
                else
                    LOG.logError("Window function is null");
            }
            else
                LOG.logError("Event factory must is LuaFunctionEventFactory");
        }
        catch (System.Exception e)
        {
            LOG.logError("Lua Error >" + e.ToString());
        }

        
    }
}

public class LuaFunctionEventFactory : BaseFactory
{
    public LuaTable mLuaWinTable;
    public LuaFunction mDoFun;

    public tWindow mOwnerWindow;

    public override bool NeedLog() { return true; }
    public override tEvent NewEvent() { return new LuaFunctionEvent(); }

    public LuaFunctionEventFactory(LuaTable luaWin, LuaFunction fun)
    {
        luaWin.Get("win", out mOwnerWindow); // = luaWin["win"] as tWindow;
        mLuaWinTable = luaWin;
        if (mLuaWinTable != null)
        {
            mDoFun = fun;
            if (mDoFun == null)
            {
                LOG.logError("LuaFunctionEventFactory need lua function param");
            }
        }
        else
            LOG.logError("Lua window is null");
    }
}
