using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Logic;
using UnityEngine;
using UnityEngine.Networking;

namespace Logic
{

    public class DownloadUpdate
    {
        public string mServerUrl;
        public List<DataRecord> mUpdateList;

        public delegate void OnDownload(DataRecord downRecord, byte[] data);
        public OnDownload mOnDownloadFunction;

        public delegate void Process(long downloadSize, long totalSize);

        public Process mDownloadProcessFunction = null;
        
        public int mAsyncDownloadCount = 10;

        public CancellationTokenSource mCanncel = new CancellationTokenSource();

        /// <summary>
        /// 下载资源数据
        /// </summary>
        /// <param name="url"></param>
        /// <param name="localSavePath"></param>
        /// <returns></returns>
        public async Task<bool> Download(String serverUrl, List<DataRecord> updateList, OnDownload onCallFunction, long totalSize, Process processFunction, int asyncCount = 10)
        {
            mServerUrl               = serverUrl;
            mUpdateList              = updateList;
            mOnDownloadFunction      = onCallFunction;
            mDownloadProcessFunction = processFunction;
            mAsyncDownloadCount      = asyncCount;
            return await DownloadFromServer(totalSize);
        }

        public class DownRequest
        {
            public UnityWebRequest mRequest;
            public DataRecord mDownFileRecord;
            public string mUrl;
        }

        DownRequest StartOnceDownload()
        {
            if (mUpdateList.Count <= 0)
                return null;

            DataRecord re = mUpdateList[0];
            mUpdateList.RemoveAt(0);


            var request = new DownRequest();

            request.mDownFileRecord = re;

            string url = mServerUrl + (string)re.getString("NAME");
            request.mUrl = url;
            request.mRequest = UnityWebRequest.Get(url);
            request.mRequest.certificateHandler = new UpdateResourceMgr.BypassCertificate();

            try
            {
                request.mRequest.SendWebRequest();                 //请求下载
                return request;
            }
            catch (Exception e)
            {
                LOG.logError("Downlad fail : " + url + " Error : " + e.ToString());
                mOnDownloadFunction(re, null);
                return StartOnceDownload();
            }
        }

        /// <summary>
        /// 实现在主线程内, 异步下载
        /// </summary>
        /// <param name="go"></param>
        /// <param name="param"></param>
        /// <returns></returns>
        public async Task<bool> DownloadFromServer(long totalSize)
        {
            long          alreadyDownloadSize = 0;
            
            DownRequest[] requestArray         = new DownRequest[mAsyncDownloadCount];

            int nowDownCount = 0;
            for (int i = 0; i < mAsyncDownloadCount; ++i)
            {
                DownRequest request = StartOnceDownload();
                if (request == null)
                    break;
                requestArray[i] = request;
                ++nowDownCount;
            }

            while (nowDownCount > 0)
            {
                if (mCanncel.IsCancellationRequested)
                    break;
                long downloadingSize = 0;
                for (int i = 0; i < nowDownCount; ++i)
                {
                    if (requestArray[i] == null)
                        continue;
                    var request = requestArray[i].mRequest;
                    if (request.isDone)
                    {
                        if (request.result != UnityWebRequest.Result.Success)
                        {
                            LOG.logError("Error = " + request.error + " : " + requestArray[i].mUrl);
                            mOnDownloadFunction(requestArray[i].mDownFileRecord, null);
                        }
                        else
                        {
                            alreadyDownloadSize += (long)request.downloadedBytes;
                            mOnDownloadFunction(requestArray[i].mDownFileRecord, request.downloadHandler.data);
                        }
                        requestArray[i] = StartOnceDownload();
                    }
                    else
                    {
                        downloadingSize += (long)request.downloadedBytes;
                    }
                }

                mDownloadProcessFunction(alreadyDownloadSize + downloadingSize, totalSize);
                
                bool bAllFinish = true;
                for (int i = 0; i < nowDownCount; ++i)
                {
                    if (requestArray[i] != null)
                    {
                        bAllFinish = false;
                        break;
                    }
                }

                if (bAllFinish)
                    break;
                else
                    await Task.Delay(1);
            }

            
            for (int i = 0; i < mAsyncDownloadCount; ++i)
            {
                DownRequest request = StartOnceDownload();
                if (request != null && request.mRequest!=null)
                    request.mRequest.Dispose();
            }
            
            return (mUpdateList.Count <= 0);

        }
    }


