using System;
using Logic;

namespace Logic
{
    public class DataRecord
    {
        public NiceTable mTable;
        FieldIndex mFieldIndex;
        object[] mDataList;
        //-------------------------------------------------------------------------
        public Data this[string indexName]
        {
            //实现索引器的get方法
            get
            {
                return getData(indexName);
            }
            //实现索引器的set方法
            set
            {
                set(indexName, value.mObj);
            }
        }
        public Data this[int indexCol]
        {
            //实现索引器的get方法
            get
            {
                return getData(indexCol);
            }
            //实现索引器的set方法
            set
            {
                set(indexCol, value.mObj);
            }
        }

        public string getString(string fieldName)
        {
            return getObject(fieldName).ToString();
        }

        public string getString(int col)
        {
            return getObject(col).ToString();
        }
        
        public int getInt(string fieldName)
        {
            return Data.toInt(getObject(fieldName));
        }

        public int getInt(int col)
        {
            return Data.toInt(getObject(col));
        }

        //--------------------------------------------------------------------------------
        public DataRecord(NiceTable t)
        {
            mTable = t;
            if (t != null)
                SetFieldIndex(t.GetField());
        }

        public DataRecord(NiceTable t, FieldIndex fieldIndex)
        {
            mTable = t;
            SetFieldIndex(fieldIndex);
        }
        //--------------------------------------------------------------------------------
        public virtual bool NeedUpdate() { return false; }
        public virtual void FullAllUpdate(bool b) { }
        public virtual bool HadChanged(int col) { return false; }
        public virtual bool NotifyChanged(int col) { return false; }

        public FieldIndex GetFieldIndex() { return mFieldIndex; }

        public bool SetFieldIndex(FieldIndex fieldIndex)
        {
            if (!fieldIndex.Ready())
                return false;

            mFieldIndex = fieldIndex;
            mDataList = new object[mFieldIndex.Count()];
            return true;
        }

        tFieldInfo GetFieldInfo(int col)
        {
            return mFieldIndex.GetField(col);
        }

        int GetFieldCol(string fieldName)
        {
            return mFieldIndex.GetCol(fieldName);
        }
        public object[] GetDataList() { return mDataList; }


        public bool restore(ref DataBuffer data)
        {
            for (int i = 0; i < mFieldIndex.Count(); ++i)
            {
                tFieldInfo info = mFieldIndex.GetField(i);
                if (!info.restore(ref mDataList, ref data))
                {
                    EventCenter.LogError("Record restore Fail: Field>" + info.getName());
                    return false;
                }
            }
            return true;
        }

        public bool restoreUpdate(ref DataBuffer data)
        {
            int checkCode;
            if (!data.read(out checkCode))
                return false;

            bool bHave = false;
            if (!data.read(out bHave))
                return false;

            int count = GetFieldIndex().Count();
            int x = (count + 7) / 8;

            byte[] state = new byte[x];
            if (!data.read(ref state, x))
                return false;

            for (int i = 0; i < count; ++i)
            {
                if ((state[i / 8] & (1 << (i % 8))) > 0)
                {
                    tFieldInfo info = GetFieldInfo(i);
                    if (info == null)
                    {
                        LOG.logError("字段为空 col>" + i.ToString());
                        return false;
                    }
                    if (!info.restore(out mDataList[i], ref data))
                    {
                        LOG.logError("字段恢复数据失败>" + info.getName());
                        return false;
                    }
                }
            }
            return true;
        }

        //--------------------------------------------------------------------------------
        public NiceData getNice(string fieldName)
        {
            return getObject(fieldName) as NiceData;
        }
        public NiceTable getTable(string fieldName)
        {
            return getObject(fieldName) as NiceTable;
        }
        public DataBuffer getBuffer(string fieldName)
        {
            return getObject(fieldName) as DataBuffer;
        }
        //-------------------------------------------------------------------------
        public Data getData(string colName)
        {
            return getData(GetFieldCol(colName));
        }

        public Data getData(int col)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null != info)
            {
                return new Data(info.get(mDataList));
                //switch (info.getType())
                //{
                //case FIELD_TYPE.FIELD_INT:
                //{
                //    int v;
                //    if (info.get (mDataList, out v))
                //        value.mObj = v;
                //    break;
                //}
                //case FIELD_TYPE.FIELD_FLOAT:
                //{
                //    float v;
                //    if (info.get (mDataList, out v))
                //        value.mObj = v;
                //    break;
                //}
                //case FIELD_TYPE.FIELD_STRING:
                //{
                //    string v;
                //    if (info.get (mDataList, out v))
                //        value.mObj = v;
                //    break;
                //}
                //case FIELD_TYPE.FIELD_BOOL:
                //{
                //    int v;
                //    if (info.get (mDataList, out v))
                //        value.mObj = v;
                //    break;
                //}
                //default:
                //    Logic.EventCenter.Log(LOG_LEVEL.ERROR, "no exist type when get data from record");
                //    break;
                //}
            }

