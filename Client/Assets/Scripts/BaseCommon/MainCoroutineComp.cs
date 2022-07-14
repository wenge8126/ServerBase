
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;
using Logic;
using System.Threading.Tasks;
using System.Threading;
using System.Diagnostics;
using System;


/// <summary>
/// 实例主线程协程, 应用实现unity要求在主线执行的异步
/// </summary>
public class MainCoroutine
{
    public delegate IEnumerator MainDo(MainCoroutine go, object[] param);
    public delegate bool Cancel();
   
    object mResult = null;
    public CancellationTokenSource mCancel;
    static public Task<object> StartMainTask(MainDo fun, object[] param, CancellationTokenSource cancelToken)
    {
        var temp = new MainCoroutine();
        return temp._StartMainTask(fun, param, cancelToken);               
    }

    async Task<object> _StartMainTask(MainDo fun, object[] param, CancellationTokenSource cancelToken)
    {
        if (mCancel!=null)
        {
            LOG.logError("Now runing ...");
            return null;
        }

        mCancel = cancelToken;
        if (mCancel==null)
            mCancel = new CancellationTokenSource(); 

        var coro = MainCoroutineComp.Instance.StartCoroutine(fun(this, param));
        try
        {
            await Task.Delay(int.MaxValue, mCancel.Token);
        }
        catch (Exception)
        {
            
        }
        
        if (coro!=null)
            MainCoroutineComp.Instance.StopCoroutine(coro);

        return mResult;
    }
    
    public void FinishResult(object result)
    {
        mResult = result;        
        mCancel.Cancel(false);
    }
}

/// <summary>
/// 启动主线程中协程组件, 直接加载到主摄像机对象
/// </summary>
public class MainCoroutineComp : MonoBehaviour
{
    static public MainCoroutineComp msInstance;

    void Awake()
    {
        msInstance = this;
    }

    static public MainCoroutineComp Instance
    {
        get
        {
            if (msInstance == null)
                msInstance = Camera.main.gameObject.AddComponent<MainCoroutineComp>();
            return msInstance;
        }
    }
}

/// <summary>
/// 利用主线程协程, 下载示例
/// </summary>
public class TestMainCoroutine
{

    static Stopwatch mBeginTime = null;
    static bool CancelDown()
    {
        return (mBeginTime.ElapsedMilliseconds > 3000);
    }

    public static async Task<byte[]> Test(string resName, string url)
    {
        mBeginTime = Stopwatch.StartNew();
        return (await MainCoroutine.StartMainTask(LoadFromServer, new object[] { resName, url }, null)) as byte[];
    }

    public static IEnumerator LoadFromServer(MainCoroutine go, object[] param)
    {
        string mResultName = (string)param[0];
        string url = (string)param[1];

        using (UnityWebRequest request = UnityWebRequest.Get(url))
        {
            //request.downloadHandler = new DownloadHandlerFile(savePath+mResultName+UpdateResourceMgr.mUpdateFileExt);
            request.SendWebRequest();                 //请求下载
            while (!request.isDone)
            {
                LOG.log(request.downloadProgress.ToString());  //下载进度
                yield return 1;
            }
            if (request.result!= UnityWebRequest.Result.Success) // request.result == UnityWebRequest.Result.ConnectionError || request.result == UnityWebRequest.Result.ProtocolError)
            {
                LOG.logError("有误 = " + request.error + " : " + url + " : ");  //下载出错
                go.FinishResult(null);
            }
            else if (request.isDone)                  //下载完成
            {
                byte[] bytes = request.downloadHandler.data;

                LOG.log("下载完成 : " + url);
                go.FinishResult(bytes);
            }
        }
        
    }
}
