using System;
using System.Collections.Generic;
using Logic;

namespace Logic
{
    public class FieldIndex
    {
        tFieldInfo[] mFieldList = null;
        Dictionary<string, int> mFieldMap = new Dictionary<string, int>();
        //public NiceTable                mTable;
        protected int mCheckCode = 0;
        bool mbNeedResetCode = false;
        //--------------------------------------------------------------------------------
        public tFieldInfo[] GetFieldList() { return mFieldList; }
        public int Count() { return mFieldMap.Count; }
        public bool Ready()
        {
            return mFieldList != null && mFieldMap.Count > 0 && mFieldMap.Count == mFieldList.Length;
        }
        //public NiceTable GetTable() { return mTable; }

        public void Reset(NiceTable table) { mFieldList = null; mFieldMap.Clear(); }

        public int GetCheckCode()
        {
            if (mbNeedResetCode || mCheckCode == 0)
            {
                string str = "";
                for (int i = 0; i < Count(); ++i)
                {
                    int t = (int)GetField(i).getType();
                    str += t.ToString() + "_";
                    //if (t == (int)FIELD_TYPE.FIELD_TABLE)
                    //{
                    //    TableFieldInfo f = GetField(i) as TableFieldInfo;
                    //    if (f != null && f.mSubFieldIndex != null)
                    //        str += f.mSubFieldIndex.GetCheckCode().ToString() + "*";
                    //}
                }
                if (str.Length > 1024 * 10)
                {
                    LOG.logError("字段哈希字符长度超过 1024 * 10");
                    mCheckCode = 0;
                }
                else
                    mCheckCode = StaticDefine.StringID(str);

                mbNeedResetCode = false;
            }
            return mCheckCode;
        }
        //--------------------------------------------------------------------------------
        public bool SetField(string fieldName, FIELD_TYPE type, int col)
        {
            tFieldInfo info = FieldInfoManager.CreateFieldInfo(type);
            if (null == info)
            {
                EventCenter.LogError("No register data type>" + type.ToString());
                return false;
            }

            int existCol = 0;
            if (mFieldMap.TryGetValue(fieldName, out existCol))
            {
                EventCenter.LogError("Field already exist >" + fieldName + ", at table >" + ", col >" + col.ToString());
                return false;
            }

            info.setName(fieldName);
            info.setPosition(col);
            if (mFieldList == null)
                mFieldList = new tFieldInfo[col + 1];
            else if (col >= mFieldList.Length)
                Array.Resize<tFieldInfo>(ref mFieldList, (col + 1));
            mFieldList[col] = info;
            mFieldMap[fieldName] = col;

            mbNeedResetCode = true;
            return true;
        }

        public bool SetField(string fieldName, string typeName, int col)
        {
            FIELD_TYPE type = StaticDefine.ToType(typeName);
            if (type == FIELD_TYPE.FIELD_NULL)
                return false;

            return SetField(fieldName, type, col);
        }

        public tFieldInfo GetField(int col)
        {
            if (col >= 0 && null != mFieldList && col < mFieldList.Length)
                return mFieldList[col];

            return null;
        }

        public tFieldInfo GetField(string fieldName)
        {
            int col;
            if (mFieldMap.TryGetValue(fieldName, out col))
                return GetField(col);
            tFieldInfo info = null;
            return info;
        }

        public int GetCol(string fieldName)
        {
            int col = -1;
            if (!mFieldMap.TryGetValue(fieldName, out col))
                col = -1;
            return col;
        }

        public bool ContainsCol(string fieldName)
        {
            return mFieldMap.ContainsKey(fieldName);
        }

        public bool Save(ref DataBuffer resultData)
        {
            ushort count = (ushort)Count();
            resultData.write(count);
            for (int i = 0; i < count; ++i)
            {
                tFieldInfo t = GetField(i);
                byte b = (byte)t.getType();
                resultData.write(b);
                resultData.writeOne(t.getName());

                if (!t.saveParam(ref resultData))
                {
                    Logic.EventCenter.Log(LOG_LEVEL.ERROR, "save field param fail>" + t.getName());
                    return false;
                }
            }

            return true;
        }

        public bool Restore(ref DataBuffer scrData)
        {
            ushort count;
            if (!scrData.read(out count))
                return false;

            for (int i = 0; i < count; ++i)
            {
                byte b;
                if (!scrData.read(out b))
                    return false;
                string name;
                if (!scrData.readOne(out name))
                    return false;
                FIELD_TYPE type = (FIELD_TYPE)b;
                if (!SetField(name, type, i))
                {
                    Logic.EventCenter.Log(LOG_LEVEL.ERROR, "set field fail>[" + name + "], type>[" + type.ToString() + "]");
                    return false;
                }
                if (!mFieldList[i].restoreParam(ref scrData))
                {
                    Logic.EventCenter.Log(LOG_LEVEL.ERROR, "restore field param fail>" + name);
                    return false;
                }
            }
            Ready();
            return true;
        }

        //public override string ToString()
        //{
        //    string strData = "";
        //    for (int i = 0; i < mFieldList.Length; ++i)
        //    {
        //        tFieldInfo t = GetField(i);                                  
        //        strData += "["+StaticDefine.ToStringType(t.getType()) + ":" + t.getName() + "]";
        //    }
        //    return strData;
        //}

        public override string ToString()
        {
            string result = "";
            for (int i = 0; i < Count(); ++i)
            {
                //if (i>0)
                //	result += " ";
                tFieldInfo info = GetField(i);
                if (info == null)
                    return "";
                result += info.getName();
                result += ":";
                result += StaticDefine.ToStringType(info.getType());
                string ext = info.getTypeParam();
                if (ext.Length > 0)
                {
                    result += "*{";
                    result += ext;
                    result += "}";
                }
                else
                    result += "*";
                // 属性数据不再序列转移，以作为本地临时使用, 主要是 是否禁止更新， 是否启动触发器

                //int attrData = info->getAttributeData();
                //result += attrData;
            }

            return result;
        }
    }
}