            return Data.NULL;
        }
        //--------------------------------------------------------------------------------

        public bool _set(int col, int nVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            return info.set(ref mDataList, nVal);
        }

        public bool _set(int col, float fVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            return info.set(ref mDataList, fVal);
        }

        public bool _set(int col, string strVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            return info.set(ref mDataList, strVal);
        }
        public bool _set(int col, object obj)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            return info.set(ref mDataList, obj);
        }
        //--------------------------------------------------------------------------------

        public bool set(int col, int nVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            NotifyChanged(col);
            if (mTable != null)
                mTable.OnChanger(this, col, nVal);
            return info.set(ref mDataList, nVal);
        }

        public bool set(int col, float fVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            NotifyChanged(col);
            return info.set(ref mDataList, fVal);
        }

        public bool set(int col, string strVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            NotifyChanged(col);
            if (mTable != null)
                mTable.OnChanger(this, col, strVal);
            return info.set(ref mDataList, strVal);
        }
        public bool set(int col, UInt64 val)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            NotifyChanged(col);
            if (mTable != null)
                mTable.OnChanger(this, col, val);
            return info.set(ref mDataList, val);
        }
        public bool get(int col, out int nVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
            {
                nVal = 0;
                return false;
            }
            return info.get(mDataList, out nVal);
        }

        public bool get(int col, out float fVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
            {
                fVal = 0f;
                return false;
            }
            return info.get(mDataList, out fVal);
        }

        public bool get(int col, out string strVal)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
            {
                strVal = "";
                return false;
            }
            return info.get(mDataList, out strVal);
        }
        public bool get(int col, out UInt64 resultValue)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
            {
                resultValue = 0;
                return false;
            }
            return info.get(mDataList, out resultValue);
        }
        public bool set(int col, object obj)
        {
            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return false;
            NotifyChanged(col);
            if (mTable != null)
                mTable.OnChanger(this, col, obj);
            return info.set(ref mDataList, obj);
        }

        private Data get(int col)
        {
            return new Data(getObject(col));
        }

        //------------------------------------------------------------------------

        public bool set(string fieldName, int nVal)
        {
            return set(GetFieldCol(fieldName), nVal);
        }

        public bool set(string fieldName, float fVal)
        {
            return set(GetFieldCol(fieldName), fVal);
        }

        public bool set(string fieldName, string strVal)
        {
            return set(GetFieldCol(fieldName), strVal);
        }
        public bool set(string fieldName, UInt64 val)
        {
            return set(GetFieldCol(fieldName), val);
        }
        public bool get(string fieldName, out int nVal)
        {
            return get(GetFieldCol(fieldName), out nVal);
        }

        public bool get(string fieldName, out float fVal)
        {
            return get(GetFieldCol(fieldName), out fVal);
        }

        public bool get(string fieldName, out string strVal)
        {
            return get(GetFieldCol(fieldName), out strVal);
        }

        public bool get(string fieldName, out UInt64 result)
        {
            return get(GetFieldCol(fieldName), out result);
        }

        public bool set(string fieldName, object obj)
        {
            return set(GetFieldCol(fieldName), obj);
        }

        public Data get(string fieldName)
        {
            int col = GetFieldCol(fieldName);
            if (col < 0)
            {
                Logic.EventCenter.Log(LOG_LEVEL.WARN, " without field >>> " + fieldName);
            }
            object dataObj = getObject(col);
            if (dataObj == null)
            {
                Logic.EventCenter.Log(LOG_LEVEL.WARN, " data object is null, col >>> " + fieldName);
            }
            return new Data(dataObj);
        }

        public object getObject(string fieldName)
        {
            int col = GetFieldCol(fieldName);
            if (col < 0)
            {
                Logic.EventCenter.Log(LOG_LEVEL.WARN, " without field >>> " + fieldName);
            }
            object dataObj = getObject(col);
            if (dataObj == null)
            {
                Logic.EventCenter.Log(LOG_LEVEL.WARN, " data object is null, col >>> " + fieldName);
            }
            return dataObj;
        }

        public object getObject(int col)
        {
            if (col < 0)
            {
                Logic.EventCenter.Log(LOG_LEVEL.WARN, " without field >>> " + col.ToString());
            }

            tFieldInfo info = mFieldIndex.GetField(col);
            if (null == info)
                return null;

            return info.get(mDataList);
        }

        
        public float getFloat(string fieldName) { return (float)get(fieldName); }
        
        //-------------------------------------------------------------------------------

        public int getIndex()
        {
            int indexCol = mTable != null ? mTable.GetIndexCol() : 0;
            tFieldInfo info = mFieldIndex.GetField(indexCol);
            if (null != info)
                return info.getIndex(mDataList);
            return 0;
        }

        public bool Contains(string key)
        {
            return mFieldIndex.ContainsCol(key);
        }
    }


}
