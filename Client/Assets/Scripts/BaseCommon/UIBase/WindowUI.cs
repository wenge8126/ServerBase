using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;



//-------------------------------------------------------------------------
public class WindowFactory : BaseFactory
{
    public tWindow mWindow;

    public override tEvent NewEvent() { return mWindow; }

    public override void Destroy()
    {
        if (mWindow != null)
            mWindow.Destroy();
    }
}

public class DefineWindowFactory<T> : WindowFactory
     where T:tWindow, new()
{
    public override tEvent NewEvent()
    {
        if (mWindow == null)
        {
            mWindow = new T();
            mWindow.SetEventFactory(this);
            mWindow.Create();
        }
        return mWindow;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
public class OpenWindowEventFactory : BaseFactory
{
    public tWindow mWindowObject;
    public string mOpenWindowName;
    public bool mbNeedCloseAllWindow;

    static tEvent msNullEvent = new BaseEvent();


    public OpenWindowEventFactory(string openWindowName, bool bCloseAll, tWindow closeWindow)
    {
        mWindowObject = closeWindow;
        mbNeedCloseAllWindow = bCloseAll;
        mOpenWindowName = openWindowName;
    }

    public override tEvent NewEvent()
    {
        // start event, then open window, so event not need
        if (mbNeedCloseAllWindow)
            UIManager.CloseAllWindow();
        else if (mWindowObject != null)
            mWindowObject.Close();

        UIManager.OpenWindow(mOpenWindowName);

        return msNullEvent;
    }
}

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
public class WindowEventFactory : BaseFactory
{
    public object mWindowObject;
    public System.Reflection.MethodInfo mFunMethod;

    public WindowEventFactory(object win, string funName)
    {
        mWindowObject = win;
        mFunMethod = win.GetType().GetMethod(funName);
    }

    public override tEvent NewEvent()
    {
        return new WindowEvent();
    }
}

public class WindowEvent : BaseEvent
{
    public override void Start(object go)
    {
        WindowEventFactory f = GetEventFactory() as WindowEventFactory;

        if (f.mFunMethod == null)
        {
            LOG.logError("Window function no exist");
            return ;
        }

        object[] p = new object[1];
        p[0] = go as GameObject;
        
        try
        {
            f.mFunMethod.Invoke(f.mWindowObject, p);
            return ;
        }
        catch (Exception e)
        {
            LOG.logError("Run action > " + e.ToString());
            return ;
        }

        //return true;
    }
}
//-------------------------------------------------------------------------
public class WindowFunctionEventFactory : BaseFactory
{
    public object mWindowObject;
    public tWindow.WinFunction mFunMethod;

    public WindowFunctionEventFactory(object win, tWindow.WinFunction function)
    {
        mWindowObject = win;
        mFunMethod = function;
    }

    public override tEvent NewEvent()
    {
        return new WindowFunctionEvent();
    }
}

public class WindowFunctionEvent : BaseEvent
{
    public override void Do()
    {
        WindowFunctionEventFactory f = GetEventFactory() as WindowFunctionEventFactory;

        if (f.mFunMethod == null)
        {
            //LOG.logError("Window function no exist");
            return ;
        }
        try
        {
            f.mFunMethod(getData());
            return ;
        }
        catch (Exception e)
        {
            LOG.logError("Run action > " + e.ToString());
            return ;
        }

        //return true;
    }
}
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------


//------------------------------------------------------------------------

public class CoroutineHelper : MonoBehaviour
{
    public IEnumerator DefferCallCorountine(float defferedSeconds, Action action)
    {
        yield return new WaitForSeconds(defferedSeconds);

        if (action != null)
            action();
    }
}

public static class tWindowExtend
{
    public static Coroutine StartCoroutine(this tWindow window, IEnumerator coroutine)
    {
        CoroutineHelper helper = window.mGameObjUI.GetComponent<CoroutineHelper>();
        if (helper == null)
            helper = window.mGameObjUI.AddComponent<CoroutineHelper>();

        return helper.StartCoroutine(coroutine);
    }

    public static void StopCoroutine(this tWindow window, IEnumerator coroutine)
    {
        CoroutineHelper helper = window.mGameObjUI.GetComponent<CoroutineHelper>();
        if (helper != null)
            helper.StopCoroutine(coroutine);
    }

    public static void StopCoroutine(this tWindow window, Coroutine coroutine)
    {
        CoroutineHelper helper = window.mGameObjUI.GetComponent<CoroutineHelper>();
        if (helper != null)
            helper.StopCoroutine(coroutine);
    }

    public static void StopAllCoroutine(this tWindow window)
    {
        CoroutineHelper helper = window.mGameObjUI.GetComponent<CoroutineHelper>();
        if (helper != null)
            helper.StopAllCoroutines();
    }

    public static void DefferCall(this tWindow window, float defferedSeconds, Action action)
    {
        CoroutineHelper helper = window.mGameObjUI.GetComponent<CoroutineHelper>();
        if (helper == null)
            helper = window.mGameObjUI.AddComponent<CoroutineHelper>();

        helper.StartCoroutine(helper.DefferCallCorountine(defferedSeconds, action));
    }
}

//共有窗口 谁打开的谁关闭
public class PartnerWindow : tWindow
{
    tWindow partnerWindow = null;
    public tWindow mPartnerWindow
    {
        get { return partnerWindow; }
        set { partnerWindow = value; }
    }

    public void Open(object param, tWindow pWindow)
    {
        partnerWindow = pWindow;
        Open(param);
    }

    public void Close(tWindow pWindow)
    {
        if (partnerWindow != null && pWindow != null && (mPartnerWindow.mWinName != pWindow.mWinName))
            return;

        Close();
    }
}
