using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Logic;
using UnityEngine;
using UnityEngine.Networking;

namespace Core
{
    public abstract class tResourceManager
    {
        // resource config file, game start load from run path, then 
        public abstract bool LoadConfig(string configName);

        public abstract UnityEngine.Object NewCreate(string resName);

        //public abstract Resource GetResource(string resourceName);

        //public abstract Resource GetResource(string resourceName, string type);
    }


    public class ResourceManager : tResourceManager
    {
        static public ResourceManager Instance = new ResourceManager();

        static public NiceTable msUpdateList = null;

        static public Dictionary<string, UnityEngine.AssetBundle> msBundleList = new Dictionary<string, UnityEngine.AssetBundle>();
        static public Dictionary<string, ResourcesPack> msPackList = new Dictionary<string, ResourcesPack>();

        // 从更新bundle中加载或者安装包内加载游戏对象, NOTE: 不支持直接从PAK包中加载游戏对象
        // NOTE: resName 相对 Assets/Resources 路径名称,且无扩展名
        static public T LoadResFromBundleOrLocal<T>(string resName, string extName) where T : UnityEngine.Object
        {
            DataRecord re = Instance.mResourcesIndex.GetRecord(resName.ToLower());
            if (re != null)
            {
                if ((string)re["TYPE"] == "BUNDLE")
                {
                    AssetBundle bundle = LoadBundle(re["BUNDLE"], true);
                    if (bundle != null)
                    {
                        return bundle.LoadAsset<T>("assets/resources/" + resName + "." + extName);
                    }
                    else
                        LOG.logError(string.Format("Load {0} from bundle {1} fail", resName, (string)re["BUNDLE"]));
                }
                //else if ((string)re["TYPE"] == "PAK")
                //{

                //}
                else 
                {
                    LOG.logError(string.Format("不支持直接加载 {0} from updata res {1} ", resName, (string)re["BUNDLE"]));
                    //return null;
                }
            }

            // 直接从资源包中加载
            return Resources.Load<T>(resName);
        }

        // 从更新bundle中加载  NOTE: resName 相对 Assets/Resources 路径名称,且无扩展名
        // NOTE: bundleName 无扩展名
        static public T LoadResFromBundle<T>(string resName, string extName, string bundleName) where T : UnityEngine.Object
        {
            AssetBundle bundle = LoadBundle(bundleName, true);
            if (bundle != null)
            {
                return bundle.LoadAsset<T>("Assets/Resources/"+resName+"."+extName);
            }
            else
                LOG.logError(string.Format("Load {0} from bundle {1} fail", "Assets/Resources/"+resName+"."+extName, bundleName));

            return null;
        }

        static public T LoadResFromBundle<T>(string resName, string bundleName) where T : UnityEngine.Object
        {
            AssetBundle bundle = LoadBundle(bundleName, true);
            if (bundle != null)
            {
                string name = Path.GetFileNameWithoutExtension(resName);
                return bundle.LoadAsset<T>(name);
            }
            else
                LOG.logError(string.Format("Load {0} from bundle {1} fail", "Assets/Resources/" + resName, bundleName));

            return null;
        }

        static public bool HasUpdateResourcesFile(string res)
        {
            string resFile = res.ToLower();
            if (msUpdateList == null)
            {
                Instance.Init();
            }
            if (msUpdateList.GetRecord(resFile) != null)
                return System.IO.File.Exists(GameCommon.MakeGamePathFileName(resFile));

            return false;
        }

        static public string GetResourcesMD5()
        {
            NiceTable updateList = new NiceTable();
            if (updateList.LoadBinary(GameCommon.MakeGamePathFileName("_result.list")))
                return updateList.mTableIndex;
            
            return GameCommon.MakeMD5("");
        }        

        // 获取路径, 以 资源包 PAK
        static public DataBuffer LoadResourceFromPack(string resName)
        {
            string szPathFilename = resName.ToLower();
            szPathFilename.Replace('\\', '/');
            DataRecord d = Instance.mResourcesIndex.GetRecord(szPathFilename);
            if (d != null)
            {
                string packName = d["BUNDLE"];
                ResourcesPack pack = LoadResourcesPack(packName);
                if (pack != null)
                    return pack.loadResource(szPathFilename);
            }
            return null;
        }

        static public ResourcesPack LoadResourcesPack(string packName)
        {
            ResourcesPack pack;
            if (msPackList.TryGetValue(packName, out pack))
                return pack;

            // 加载
            string path = GameCommon.MakeGamePathFileName(packName);
            pack = new ResourcesPack();
            if (pack.load(path))
            {
                msPackList[packName] = pack;
                return pack;
            }

            return null;
        }
        
        static public bool DestoryPack(string packName)
        {
            ResourcesPack pack;
            if (msPackList.TryGetValue(packName, out pack))
            {
                pack.close();
                msPackList.Remove(packName);
                return true;
            }
            return false;
        }

        // 释放必须使用此函数
        static public bool DestoryBundle(string bundleName)
        {
            AssetBundle bundle = null;
            
            if (msBundleList.TryGetValue(bundleName.ToLower(), out bundle))
            {
                bundle.Unload(false);
                msBundleList.Remove(bundleName.ToLower());
                return true;
            }
            return false;
        }

