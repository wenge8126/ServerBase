
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using UnityEngine;
using UnityEngine.Networking;

namespace Logic
{
    /// <summary>
    /// 更新工作者, 主要用于提供URL地址, 显示更新界面
    /// </summary>
    public interface IUpdateWorker
    {
        public void StartUpdate(string typeInfo, out string url);
        public void ShowInfo(string info);
        public void BeginDownload(Int64 totalSize, Int64 alreadyDownloadSize);
        public void UpdateProcess(Int64 downloadSize);
        public void FinishUpdate(bool bSucceed, string info);

        public void OnUpdateFail(List<string> failResources);

        public void Clean();
    }

    public class TestUpdateWorker : IUpdateWorker
    {
        public  void Clean(){}
        public void StartUpdate(string typeInfo, out string url)
        {
            url = GameCommon.DownloadAddr;
            LOG.log("Start update : "+typeInfo);
        }

        public void ShowInfo(string info)
        {
            LOG.log("Update info : "+info);
        }

        public void BeginDownload(Int64 totalSize, Int64 alreadyDownloadSize)
        {
            LOG.log("Need update all size : "+totalSize);
        }

        public void UpdateProcess(Int64 downloadSize)
        {
            LOG.log("Now already download : " + downloadSize.ToString());
        }

        public void FinishUpdate(bool bSucceed, string info)
        {
            LOG.log("Update finsih : "+(bSucceed?" OK": "Fail")+", info : "+info);
        }

        public void OnUpdateFail(List<string> failResources)
        {
            LOG.logError("---------- Have update fail --------");
            List<string> resList = new List<string>();
            foreach (var re in failResources)
            {
                string name = re;
                LOG.logError("Update fail : " + name);
            }
            LOG.logError("-------------------------------------");
        }
    }

    /// <summary>
    /// 本地资源路径, 相对于可写目录的相对路径目录
    /// </summary>
    public class LocalResourcePath
    {
        static public string PatchPath   = "patch_dll/";
        
        static public string ConfigTablePath    = "config/";
        static public string CSVConfigTablePath = "csv_config/";
        static public string GameResourcePath   = "downloaded/files/"; // index/  blob/
        
        static public string LuaCodePath    = "lua_code/";

        static public string PatchListName = "and_patch_list";

        static public string ConfigListName       = "config_list";
        static public string CSVConfigListName    = "csv_config_list";
        static public string ResourceListName     = "resource_list";
 
        static public string LuaCodeListName  = "lua_list";

        static public string PatchMD5Name     = "and_patch_md5";
        static public string ConfigMD5Name    = "config_md5";
        static public string CSVConfigMD5Name = "csv_config_md5";
        static public string ResourceMD5Name  = "resource_md5";
        static public string LuaCodeMD5Name   = "luacode_md5";

    }

    public enum EUpdateType
    {
        eUpdataType_Default = 0,
        eUpdateType_Resourse,
        eUpdateType_Config,
        eUpdateType_Lua,
        eUpdateType_Path,
        eUpdateType_Csv,
    }
    
    /// <summary>
    /// 更新功能
    /// 1 更新文件全部zip压缩, 无扩展名
    /// 2 更新列表内保存MD5, SIZE, ZIP_SIZE
    /// 3 更新列表文件压缩后, 后面加上MD5
    /// </summary>
    public class UpdateResourceMgr
    {
        static public IUpdateWorker msUpdateWorker = new TestUpdateWorker();
        static public string mDownloadUrl = GameCommon.DownloadAddr;
        static public string        mUpdateFileExt = "";


        static public string GetUpdateResourceFullPath(string resourcePath, string resourceName)
        {
#if UNITY_EDITOR
            return "D:/TestUpdate/" + GameCommon.Version +"/"+  resourcePath + resourceName;
#else
            return GameCommon.MakeGamePathFileName(resourcePath+resourceName);
#endif

        }

        /// <summary>
        /// 读取指定文件
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        static public byte[] LoadFileData(string filePath)
        {
            if (!File.Exists(filePath))
            {
                LOG.logWarn("No exist update file : " + filePath);
                return null;
            }

            using (FileStream fs = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                var data = new byte[fs.Length];
                if (!fs.CanRead || (int)fs.Length <= 0 || fs.Read(data, 0, (int)fs.Length) != (int)fs.Length)
                {
                    LOG.logError("Read dll fail : " + filePath);
                    return null;
                }
                fs.Close();
                return data;
            }
        }

        static public byte[] LoadUpdateFileData(string resourcePath, string resourceName)
        {
            resourceName = resourceName.ToLower();
            return LoadFileData(GetUpdateResourceFullPath(resourcePath, resourceName));
        }

