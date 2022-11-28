using HybridCLR;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.SceneManagement;

public class LoadDll : MonoBehaviour
{
    // 华佗热更
    public static List<string> AOTMetaAssemblyNames { get; } = new List<string>()
    {
         "mscorlib.dll",
         "System.dll",
         "System.Core.dll",
    };

    void Start()
    {
        DontDestroyOnLoad(gameObject);
        StartCoroutine(DownLoadAssets(this.StartGame));
    }

    private static Dictionary<string, byte[]> s_assetDatas = new Dictionary<string, byte[]>();

    public static byte[] GetAssetData(string dllName)
    {
        if (!s_assetDatas.ContainsKey(dllName))
        {
            //LOG.logError("No exist dll data : "+dllName);
            return null;
        }

        return s_assetDatas[dllName];
    }

    private string GetWebRequestPath(string asset)
    {
        var path = $"{Application.streamingAssetsPath}/{asset}";
        if (!path.Contains("://"))
        {
            path = "file://" + path;
        }
        return path;
    }

    IEnumerator DownLoadAssets(Action onDownloadComplete)
    {
        var assets = new List<string>
        {
            "prefabs",
           // "Assembly-CSharp.dll",
            "TestDll.dll",
        }.Concat(AOTMetaAssemblyNames);

        foreach (var asset in assets)
        {
            string dllPath = GetWebRequestPath(asset);
            Debug.Log($"start download asset:{dllPath}");
            UnityWebRequest www = UnityWebRequest.Get(dllPath);
            yield return www.SendWebRequest();

#if UNITY_2020_1_OR_NEWER
            if (www.result != UnityWebRequest.Result.Success)
            {
                Debug.Log(www.error);
            }
#else
            if (www.isHttpError || www.isNetworkError)
            {
                Debug.Log(www.error);
            }
#endif
            else
            {
                // Or retrieve results as binary data
                byte[] assetData = www.downloadHandler.data;
                Debug.Log($"dll:{asset}  size:{assetData.Length}");
                s_assetDatas[asset] = assetData;
            }
        }

        onDownloadComplete();
    }


    void StartGame()
    {
        LoadMetadataForAOTAssemblies();

#if !UNITY_EDITOR
        var gameAss2 = System.Reflection.Assembly.Load(GetAssetData("TestDll.dll"));
      //  var gameAss = System.Reflection.Assembly.Load(GetAssetData("Assembly-CSharp.dll"));
#else
        var gameAss2 = AppDomain.CurrentDomain.GetAssemblies().First(assembly => assembly.GetName().Name == "TestDll");
       // var gameAss = AppDomain.CurrentDomain.GetAssemblies().First(assembly => assembly.GetName().Name == "Assembly-CSharp");
#endif

        AssetBundle prefabAb = AssetBundle.LoadFromMemory(GetAssetData("prefabs"));
        GameObject testPrefab = Instantiate(prefabAb.LoadAsset<GameObject>("Logic.prefab"));
        
        //StartLogic();
        //SceneManager.LoadScene(1);

        var x = gameAss2.CreateInstance("Test");
        if (x!=null)
            Debug.Log($" ****** Create : "+x.ToString());
        else
        {
            Debug.Log("xxx create fail MainStart is not exist");
        }
        
        
        //var t = gameAss.GetType("MainStart");
        //if (t!=null)
        //    Debug.Log("MainStart is : "+t.ToString());
        //else
        //{
        //    Debug.Log("MainStart is not exist");
        //}
        
        // var logicObject = Resources.Load<GameObject>("Logic");
        // var t = gameAss2.GetType("TestCom");
        // var obj = GameObject.Instantiate(logicObject);
        // var com = obj.AddComponent(t);
        // //var com = UnityEngineInternal.APIUpdaterRuntimeServices.AddComponent<>(obj, "Assets/Main/LoadDll.cs (117,9)", "TestCom");
        // if (com == null)
        // {
        //     Debug.Log("XXX ADD COM FAIL");
        // }
    }



    /// <summary>
    /// Ϊaot assembly����ԭʼmetadata�� ��������aot�����ȸ��¶��С�
    /// һ�����غ����AOT���ͺ�����Ӧnativeʵ�ֲ����ڣ����Զ��滻Ϊ����ģʽִ��
    /// </summary>
    private static void LoadMetadataForAOTAssemblies()
    {
        // ���Լ�������aot assembly�Ķ�Ӧ��dll����Ҫ��dll������unity build���������ɵĲü����dllһ�£�������ֱ��ʹ��ԭʼdll��
        // ������BuildProcessors������˴�����룬��Щ�ü����dll�ڴ��ʱ�Զ������Ƶ� {��ĿĿ¼}/HybridCLRData/AssembliesPostIl2CppStrip/{Target} Ŀ¼��

        /// ע�⣬����Ԫ�����Ǹ�AOT dll����Ԫ���ݣ������Ǹ��ȸ���dll����Ԫ���ݡ�
        /// �ȸ���dll��ȱԪ���ݣ�����Ҫ���䣬�������LoadMetadataForAOTAssembly�᷵�ش���
        /// 
        foreach (var aotDllName in AOTMetaAssemblyNames)
        {
            byte[] dllBytes = GetAssetData(aotDllName);
            // ����assembly��Ӧ��dll�����Զ�Ϊ��hook��һ��aot���ͺ�����native���������ڣ��ý������汾����
            LoadImageErrorCode err = RuntimeApi.LoadMetadataForAOTAssembly(dllBytes, HomologousImageMode.Consistent);
            Debug.Log($"LoadMetadataForAOTAssembly:{aotDllName}. ret:{err}");
        }
    }
}
