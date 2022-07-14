using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;

using UnityEngine;

public class HotUpdateWindow: export_resource_window
{
    public static HotUpdateWindow Instance
    {
        get
        {
            if (!Logic.EventCenter.Instance.ExistEventFactory("HotUpdateWindow"))
                UIManager.Register<HotUpdateWindow>("HotUpdateWindow");
            HotUpdateWindow window = UIManager.GetWindow("HotUpdateWindow") as HotUpdateWindow;
            return window;
        }
    }
    
    public void SetInfoText(string info)
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


public class HotUpdateNewWorker : Logic.IUpdateWorker
{
    private Int64     mAlreadySize = 0;
    private Int64     mTotalSize   = 1;
    private Stopwatch mTotalTime;
    private string    mTypeInfo = "";
    
    public void  StartUpdate(string typeInfo, out string url)
    {
        url = GameCommon.DownloadAddr;
        LOG.log("Start update : "+typeInfo + " URL : "+url);
        mTypeInfo = typeInfo;
    }

    public void ShowInfo(string info)
    {        
        LOG.log("Update info : "+info);
    }
    
    public void BeginDownload(Int64 totalSize, Int64 alreadyDownloadSize)
    {
        mAlreadySize = alreadyDownloadSize;
        mTotalSize = totalSize;
        if (totalSize <= 0)
            mTotalSize = 1;
        
        LOG.log("Need update all size : "+totalSize);
        mTotalTime = Stopwatch.StartNew();
        //UpdateProcess(0);
        HotUpdateWindow.Instance.Open(null);
    }
    
    public void UpdateProcess(Int64 downloadSize)
    {
        float r = mTotalTime.ElapsedMilliseconds > 0 ? (downloadSize- mAlreadySize) / mTotalTime.ElapsedMilliseconds : 0;
        var d = new TimeSpan(mTotalTime.ElapsedMilliseconds*10000).ToString(@"hh\:mm\:ss");
        var lastT = new TimeSpan((long)((mTotalSize - downloadSize) / r * 10000)).ToString(@"hh\:mm\:ss");
        string inforTextStr =
            $"{downloadSize.ToString()} / {mTotalSize.ToString()} \r\n {string.Format("{0:F2} %", (float)((double)downloadSize * 100 / mTotalSize))}   {string.Format("{0:F1} ", r)}K / Sec \r\n {d} , last {lastT}";
       
        
        string inforTextStr2 = String.Format("{0}{1}MB/{2}MB",GetInfoText(),((float)downloadSize/1024/1024).ToString("F2"),((float)mTotalSize/1024/1024).ToString("F2")); 
        HotUpdateWindow.Instance.SetInfoText(inforTextStr2);
        
       // GetHotupPage().SetTypeText(mTypeInfo);
        float process = (float)downloadSize / (float)mTotalSize;
        HotUpdateWindow.Instance.SetProcess(process);
    }

    private string GetInfoText()
    {
        //RyUILabelMessage mMessage = RyUILabelMessage.CreateLabelMessage(UILabelFile.AppUIHotUpdate.ToString(), mTypeInfo);
        //return RyUI.MessageHandler.ParseFromMessage(mMessage);
        return "None";
    }

    public async void FinishUpdate(bool bSucceed, string info)
    {
        string result = ("Update finsih : "+(bSucceed ? " OK": "Fail")+", info : "+info);
        LOG.log(result);       
        //if (!bSucceed)
          //  await RyLCX.Instance.ShowNetErrorAsync("资源更新", "更新失败");
        
        HotUpdateWindow.Instance.Close();
    }

    public async void OnUpdateFail(List<string> failResources)
    {
        string listString = "";
        LOG.logError("---------- Have update fail --------");
        List<string> resList = new List<string>();
        foreach (var re in failResources)
        {
            string name = re;
            LOG.logError("Update fail : " + name);
            listString += ", " + name;
        }
        LOG.logError("---update fail--list---"+listString);
        //GetHotupPage().SetInfoText(listString);
        HotUpdateWindow.Instance.SetInfoText(GetFailTipsStr());
		//await RyLCX.Instance.ShowNetErrorAsync("资源更新", "更新失败");
    }
    
    private string GetFailTipsStr()
    {
        //多语言生成
        // RyUILabelMessage mMessage = RyUILabelMessage.CreateLabelMessage(UILabelFile.AppUIHotUpdate.ToString(), mTypeInfo);
        // return RyUI.MessageHandler.ParseFromMessage(mMessage);
        return "更新失败";
    }

    public void Clean()
    {
        LOG.log(":_: mHotupPage::Clean--------");
        HotUpdateWindow.Instance.Close();
    }
}