        static public byte[] LoadZipFile(string filePath)
        {
            byte[] data = LoadFileData(filePath);
            if (data != null)
            {
                return ZipTool.UnGZip(data, $"File {filePath} data unzip fail");
            }
            return null;
        }


        /// <summary>
        /// 解压缩数据
        /// </summary>
        /// <param name="resourceData"></param>
        /// <returns></returns>
        static public byte[] UnZipData(byte[] resourceData)
        {
            //if (resourceData.Length <= 32)
            //    return null;
            try
            {
                var scrData = ZipTool.UnGZip(resourceData);
                return scrData;
            }
            catch (Exception e)
            {
                LOG.logError("Unzip update data fail : " + resourceData + " : " + e.ToString());
            }
            return null;
        }

        /// <summary>
        /// 加载可写空间中的DLL补丁
        /// </summary>
        /// <param name="dllName"></param>
        /// <returns></returns>
        static public bool LoadRunDll(string dllPath, string dllName)
        {
            // var dllData = LoadUpdateFileData(dllPath, dllName);
            // if (dllData != null)
            // {
            //     LOG.log($"loading dll {dllName} ...");
            //     var sw = Stopwatch.StartNew();
            //     try
            //     {
            //         PatchManager.Load(new MemoryStream(dllData));
            //         LOG.log($"=== patch {dllName} ok, using { sw.ElapsedMilliseconds }  ms");
            //         return true;
            //     }
            //     catch (Exception e)
            //     {
            //         LOG.DebugUI($"XXXXXXX loading dll {dllName} fail : " + e.ToString(), true);                    
            //     }
            // }
            // else
            //     LOG.logError("Dll patch data read fail : " + dllName);

            return false;
        }

        /// <summary>
        /// 从可写空间读取加载资源表格(二进制数据)
        /// </summary>
        /// <param name="tableResourceName"></param>
        /// <returns></returns>
        static public NiceTable LoadTableFromUpdate(string resourcePath, string tableResourceName)
        {
            var data = LoadUpdateFileData(resourcePath, tableResourceName);

            if (data != null)
            {
                NiceTable t = new NiceTable();
                var d = new DataBuffer(data);
                if (t.restore(ref d))
                    return t;
                else
                    LOG.logError("Resote table fail : " + tableResourceName);
            }
            else
                LOG.logWarn("Table update data no exist : " + tableResourceName);

            return null;
        }

        /// <summary>
        /// // 检查更新
        /// 1 下载 udpate_md5.txt,  里面是更新列表的数据生成的 MD5, 与本地比对, 不一至, 2
        /// 2 下载 update_list.bymb,  获取最新的更新列表
        /// 3 根据 更新列表, 分别读取本地资源文件尾部的md5, 把不一至的, 加入到待更新列表
        /// 4 根据更新列表, 逐个更新下载, 并保存到的本地
        /// 5 全部下载完成后, 现次一一检查MD5, 无误后, 保存 update_md5.txt 到本地
        /// </summary>
        /// <returns></returns>
  
        public struct UpdateResult
        {
            public bool mbSucceed;
            public bool mbHaveUpdate;
        }
        