    //public class DownloadUpdate
    //{
    //    public string mServerUrl;
    //    public List<DataRecord> mUpdateList;

    //    public delegate void OnDownload(DataRecord downRecord, byte[] data);
    //    public OnDownload mOnDownloadFunction;

    //    public int mAsyncDownloadCount = 20;

    //    public CancellationTokenSource mCanncel = new CancellationTokenSource();

    //    /// <summary>
    //    /// 下载资源数据
    //    /// </summary>
    //    /// <param name="url"></param>
    //    /// <param name="localSavePath"></param>
    //    /// <returns></returns>
    //    public async Task<bool> Download(String serverUrl, List<DataRecord> updateList, OnDownload onCallFunction, int asyncCount = 20)
    //    {
    //        mServerUrl = serverUrl;
    //        mUpdateList = updateList;
    //        mOnDownloadFunction = onCallFunction;

    //        mAsyncDownloadCount = asyncCount;
    //        return (bool)(await MainCoroutine.StartMainTask(DownloadFromServer, new object[] { this }, mCanncel));
    //    }

    //    public class DownRequest
    //    {
    //        public UnityWebRequest mRequest;
    //        public DataRecord mDownFileRecord;
    //        public string mUrl;
    //    }

    //    DownRequest StartOnceDownload()
    //    {
    //        if (mUpdateList.Count <= 0)
    //            return null;

    //        DataRecord re = mUpdateList[0];
    //        mUpdateList.RemoveAt(0);


    //        var request = new DownRequest();

    //        request.mDownFileRecord = re;

    //        string url = mServerUrl + (string)re.getString("NAME");
    //        request.mUrl = url;
    //        request.mRequest = UnityWebRequest.Get(url);
    //        request.mRequest.certificateHandler = new UpdateResourceMgr.BypassCertificate();

    //        try
    //        {
    //            request.mRequest.SendWebRequest();                 //请求下载
    //            return request;
    //        }
    //        catch (Exception e)
    //        {
    //            LOG.logError("Downlad fail : " + url + " Error : " + e.ToString());                
    //            mOnDownloadFunction(re, null);
    //            return StartOnceDownload();
    //        }
    //    }

    //    /// <summary>
    //    /// 实现在主线程内, 异步下载
    //    /// </summary>
    //    /// <param name="go"></param>
    //    /// <param name="param"></param>
    //    /// <returns></returns>
    //    public IEnumerator DownloadFromServer(MainCoroutine go, object[] param)
    //    {

    //        DownRequest[] requestArray = new DownRequest[mAsyncDownloadCount];

    //        int nowDownCount = 0;
    //        for (int i=0; i<mAsyncDownloadCount; ++i)
    //        {
    //            DownRequest request = StartOnceDownload();
    //            if (request == null)
    //                break;
    //            requestArray[i] = request;
    //            ++nowDownCount;
    //        }



    //        while (nowDownCount>0)
    //        {
    //            for (int i = 0; i < nowDownCount; ++i)
    //            {
    //                if (requestArray[i]==null)
    //                    continue;
    //                var request = requestArray[i].mRequest;
    //                if (request.isDone)
    //                {
    //                    if (request.result != UnityWebRequest.Result.Success)
    //                    {                            
    //                        LOG.logError("Error = " + request.error + " : " + requestArray[i].mUrl);
    //                        mOnDownloadFunction(requestArray[i].mDownFileRecord, null);
    //                    }
    //                    else
    //                    {                            
    //                        mOnDownloadFunction(requestArray[i].mDownFileRecord, request.downloadHandler.data);
    //                    }
    //                    requestArray[i] = StartOnceDownload();
    //                }
    //            }
    //            bool bAllFinish = true;
    //            for (int i = 0; i < nowDownCount; ++i)
    //            {
    //                if (requestArray[i]!=null)
    //                {
    //                    bAllFinish = false;
    //                    break;
    //                }
    //            }

    //            if (bAllFinish)                
    //                break;                    
    //             else
    //                yield return 1;
    //        }

    //        go.FinishResult(mUpdateList.Count<=0);

    //    }
    //}

}
