using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//using Cysharp.Threading.Tasks;
//using Ryu.Framework;
using System;
using XLua;
using System.Threading.Tasks;
using UnityEngine.Events;

public static  class LuaConfig 
{
    // 导出代理
    [CSharpCallLua]
    public static List<Type> CSharpCallLua = new List<Type>()
    {
        typeof(Action),
        typeof(Action<bool>),
        typeof(UnityAction),
        typeof(LuaMain.AsyncCFun)
    };

    // 导出对象交换文件
    [LuaCallCSharp]
    public static List<Type> LuaCallCSharp
    {
        get
        {
            return new List<Type>()
            {
                typeof(WaitForSeconds),
                typeof(Task),
                //typeof(Func<Task>),
                //typeof(UniTask),
                //typeof(Func<UniTask>)                
            };
        }
    }
}



