using System;
using Logic;


//!!! 优化网络消息, 序列化效率及大小, 使用快速
public class ArrayNiceData : tNiceData
{
    protected object[] mDataArray = new object[8];

    public override NICEDATA_TYPE getType() { return NICEDATA_TYPE.ARRAY_NICEDATA; }
    //---------------------------------------------------------------
    public int size() { return mDataArray.Length; }
    public override Data this[int indexKey]
    {
        //实现索引器的get方法
        get
        {
            return get(indexKey);
        }
        //实现索引器的set方法
        set
        {
            set(indexKey, value.mObj);
        }
    }

    public override bool remove(string key)
    {
        int nkey = 0;
        if (int.TryParse(key, out nkey))
            return remove(nkey);
        return false;
    }
    public override bool remove(int key)
    {
        if (key >= 0 && key < mDataArray.Length)
        {
            mDataArray[key] = null;
            return true;
        }
        return false;
    }

    public override bool Contains(string indexName)
    {
        int key = -1;
        if (int.TryParse(indexName, out key))
        {
            if (key >= 0 && key < mDataArray.Length)
                return mDataArray[key] != null;
        }
        return false;
    }
    public override bool Contains(int key)
    {
        if (key >= 0 && key < mDataArray.Length)
            return mDataArray[key] != null;
        return false;
    }

    public override void set(int indexKey, object objVal)
    {
        if (indexKey >= mDataArray.Length)
        {
            object[] temp = new object[indexKey + 1];
            Array.Copy(mDataArray, temp, mDataArray.Length);
            mDataArray = temp;
        }
        mDataArray[indexKey] = objVal;
    }

    public override Data get(int index)
    {
        if (index >= 0 && index < mDataArray.Length)
            return new Data(mDataArray[index]);
        return Data.NULL;
    }

    public override void set(string indexName, object objVal)
    {
        int key = -1;
        if (int.TryParse(indexName, out key))
        {
            set(key, objVal);
        }
        else if (StaticDefine.debug)
            LOG.logError("不支持字符KEY >" + indexName);
    }

    public override Data get(string indexName)
    {
        int key = -1;
        if (int.TryParse(indexName, out key))
        {
            return get(key);
        }
        else if (StaticDefine.debug)
            LOG.logError("不支持字符KEY >" + indexName);

        return Data.NULL;
    }

    public override object getObject(string indexName)
    {
        int key = -1;
        if (int.TryParse(indexName, out key))
        {
            if (key >= 0 && key < mDataArray.Length)
            {
                return mDataArray[key];
            }
        }
        else if (StaticDefine.debug)
            LOG.logError("不支持字符KEY >" + indexName);

        return null;
    }

    public override object getObject(int key)
    {
        if (key >= 0 && key < mDataArray.Length)
        {
            return mDataArray[key];
        }

        return null;
    }

    //--------------------------------------------------------------------------------

    public override bool getData(string indexName, out object value)
    {
        int index = -1;
        if (int.TryParse(indexName, out index))
        {
            if (index >= 0 && index < mDataArray.Length)
            {
                value = mDataArray[index];
                return true;
            }
        }
        value = null;
        return false;

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
        if (mDataArray.Length > 0xFFFF)
        {
            LOG.logError("数组下标超过 65535");
            return false;
        }

        resultData.write((ushort)mDataArray.Length);

        for (int i = 0; i < mDataArray.Length; ++i)
        {
            object obj = mDataArray[i];
            if (obj == null)
                resultData.write((byte)FIELD_TYPE.FIELD_NULL);
            else
            {
                byte type = (byte)StaticDefine.GetType(obj);
                resultData.write(type);
                tFieldInfo info = FieldInfoManager.GetFieldInfo((FIELD_TYPE)type);
                if (null != info)
                {
                    if (!info.serialize(obj, ref resultData))
                        return false;
                }
                else
                {
                    LOG.logError("Error: not find data type field infomation > unknow type:" + type.ToString() + ", obj >" + obj.ToString());
                    return false;
                }
            }
        }

        return true;
    }
    //--------------------------------------------------------------------------------

    public override bool restore(ref DataBuffer scrData)
    {
        ushort num = 0;
        if (!scrData.read(out num))
            return false;

        mDataArray = new object[num];
        for (int i = 0; i < mDataArray.Length; ++i)
        {
            byte type = 0;
            if (!scrData.read(out type))
                return false;

            if (type != (byte)FIELD_TYPE.FIELD_NULL)
            {
                tFieldInfo info = FieldInfoManager.GetFieldInfo((FIELD_TYPE)type);
                if (null != info)
                {
                    object obj = null;
                    if (!info.restore(out obj, ref scrData))
                        return false;

                    mDataArray[i] = obj;
                }
                else
                {
                    LOG.logError("Error: not find data type field infomation > unknow type:" + type.ToString());
                    return false;
                }
            }
        }

        return true;
    }

    public bool restoreEx(ref DataBuffer scrData, string debugInfo)
    {
        ushort num = 0;

#if UNITY_EDITOR
        LOG.LogFormat("-------------- Now ArrayNiceData restore {0} ----------------------", debugInfo);
#endif

        if (!scrData.read(out num))
            return false;
        mDataArray = new object[num];
        for (int i = 0; i < mDataArray.Length; ++i)
        {
            byte type = 0;
            if (!scrData.read(out type))
                return false;
#if UNITY_EDITOR
            int debugPos = scrData.tell();
#endif
            if (type != (byte)FIELD_TYPE.FIELD_NULL)
            {
                tFieldInfo info = FieldInfoManager.GetFieldInfo((FIELD_TYPE)type);
                if (null != info)
                {
                    object obj = null;
                    if (!info.restore(out obj, ref scrData))
                        return false;
#if UNITY_EDITOR
                    int size = scrData.tell() - debugPos;
                    LOG.LogFormat("{0} {1} size {2}", i.ToString(), StaticDefine.ToStringType((FIELD_TYPE)type), size);
#endif
                    mDataArray[i] = obj;
                }
                else
                {
                    LOG.logError("Error: not find data type field infomation > unknow type:" + type.ToString());
                    return false;
                }
            }
        }
#if UNITY_EDITOR
        LOG.log("-----------------------------------------------------------------------");
#endif
        return true;
    }

    public override void dump(string infoParam)
    {
        if (true)
        {
            string info = mDataArray.Length.ToString() + "-------------------\r\n";
            for (int i = 0; i < mDataArray.Length; ++i)
            {
                if (mDataArray[i] != null)
                {
                    //info += i.ToString() + " = "+mDataArray[i].ToString();
                    info += string.Format("{0} = [{1}] {2}\r\n", i, StaticDefine.ToStringType((FIELD_TYPE)StaticDefine.GetType(mDataArray[i])), mDataArray[i].ToString());
                }
                else
                    info += i.ToString() + " = [NONE] null\r\n";
            }
            info += "-----------------------------";
            LOG.log(info);
            //LOG.log("ArrayNice 未实现 dump");
        }
    }

    public override bool FullData(object scrObject)
    {
        //LOG.log("ArrayNice 未实现 FullData");
        return false;
    }

    public override bool RestoreToTarget(ref object targetObject)
    {

        return true;
    }

    public override void clear()
    {
        Array.Clear(mDataArray, 0, mDataArray.Length);
    }
}
