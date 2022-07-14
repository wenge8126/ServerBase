using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

using Logic;


public enum NICEDATA_TYPE
{
    NULL_NICEDATA,
    //ID_NICEDATA,
    NICEDATA,
    ARRAY_NICEDATA,
};

public class ExNiceData : NiceData{}

public class NiceData : tNiceData
{
    public Dictionary<string, object> mDataMap = new Dictionary<string, object>();


    public override NICEDATA_TYPE getType() { return NICEDATA_TYPE.NICEDATA; }

    public int Count() { return mDataMap.Count; }

    //--------------------------------------------------------------------------------
    public override Data this[string indexName]
    {
        //实现索引器的get方法
        get
        {
            //if (UseID())
            //{
            //    return mIDIndex.get(indexName);
            //}
            object val;

            if (!mDataMap.TryGetValue(indexName, out val))
            {
                LOG.log("XXX WARN: no exist data >>>" + indexName);
            }
            return new Data(val);
        }
        //实现索引器的set方法
        set
        {
            //if (UseID())
            //{
            //    mIDIndex.set(indexName, value.mObj);
            //    return;
            //}
            mDataMap[indexName] = value.mObj;
        }
    }

    public override bool remove(string key)
    {
        return mDataMap.Remove(key);
    }
    public override bool remove(int key)
    {
        return mDataMap.Remove(key.ToString());
    }

    public override void set(string indexName, object objVal)
    {
        if (objVal == null)
            return;

        if (objVal.GetType() == typeof(uint))
            objVal = (int)(uint)objVal;

        //if (UseID())
        //{
        //    mIDIndex.set(indexName, objVal);
        //    return;
        //}

        //mDataMap.Add((int)StaticDefine.StringID(indexName), objVal);
        mDataMap[indexName] = objVal;
    }

    public override Data get(string indexName)
    {
        //if (UseID())
        //{            
        //    return mIDIndex.get(indexName);
        //}

        object val;

        if (!mDataMap.TryGetValue(indexName, out val))
        {
            LOG.log("XXX WARN: no exist data >>>" + indexName);
        }
        return new Data(val);
        //return (Data)val;
    }

    public override object getObject(string indexName)
    {
        //if (UseID())
        //{
        //    return mIDIndex.getObject(indexName);       
        //}
        object val = null;

        if (!mDataMap.TryGetValue(indexName, out val))
        {
            LOG.log("XXX WARN: no exist data >>>" + indexName);
        }
        return val;
    }

    //public override object getObject(int key)
    //{
    //    //if (UseID())
    //    //{
    //    //    return mIDIndex.getObject(key);
    //    //}
    //    //else
    //        return base.getObject(key);
    //}

    //--------------------------------------------------------------------------------

    public override bool getData(string indexName, out object value)
    {
        //if (UseID())
        //{
        //    return mIDIndex.getData(indexName, out value);       
        //}
        if (!mDataMap.TryGetValue(indexName, out value))
        {
            //LOG.log("XXX WARN: no exist data >>>" + indexName);
            return false;
        }

        return true;

    }
    //--------------------------------------------------------------------------------

    public override bool get(string indexName, out int nVal)
    {
        object val;

        bool b = getData(indexName, out val);
        if (b && val is int)
        {
            nVal = (int)val;
            return true;
        }
        else
            nVal = 0;

        return false;
    }
    //--------------------------------------------------------------------------------

    public override bool get(string indexName, out float fVal)
    {
        object val;

        bool b = getData(indexName, out val);
        if (b && val is float)
        {
            fVal = (float)val;
            return true;
        }
        else
            fVal = 0;

        return false;
    }
    //--------------------------------------------------------------------------------

    public override bool get(string indexName, out string strVal)
    {
        object val;

        bool b = getData(indexName, out val);
        if (b && val is string)
        {
            strVal = (string)val;
            return true;
        }
        else
            strVal = "";

        return false;
    }

    public override bool get(string indexName, out UInt64 resultVal)
    {
        object val;

        bool b = getData(indexName, out val);
        if (b && val is UInt64)
        {
            resultVal = (UInt64)val;
            return true;
        }
        else
            resultVal = 0;

        return false;
    }

    public override bool get(string indexName, out tNiceData resultVal)
    {
        object val;

        bool b = getData(indexName, out val);
        if (b && val is tNiceData)
        {
            resultVal = (tNiceData)val;
            return true;
        }
        else
            resultVal = null;

        return false;
    }

    public override bool serialize(ref DataBuffer resultData)
    {
        //if (UseID())
        //{
        //    return mIDIndex.serialize(ref resultData);       
        //}
        //byte nicetype = (byte)NICEDATA_TYPE.NICEDATA; // mbSaveIDData ? (byte)NICEDATA_TYPE.ID_NICEDATA : (byte)NICEDATA_TYPE.NICEDATA;
        //resultData.write(nicetype);          
        // save count
        byte count = (byte)mDataMap.Count;
        resultData.write(count);
        // each save data
        foreach (KeyValuePair<string, object> it in mDataMap)
        {
            // save key id
            //if (mbSaveIDData)
            //{
            //    resultData.write((int)StaticDefine.StringID(it.Key));
            //}
            //else
            resultData.writeString(it.Key);

            object val = it.Value;

            // save type
            byte type = (byte)StaticDefine.GetType(val);
            resultData.write(type);
            tFieldInfo info = FieldInfoManager.GetFieldInfo((FIELD_TYPE)type);
            if (null != info)
            {
                if (!info.serialize(val, ref resultData))
                    return false;
            }
            else
            {
                throw new Exception("Error: not find data type field infomation >" + it.Key.ToString() + " > unknow type:" + type.ToString());
            }
        }
        return true;
    }
    //--------------------------------------------------------------------------------

