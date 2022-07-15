using System;


namespace Logic
{
    class IntFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_INT; }
        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.write((int)0);
                return true;
            }
            resultData.write((int)data);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            int val;
            if (scrData.read(out val))
            {
                data = val;
                return true;
            }
            data = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    dataList[mPosition] = (int)obj;
                }
                else if (obj.GetType() == typeof(string))
                {
                    try
                    {
                        dataList[mPosition] = int.Parse((string)obj);
                    }
                    catch
                    {
                        dataList[mPosition] = 0;
                    }
                }
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (int)dataList[mPosition];
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (int)dataList[mPosition];
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = dataList[mPosition].ToString();
                return true;
            }
            strVal = "";
            return false;
        }
        public override int getIndex(object[] dataList)
        {
            return (int)dataList[mPosition];
        }
        public override int MakeIndex(int nVal) { return nVal; }
        public override int MakeIndex(string strVal) { int id = 0; int.TryParse(strVal, out id); return id; }
    };
    //--------------------------------------------------------------------------------

    class ShortFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_SHORT; }

        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.write((short)0);
                return true;
            }
            resultData.write((short)data);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            short val;
            if (scrData.read(out val))
            {
                data = val;
                return true;
            }
            data = (short)0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(short))
                    dataList[mPosition] = obj;
                else if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = (short)(int)obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    dataList[mPosition] = (short)(float)obj;
                }
                else if (obj.GetType() == typeof(string))
                {
                    try
                    {
                        dataList[mPosition] = short.Parse((string)obj);
                    }
                    catch
                    {
                        dataList[mPosition] = (short)0;
                    }
                }
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (short)dataList[mPosition];
                return true;
            }
            nVal = (short)0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (short)dataList[mPosition];
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = dataList[mPosition].ToString();
                return true;
            }
            strVal = "";
            return false;
        }
        public override int getIndex(object[] dataList)
        {
            return (short)dataList[mPosition];
        }
        public override int MakeIndex(int nVal) { return nVal; }
        public override int MakeIndex(string strVal) { int id = 0; int.TryParse(strVal, out id); return id; }
    }
    //--------------------------------------------------------------------------------

    class StringFieldInfo : BaseFieldInfo
    {
        protected int mMaxLength = 32;

        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_STRING; }

        public override string getTypeParam()
        {
            return mMaxLength.ToString();
        }

        public override bool saveParam(ref DataBuffer resultData) { resultData.write(mMaxLength); return true; }
        public override bool restoreParam(ref DataBuffer scrData)
        {
            if (scrData.read(out mMaxLength))
                return true;
            mMaxLength = 32;
            return false;
        }

        public override void setMaxLength(int maxLen)
        {
            mMaxLength = maxLen;
        }
        public override int getMaxLength()
        {
            return mMaxLength;
        }
        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.writeString("");
                return true;
            }
            resultData.writeString((string)data);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            string val;
            if (scrData.readString(out val))
            {
                data = val;
                return true;
            }
            data = "";
            return false;
        }
        //------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = obj.ToString();
                }
                else if (obj.GetType() == typeof(float))
                {
                    dataList[mPosition] = obj.ToString();
                }
                else if (obj.GetType() == typeof(string))
                {
                    try
                    {
                        dataList[mPosition] = (string)obj;
                    }
                    catch
                    {
                        dataList[mPosition] = "";
                    }
                }
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            nVal = 0;
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                try
                {
                    nVal = int.Parse(dataList[mPosition].ToString());
                }
                catch
                {

                }
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                try
                {
                    fVal = float.Parse(dataList[mPosition].ToString());
                }
                catch
                {
                    fVal = 0.0f;
                }
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = (string)dataList[mPosition];
                return true;
            }
            strVal = "";
            return false;
        }

        public override int getIndex(object[] dataList)
        {
            return StaticDefine.StringID((string)dataList[mPosition]);
        }

        public override int MakeIndex(int nVal) { return StaticDefine.StringID(nVal.ToString()); }
        public override int MakeIndex(string strVal) { return StaticDefine.StringID(strVal); }
    };
    //--------------------------------------------------------------------------------

    class FloatFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_FLOAT; }
        //--------------------------------------------------------------------------------
        public override int getIndex(object[] dataList)
        {
            Logic.EventCenter.Log(LOG_LEVEL.WARN, "Use float type index");
            int key = 0;
            if (get(dataList, out key))
                return key;
            return 0;
        }

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.write((float)0f);
                return true;
            }
            resultData.write((float)data);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            float val;
            if (scrData.read(out val))
            {
                data = val;
                return true;
            }
            data = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = (float)(int)obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    dataList[mPosition] = obj;
                }
                else if (obj.GetType() == typeof(string))
                {
                    try
                    {
                        dataList[mPosition] = float.Parse((string)obj);
                    }
                    catch
                    {
                        dataList[mPosition] = 0.0f;
                    }
                }
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (int)dataList[mPosition];
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (float)dataList[mPosition];
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = dataList[mPosition].ToString();
                return true;
            }
            strVal = "";
            return false;
        }
    };
    //--------------------------------------------------------------------------------

    class DoubleFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_DOUBLE; }
        //--------------------------------------------------------------------------------
        public override int getIndex(object[] dataList)
        {
            Logic.EventCenter.Log(LOG_LEVEL.WARN, "Use double type index");
            int key = 0;
            if (get(dataList, out key))
                return key;
            return 0;
        }

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.write((double)0f);
                return true;
            }
            resultData.write((double)data);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            float val;
            if (scrData.read(out val))
            {
                data = (double)val;
                return true;
            }
            data = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = (float)(int)obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    dataList[mPosition] = (double)(float)obj;
                }
                else if (obj.GetType() == typeof(double))
                {
                    dataList[mPosition] = (double)obj;
                }
                else if (obj.GetType() == typeof(string))
                {
                    try
                    {
                        dataList[mPosition] = float.Parse((string)obj);
                    }
                    catch
                    {
                        dataList[mPosition] = 0.0f;
                    }
                }
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (int)(double)dataList[mPosition];
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (float)(double)dataList[mPosition];
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = dataList[mPosition].ToString();
                return true;
            }
            strVal = "";
            return false;
        }
    };
    //--------------------------------------------------------------------------------

    class NiceDataFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_NICEDATA; }
        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            tNiceData d = data as tNiceData;
            byte dataType = d == null ? (byte)NICEDATA_TYPE.NULL_NICEDATA : (byte)d.getType();

            //tNiceData d = data as NiceData;
            //if (d != null)
            //{
            //    if ((d as NiceData).UseID() || (d as NiceData).mbSaveIDData)
            //        dataType = (byte)NICEDATA_TYPE.ID_NICEDATA;
            //    else
            //        dataType = (byte)NICEDATA_TYPE.NICEDATA;
            //}
            //else
            //{
            //    d = data as ArrayNiceData;
            //    if (d != null)
            //        dataType = (byte)NICEDATA_TYPE.ARRAY_NICEDATA;
            //    else
            //    {
            //        d = data as IDNiceData;
            //        if (d != null)
            //            dataType = (byte)NICEDATA_TYPE.ID_NICEDATA;
            //    }
            //}

            resultData.write(dataType);
            if (d != null)
                return d.serialize(ref resultData);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            byte dataType = (byte)NICEDATA_TYPE.NULL_NICEDATA;
            if (scrData.read(out dataType))
            {
                tNiceData d = null;

                //if (dataType == (byte)NICEDATA_TYPE.ID_NICEDATA)
                //    d = new IDNiceData();
                //else 
                if (dataType == (byte)NICEDATA_TYPE.ARRAY_NICEDATA)
                    d = new ArrayNiceData();
                else if (dataType == (byte)NICEDATA_TYPE.NICEDATA)
                    d = new ExNiceData();

                if (d != null)
                {
                    if (d.restore(ref scrData))
                    {
                        data = d;
                        return true;
                    }
                    data = null;
                    return false;
                }
                data = new ExNiceData();
                return true;
            }
            data = null;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(NiceData))
                {
                    dataList[mPosition] = obj;
                    return true;
                }
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {

            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {

            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {

            strVal = "";
            return false;
        }
    };
    //--------------------------------------------------------------------------------

    class BoolFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_BOOL; }
        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.write((byte)0);
                return true;
            }
            byte d = (byte)((bool)data ? 1 : 0);
            resultData.write(d);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            byte val;
            if (scrData.read(out val))
            {
                data = (bool)(val != 0);
                return true;
            }
            data = false;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = (int)obj != 0;
                }
                else if (obj.GetType() == typeof(float))
                {
                    float v = (float)obj;
                    dataList[mPosition] = v > 0.0000001 || v < -0.0000001;
                }
                else if (obj.GetType() == typeof(string))
                {
                    dataList[mPosition] = ((string)obj) == "True" || ((string)obj) == "TRUE" || ((string)obj) == "yes" || ((string)obj) == "1" || ((string)obj) == "YES" || ((string)obj) == "true";
                }
                else if (obj.GetType() == typeof(bool))
                    dataList[mPosition] = obj;
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (bool)dataList[mPosition] ? 1 : 0;
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (bool)dataList[mPosition] ? 1.0f : 0.0f;
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = (bool)dataList[mPosition] ? "true" : "false";
                return true;
            }
            strVal = "";
            return false;
        }
    };
    //--------------------------------------------------------------------------------
    class UINT64FieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_UINT64; }
        //--------------------------------------------------------------------------------

        public override int getIndex(object[] dataList)
        {
            int key = 0;
            if (get(dataList, out key))
                return key;
            return 0;
        }

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (data != null)
            {
                try
                {
                    resultData.write((UInt64)data);
                    return true;
                }
                catch
                {
                    LOG.logError(data.ToString() + " > type is not UInt64");
                    return false;
                }
            }
            else
                resultData.write((UInt64)0);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            UInt64 val;
            if (scrData.read(out val))
            {
                data = val;
                //data = (bool)(val != 0);
                return true;
            }
            data = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = (int)obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    float v = (float)obj;
                    dataList[mPosition] = (UInt64)v;
                }
                else if (obj.GetType() == typeof(string))
                {
                    dataList[mPosition] = UInt64.Parse((string)obj); //!(((string)obj) == "false" || ((string)obj) == "0" || ((string)obj) == "NO");
                }
                else if (obj.GetType() == typeof(bool))
                    dataList[mPosition] = 1;
                else if (obj.GetType() == typeof(UInt64))
                    dataList[mPosition] = (UInt64)obj;
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (int)((UInt64)dataList[mPosition] & 0Xffffffff);
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (float)(UInt64)dataList[mPosition];
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = ((UInt64)dataList[mPosition]).ToString();
                return true;
            }
            strVal = "";
            return false;
        }

        public override bool get(object[] dataList, out UInt64 result)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                result = (UInt64)dataList[mPosition];
                return true;
            }
            result = 0;
            return false;
        }
    };

    //--------------------------------------------------------------------------------

    class ByteFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_BYTE; }
        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            if (null == data)
            {
                resultData.write((byte)0);
                return true;
            }
            resultData.write((byte)data);
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            byte val;
            if (scrData.read(out val))
            {
                data = val;
                return true;
            }
            data = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(byte))
                    dataList[mPosition] = obj;
                else if (obj.GetType() == typeof(int))
                {
                    dataList[mPosition] = (byte)obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    dataList[mPosition] = (byte)obj;
                }
                else if (obj.GetType() == typeof(string))
                {
                    try
                    {
                        dataList[mPosition] = byte.Parse((string)obj);
                    }
                    catch
                    {
                        dataList[mPosition] = 0;
                    }
                }
                else
                    return false;

                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                nVal = (byte)dataList[mPosition];
                return true;
            }
            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                fVal = (byte)dataList[mPosition];
                return true;
            }
            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                strVal = dataList[mPosition].ToString();
                return true;
            }
            strVal = "";
            return false;
        }
        public override int getIndex(object[] dataList)
        {
            return (int)dataList[mPosition];
        }
        public override int MakeIndex(int nVal) { return nVal; }
        public override int MakeIndex(string strVal) { return int.Parse(strVal); }
    };

    //--------------------------------------------------------------------------------
    class DataFieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_DATA; }
        //--------------------------------------------------------------------------------

        public override bool serialize(object data, ref DataBuffer resultData)
        {
            DataBuffer d = data as DataBuffer;
            
            if (d != null)
                return resultData.writeData(ref d, d.size());
            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            var d = new DataBuffer();
            data = d;
            if (!scrData.readData(ref d))
            {
                LOG.logError("Read data fail");
                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool set(ref object[] dataList, object obj)
        {
            if (mPosition < dataList.Length)
            {
                if (obj.GetType() == typeof(DataBuffer))
                {
                    dataList[mPosition] = obj;
                    return true;
                }
            }
            return false;
        }
        //--------------------------------------------------------------------------------

        public override object get(object[] dataList)
        {
            if (mPosition < dataList.Length)
            {
                return dataList[mPosition];
            }
            return null;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out int nVal)
        {

            nVal = 0;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out float fVal)
        {

            fVal = 0.0f;
            return false;
        }
        //--------------------------------------------------------------------------------

        public override bool get(object[] dataList, out string strVal)
        {

            strVal = "";
            return false;
        }
    };
    //--------------------------------------------------------------------------------

}
