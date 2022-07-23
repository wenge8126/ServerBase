using System;

namespace Logic
{
    class INT64FieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType()
        {
            return FIELD_TYPE.FIELD_INT64;
        }
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
                    resultData.write((Int64) data);
                    return true;
                }
                catch
                {
                    LOG.logError(data.ToString() + " > type is not Int64");
                    return false;
                }
            }
            else
                resultData.write((Int64) 0);

            return true;
        }
        //--------------------------------------------------------------------------------

        public override bool restore(out object data, ref DataBuffer scrData)
        {
            Int64 val;
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
                    dataList[mPosition] = (int) obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    float v = (float) obj;
                    dataList[mPosition] = (Int64) v;
                }
                else if (obj.GetType() == typeof(string))
                {
                    dataList[mPosition] =
                        Int64.Parse(
                            (string) obj); //!(((string)obj) == "false" || ((string)obj) == "0" || ((string)obj) == "NO");
                }
                else if (obj.GetType() == typeof(bool))
                    dataList[mPosition] = 1;
                else if (obj.GetType() == typeof(Int64))
                    dataList[mPosition] = (Int64) obj;
                else if (obj.GetType() == typeof(UInt64))
                {
                    var v = (UInt64) obj;
                    dataList[mPosition] = (Int64) v;
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
                nVal = (int) ((Int64) dataList[mPosition]);
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
                fVal = (float) (Int64) dataList[mPosition];
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
                strVal = ((Int64) dataList[mPosition]).ToString();
                return true;
            }

            strVal = "";
            return false;
        }

        public override bool get(object[] dataList, out UInt64 result)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                var v = (Int64) dataList[mPosition];
                result = (UInt64) v;
                return true;
            }

            result = 0;
            return false;
        }
    };

    /// <summary>
    /// UInt64
    /// </summary>
    public class UINT64FieldInfo : BaseFieldInfo
    {
        public override FIELD_TYPE getType()
        {
            return FIELD_TYPE.FIELD_UINT64;
        }
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
                    resultData.write((UInt64) data);
                    return true;
                }
                catch
                {
                    LOG.logError(data.ToString() + " > type is not UInt64");
                    return false;
                }
            }
            else
                resultData.write((UInt64) 0);

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
                    dataList[mPosition] = (int) obj;
                }
                else if (obj.GetType() == typeof(float))
                {
                    float v = (float) obj;
                    dataList[mPosition] = (UInt64) v;
                }
                else if (obj.GetType() == typeof(string))
                {
                    dataList[mPosition] =
                        UInt64.Parse(
                            (string) obj); //!(((string)obj) == "false" || ((string)obj) == "0" || ((string)obj) == "NO");
                }
                else if (obj.GetType() == typeof(bool))
                    dataList[mPosition] = 1;
                else if (obj.GetType() == typeof(UInt64))
                    dataList[mPosition] = (UInt64) obj;
                else if (obj.GetType() == typeof(Int64))
                {
                    var v = (Int64) obj;
                    dataList[mPosition] = (UInt64) v;
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
                nVal = (int) ((UInt64) dataList[mPosition] & 0Xffffffff);
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
                fVal = (float) (UInt64) dataList[mPosition];
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
                strVal = ((UInt64) dataList[mPosition]).ToString();
                return true;
            }

            strVal = "";
            return false;
        }

        public override bool get(object[] dataList, out UInt64 result)
        {
            if (mPosition < dataList.Length && null != dataList[mPosition])
            {
                result = (UInt64) dataList[mPosition];
                return true;
            }

            result = 0;
            return false;
        }
    };
}