        public async static Task<UpdateResult> CheckAndUpdateRecources(EUpdateType updateType, string updateTypeInfo, bool bZip = true, bool bCheckLocal = false)
        {
            UpdateResult result = new UpdateResult();
            result.mbHaveUpdate = false;
            result.mbSucceed    = false;
            if (msUpdateWorker == null)
            {
                LOG.logError("No set update worker");
                return result;
            }

            string updateTypePath = "default/";
            string listName       = "default_list";
            string md5FileName    = "default_md5";
            switch (updateType)
            {
                case EUpdateType.eUpdateType_Config:
                    updateTypePath = LocalResourcePath.ConfigTablePath;
                    listName       = LocalResourcePath.ConfigListName;
                    md5FileName    = LocalResourcePath.ConfigMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Path:
                    updateTypePath = LocalResourcePath.PatchPath;
                    listName       = LocalResourcePath.PatchListName;
                    md5FileName    = LocalResourcePath.PatchMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Lua:
                    updateTypePath = LocalResourcePath.LuaCodePath;
                    listName       = LocalResourcePath.LuaCodeListName;
                    md5FileName    = LocalResourcePath.LuaCodeMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Resourse:
                    updateTypePath = LocalResourcePath.GameResourcePath;
                    listName       = LocalResourcePath.ResourceListName;
                    md5FileName    = LocalResourcePath.ResourceMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Csv:
                    updateTypePath = LocalResourcePath.CSVConfigTablePath;
                    listName       = LocalResourcePath.CSVConfigListName;
                    md5FileName    = LocalResourcePath.CSVConfigMD5Name;
                    break;
            }

            string resourcesPath = updateTypePath;
            
            
            
            msUpdateWorker.StartUpdate(updateTypeInfo, out mDownloadUrl);
            var versionMD5Data = await DownLoadResource(resourcesPath, md5FileName);
            if (versionMD5Data == null)
            {
                //LOG.logError("Web load ver md5 fail : update_md5");
                msUpdateWorker.FinishUpdate(false, "Web load ver md5 fail : "+md5FileName);
                return result;
            }
            // 使用最新MD5 与 本地的资源MD5比较
            string lastMD5 = StaticDefine.GetString(versionMD5Data, 0, versionMD5Data.Length);
            var localListData =  LoadFileData(GetUpdateResourceFullPath(resourcesPath, listName) );
            string localMD5 = "";
            if (localListData == null)
            {
                LOG.logWarn($" Local {listName} no exist");
            }
            else
                localMD5 = StaticDefine.MakeMD5(localListData, localListData.Length);            

            if (lastMD5 != localMD5)
                LOG.log($"Need update : now {lastMD5} != {localMD5} local md5");
            else
            {
                LOG.log($"Now new version : now {lastMD5} == {localMD5} local md5");
                result.mbSucceed = true;
                
                return result;
            }
            
            // 加载资源列表
            
            
            var listTableZipData = await DownLoadResource(resourcesPath, listName);            
            if (listTableZipData == null)
            {
                string info = ("Web load ver md5 fail : "+listName);
                msUpdateWorker.FinishUpdate(false, info);
                
                return result;
            }
           
            var listTableData = ZipTool.UnGZip(listTableZipData, $"{listName} unzip fail");
            if (listTableData == null)
            {
                string info = ("Web load ver md5 fail : "+listName);
                msUpdateWorker.FinishUpdate(false, info);
                
                return result;
            }
            string listMD5 = StaticDefine.MakeMD5(listTableData, listTableData.Length);
            if (listMD5 != lastMD5)
            {
                string info  = ($"{listName} MD5 check fail : {listMD5}!={lastMD5} last md5");
                msUpdateWorker.FinishUpdate(false, info);
               
                return result;
            }
           
            var listTable = new NiceTable();
            var tableBuffer = new DataBuffer(listTableData);
            if (!listTable.restore(ref tableBuffer))
            {
                string info =  ($"{listName} table read restore fail");
                msUpdateWorker.FinishUpdate(false, info);
                
                return result;
            }
            
            
            // 检查需要的更新
            // 本地MD5索引
            NiceTable localListTable = null;
            if (localListData != null)
            {
                localListTable = new NiceTable();
                DataBuffer d = new DataBuffer(localListData);
                if (!localListTable.restore(ref d))
                {
                    LOG.logError("Local list table read restor fail : " + listName);
                    localListTable = null;
                }
                LOG.log($"{listName} *** Local ver : {localListTable.mTableIndex}");
            }
            else
                LOG.log($"{listName} !!! Local ver : no exist");
            LOG.log($"{listName} &&& Newest ver : {listTable.mTableIndex}");

            bool bExistLocalList = localListTable != null;
            if (localListTable == null)
            {
                localListTable = new NiceTable();
                for (int i = 0; i < listTable.GetField().Count(); ++i)
                {
                    var fieldInfo = listTable.GetField().GetField(i);
                    localListTable.SetField(fieldInfo.getName(), fieldInfo.getType(), i);
                }
            }

            var updateList = new List<DataRecord>();
            Int64 totalSize = 0;
            Int64 totalDownloadSize = 0;

            if (listTable.GetRecordCount() > 1000 || updateType== EUpdateType.eUpdateType_Resourse)
                msUpdateWorker.BeginDownload(totalSize, totalDownloadSize);
#if !UNITY_EDITOR
            if (listTable.GetRecordCount()>0 && updateType == EUpdateType.eUpdateType_Resourse)
            {
                // 如果需要更新资源,需要重新导出,不然有可能会保留上一次更新的资源
                //??? 暂时不使用,可能考虑使用手工恢复 await CnPatchHelper.ExportFromResourcePakToPersistentDataPath(true);
            }
#endif
            bool bNeedSaveLocalList = false;
            foreach (var v in listTable.GetAllRecord().Values)
            {
                string name = (string)v.getObject("NAME");
                string md5 = (string)v.getObject("MD5");
                
                if (bExistLocalList)
                {
                    string resMd5 = "";
                    var    r      = localListTable.GetRecord(name);
                    if (r != null)
                    {
                        resMd5 = (string)r.getObject("MD5");
                        if (md5 == resMd5)
                            continue;
                    }
                }

                totalSize += (int)v.getObject("BYMD_SIZE");
                if (bCheckLocal)
                {
                    var d = LoadUpdateFileData(resourcesPath, name);
                    if (d != null && StaticDefine.MakeMD5(d, d.Length) == md5)
                    {
                        totalDownloadSize += (int)v.getObject("BYMD_SIZE");
                        var resRe = localListTable.CreateRecord(name);
                        resRe.set("MD5", md5);
                        resRe.set("SIZE", d.Length);
                        resRe.set("BYMD_SIZE", v.getInt("BYMD_SIZE"));
                        bNeedSaveLocalList = true;
                        continue;
                    }
                }
                
                updateList.Add(v);
            }
            
            if (bNeedSaveLocalList)
            {
                var tableData2 = new DataBuffer();
                if (localListTable.SortSaveData(ref tableData2, true))
                    SaveLocalFile(resourcesPath, listName, tableData2.mData, tableData2.tell());
            }

            if (updateList.Count > 0)
            {
                result.mbHaveUpdate = true;
                msUpdateWorker.BeginDownload(totalSize, totalDownloadSize);
                
                msUpdateWorker.StartUpdate($"{ updateTypeInfo}", out mDownloadUrl);
            }
            // 批量下载
            var lastList          = new List<DataRecord>();

            DownloadUpdate.OnDownload OnDownload = (DataRecord downRecord, byte[] data) =>
            {
                string name = (string) downRecord.getObject("NAME");
                string md5 = (string) downRecord.getObject("MD5");
                //msUpdateWorker.ShowInfo("Begin download : "+name);

                if (data == null)
                {
                    string info = ("Update fail : " + name);
                    msUpdateWorker.ShowInfo(info);
                    lastList.Add(downRecord);
                    return;
                }

                byte[] resData = null;
                if (bZip)
                    resData = ZipTool.UnGZip(data, $"Download {name} unzip fail");
                else
                    resData = data;
                if (resData == null)
                {
                    string info = ("Unzip fail : " + name);
                    msUpdateWorker.ShowInfo(info);
                    lastList.Add(downRecord);
                    return;
                }

                // 检查MD5
                string resmd5 = StaticDefine.MakeMD5(resData, resData.Length);
                if (md5 != resmd5)
                {
                    string info = ($"{name} MD5 check fail : {md5}!={resmd5} download");
                    msUpdateWorker.ShowInfo(info);
                    lastList.Add(downRecord);
                    return;
                }

                // 落地
                SaveLocalFile(resourcesPath, name, resData, 0);

                var resRe = localListTable.CreateRecord(name);
                resRe.set("MD5", md5);
                resRe.set("SIZE", resData.Length);
                resRe.set("BYMD_SIZE", data.Length);
                totalDownloadSize += data.Length;
                //msUpdateWorker.UpdateProcess(totalDownloadSize);
            };
            

            var loader = new DownloadUpdate();
            bool bResult = await loader.Download(mDownloadUrl + resourcesPath, updateList, OnDownload, totalSize
                    , (nowSize, allSize) =>
                    {
                        msUpdateWorker.UpdateProcess(nowSize);
                    }
                    , 10
                );

            int nUpdateResult = (bResult && lastList.Count <= 0) ? 1 : 2;
            
            
            if (!bResult || lastList.Count > 0)
            {
                LOG.log("---------- Have update fail --------");
                List<string> resList = new List<string>();
                foreach (var re in lastList)
                {
                    string name = re.get("NAME");
                    LOG.log("Update fail : " + name);
                    resList.Add(name);
                }
                LOG.log("----------- Update fail --------------");
                msUpdateWorker.OnUpdateFail(resList);
                var tableData2 = new DataBuffer();
                if (localListTable.SortSaveData(ref tableData2, true))
                    SaveLocalFile(resourcesPath, listName, tableData2.mData, tableData2.tell());
                else
                    LOG.logError($"{listName} table save fail");
                LOG.DebugUI($"XXX All update fail : {listName} Ver : {listTable.mTableIndex}", true);
                msUpdateWorker.FinishUpdate(false, "Exist update fail resource");
            }
            else
            {
                LOG.log("=======All update ok======= : "+listName);
                // 全部成功, 才落地保存 update_list
                SaveLocalFile(resourcesPath, listName, listTableData, 0);
                LOG.DebugUI($"===All update ok : {listName} Ver : {listTable.mTableIndex}", true);
                msUpdateWorker.FinishUpdate(true, "=======All update ok======= : "+listName);
                result.mbSucceed = true;
                return result;
            }
            
            return result;
        }

