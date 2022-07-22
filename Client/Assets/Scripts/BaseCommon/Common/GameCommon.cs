using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Logic;

using UnityEditor;
using UnityEngine;


public class GameCommon
{
    static public readonly bool IsDevelopTest = true;
    
    // ANDROID
    static public readonly string BigAndVersion        = "2.11"; // 打包时确定大版本号
    static public          string UpdateAndroidVersion = "001";   // 安卓更新的小版本号
    // IOS
    static public readonly string BigIosVersion        = "8.00"; // 打包时确定大版本号
    static public          string UpdateIosVersion     = "001";   // 苹果更新的小版本号
    // TEST
    static public readonly string BigTestVersion = "8.00"; // 打包时确定大版本号
    static public          string TestVersion   = "001";

    static public string ResourcesAddress
    {
        get
        {
            return "";
        }
    }

    // 正式版地址
    //static public string ResourcesAddress = "https: //ryu-cdn-obs-tt.mobage.cn/ryu/";
    //https://ryu-cdn-obs-tt.mobage.cn/ryu/Android/2.11/02/
    static public string BigVersion
    {
        get
        {
#if UNITY_ANDROID
            return BigAndVersion;
#elif UNITY_IOS || UNITY_IPHONE
            return BigIosVersion;
#else
            return BigTestVersion;
#endif
        }
    }
    
    static public bool CheckBigVersion(string serverVersion)
    {
        string[] strList = serverVersion.Split('.');
        if (strList.Length >= 3)
        {
            string ver = $"{strList[0]}.{strList[1]}";
            if (ver!=BigVersion)
                LOG.log($"Server ver {ver}!= Now ver {BigVersion}");
            return ver == BigVersion;
        }

        return false;
    }
    
    static public string UpdateVersion
    {
        get
        {
#if UNITY_ANDROID
            return UpdateAndroidVersion;
#elif UNITY_IOS || UNITY_IPHONE
            return UpdateIosVersion;
#else
            return TestVersion;
#endif
        }
    }    

    static public string UpdatePath
    {
        get
        {
#if UNITY_ANDROID
            return "Android";
#elif UNITY_IOS || UNITY_IPHONE
            return "Ios";
#else
            return "Test";
#endif
        }
    }
    
    static public string Version
    {
        get
        {
#if UNITY_ANDROID
            return BigVersion + "." + UpdateAndroidVersion;
#elif UNITY_IOS || UNITY_IPHONE
            return BigVersion + "." + UpdateIosVersion;
#else
            return BigVersion + "." + TestVersion;
#endif
        }
    }
    
    static public string DownloadAddr
    {
        get
        {
            return ResourcesAddress +UpdatePath +"/" + BigVersion + "/"+UpdateVersion+"/";
        }
    }

    /// <summary>
    /// 生成更新资源路径
    /// </summary>
    /// <param name="rootPath">生成的根目錄</param>
    /// <param name="bBasePath">是否為打包時生成的原始包</param>
    /// <returns></returns>
    static public string MakeUpdatePath(bool bBasePath)
    {
        string rootPath = "../../";
        if (bBasePath)
        {
#if UNITY_ANDROID
            return rootPath + "ResourceAnd/" + BigVersion + "/";
#elif UNITY_IPHONE || UNITY_IOS
            return rootPath + "ResourceIos/" + BigVersion + "/";
#else
            return rootPath + "ResourceTest/" + BigVersion + "/";
#endif
        }
        else
        {
            var makeVersion = GameCommon.ReadMakeUpdateVersion();
            if (string.IsNullOrEmpty(makeVersion))
            {
                LOG.logError("No exist update version file, Must create and set update version");
                return rootPath + "Error/";
            }
            return rootPath + UpdatePath + "/"+BigVersion+"/"+makeVersion+"/";
        }
    }

    /// <summary>
    /// 相对大版本的生成的资源更新目录
    /// </summary>
    static public string MakeUpdateBigVersionPath
    {
        get
        {
            string rootPath = "../../";
            return rootPath + UpdatePath + "/" + BigVersion + "/";
        }
    }

