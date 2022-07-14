using UnityEngine;
using System;
using System.Reflection;
using System.Collections.Generic;
using Logic;

public enum ERROR_CODE
{

}

public enum UIMESSAGE_Code
{

}

public class UIManager 
{
    static List<string> mOpenWindowList = new List<string>();
    static List<tWindow> mAllWindowList = new List<tWindow>();

    

   
    static public tEvent msCurrentTalkEvent;

    static public void Register<T>(string windowName) where T:tWindow, new()
    {
        EventCenter.StaticRegister(windowName, new DefineWindowFactory<T>());
    }

    static public void InitClearOpenList() { mOpenWindowList.Clear(); }

    static public void AppendOpenWindow(string winName)
    {
        for (int i = 0; i < mOpenWindowList.Count; )
        {
            if (mOpenWindowList[i] == winName)
            {
                mOpenWindowList.RemoveAt(i);
                continue;
            }

            ++i;
        }

        mOpenWindowList.Add(winName);
        LogOpenList();
    }

    static public bool UpdateOpenList(string winName)
    {
        bool find = false;
        for (int i = 0; i < mOpenWindowList.Count; )
        {
            if (mOpenWindowList[i].Equals(winName) || find)
            {
                find = true;
                mOpenWindowList.RemoveAt(i);
                continue;
            }

            ++i;
        }

        if (find)
            LogOpenList();

        return find;
    }

    //static public void RemoveLastOpen() { if (mOpenWindowList.Count>0) mOpenWindowList.RemoveAt(mOpenWindowList.Count-1); }
    //static public void RemoveLastOpen(string winName)
    //{
    //    if (mOpenWindowList.Count > 0)
    //    {
    //        if (mOpenWindowList[mOpenWindowList.Count - 1] == winName)
    //            RemoveLastOpen();
    //    }
    //}
    static public void LogOpenList()
    {
        string openList = "";
        foreach (string winName in mOpenWindowList)
        {
            openList += "> " + winName;
        }
        Debug.Log(openList);
    }

    static public void ReturnOpenWindow()
    {
        if (mOpenWindowList.Count > 0)
        {
            string last = mOpenWindowList[mOpenWindowList.Count - 1];
            UIManager.CloseWindow(last);
            mOpenWindowList.RemoveAt(mOpenWindowList.Count - 1);
        }
        if (mOpenWindowList.Count > 0)
        {
            string last = mOpenWindowList[mOpenWindowList.Count - 1];
            UIManager.OpenWindow(last);
        }
    }

    static public tWindow OpenWindow(string windowName, GameObject parentRoot = null)
    {
        Debug.Log("OpenWindow : " + windowName);
        return OpenWindow(windowName, true, parentRoot);
    }

    static public tWindow OpenWindow(string windowName, object param, GameObject parentRoot = null)
    {
        //WindowFactory f = Logic.EventCenter.Instance.GetEventFactory(StaticDefine.StringID(windowName)) as WindowFactory;
        //if (f != null && f.mWindow != null)
        //{
        //    f.mWindow.Open(param);
        //    //f.mWindow.OnOpen();
        //    return f.mWindow;
        //}

        tWindow win = EventCenter.Start(windowName) as tWindow;
        if (win!=null)
        {
            win.Open(param, parentRoot);
            return win;
        }
        return null;
    }

    static public bool CloseWindow(string windowName, bool user = true)
    {
        //Debug.Log("CloseWindow : " + windowName);
        tWindow win = GetWindow(windowName);
        if (win != null)
            win.Close();
        return win != null;
    }

    static public bool RunActive(string logicDataName, string activeFunctionName, object param)
    {
        tWindow win = GetWindow(logicDataName);
        if (win != null)
        {
            Assembly a = Assembly.GetExecutingAssembly();
            Type t = a.GetType(logicDataName);
            if (t != null)
            {
                MethodInfo info = t.GetMethod(activeFunctionName);
                if (info != null)
                {
                    object[] p = new object[1];
                    p[0] = param;
                    info.Invoke(win, p);
                    return true;
                }
            }
        }
        //LOG.logError("Run function >" + activeFunctionName + " by win>" + logicDataName);
        return false;
    }

    static public void CloseAllWindow()
    {
        List<string> exceptWindowNameList = new List<string>() { "TopMessageWindow", "AnnouncementWindow", "NotifyWindow", "NotifyGMWindow" };
        CloseAllWindowExcept(exceptWindowNameList);
    }

