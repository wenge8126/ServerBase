using System;

public abstract class tNiceData
{
    public abstract NICEDATA_TYPE getType();

    public virtual Data this[int indexKey]
    {
        get
        {
            LOG.logError("未实现 Data this[int indexKey]");
            return new Data();
        }
        set
        {
            LOG.logError("未实现 Data this[int indexKey]");
        }
    }
    public virtual Data this[string indexKey]
    {
        get
        {
            LOG.logError("未实现 Data this[string indexKey]");
            return new Data();
        }
        set
        {
            LOG.logError("未实现 Data this[string indexKey]");
        }
    }

    public abstract void clear();
    public abstract bool Contains(string key);
    public abstract bool Contains(int key);

    public abstract bool remove(string key);
    public abstract bool remove(int key);

    public virtual void set(int indexKey, object objVal) { LOG.logError("Set fail >" + indexKey.ToString() + " = " + objVal.ToString()); }

    public virtual Data get(int index) { LOG.logError("No use this"); return Data.NULL; }
    public virtual object getObject(int key) { LOG.logError("No use this"); return null; }

    public abstract void set(string indexName, object objVal);

    public abstract Data get(string indexName);
    public abstract object getObject(string indexName);
    public abstract bool getData(string indexName, out object value);

    public virtual bool get(string indexName, out int nVal) { nVal = 0; return false; }

    public virtual bool get(string indexName, out float fVal) { fVal = 0; return false; }

    public virtual bool get(string indexName, out string strVal) { strVal = ""; return false; }

    public virtual bool get(string index, out UInt64 val) { val = 0; return false; }

    public virtual bool get(string index, out tNiceData val) { val = null; return false; }

    public abstract bool serialize(ref DataBuffer resultData);

    public abstract bool restore(ref DataBuffer scrData);

    public abstract void dump(string info);

    public virtual bool FullData(object scrObject) { return false; }
    public virtual bool RestoreToTarget(ref object targetObject) { return false; }

    public bool SetStringArray(string szKey, ref String[] scrArray)
    {
        DataBuffer d = new DataBuffer(64);
        if (d.writeStringArray(ref scrArray))
        {
            set(szKey, d);
            return true;
        }
        return false;
    }

    public bool GetStringArray(string szKey, out String[] resultArray)
    {
        DataBuffer d = getObject(szKey) as DataBuffer;
        if (d == null)
        {
            resultArray = null;
            return false;
        }
        d.seek(0);
        return d.readStringArray(out resultArray);
    }

    public bool SetIntArray(string szKey, ref int[] scrArray)
    {
        DataBuffer d = new DataBuffer(64);
        if (d.writeIntArray(ref scrArray))
        {
            set(szKey, d);
            return true;
        }
        return false;
    }

    public bool GetIntArray(string szKey, out int[] resultArray)
    {
        DataBuffer d = getObject(szKey) as DataBuffer;
        if (d == null)
        {
            resultArray = null;
            return false;
        }
        d.seek(0);
        return d.readIntArray(out resultArray);
    }

    public bool SetUintArray(string szKey, ref uint[] scrArray)
    {
        DataBuffer d = new DataBuffer(64);
        if (d.writeUintArray(ref scrArray))
        {
            set(szKey, d);
            return true;
        }
        return false;
    }

    public bool GetUintArray(string szKey, out uint[] resultArray)
    {
        DataBuffer d = getObject(szKey) as DataBuffer;
        if (d == null)
        {
            resultArray = null;
            return false;
        }
        d.seek(0);
        return d.readUintArray(out resultArray);
    }

    public virtual bool get(string index, out bool val)
    {
        object obj = null;
        if (getData(index, out obj))
        {
            Data d;
            d.mObj = obj;
            val = d;
            return true;
        }
        val = false;
        return false;
    }

    public virtual bool get(int index, out bool val)
    {
        return get(index.ToString(), out val);
    }

    public int getInt(string indexName)
    {

        return get(indexName);
    }

    public float getFloat(string indexName)
    {

        return get(indexName);
    }

    public string getString(string indexName)
    {
        return get(indexName);
    }

    public bool getBool(string indexName)
    {
        return get(indexName);
    }

    public NiceData getNice(string indexName)
    {
        return getObject(indexName) as NiceData;
    }
}