        /// <summary>
        /// 应用更新所有的DLL补丁代码
        /// </summary>
        /// <returns></returns>
        public static bool UpdatePatchDllCode()
        {
            
            var path = LocalResourcePath.PatchPath;
            var dllListTable = LoadTableFromUpdate( LocalResourcePath.PatchPath, LocalResourcePath.PatchListName);


            if (dllListTable != null)
            {
                LOG.Show(">>>>>>>> Start patch version : " + dllListTable.mTableIndex);
                bool bSucceed = true;
                foreach (var v in dllListTable.GetAllRecord().Values)
                {
                    var dllName = v["NAME"];
                    if (!LoadRunDll(path, dllName))
                    {
                        bSucceed = false;
                        //LOG.logError("Load dll fail : " + dllName);
                    }
                }
                string verInfo = dllListTable.mTableIndex+" === Result : "+(bSucceed?"OK":"XXX FAIL");
                
                LOG.Show(">>>>>>>> Patch version : " + verInfo);
                LOG.DebugUI("Patch version: " + verInfo, true);
                return bSucceed;
            }
            else
            {
                LOG.log("No exist dll list update resource, may not need patch code");
                return true;
            }
        }

        /// <summary>
        /// 在可写空间, 保存或替换文件
        /// </summary>
        /// <param name="filePath"></param>
        /// <param name="data"></param>        
        public static void SaveLocalFile(string resourcePath, string filePath, byte[] data, int length)
        {
            if (length > data.Length || length <= 0)
                length = data.Length;
            string path = GetUpdateResourceFullPath(resourcePath, filePath);
            if (File.Exists(path))
                File.Delete(path);
            else
                CreateDirctory(SplitPath(path));
            using (var fs = new FileStream(path, FileMode.Create))
            {
                fs.Write(data, 0, length);
                fs.Close();
            }
        }