    /// <summary>
    /// 读取需要制作的更新小版本号 （文件保存在平台对应的生成的更新文件目录中 UpdateAnd (或 UpdateIos) /BigVersion/make_version.txt）
    /// 每次制作生成更新文件前修改此版本号
    /// </summary>
    static public string ReadMakeUpdateVersion()
    {
        string rootPath = Application.dataPath + "/../../";

        string path    = rootPath + UpdatePath + "/" + BigVersion + "/make_version.txt";
        var    verData = UpdateResourceMgr.LoadFileData(path);
        if (verData == null)
        {
            LOG.logError($"Make version file no exsit : {path}, need create file and set make version, Examples 001");
            return "";
        }

        var makeVersion = StaticDefine.GetString(verData, 0, verData.Length);
        var verValue    = 0;
        int.TryParse(makeVersion, out verValue);
        if (verValue <= 0)
        {
            LOG.logError($"Make version file no exsit : {path}, {makeVersion} must > 0");
            return "";
        }
        
        // 遍历当前存在版本目录，如果小于或等于最大目录，返回空
        
        return makeVersion;
    }

    static public string msUIPrefabPath = "UI/Prefab/";

#if UNITY_IPHONE || !UNITY_EDITOR && (UNITY_IPHONE || UNITY_ANDROID || DEBUG_IO)
    static public bool RunOnWindow = false;
    static public bool LogToFile = true;
#else
    static public bool RunOnWindow = true;
    static public bool LogToFile = true;
#endif
    static public bool UseUpdate = false;

    public delegate void StartLua();

    static public  StartLua msStartLuaFunction = null;
    
    public delegate void YiDunSetRole(string roleId, string roleName, string roleAccount, string roleServer, int serverId, string gameJson);
    static public YiDunSetRole msYiDunSetRoleFunction = null;

    public delegate bool LoopCheckFun();
    

    static public void CheckStartLogic(string logicName, object param)
    {
        if (logicName.LastIndexOf("(Clone)")>0)
            logicName = logicName.Replace("(Clone)", "");
        // 是否存在
        if (!EventCenter.Instance.ExistEventFactory(logicName))
        {
            LOG.logWarn("No exist logic : "+logicName);
            return;
        }
        // 判断是否需要执行

        tEvent evt = EventCenter.Start(logicName);
        if (evt != null)
        {
            LOG.log("Logic start  : " + logicName);
            evt.set("PARAM", param);
            evt.Do();
        }
        else
            LOG.logError("Logic start is null : " + logicName);
    }

  
    static public void SaveJson(string jsonData)
    {
        msJsonData = jsonData;
        var d = StaticDefine.GetBytes(jsonData);
        Logic.UpdateResourceMgr.SaveLocalFile("GameData/", "json.txt", d, d.Length);
    }

    static public string msJsonData = "";
    
    static public string LoadJson()
    {
        if (!string.IsNullOrEmpty(msJsonData))
            return msJsonData;

        var d = UpdateResourceMgr.LoadUpdateFileData("GameData/", "json.txt");
        if (d != null)
            msJsonData = StaticDefine.GetString(d, 0, d.Length);
        else
            msJsonData = "{}";
        return msJsonData;
    }

    static public async Task<bool> LoopCheck(int  onceMilSec, int overMilSec, LoopCheckFun loopFun)
    {
        var sw = Stopwatch.StartNew();
        while (true)
        {
            if (loopFun())
                break;

            if (sw.ElapsedMilliseconds >= overMilSec)
                return false;

            await System.Threading.Tasks.Task.Delay(onceMilSec);
        }
        return true;
    }

    /// <summary>
    /// 二进制比较
    /// </summary>
    /// <param name="parentObject"></param>
    /// <param name="targetObjName"></param>
    /// <returns></returns>

    static public bool CompareBytes(byte[] left, byte[] right)
    {
        if (left.Length != right.Length)
            return false;

        for (int i = 0; i < left.Length; i++)
        {
            if (left[i] != right[i])
                return false;
        }

        return true;
    }

    static public GameObject FindObject(GameObject parentObject, string targetObjName)
    {
        if (parentObject == null)
            return null;

        if (parentObject.name == targetObjName)
            return parentObject;

        Transform tran = null;
        for (int i = 0; i < parentObject.transform.childCount; i++)
        {
            tran = parentObject.transform.GetChild(i);
            string n = tran.gameObject.name;

            if (n == targetObjName)
                return tran.gameObject;

            GameObject o = FindObject(tran.gameObject, targetObjName);
            if (o != null)
                return o;
        }

        return null;
    }