    static public void CloseAllWindowExceptTaskReward()
    {
        List<string> exceptWindowNameList = new List<string>() { "GainRewardWindow", "TopMessageWindow", "NotifyWindow", "NotifyGMWindow" };
        CloseAllWindowExcept(exceptWindowNameList);
    }

    static public void CloseAllWindowExceptHostWindow()
    {
        List<string> exceptWindowNameList = new List<string>() { "MainWindow", "TaskTroopWindow", "ChatWindow", "TopMessageWindow", "TownYaoganWindow", "ActivityPromptWindow", "NotifyWindow", "NotifyGMWindow" };
        CloseAllWindowExcept(exceptWindowNameList);
    }

    static public void CloseAllWindowExcept(List<string> exceptWindowName)
    {
        foreach (tWindow w in mAllWindowList)
        {
            if (w != null && w.IsOpen() && !exceptWindowName.Contains(w.mWinName))
            {
                w.Close();

//                 if (w.mClickCloseCallBack != null)
//                     w.mClickCloseCallBack = null;
            }
        }
    }

    static public tWindow OpenPartnerWindow(string windowName, object param, tWindow partnerWindow)
    {
        tWindow window = OpenWindow(windowName, param);
        if (window != null)
        {
            PartnerWindow pWindow = window as PartnerWindow;
            if (pWindow != null)
                pWindow.mPartnerWindow = partnerWindow;
        }

        return window;
    }

    static public void ClosePartnerWindow(string windowName, tWindow partnerWindow)
    {
        tWindow win = GetWindow(windowName);
        if (win != null)
        {
            PartnerWindow pWindow = win as PartnerWindow;
            if (pWindow != null)
                pWindow.Close(partnerWindow);
        }
    }

    static public void RefreshWindow(string winName, object param)
    {
        tWindow win = GetWindow(winName);
        if (win != null && win.IsOpen())
            win.Refresh(param);
    }

    static public void InitReadyWindow()
    {
        //mAllWindowList.Clear();
        //foreach (KeyValuePair<int, DataRecord> reVal in DataCenter.mWindowConfig.GetAllRecord())
        //{
        //            win = System.Reflection.Assembly.GetExecutingAssembly().CreateInstance(winName) as tWindow;

        //        if (win != null)
        //        {
        //            win.mWinPrefabName = fabName;
        //            win.mBundlename = re["RES_BUNDLE"];
        //            win.mAnchor = re.get("ANCHOR");
        //            win.mWinName = winName;
        //            win.mIsRecordOpen = re["IS_RECORD"];
        //            win.mRootName = re["UI_ROOT"];
        //            win.mIsCache = re["IS_CACHE"];
        //            win.mbCanOpwnGuide = re["CAN_OPEN_GUIDE"];
        //            win.mCfgOpenSound = re["OPEN_SOUND"];
        //            win.mCfgCloseSound = re["CLOSE_SOUND"];
        //            if (win.mRootName == "")
        //                win.mRootName = CommonParam.msUIRootName;

        //            WindowFactory f = new WindowFactory();
        //            f.mWindow = win;
        //            Logic.EventCenter.StaticRegister(winName, f);
        //            Logic.EventCenter.Start(winName);
        //            win.Init();
        //            mAllWindowList.Add(win);
        //        }
        //        else
        //        {
        //            Logic.EventCenter.Log(LOG_LEVEL.ERROR, "No exist script>" + winName + " of " + winName);
        //        }
        //    }
        //}
    }

    static public tWindow GetWindow(string name)
    {
        // WindowFactory f = Logic.EventCenter.Instance.GetEventFactory(name) as WindowFactory;
        // if (f != null)
        //     return f.mWindow;
        //return null;
        
        return EventCenter.Start(name) as tWindow;
    }

    static public void CacheOpenWindow()
    {
        List<string> windowNameList = new List<string>() { /*"RoleEquipWindow",*/ "StageInfoWindow"/*, "RoleCultivateWindow"*/ };
        foreach (var v in windowNameList)
        {
            tWindow window = UIManager.GetWindow(v);
            if (window != null)
            {
                window.CacheOpen();
            }
        }
    }




    static public void GCWindow()
    {
        foreach (tWindow window in mAllWindowList)
        {
            bool isCache = window.mIsCache;
            if (!window.IsOpen() && !isCache)
                window.Destroy();
        }
    }

    #region -- message window





    #endregion
}