        public static void SaveFile(string fullFilePath, byte[] data, int length)
        {
            if (length > data.Length || length<=0)
                length = data.Length;
           
            string path = fullFilePath;
            if (File.Exists(path))
                File.Delete(path);
            else
                CreateDirctory(SplitPath(path));
            using (var fs = new FileStream(path, FileMode.Create))
            {
                fs.Write(data, 0, length);
                fs.Close();
            }
        }

        public static string SplitPath(string fullPath)
        {
            int pos = fullPath.LastIndexOf('/');
            if (pos > 0)
                return fullPath.Substring(0, pos);
            else
                return null;
        }

        /// <summary>
        /// 创建多级目录, 如果path 结尾不是 / 最后的串不认为是目录, 不创建
        /// </summary>
        public static void CreateDirctory(string path)
        {
            path = path.Replace('\\', '/');
            string[] pathList = path.Split(new char[] { '/' });
            string dir = "";

            for (int i = 0; i < pathList.Length; ++i)
            {
                if (i > 0)
                    dir += "/";
                if (string.IsNullOrEmpty(pathList[i]))
                    continue;
                dir += pathList[i];
                if (!Directory.Exists(dir))
                    Directory.CreateDirectory(dir);
            }
        }

        /// <summary>
        /// 下载资源文件, url = 固定地址 + resourceName + 扩展名 (.bymd : 二进制数据+md5)
        /// </summary>
        /// <param name="resourceName"></param>
        /// <returns></returns>
        async public static Task<byte[]> DownLoadResource(string resourcePath, string resourceName)
        {
            var data = await Download(mDownloadUrl + resourcePath + resourceName + mUpdateFileExt);
            return data;

            //if (mDownloadTool==null)
            //{
            //    LOG.logError("Download tool is not init");
            //    return null;
            //}
            //var data = await mDownloadTool.StartDownload(mDownloadUrl+resourceName+mUpdateFileExt, resourceName);

            ////string result = data != null ? data.Length.ToString() : "fail xxx";
            ////LOG.log($"========= Download finish : comlib, size : {result}");
            //return data;
        }

        /// <summary>
        /// 是否中止取消下载, 设置此值为true中止取消
        /// </summary>        
        static public bool mbCancelDownload = false;


        /// <summary>
        /// 下载资源数据
        /// </summary>
        /// <param name="url"></param>
        /// <param name="localSavePath"></param>
        /// <returns></returns>
        public static async Task<byte[]> Download(string url)
        {
            try
            {
                using (UnityWebRequest request = UnityWebRequest.Get(url))
                {
                    request.certificateHandler = new BypassCertificate();

                    try
                    {
                        var op = request.SendWebRequest(); //请求下载
                        while (!op.isDone)
                        {
                            await Task.Delay(10);
                        }
                    }
                    catch (Exception e)
                    {
                        LOG.logError("Downlad fail : " + url + " Error : " + e.ToString());
                        return null;
                    }

                    // while (!request.isDone)
                    // {
                    //     //LOG.log(request.downloadProgress.ToString());  //下载进度
                    //     await Task.Delay(1);
                    // }
                    if (request.result !=
                        UnityWebRequest.Result
                            .Success) // request.result == UnityWebRequest.Result.ConnectionError || request.result == UnityWebRequest.Result.ProtocolError)
                    {
                        LOG.logError("Error = " + request.error + " : " + url); //下载出错
                        return null;
                    }
                    else if (request.isDone) //下载完成
                    {
                        byte[] bytes = null;
                        if (request.downloadHandler.data != null)
                        {
                            bytes = new byte[request.downloadHandler.data.Length];
                            Array.Copy(request.downloadHandler.data, bytes, request.downloadHandler.data.Length);
                        }

                        if (bytes != null)
                            LOG.log("Download ok : " + bytes.Length.ToString() + " url : " + url);
                        else
                            LOG.log("Download fail , url : " + url);
                        return bytes;
                    }
                }
            }
            catch (Exception e)
            {
                LOG.log($"Download {url} exception :\r\n{e.ToString()}");
            }

            return null;
        }