    static public GameObject LoadUIPrefabs(string strPrefabsName, GameObject parentObj)
    {
        GameObject obj = GameCommon.CreateObject(msUIPrefabPath + strPrefabsName, parentObj) as GameObject;

        if (obj == null)
        {
            Logic.EventCenter.Log(LOG_LEVEL.WARN, "this prefab is not exist>" + strPrefabsName);
        }
        else
            obj.name = strPrefabsName;
        return obj;
    }

    static public GameObject CreateObject(string prefabName, GameObject parentObj)
    {
        if (parentObj == null)
        {
            Logic.EventCenter.Log(LOG_LEVEL.ERROR, "No exist gameobject parentObj when create " + prefabName);
            return null;
        }
        GameObject uiObject = CreateObject(prefabName) as GameObject;
        if (uiObject != null)
        {
            uiObject.transform.SetParent(parentObj.transform, false);
            uiObject.transform.localPosition = new Vector3(0, 0, 0);
            uiObject.transform.localScale = new Vector3(1, 1, 1);
        }
        return uiObject;
    }

    static public GameObject CreateObject(string prefabName)
    {
        try
        {
            UnityEngine.Object o = Resources.Load(prefabName, typeof(GameObject));
            if (o != null)
                return GameObject.Instantiate(o) as GameObject;
        }
        catch (Exception e)
        {
            LOG.logError("Load prefab fail >" + prefabName + ", " + e.ToString());
        }
       
        return null;
    }

    static public DataBuffer LoadData(string resName)
    {
        var data = Resources.Load<TextAsset>(resName);
        if (data)
        {
            var buffer = new DataBuffer(data.bytes);
            return buffer;
        }
        LOG.logError("No exist resource " + resName);
        return null;
    }

    static public DataBuffer LoadLuaData(string resName)
    {
        //var data = Resources.Load<LuaAsset>(resName);
        //if (data)
        //{
        //    var buffer = new DataBuffer(data.GetDecodeBytes());
        //    string txt = StaticDefine.GetString(buffer.mData, 0, buffer.size());
        //    LOG.log("------------------\r\n"+txt+"\r\n------------------\r\n");
        //    return buffer;
        //}

        string pathFile = GameCommon.MakeGamePathFileName("LuaCode/"+resName+".lua");
        if (!File.Exists(pathFile))
        {
            LOG.logError("No exist resource : " + pathFile);
            return default;
        }
        FileStream fs = new FileStream(pathFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
        DataBuffer data = new DataBuffer((int)fs.Length);

        if (fs.Read(data.getData(), 0, (int)fs.Length) != fs.Length)
            data = null;
        else
            data.seek((int)fs.Length);

        fs.Close();

        return data;

    }

    public static string MakeGamePathFileName(string strFileName)
    {
        if (GameCommon.RunOnWindow)
        {
            return Application.dataPath + "/" + strFileName;
        }
        else
        {
            return Application.persistentDataPath + "/" + strFileName;
        }
    }

    public static string MakePathFileNameForWWW(string strFileName)
    {
        string fileName = "file://";
        if (GameCommon.RunOnWindow)
            fileName += Application.dataPath + "/" + strFileName;
        else
            fileName += Application.persistentDataPath + "/" + strFileName;

        return fileName;
    }

    static public string _MakeMD5String(byte[] bytHash)
    {
        string result = "";
        for (int i = 0; i < bytHash.Length; i++)
        {
            result += bytHash[i].ToString("X").PadLeft(2, '0');
        }
        return result.ToLower();
    }

    static public string MakeMD5(string scrString)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] bytValue, bytHash;
        bytValue = StaticDefine.GetBytes(scrString);
        bytHash = md5.ComputeHash(bytValue);
        md5.Clear();

        //string result = "";
        //for (int i = 0; i < bytHash.Length; i++)
        //{
        //    result += bytHash[i].ToString("X").PadLeft(2, '0');
        //}
        //return result.ToLower();

