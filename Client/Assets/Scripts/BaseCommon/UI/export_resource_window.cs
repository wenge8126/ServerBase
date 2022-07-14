// Auto generate for export_resource_window
using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;

public class export_resource_window : tWindow
{
    public override string getPrefabName(){ return "export_resource_window" ; }

    protected UnityEngine.UI.Text info_text = null;
    protected UnityEngine.UI.Slider slider = null;

    
    public override void OnInit()
    {
    }


    
    public override bool InitUI()
    {
        var rc = mGameObjUI.GetComponent<ReferenceCollector>();


        info_text = rc.Get<UnityEngine.UI.Text>("info_text");
                if (info_text==null) { LOG.logError("No exist : info_text"); return false;  }
        slider = rc.Get<UnityEngine.UI.Slider>("slider");
                if (slider==null) { LOG.logError("No exist : slider"); return false;  }
        return true;
    }

    
    protected override void OnDestroy()
    {
        info_text=null;
        slider=null;
    }

}