        /// <summary>
        /// SSL 过滤, 必须直接返回true, 不然在手机端 显示 下载错误 : 
        /// </summary>
        public class BypassCertificate : CertificateHandler
        {
            protected override bool ValidateCertificate(byte[] certificateData)
            {
                //undefined
                return true;
            }
        }

        /// <summary>
        /// 获取指定目录中所有的文件        
        /// </summary>
        /// <param name="sw"></param>
        /// <param name="path"></param>
        /// <param name="indent"></param>
        public static List<string> FindAllFileInPath(string path, string extName)
        {
            DirectoryInfo root = new DirectoryInfo(path);
            List<string> list = new List<string>();
            
            foreach (FileInfo f in root.GetFiles())
            {
                if (SplitFileExtName(f.Name) == extName)
                    list.Add(f.Name);
            }
            return list;
        }
        
        public static List<string> FindAllFileInPath(string path)
        {
            DirectoryInfo root = new DirectoryInfo(path);
            List<string>  list = new List<string>();
            
            foreach (FileInfo f in root.GetFiles())
            {
                list.Add(f.Name);
            }
            return list;
        }
        
        /// <summary>
        /// 获取指定目录下所有的子目录
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static List<string> FindAllDirInPath(string path)
        {
            DirectoryInfo root = new DirectoryInfo(path);
            List<string>  list = new List<string>();
            
            foreach (var f in root.GetDirectories())
            {
                list.Add(f.Name);
            }
            return list;
        }
        
        /// <summary>
        /// 递归获取目录, 及目录下子目录中所有的文件
        /// </summary>
        /// 返回的是绝对路径
        /// <param name="path"></param>
        /// <param name="resultList"></param>
        public static void RecursionFindAllFile(string path, ref List<string> resultList)
        {
            path = path.Replace('\\', '/');
            if (path.LastIndexOf("/") != path.Length - 1)
                path += "/";
            DirectoryInfo root = new DirectoryInfo(path);
            foreach (FileInfo f in root.GetFiles())
            {                
                 resultList.Add(path + f.Name);
            }

            DirectoryInfo[] dis = root.GetDirectories();//目录下的子目录
            foreach (DirectoryInfo item in dis)
            {
                RecursionFindAllFile(item.FullName, ref resultList);
            }
        }

        public static string SplitFileExtName(string fileName)
        {
            int pos = fileName.LastIndexOf('.');
            if (pos >= 0)
                return fileName.Substring(pos + 1, fileName.Length - pos - 1);

            return "";
        }

        public static string SplitFileName(string fileName)
        {
            fileName = fileName.Replace('\\', '/');
            if (fileName.IndexOf('/') >= 0)
            {
                string path = SplitPath(fileName);

                return fileName.Substring(path.Length + 1, fileName.Length - path.Length - 1);
            }
            return fileName;
        }

        public static string SplitFileNameWithoutExt(string fileName)
        {
            fileName = SplitFileName(fileName);
            int pos = fileName.LastIndexOf('.');
            if (pos >= 0)
                return fileName.Substring(0, pos);

            return "";
        }


        /// <summary>
        /// 生成更新资源
        /// </summary>
        /// <param name="resName"></param>
        /// <param name="scrFile"></param>
        /// <param name="destUpdateFile"></param>
        /// <param name="listTable"></param>
        /// <returns></returns>
        static public bool MakeUpdateFile(string resName, string scrFile, string destUpdateFile, NiceTable listTable, bool bZip)
        {
            // 全部传为小写
            resName = resName.ToLower();

            string path = scrFile;

            var data = LoadFileData(path);
            if (data == null)
            {
                LOG.logError("Read resource file fail : " + path);
                return false;
            }

            string md5String = StaticDefine.MakeMD5(data, data.Length);
            byte[] zipData = null;
            if (bZip)
                zipData = ZipTool.CompressGZip(data);
            else
                zipData = data;

            string destPath = destUpdateFile;

            if (File.Exists(destPath))
                File.Delete(destPath);
            else
                UpdateResourceMgr.CreateDirctory(UpdateResourceMgr.SplitPath(destPath));

            using (var resultFile = new FileStream(destPath, FileMode.Create))
            {
                resultFile.Write(zipData, 0, zipData.Length);
                resultFile.Close();
            }
            if (listTable != null)
            {
                var record = listTable.CreateRecord(resName);
                record.set("SIZE", data.Length);
                record.set("MD5", md5String);
                record.set("BYMD_SIZE", zipData.Length);
            }

            //LOG.log("****** Succeed make update file : " + destPath + " size : " + zipData.Length.ToString() + ", scr size : " + data.Length.ToString());

            return true;
        }