        return _MakeMD5String(bytHash);
    }

    static public string MakeMD5(byte[] scrData, int size)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] bytHash;

        bytHash = md5.ComputeHash(scrData, 0, size);
        md5.Clear();

        //string result = "";
        //for (int i = 0; i < bytHash.Length; i++)
        //{
        //    result += bytHash[i].ToString("X").PadLeft(2, '0');
        //}
        //return result.ToLower();

        return _MakeMD5String(bytHash);
    }

    static public string MakeMD5(Stream stream)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] bytHash;

        bytHash = md5.ComputeHash(stream);
        md5.Clear();

        //string result = "";
        //for (int i = 0; i < bytHash.Length; i++)
        //{
        //    result += bytHash[i].ToString("X").PadLeft(2, '0');
        //}
        //return result.ToLower();

        return _MakeMD5String(bytHash);
    }

    static public NiceTable LoadTable(string tableName, bool bBytes)
    {
        var tableData = UpdateResourceMgr.LoadFileData(tableName);
        if (tableData != null)
        {
            var table = new NiceTable();

            var d = new DataBuffer(tableData);
            if (bBytes)
            {
                if (table.restore(ref d))
                    return table;
            }
            else
            {
                if (table.LoadTable(d, Encoding.Default))
                    return table;
            }

            LOG.logError("Load table fail: " + tableName);
        }
        return null;
    }
    
    static Dictionary<string, AssetBundle> msBundleList = new Dictionary<string, AssetBundle>();

    
    public static GameObject LoadFromBundle(string bundlePath, string bundleName, string resName)
    {
        //await Logic.UpdateResourceMgr.CheckAndUpdateRecources(Logic.LocalResourcePath.ConfigTablePath, Logic.LocalResourcePath.ConfigListName, Logic.LocalResourcePath.ConfigMD5Name, "配置表格");
        

        AssetBundle ab;
        if (msBundleList.TryGetValue(bundleName, out ab))
        {
            GameObject obj = ab.LoadAsset<GameObject>(resName);
            if (obj == null)
                LOG.logError($"No exist : {resName} in bunlde {bundlePath}/{bundleName}");
            return obj;
        }

        try
        {
            // 测试加载bundle
            var data = UpdateResourceMgr.LoadUpdateFileData(bundlePath, bundleName);
            if (data != null)
            {
                ab = AssetBundle.LoadFromMemory(data);
                if (ab != null)
                {
                    msBundleList[bundleName] = ab;
                    GameObject obj = ab.LoadAsset<GameObject>(resName);
                    if (obj == null)
                        LOG.logError($"No exist : {resName} in bunlde {bundlePath}/{bundleName}");
                    return obj;
                }
            }

            LOG.DebugUI($"XXX: ab 1 load fail : {bundleName}", true);
            LOG.logError($"XXX: ab 1 load fail : {bundleName}");
        }
        catch (Exception e)
        {
            LOG.logError($"XXX: ab 1 load fail : {bundleName}, Exc : {e.ToString()}");
        }

        return null;
    }

    static public string AgeTipsUrl
    {
        get
        {
            return "https://lcmx-agecontent.mobage.cn/";
        }
    }

    public static void RestartUnity()
    {
#if !UNITY_EDITOR
#if UNITY_ANDROID
        using (var unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer"))
        {
            const int kIntent_FLAG_ACTIVITY_CLEAR_TASK = 0x00008000;
            const int kIntent_FLAG_ACTIVITY_NEW_TASK   = 0x10000000;

            var currentActivity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
            var pm              = currentActivity.Call<AndroidJavaObject>("getPackageManager");
            var intent          = pm.Call<AndroidJavaObject>("getLaunchIntentForPackage", Application.identifier);

            intent.Call<AndroidJavaObject>("setFlags", kIntent_FLAG_ACTIVITY_NEW_TASK | kIntent_FLAG_ACTIVITY_CLEAR_TASK);
            currentActivity.Call("startActivity", intent);
            currentActivity.Call("finish");
            var process = new AndroidJavaClass("android.os.Process");
            int pid     = process.CallStatic<int>("myPid");
            process.CallStatic("killProcess", pid);
        }
#endif
#endif
    }
}


