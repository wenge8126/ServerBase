//#define SAVE_BYTE_CONFIG_TABLE

using System;
using System.Collections.Generic;
using System.IO;

using Logic;
//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------


public class TableManager
{
    public delegate NiceTable LoadTableFunction(string path, string tableFileName);
    
    static public TableManager Instance = new TableManager();

    static public TableManager GetMe() { return Instance; }

#if UNITY_EDITOR && SAVE_BYTE_CONFIG_TABLE
    static public EasyGenerateResourcesPack mPackTool = new EasyGenerateResourcesPack(false, true);
#endif
#if UNITY_EDITOR
    public
#endif
    Dictionary<string, NiceTable> mConfigMap = new Dictionary<string, NiceTable>();

    public TableManager()
    {

    }

    public void InitConfig()
    {
        Type t = System.Reflection.Assembly.GetExecutingAssembly().GetType("Config");
        if (t!=null)
        {        
            System.Reflection.MethodInfo initFun = (t.GetMethod("Init"));
            if (initFun!=null)
                initFun.Invoke(null, null);     

        // 测试读取配置 
        //WindowConfig d = new WindowConfig("TestWindow");
        //LOG.log("Load config win >" + d.WIN_PREFAB);
        }
    }

    public bool LoadConfig(string path, String configListTableName/*, LOAD_MODE loadMode*/, LoadTableFunction loadFun)
    {
        NiceTable configList = loadFun(path, configListTableName);
        if (configList != null)
        {
            return LoadConfig(path,configList ,loadFun);
        }
        LOG.logError("Load config list table fail. No exist >" + configListTableName);
        return false;
    }

    public bool LoadConfig(string path, NiceTable configListTable/*, LOAD_MODE loadMode*/, LoadTableFunction loadFun)
    {
        if (configListTable != null)
        {
            foreach (var kv in configListTable.GetAllRecord())
            {
                DataRecord r = kv.Value;
                string configIndex = (string)r.get("INDEX");
                string tableFileName = (string)r.get("FILE");
                string fileType = (string)r.get("TYPE");
      
                try
                {
                    //LOG.log("Start load >["+tableFileName+"] ...");
                    NiceTable config = loadFun(path, tableFileName);
                    string    info   = "";
                    if (config != null)
                    {
                        NiceTable exist;
                        if (mConfigMap.TryGetValue(configIndex, out exist))
                            LOG.logWarn("Table alread exist now resplace >" + configIndex);
                        mConfigMap[configIndex] = config;
                        info += "Succeed load ";

                    }
                    else
                    {
                        info += "Fail load ";
                        LOG.logError("Fail load config table >" + tableFileName);
                    }
                    info += configIndex;
                    info += " : " + tableFileName;
                    info += " TYPE: " + fileType;
                    info += " Record count " + config.GetRecordCount().ToString();
                    LOG.log(info);

                }
                catch (Exception e)
                {
                    LOG.log("Error: load table fail >" + tableFileName + " >>" + e.ToString());
                }
            }
        }
        return true;
    }

    public bool LoadFromPack(string tablePackFileName, string configListName)
    {
        //string fileName = GameCommon.MakeGamePathFileName(tablePackFileName);
        //if (!File.Exists(fileName))
        //{
        //    LOG.logError("Config table packet is no exist");
        //    return false;
        //}

        //ResourcesPack resPack = new ResourcesPack();
        //if (!resPack.load(fileName))
        //{
        //    resPack.close();
        //    LOG.logError("Load resources pack file fail >" + fileName);
        //    return false;
        //}

        //bool re =  _LoadConfigFromPack(resPack, configListName, fileName);
        //resPack.close();
        //return re;
        return false;
    }

//    public bool _LoadConfigFromPack(ResourcesPack resPack, string configListName, string tablePackFileName)
//    {
//        DataBuffer listData = resPack.loadResource(configListName);
//        if (listData == null)
//        {
//            LOG.logError("Load table list fail >" + configListName + "  from pack >" + tablePackFileName);
//            return false;
//        }

//        NiceTable listTable = new NiceTable();
//        listData.seek(0);
//        if (!listTable.restore(ref listData))
//        {
//            LOG.logError("Restor list table fail from >" + tablePackFileName);
//            return false;
//        }
//        LOG.log("Start load config from resource packet");
//        int count = 0;
//        foreach (KeyValuePair<int, DataRecord> kv in listTable.GetAllRecord())
//        {
//            DataRecord r = kv.Value;
//            string configIndex = (string)r.get("INDEX");
//            string tableFileName = (string)r.get("FILE");

//            string path = Path.GetDirectoryName(tableFileName);
//            string fName = Path.GetFileNameWithoutExtension(tableFileName) + ".bytes";
//			tableFileName = path + "/" + fName;
//			DataBuffer tableData = resPack.loadResource(tableFileName);
//            if (tableData != null)
//            {
//                tableData.seek(0);
//                NiceTable table = new NiceTable();
//                if (table.restore(ref tableData))
//                {
//                    if (existTable(configIndex))
//                        LOG.logWarn("Table already exist>" + configIndex);

//                    mConfigMap[configIndex] = table;
//                    ++count;
//#if UNITY_EDITOR
//                    LOG.log("Succeed load table >" + tableFileName +" record >"+table.GetRecordCount().ToString() + " from table pack >" + tablePackFileName);
//#endif
//                }
//                else
//                    LOG.logError(tableFileName + "> Resotre table fail from packet");
//            }
//            else
//            {
//                LOG.logError("No exist table >" + tableFileName + " data from pack >" + tablePackFileName);
//            }
//        }
//        resPack.close();
//        LOG.log("Finish load config from resource packet, count = " + count.ToString());

//        InitConfig();

//        return true;
       
//    }

    static public NiceTable GetTable(string configIndex)
    {
        NiceTable table;
        if (!Instance.mConfigMap.TryGetValue(configIndex, out table))
            LOG.log("没有配置表格>>>" + configIndex);

        return table;
    }

    static public bool existTable(string configIndex)
    {
        return Instance.mConfigMap.ContainsKey(configIndex);
    }

    static public Data GetData(string tableIndex, int keyIndex, string fieldName)
    {
        NiceTable table = GetTable(tableIndex);
        if (table != null)
        {
            DataRecord re = table.GetRecord(keyIndex);
            if (re != null)
                return re.getData(fieldName);
        }
        return Data.NULL;
    }

    static public Data GetData(string tableIndex, string keyIndex, string fieldName)
    {
        NiceTable table = GetTable(tableIndex);
        if (table != null)
        {
            DataRecord re = table.GetRecord(keyIndex);
            if (re != null)
                return re.getData(fieldName);
        }
        return Data.NULL;
    }

    static public DataRecord GetRecord(string tableIndex, int keyIndex)
    {
        NiceTable table = GetTable(tableIndex);
        if (table != null)
        {
            return table.GetRecord(keyIndex);
        }
        return null;
    }

    static public DataRecord GetRecord(string tableIndex, string keyIndex)
    {
        NiceTable table = GetTable(tableIndex);
        if (table != null)
        {
            return table.GetRecord(keyIndex);
        }
        return null;
    }


    public void ClearAll()
    {
        mConfigMap.Clear();
    }

}