        /// <summary>
        /// 检查更新文件是否正确
        /// </summary>
        /// <param name="resourceName"></param>
        /// <param name="scrFile"></param>
        /// <param name="destUpdateFile"></param>
        /// <param name="listTable"></param>
        /// <returns></returns>
        static public bool CheckFile(string resourceName, string scrFile, string destUpdateFile, NiceTable listTable, bool bZip)
        {
            resourceName = resourceName.ToLower();
            var indexRecord = listTable.GetRecord(resourceName);
            if (indexRecord == null)
            {
                LOG.logError("No exist index in update list : " + resourceName);
                return false;
            }

            string md5String = "";
            if (true)
            {
                string path = scrFile;

                var data = UpdateResourceMgr.LoadFileData(path);
                if (data == null)
                {
                    LOG.logError("Read udpate dll fail : " + path);
                    return false;
                }

                md5String = StaticDefine.MakeMD5(data, data.Length);

                int size = indexRecord.get("SIZE");
                string md = indexRecord.get("MD5");
                if (size != data.Length || md != md5String)
                {
                    LOG.logError($"{resourceName} : Now resource size {data.Length.ToString()}!= index {size.ToString()}, or Now md5 {md5String} != index md5 {md}");
                    return false;
                }
            }


            byte[] scrData = bZip ? LoadZipFile(destUpdateFile) : LoadFileData(destUpdateFile);

            var nowMD5 = StaticDefine.MakeMD5(scrData, scrData.Length);

            if (nowMD5 != md5String)
            {
                LOG.logError($"Unzip dll md5 {nowMD5} != now dll md5 {md5String}");
                return false;
            }

            LOG.log(" ok ok == Check succeed : " + destUpdateFile);
            return true;
        }

        /// <summary>
        /// 检查生成的配置是否正确
        /// </summary>
        /// resourceList KEY: 资源索引名, 源文件路径
        /// <returns></returns>
        static public bool _CheckUpdateFile(string updatePath, string listName, string md5FileName, Dictionary<string, string> resourceList, bool bZip = true)
        {
            updatePath = Application.dataPath + updatePath;

            NiceTable mUpdateListTable = new NiceTable();
            string updateTableName = updatePath + listName + UpdateResourceMgr.mUpdateFileExt;
           
            var scrData = UpdateResourceMgr.LoadZipFile(updateTableName);
            if (scrData == null)
            {
                LOG.logError("Load udpate list data fail : " + updateTableName);
                return false;
            }

            string destMD5 = StaticDefine.MakeMD5(scrData, scrData.Length);

            var bufferData = new DataBuffer(scrData);
            if (!mUpdateListTable.restore(ref bufferData))
            {
                LOG.logError("Load udpate list read restore fail : " + updateTableName);
                return false;
            }
            LOG.log($"*** Update {listName} ver : {mUpdateListTable.mTableIndex}");

            var listMD5File = updatePath + md5FileName + UpdateResourceMgr.mUpdateFileExt;
            var md5Data = UpdateResourceMgr.LoadFileData(listMD5File);
            var verMD5 = StaticDefine.GetString(md5Data, 0, md5Data.Length);
            if (verMD5 != destMD5)
            {
                LOG.logError($"Udpate list md5 {destMD5} != {verMD5} :  " + listMD5File);
                return false;
            }

            int count = 0;
            bool bError = false;
            foreach (var kv in resourceList)
            {
                string fileName = kv.Key;
                string destName = updatePath + fileName.ToLower() + UpdateResourceMgr.mUpdateFileExt;
                string path = kv.Value;
                if (!UpdateResourceMgr.CheckFile(fileName, path, destName, mUpdateListTable, bZip))
                    bError = true;
                else
                    ++count;
            }

            if (bError)
            {
                LOG.logError("检查更新下载文件失败, 存在错误, 请检查处理后, 重新生成");
                return false;
            }
            else
                LOG.log("ok ok  检查更新下载文件全部成功 : " + updatePath + " :  count " + count.ToString());

            return true;
        }