        // 从更新资源中加载
        static public UnityEngine.AssetBundle LoadBundle(string resName, bool bFromResources, bool bLoadDependencies = true)
        {
            resName += ".bundle";
#if UNITY_IPHONE
            resName = "ios_"+resName;
#elif UNITY_EDITOR
            resName = "win_" + resName;
#else
            resName = "and_" + resName;
#endif

           AssetBundle bundle = null;
           if (msBundleList.TryGetValue(resName.ToLower(), out bundle))
               return bundle;

            //if (CommonParam.UseUpdate && !HasUpdateResourcesFile(resName))
            //    return null;

            FileStream fs = null;

            if (bFromResources)
            {
                bundle = Resources.Load<AssetBundle>(resName);                
            }
            else
            {
                string fileName = GameCommon.MakeGamePathFileName(resName);

                if (!File.Exists(fileName))
                {
                    LOG.logError("Table file no exist>" + fileName);
                    return null;
                }

                fs = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                DataBuffer data = new DataBuffer((int)fs.Length);
                int readlen = fs.Read(data.mData, 0, (int)fs.Length);

                //fs.Close();

                bundle = UnityEngine.AssetBundle.LoadFromMemory(data.mData);
            }            


            if (bundle != null)
            {
                msBundleList[resName.ToLower()] = bundle;

                if (!bLoadDependencies)
                {
                    if (fs != null)
                        fs.Close();
                }
                else
                {
                    if (fs == null)
                    {
                        string fileName = GameCommon.MakeGamePathFileName(resName);

                        if (!File.Exists(fileName))
                        {
                            LOG.logError("Table file no exist>" + fileName);
                            return bundle;
                        }

                        fs = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                    }

                    if (fs != null)
                    {
                        // 加载依赖, 打包时,(Tools.cs)把依赖包写在了包的最后部分(嘿嘿)
                        // Bundle data + DataBuffer + (long)Data Position
                        fs.Seek(-sizeof(long), SeekOrigin.End);
                        long nowPos = fs.Position;
                        byte[] temp = new byte[sizeof(long)];
                        int x = fs.Read(temp, 0, temp.Length);
                        if ( x==sizeof(long))
                        {
                            long pos = BitConverter.ToInt64(temp, 0);
                            fs.Seek(pos, SeekOrigin.Begin);
                            long len = nowPos - pos;
                            DataBuffer data = new DataBuffer((int)len);
                            int re = fs.Read(data.mData, 0, (int)len);
                            if (re==len)
                            {
                                int count = 0;
                                data.read(out count);
                                for (int i = 0; i < count; ++i)
                                {
                                    string name;
                                    if (data.readString(out name))
                                    {
                                        if(name.Contains(".bundle"))                                        
                                            name=name.Substring(0, name.Length - 7);
                                        
                                        if (!LoadBundle(name, false, true))
                                        {
                                            LOG.logError(resName + " Load Dependencie Bundle fail >" + name);
                                        }
                                    }
                                } // for
                                // 加载材质Shader 关联信息表
                                NiceTable matInfo = new NiceTable();
                                if (matInfo.restore(ref data))
                                {
                                    foreach (DataRecord r in matInfo.GetAllRecord().Values)
                                    {
                                        string matName = r[0];
                                        string shaderName = r[1];
                                        if (matName != "" && shaderName != "")
                                        {
                                            Material m = bundle.LoadAsset<Material>(matName);
                                            if (m != null)
                                            {
                                                Shader s;
                                                if (m.shader != null)
                                                    s = Shader.Find(m.shader.name);
                                                else
                                                    s = Shader.Find(shaderName);
                                                //??? 如果shader 不存在则需要从包中加载
                                                if (s!=null)
                                                    m.shader = s;
                                            }                                           
                                        }
                                    }
                                }
                            }
                        }
                        fs.Close();
                    }
                }
            }

            return bundle;
        }

        public NiceTable mResourcesIndex;

        ResourceManager()
        {

        }

        public void Init()
        {
            msUpdateList = new NiceTable();
            if (!msUpdateList.LoadBinary(GameCommon.MakeGamePathFileName("_result.list")))
            {
                LOG.logWarn("Load update resources list fail >" + GameCommon.MakeGamePathFileName("_result.list"));
                msUpdateList = new NiceTable();
                msUpdateList.SetField("INDEX", FIELD_TYPE.FIELD_STRING, 0);
                msUpdateList.mTableIndex = "";
            }

            mResourcesIndex = new NiceTable();
            if (!mResourcesIndex.LoadBinary(GameCommon.MakeGamePathFileName("_resourceindex.list")))
                LOG.logError("Load resources index fail >" + GameCommon.MakeGamePathFileName("_resourceindex.list"));
        }

        // resource config file, game start load from run path, then 
        public override bool LoadConfig(string configName) { return true; }

        //public override Resource GetResource(string resourceName)
        //{
        //    return (Resource)null;
        //}

        //public override Resource GetResource(string resourceName, string type)
        //{
        //    Resource resource = null;
        //    switch (type)
        //    {
        //        case "file":
        //        case "File":
        //        case "FILE":
        //            {
        //                resource = new FileResource();
        //                resource.SetResourceName(resourceName);
        //            }
        //            break;

        //        case "unity":
        //        case "Unity":
        //        case "UNITY":
        //            {
        //                resource = new UnityResource();
        //                resource.SetResourceName(resourceName);
        //            }
        //            break;
        //    }

        //    return resource;
        //}

        public override UnityEngine.Object NewCreate(string resName) 
        { 

            return null; 
        }
    }


}
