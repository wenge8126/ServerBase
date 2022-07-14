// Auto generate for message_window
using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;

public class message_window : tWindow
{
    public override string getPrefabName(){ return "message_window" ; }

    protected ButtonEvent btn_base = null;
    protected UnityEngine.UI.Text exception_txt = null;
    protected UnityEngine.UI.Text title_txt = null;
    protected UnityEngine.UI.Text wait_txt = null;
    protected UnityEngine.UI.Text btn_txt = null;


    public override void OnInit()
    {
        // btn_base
        RegisterButtonWhenNoRegister("btn_base");
    }



    public override bool InitUI()
    {
        var rc = mGameObjUI.GetComponent<ReferenceCollector>();


        btn_base = rc.Get<ButtonEvent>("btn_base");
                if (btn_base==null) { LOG.logError("No exist : btn_base"); return false;  }
        exception_txt = rc.Get<UnityEngine.UI.Text>("exception_txt");
                if (exception_txt==null) { LOG.logError("No exist : exception_txt"); return false;  }
        title_txt = rc.Get<UnityEngine.UI.Text>("title_txt");
                if (title_txt==null) { LOG.logError("No exist : title_txt"); return false;  }
        wait_txt = rc.Get<UnityEngine.UI.Text>("wait_txt");
                if (wait_txt==null) { LOG.logError("No exist : wait_txt"); return false;  }
        btn_txt = rc.Get<UnityEngine.UI.Text>("btn_txt");
                if (btn_txt==null) { LOG.logError("No exist : btn_txt"); return false;  }
        return true;
    }

   
    protected override void OnDestroy()
    {
        btn_base=null;
        exception_txt=null;
        title_txt=null;
        wait_txt=null;
        btn_txt=null;
    }

}
