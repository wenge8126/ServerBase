using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ExportResourceWindow : export_resource_window
{
    public static ExportResourceWindow Instance
    {
        get
        {
            if (!Logic.EventCenter.Instance.ExistEventFactory("ExportResourceWindow"))
                UIManager.Register<ExportResourceWindow>("ExportResourceWindow");
            ExportResourceWindow window = UIManager.GetWindow("ExportResourceWindow") as ExportResourceWindow;
            return window;
        }
    }
    
    public void SetInfo(string info)
    {
        if (info_text!=null)
        {
            info_text.text = info;
        }
    }

    public void SetProcess(float fProcess)
    {
        if (slider != null)
            slider.value = Mathf.Clamp01(fProcess);
    }
}
