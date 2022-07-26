// Auto generate for login_window
using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;

public class login_window : tWindow
{
    public override string getPrefabName(){ return "login_window" ; }

    protected UnityEngine.GameObject edit_account = null;
    protected UnityEngine.GameObject but_account = null;

    public override void OnInit()
    {
        // but_start
        RegisterButtonWhenNoRegister("but_start");
    }


    public override bool InitUI()
    {
        var rc = mGameObjUI.GetComponent<ReferenceCollector>();


        edit_account = rc.Get<UnityEngine.GameObject>("edit_account");
                if (edit_account==null) { LOG.logError("No exist : edit_account"); return false;  }
        but_account = rc.Get<UnityEngine.GameObject>("but_account");
                if (but_account==null) { LOG.logError("No exist : but_account"); return false;  }
        return true;
    }

    protected override void OnDestroy()
    {
        edit_account=null;
        but_account=null;
    }

}