        /// <summary>
        /// 生成更新下载文件
        /// </summary>
        /// <param name="updateTypePath">資源類型目錄</param>
        /// <param name="listName"></param>
        /// <param name="md5FileName"></param>
        /// <param name="resourceList"></param>
        /// <returns></returns>
        static public bool _MakeUpdateFile(EUpdateType updateType, Dictionary<string, string> resourceList, string version = "", bool bZip = true, bool bBasePath = false)
        {
            // 0 删除目标目录中所有的可更新文件
            // 1 创建索引表, SIZE: 原文件大小, MD5: 原文件MD5, NAME : 文件名(无扩展名)
            // 2 产生所有的更新下载文件
            // 3 生成更新列表下载文件 同下载资源一样处理
            // 4 保存下载列表二进制生成的MD5 : md5FileName, 检查是否需要更新使用此文件中的MD5检查
            string updateTypePath = "default/";
            string listName       = "default_list";
            string md5FileName    = "default_md5";
            switch (updateType)
            {
                case EUpdateType.eUpdateType_Config:
                    updateTypePath = LocalResourcePath.ConfigTablePath;
                    listName       = LocalResourcePath.ConfigListName;
                    md5FileName    = LocalResourcePath.ConfigMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Path:
                    updateTypePath = LocalResourcePath.PatchPath;
                    listName       = LocalResourcePath.PatchListName;
                    md5FileName    = LocalResourcePath.PatchMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Lua:
                    updateTypePath = LocalResourcePath.LuaCodePath;
                    listName       = LocalResourcePath.LuaCodeListName;
                    md5FileName    = LocalResourcePath.LuaCodeMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Resourse:
                    updateTypePath = LocalResourcePath.GameResourcePath;
                    listName       = LocalResourcePath.ResourceListName;
                    md5FileName    = LocalResourcePath.ResourceMD5Name;
                    break;
                
                case EUpdateType.eUpdateType_Csv:
                    updateTypePath = LocalResourcePath.CSVConfigTablePath;
                    listName       = LocalResourcePath.CSVConfigListName;
                    md5FileName    = LocalResourcePath.CSVConfigMD5Name;
                    break;
            }
            
            if (version == "")
                version = GameCommon.Version +" : " + DateTime.Now.ToString();
            
            //updatePath = Application.dataPath + "/" + updatePath;
            string updatePath = Application.dataPath + "/" + GameCommon.MakeUpdatePath(bBasePath) + updateTypePath;

            if (Directory.Exists(updatePath))
                Directory.Delete(updatePath, true);

            NiceTable mUpdateListTable = new NiceTable();
            int i = 0;
            mUpdateListTable.SetField("NAME", FIELD_TYPE.FIELD_STRING, i++);
            mUpdateListTable.SetField("SIZE", FIELD_TYPE.FIELD_INT, i++);
            mUpdateListTable.SetField("MD5", FIELD_TYPE.FIELD_STRING, i++);        // 原文件的MD5
            mUpdateListTable.SetField("BYMD_SIZE", FIELD_TYPE.FIELD_INT, i++);      // 下载文件的大小, 用于统计下载进度

            bool bError = false;
            int count = 0;
            foreach (var vk in resourceList)
            {
                string fileName = vk.Key;
                string destName = updatePath + fileName.ToLower() + UpdateResourceMgr.mUpdateFileExt;
                string path = vk.Value;
                if (!UpdateResourceMgr.MakeUpdateFile(fileName, path, destName, mUpdateListTable, bZip))
                    bError = true;
                else
                {
                    ++count;

                }
            }

            if (bError)
            {
                LOG.logError("生成更新下载文件失败, 存在错误, 请检查处理后, 重新生成 : " + listName);
            }
            else
            {
                if (true)
                {
                    DataBuffer data = new DataBuffer();
                    mUpdateListTable.mTableIndex = version;
                    if (mUpdateListTable.SortSaveData(ref data, true))
                    {
                        string listTableName = updatePath + "/Log/" + listName + ".table";
                        SaveFile(listTableName, data.mData, data.tell());

                        mUpdateListTable.SaveTable(updatePath + "/Log/" + listName + ".csv", Encoding.UTF8);

                        string destName = updatePath + listName + mUpdateFileExt;
                        // 列表都是压缩的
                        if (MakeUpdateFile("list", listTableName, destName, null, true))
                        {
                            string listMD5 = StaticDefine.MakeMD5(data.mData, data.tell());
                            SaveFile(updatePath + md5FileName + mUpdateFileExt, StaticDefine.GetBytes(listMD5), 0);

                            LOG.log("ok ok  生成更新下载文件全部成功 : " + updatePath + " :  count " + count.ToString());

                            return true;
                        }
                        else
                            LOG.logError("生成更新列表下载文件失败 : " + destName);
                    }
                }
            }
            return false;
        }


    }
}
