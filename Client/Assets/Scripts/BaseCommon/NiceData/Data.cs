using System;
using Logic;

public struct Data
{
    static public Data NULL = new Data(null);

    public object mObj;

    //public Data() { }
    public Data(object val) { mObj = val; }

    public bool Empty() { return mObj == null; }

    public override string ToString()
    {
        return (string)this;
    }
    public string String()
    {
        return (string)this;
    }
    public int Int()
    {
        return (int)this;
    }

    public float Float()
    {
        return (float)this;
    }

    static public implicit operator int(Data current)
    {
        return toInt(current.mObj);
    }
    
    static public implicit operator uint(Data current)
    {
        return (uint)toInt(current.mObj);
    }
    
    static public int toInt(object value)
    {
        if (/*current==null ||*/ value == null)
            return 0;

        if (typeof(int) == value.GetType())
            return (int)value;

        if (typeof(short) == value.GetType())
            return (int)(short)value;

        if (typeof(float) == value.GetType())
            return (int)(float)value;

        if (typeof(double) == value.GetType())
            return (int)((double)value);

        if (typeof(UInt64) == value.GetType())
            return (int)(UInt64)value;

        if (typeof(byte) == value.GetType())
            return (int)(byte)value;

        if (typeof(bool) == value.GetType())
            return (bool)value ? 1 : 0;

        if (typeof(string) == value.GetType())
        {
            string s = (string)value;
            if (s != "")
            {
                int d = 0;
                if (int.TryParse(s, out d))
                    return d;
                else
                {
                    EventCenter.LogError("string to int fail >" + s);
                }
            }
            return 0;
        }
        if (typeof(byte) == value.GetType())
        {
            return (int)((byte)value);
        }
        EventCenter.LogError("No unkwon data type>" + value.GetType().ToString());
        return 0;
    }

    static public implicit operator short(Data current)
    {
        return (short)(int)current;
    }

    static public implicit operator byte(Data current)
    {
        return (byte)(int)current;
    }

    static public implicit operator float(Data current)
    {
        if (/*current==null ||*/ current.mObj == null)
            return 0;

        if (typeof(int) == current.mObj.GetType())
            return (float)(int)current.mObj;

        if (typeof(float) == current.mObj.GetType())
            return (float)current.mObj;

        if (typeof(double) == current.mObj.GetType())
            return (float)((double)current.mObj);

        if (typeof(UInt64) == current.mObj.GetType())
            return (float)(UInt64)current.mObj;

        if (typeof(byte) == current.mObj.GetType())
            return (float)(byte)current.mObj;

        if (typeof(string) == current.mObj.GetType())
        {
            string s = (string)current.mObj;
            if (s != "")
            {
                float f = 0;
                if (float.TryParse(s, out f))
                    return f;
            }
            return 0;
        }
        if (typeof(bool) == current.mObj.GetType())
        {
            bool s = (bool)current.mObj;
            return s ? 1 : 0;
        }
        if (typeof(byte) == current.mObj.GetType())
        {
            return (float)((byte)current.mObj);
        }
        EventCenter.LogError("No unkwon data type>" + current.mObj.GetType().ToString());
        return 0;
    }
    
    static public implicit operator string(Data current)
    {
        if (/*current==null ||*/ current.mObj == null)
            return "";

        if (typeof(int) == current.mObj.GetType())
            return ((int)current.mObj).ToString();

        if (typeof(short) == current.mObj.GetType())
            return current.mObj.ToString();

        if (typeof(float) == current.mObj.GetType())
            return ((float)current.mObj).ToString();

        if (typeof(double) == current.mObj.GetType())
            return ((double)current.mObj).ToString();

        if (typeof(string) == current.mObj.GetType())
            return (string)current.mObj;

        if (typeof(bool) == current.mObj.GetType())
            return (string)current.mObj.ToString();

        if (typeof(byte) == current.mObj.GetType())
        {
            return current.mObj.ToString();
        }

        if (typeof(UInt64) == current.mObj.GetType())
            return current.mObj.ToString();

        EventCenter.LogError("No unkwon data type>" + current.mObj.GetType().ToString());
        return current.mObj.ToString();
    }

    static public implicit operator bool(Data current)
    {
        if (/*current==null ||*/ current.mObj == null)
            return false;

        if (typeof(bool) == current.mObj.GetType())
            return ((bool)current.mObj);

        if (typeof(int) == current.mObj.GetType())
            return ((int)current.mObj) != 0;

        if (typeof(float) == current.mObj.GetType())
        {
            float v = (float)current.mObj;
            return v < -0.00001f || v > 0.00001f;
        }

        if (typeof(double) == current.mObj.GetType())
            return (double)current.mObj > 0;

        if (typeof(string) == current.mObj.GetType())
        {
            string v = (string)current.mObj;
            v = v.ToLower();
            if (v == "yes" || v == "true")
                return true;
            int d = 0;
            int.TryParse(v, out d);
            return d != 0;
        }
        if (typeof(byte) == current.mObj.GetType())
        {
            return ((byte)current.mObj) != 0;
        }

        if (typeof(UInt64) == current.mObj.GetType())
            return (UInt64)current.mObj != 0;

        EventCenter.LogError("No unkwon data type>" + current.mObj.GetType().ToString());
        return false;
    }

    static public implicit operator UInt64(Data current)
    {
        if (/*current==null ||*/ current.mObj == null)
            return 0; //(UInt64)(0xffffffffffffffff);

        if (typeof(UInt64) == current.mObj.GetType())
            return ((UInt64)current.mObj);

        if (typeof(string) == current.mObj.GetType())
        {
            string s = (string)current.mObj;
            if (s != "")
            {
                UInt64 d = 0;
                if (UInt64.TryParse(s, out d))
                    return d;
            }
            return 0;
        }

        if (typeof(double) == current.mObj.GetType())
            return (UInt64)((double)current.mObj);

        if (typeof(byte) == current.mObj.GetType())
        {
            return (UInt64)((byte)current.mObj);
        }

        return (UInt64)((int)current);
    }

    static public implicit operator Int64(Data current)
    {
        return (Int64)(UInt64)current;
    }
}
