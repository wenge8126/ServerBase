#define UNITY_EDITOR
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;
#if UNITY_EDITOR || UNITY_IPHONE
    using UnityEngine;
#endif

public class LOG
{
    static public tLog msLogicLog = new GameLog();

    static public void log(string info)
    {
#if UNITY_EDITOR
        Debug.Log(info);
        if (msLogicLog != null)
            msLogicLog.Log(info);
#else
#if UNITY_IPHONE
        Debugger.Log(info);
#endif
        Console.WriteLine(info);
        if (msLogicLog!=null)
            msLogicLog.Log(info);

#endif
    }

    static public void logError(string info)
    {
#if UNITY_EDITOR
        Debug.LogError(info);
        if (msLogicLog != null)
            msLogicLog.Log("Error: " + info);
#else
#if UNITY_IPHONE
        Debugger.LogError(info);
#endif
        Console.WriteLine("Error: "+info);
         if (msLogicLog!=null)
            msLogicLog.Log("Error: "+info);
#endif
    }

    static public void logWarn(string info)
    {
#if UNITY_EDITOR
        Debug.LogWarning(info);
#else
#if UNITY_IPHONE
        Debugger.LogWarning(info);
#endif
        Console.WriteLine("Warn: " + info);
         if (msLogicLog!=null)
            msLogicLog.Log("Warn: "+info);
#endif
    }

    static public void LogFormat(string info, params object[] param)
    {
        log(string.Format(info, param));
    }

    static public void Show(string info)
    {
        Debug.Log("*** SHOW : " + info);
        //Console.WriteLine("*** SHOW : " + info);
        if (msLogicLog != null)
            msLogicLog.Log(info);
    }
    
    static public void DebugUI(string info, bool bAppend)
    {
        if (bAppend)
            msInfoString += "\r\n" + info;
        else
            msInfoString = info;
        log(info);
    }

    static public void Write(string info)
    {
        if (msLogicLog != null)
            msLogicLog.Log(info);
    }

    static public string msInfoString = "Ver: "+ GameCommon.Version;
}