    public override bool restore(ref DataBuffer scrData)
    {
        //byte nicetype = 0;
        //if (!scrData.read(out nicetype))
        //    return false;

        //if (nicetype == (byte)NICEDATA_TYPE.ID_NICEDATA)
        //{
        //    // 一定需要回退一个标识字节
        //    scrData.seek(scrData.tell() - 1);
        //    mIDIndex = new IDNiceData();
        //    return mIDIndex.restore(ref scrData);
        //}

        //if (nicetype!=(byte)NICEDATA_TYPE.NICEDATA)
        //{
        //    LOG.logError("Type >"+nicetype.ToString()+" > NiceData无法恢复");
        //    return false;
        //}

        // read count
        byte count;
        if (!scrData.read(out count))
            return false;
        for (int i = 0; i < count; ++i)
        {
            // read id
            string key;
            if (!scrData.readString(out key))
                return false;

            //EventCenter.Log(false, "Read key>" + key);

            if (mDataMap.ContainsKey(key))
                EventCenter.Log(LOG_LEVEL.WARN, "already add key > " + key);

            byte type;
            if (!scrData.read(out type))
                return false;

            tFieldInfo info = FieldInfoManager.GetFieldInfo((FIELD_TYPE)type);
            if (null != info)
            {
                object obj;
                if (!info.restore(out obj, ref scrData))
                    return false;

                //mDataMap.Add(id, obj);
                mDataMap[key] = obj;

            }
            else
            {

                EventCenter.Log(LOG_LEVEL.ERROR, "unkown type > " + type.ToString());

                mDataMap[key] = "UNKNOW";

                //throw new Exception("Error: not find data type field infomation.");
            }

        }
        return true;
    }
    static int subLayer = -1;
    public override void dump(string infoParam)
    {
        if (true)
        {
            //if (UseID())
            //{
            //    mIDIndex.dump(infoParam);
            //    return;
            //}
            ++subLayer;
            string spaceString = "";
            if (subLayer > 0)
            {
                for (int i = 0; i < subLayer; ++i)
                {
                    spaceString += "    ";
                }
            }
            LOG.log(spaceString + "--------------------------------------------------------------------");
            if (subLayer == 0 && infoParam != null && infoParam != "")
                LOG.log(infoParam);
            foreach (KeyValuePair<string, object> it in mDataMap)
            {
                string info;
                if (it.Value == null)
                {
                    info = "[UNKNOW:" + it.Key + "] = NULL";
                    LOG.log(info);
                    continue;
                }

                // save key id
                info = spaceString + "[";
                FIELD_TYPE type = (FIELD_TYPE)StaticDefine.GetType(it.Value);
                info += StaticDefine.ToStringType(type);
                info += "]:[";
                {
                    info += it.Key + "] = ";
                    info += type == FIELD_TYPE.FIELD_TABLE ? "TABLE" : it.Value.ToString();
                }
                LOG.log(info);

#if !UNITY_EDITOR && (UNITY_IPHONE || UNITY_ANDROID || DEBUG_IO)
#else
                if (type == FIELD_TYPE.FIELD_TABLE)
                {
                    string logTableName = "TableLog/";
                    if (infoParam != "")
                        logTableName += infoParam + "_" + it.Key + "_.csv";
                    else
                        logTableName += "Unknow_" + it.Key + "_.csv";
                    try
                    {
                        NiceTable t = it.Value as NiceTable;
                        if (t != null)
                        {
                            logTableName = logTableName.Replace(':', '_');
                            t.SaveTable(logTableName, Encoding.Default);
                        }
                    }
                    catch
                    {
                        LOG.log("Error: Fail save log table >" + logTableName + ", may be opening");
                    }
                }
#endif
                if (type == FIELD_TYPE.FIELD_NICEDATA)
                {
                    NiceData d = it.Value as NiceData;
                    if (d != null)
                        d.dump(info);
                }
            }
            LOG.log(spaceString + "--------------------------------------------------------------------");
            --subLayer;
        }
    }

    public override bool FullData(object scrObject)
    {
        Type type = scrObject.GetType();

        FieldInfo[] fields = type.GetFields();
        foreach (FieldInfo field in fields)
        {
            object objVal = field.GetValue(scrObject);

            int nType = StaticDefine.GetType(objVal);
            if (nType != (int)FIELD_TYPE.FIELD_NULL)
            {
                set(field.Name, objVal);
            }
        }
        return true;
    }

    public override bool RestoreToTarget(ref object targetObject)
    {
        //if (UseID())
        //{
        //    EventCenter.LogError("IDNiceData can not RestoreToTarget");
        //    return false;
        //}

        Type type = targetObject.GetType();

        foreach (KeyValuePair<string, object> it in mDataMap)
        {
            FieldInfo field = type.GetField(it.Key);
            if (field != null)
            {
                if (field.FieldType == it.Value.GetType())
                    field.SetValue(targetObject, it.Value);
            }
        }
        return true;
    }



    public override bool Contains(string key)
    {
        //if (UseID())
        //{
        //    return mIDIndex.Contains(key);
        //}
        return mDataMap.ContainsKey(key);
    }

    public override bool Contains(int key)
    {
        return mDataMap.ContainsKey(key.ToString());
    }

    public override void clear()
    {
        //if (UseID())
        //    mIDIndex.clear();
        mDataMap.Clear();
    }
};
