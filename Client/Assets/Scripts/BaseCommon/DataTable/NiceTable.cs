using System;
using System.Collections.Generic;

using System.Text;
using System.IO;
using Logic;

namespace Logic
{
    // 迭代表格
    public class TableIt
    {
        Dictionary<string, DataRecord>.ValueCollection mItColl;
        Dictionary<string, DataRecord>.ValueCollection.Enumerator mIt;

        public TableIt(NiceTable t)
        {
            mItColl = t.GetAllRecord().Values;
            mIt = mItColl.GetEnumerator();
        }

        public DataRecord GetRecord()
        {
            return mIt.Current;
        }

        public bool MoveNext()
        {
            return mIt.MoveNext();
        }

        public void Begin()
        {
            mIt = mItColl.GetEnumerator();
        }
    }


    public class NiceTable
    {
        static public Encoding fileEncoding = Encoding.Default;
        static public bool SetFileEncoding(int codePage)
        {
            fileEncoding = Encoding.GetEncoding(codePage);
            return fileEncoding != null;
        }
        static public void SetUnicodeEncoding()
        {
            fileEncoding = Encoding.Unicode;
        }

        public TableIt GetTableIt()
        {
            return new TableIt(this);
        }

        public DataRecord this[string indexName]
        {
            //实现索引器的get方法
            get
            {
                return GetRecord(indexName);
            }
        }
        public DataRecord this[int nIndex]
        {
            //实现索引器的get方法
            get
            {
                return GetRecord(nIndex);
            }
        }

        Dictionary<string, DataRecord> mRecordMap = new Dictionary<string, DataRecord>();
        protected FieldIndex mFieldIndex = new FieldIndex();
        int mIndexCol = 0;
        public string mTableIndex = "DEFAULT";
        //--------------------------------------------------------------------------------
        public NiceTable()
        {
            //mFieldIndex.mTable = this;
        }

        public NiceTable(FieldIndex field)
        {
            if (field != null)
                mFieldIndex = field;
        }


        public void SetField(string fieldName, FIELD_TYPE type, int col)
        {
            mFieldIndex.SetField(fieldName, type, col);
        }

        public void SetField(string fieldName, string strType, int col)
        {
            mFieldIndex.SetField(fieldName, strType, col);
        }

        public void ResetField()
        {
            mRecordMap.Clear();

            if (mFieldIndex != null)
                mFieldIndex.Reset(this);
        }

        public FieldIndex GetField() { return mFieldIndex; }
        public Dictionary<string, DataRecord> GetAllRecord() { return mRecordMap; }
        public int GetRecordCount() { return mRecordMap.Count; }
        public int GetIndexCol() { return mIndexCol; }

        public bool AppendRecord(DataRecord r, bool bReplace)
        {
            string indexKey = r.getObject(mIndexCol).ToString();
            if (!bReplace)
            {
                DataRecord existRe;
                if (mRecordMap.TryGetValue(indexKey, out existRe) && existRe != null)
                    return false;
            }

            mRecordMap[indexKey] = r;
            return true;
        }

        public Data GetData(string key, string field)
        {
            DataRecord re = GetRecord(key);
            if (re != null)
                return re.get(field);

            return new Data();
        }

        public Data GetData(int key, string field)
        {
            DataRecord re = GetRecord(key);
            if (re != null)
                return re.get(field);

            return new Data();
        }

        public override string ToString()
        {
            string strData = "{" + mFieldIndex.ToString() + "}";

            foreach (var r in mRecordMap)
            {
                object[] d = r.Value.GetDataList();
                if (null != d)
                {
                    strData += "{";
                    for (int i = 0; i < d.Length; ++i)
                    {

                        strData += "[";

                        if (d[i] != null)
                            strData += d[i].ToString();
                        strData += "]";
                    }
                    strData += "}";
                }
            }

            return strData;
        }

        public void OnChanger(DataRecord record, int col, int destVal)
        {
            if (GetIndexCol() == col)
            {
                string index = record.getObject(mIndexCol).ToString();
                mRecordMap.Remove(index);
                mRecordMap[ConvertIndex(destVal)] = record;
            }
        }
        public void OnChanger(DataRecord record, int col, string destVal)
        {
            if (GetIndexCol() == col)
            {
                string index = record.getObject(mIndexCol).ToString();
                mRecordMap.Remove(index);
                mRecordMap[ConvertIndex(destVal)] = record;
            }
        }

        public void OnChanger(DataRecord record, int col, object destVal)
        {

        }
        //--------------------------------------------------------------------------------
        string ConvertIndex(int nIndex)
        {
            return nIndex.ToString();
            //tFieldInfo info = mFieldIndex.GetField(GetIndexCol());
            //if (null == info)
            //    return 0;
            //return info.MakeIndex(nIndex);
        }

        string ConvertIndex(string strIndex)
        {
            return strIndex;
            //tFieldInfo info = mFieldIndex.GetField(GetIndexCol());
            //if (null == info)
            //    return 0;
            //return info.MakeIndex(strIndex);
        }

        protected DataRecord _CreateRecord(string index)
        {
            DataRecord r = NewRecord();
           // r._set(GetIndexCol(), index);
            mRecordMap[index] = r;
            return r;
        }

        public virtual DataRecord NewRecord() { return new DataRecord(this, mFieldIndex); }

        public DataRecord CreateRecord(int index)
        {
            DataRecord r = _CreateRecord(ConvertIndex(index));
            if (GetField().GetField(GetIndexCol()).getType() == FIELD_TYPE.FIELD_UINT64)
                r._set(GetIndexCol(), (UInt64)index);
            else
                r._set(GetIndexCol(), index);
            return r;
        }

        public DataRecord CreateRecord(string index)
        {
            DataRecord r = _CreateRecord(ConvertIndex(index));
            r._set(GetIndexCol(), index);
            return r;
        }

        public bool DeleteRecord(int index)
        {
            return mRecordMap.Remove(ConvertIndex(index));
        }

        public bool DeleteRecord(string index)
        {
            return mRecordMap.Remove(ConvertIndex(index));
        }

        public void ClearAll()
        {
            mRecordMap.Clear();
        }

        public DataRecord GetRecord(int index)
        {
            DataRecord destRecord;
            mRecordMap.TryGetValue(ConvertIndex(index), out destRecord);
            return destRecord;
        }

        public DataRecord GetRecord(string index)
        {
            DataRecord destRecord;
            mRecordMap.TryGetValue(ConvertIndex(index), out destRecord);
            return destRecord;
        }

        public DataRecord GetFirstRecord()
        {
            foreach (var kRe in mRecordMap)
            {
                return kRe.Value;
            }
            return null;
        }

        public bool SaveTable(string pathFileName)
        {
            return SaveTable(pathFileName, fileEncoding);
        }

        public bool SaveTable(string pathFileName, Encoding encoding)
        {
            try
            {
                FileStream fs = new FileStream(pathFileName, FileMode.Create);
                return SaveTable(fs, encoding);
            }
            catch (System.Exception e)
            {
                EventCenter.LogError("Create table file fail >" + pathFileName + ", Error>" + e.ToString());
            }
         
            return false;
        }

        public bool SaveTable(ref DataBuffer resultBuffer, Encoding encoding)
        {
            if (!mFieldIndex.Ready())
                return true;

            try
            {

                for (int i = 0; i < mFieldIndex.Count(); ++i)
                {
                    tFieldInfo info = mFieldIndex.GetField(i);
                    string typeName = StaticDefine.ToStringType(info.getType());
                    resultBuffer._writeString(typeName);
                    if (i < mFieldIndex.Count() - 1)
                        resultBuffer._writeString(",");
                }
                resultBuffer._writeString("\n");
                for (int i = 0; i < mFieldIndex.Count(); ++i)
                {
                    tFieldInfo info = mFieldIndex.GetField(i);
                    string name = info.getName();
                    resultBuffer._writeString(name);
                    if (i < mFieldIndex.Count() - 1)
                        resultBuffer._writeString(",");
                }
                resultBuffer._writeString("\n");
                foreach (var r in mRecordMap)
                {
                    object[] d = r.Value.GetDataList();
                    if (null != d)
                    {
                        for (int i = 0; i < d.Length; ++i)
                        {
                            if (d[i] != null)
                                resultBuffer._writeString(d[i].ToString());
                            if (i < d.Length - 1)
                                resultBuffer._writeString(",");
                        }
                        resultBuffer._writeString("\n");
                    }
                }

            }
            catch (Exception e)
            {
                Log(e.ToString());
                return false;
            }
            return true;

        }

        public bool SaveTable(Stream resultData, Encoding encoding)
        {
            if (!mFieldIndex.Ready())
                return true;

            try
            {
                //StreamWriter writer = new StreamWriter(pathFileName, false, encoding);
                StreamWriter writer = new StreamWriter(resultData, encoding);

                for (int i = 0; i < mFieldIndex.Count(); ++i)
                {
                    tFieldInfo info = mFieldIndex.GetField(i);
                    string typeName = StaticDefine.ToStringType(info.getType());
                    writer.Write(typeName);
                    if (i < mFieldIndex.Count() - 1)
                        writer.Write(',');
                }
                writer.Write("\r\n");
                for (int i = 0; i < mFieldIndex.Count(); ++i)
                {
                    tFieldInfo info = mFieldIndex.GetField(i);
                    string name = info.getName();
                    writer.Write(name);
                    if (i < mFieldIndex.Count() - 1)
                        writer.Write(',');
                }
                writer.Write("\r\n");
                foreach (var r in mRecordMap)
                {
                    object[] d = r.Value.GetDataList();
                    if (null != d)
                    {
                        for (int i = 0; i < d.Length; ++i)
                        {
                            if (d[i] != null)
                            {
                                String value = d[i].ToString();
                                if (value.IndexOf(',') >= 0)
                                    writer.Write('"');
                                writer.Write(value);
                                if (value.IndexOf(',') >= 0)
                                    writer.Write('"');
                            }
                            if (i < d.Length - 1)
                                writer.Write(',');
                        }
                        writer.Write("\r\n");
                    }
                }
                writer.Close();
            }
            catch (Exception e)
            {
                Log(e.ToString());
                return false;
            }
            return true;
        }

        public bool LoadTable(string pathFileName)
        {
            return LoadTable(pathFileName, fileEncoding);
        }

        public bool LoadTable(string pathFileName, Encoding encoding)
        {
            try
            {
                FileStream fs = new FileStream(pathFileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                return LoadTable(fs, encoding);
            }
            catch (System.Exception e)
            {
                EventCenter.LogError("Open table file fail >" + pathFileName + ", Error>" + e.ToString());
            }
         
            return false;
        }

        public bool LoadTable(DataBuffer data, Encoding encoding)
        {
            Stream stream = new MemoryStream(data.mData);
            return LoadTable(stream, encoding);
        }

        public bool LoadTable(Stream dataStream, Encoding encoding)
        {
            ResetField();
            try
            {
                //StreamReader reader = new StreamReader(pathFileName, encoding, false);
                StreamReader reader = new StreamReader(dataStream, encoding);
                string s = "";
                if ((s = reader.ReadLine()) == null)
                    return true;

                char[] pa = { ',' };
                // get field type            
                string[] typeList = s.Split(pa);
                if (typeList.Length <= 0)
                    return true;
                if ((s = reader.ReadLine()) == null)
                    return true;
                // get field type            
                string[] nameList = s.Split(pa);
                if (typeList.Length != nameList.Length)
                    return false;

                ResetField();

                for (int i = 0; i < typeList.Length; ++i)
                {
                    if (!mFieldIndex.SetField(nameList[i], typeList[i], i))
                    {
                        Log("ERROR: load field type and name fail > " + nameList[i] + " > Type>" + typeList[i] + "at table >" + mTableIndex);
                        return false;
                    }
                }

                String[] dataList = new String[typeList.Length];
                while ((s = reader.ReadLine()) != null)
                {
                    //string[] dataList = s.Split(pa);
                    if (!_SplitCsvLine(s, ref dataList)) //dataList.Length != typeList.Length)
                    {
                        Log("WARN: data record col count is not same with type count at table >" + mTableIndex + "\r\n" + s);
                        return true;
                    }
                    string key = dataList[GetIndexCol()];
                    if (key != "")
                    {
                        DataRecord newRe = CreateRecord(key);
                        for (int i = 0; i < dataList.Length; ++i)
                        {
                            if (i != GetIndexCol())
                            {
                                if (!newRe.set(i, dataList[i]))
                                {
                                    Log(" record set data fail");
                                    return false;
                                }
                            }
                        }
                    }
                }
                reader.Close();
            }
            catch (Exception e)
            {
                Log(e.ToString());
                return false;
            }

            return true;
        }

        public bool _SplitCsvLine(String lineStr, ref String[] destList)
        {
            int start = 0;

            for (int i = 0; i < destList.Length; ++i)
            {
                if (start >= lineStr.Length)
                {
                    if (i == destList.Length - 1)
                    {
                        destList[i] = "";
                        break;
                    }
                    return false;
                }
                // 找"
                if (lineStr[start] == '"')
                {
                    int pos2 = lineStr.IndexOf('"', start + 1);
                    if (pos2 < 0)
                        return false;
                    start += 1;
                    destList[i] = lineStr.Substring(start, pos2 - start);
                    if (i == destList.Length - 1)
                    {
                        break;
                    }
                    if (pos2 + 1 >= lineStr.Length || lineStr[pos2 + 1] != ',')
                        return false;

                    start = pos2 + 2;
                }
                else
                {
                    int pos = lineStr.IndexOf(',', start);
                    if (pos < 0)
                    {
                        if (i == destList.Length - 1)
                        {
                            destList[i] = lineStr.Substring(start);
                            break;
                        }
                        else
                            return false;
                    }
                    destList[i] = lineStr.Substring(start, pos - start);
                    start = pos + 1;
                }
            }
            return true;
        }

        public bool SaveBinary(string pathFile)
        {
            DataBuffer data = new DataBuffer();
            if (serialize(ref data) > 0)
            {
                if (File.Exists(pathFile))
                    File.Delete(pathFile);

                FileStream fs = new FileStream(pathFile, FileMode.CreateNew);
                fs.Write(data.getData(), 0, data.tell());

                fs.Close();
                return true;
            }
            return false;
        }

        public bool LoadBinary(string pathFile)
        {
            if (!File.Exists(pathFile))
            {
                LOG.logError("Table file no exist>" + pathFile);
                return false;
            }

            FileStream fs = new FileStream(pathFile, FileMode.Open, FileAccess.Read);

            DataBuffer data = new DataBuffer((int)fs.Length);
            bool re = fs.Read(data.getData(), 0, (int)fs.Length) == fs.Length;
            fs.Close();

            if (re)
            {
                return restore(ref data);
            }

            return false;
        }

        enum TABLE_SAVE_TYPE
        {
            eSaveAll = 0,
            eOnlySaveRecordData = 1,
            eOnlyUpdateRecordData = 2,
        };

        public int serialize(ref DataBuffer data)
        {
            if (!mFieldIndex.Ready())
            {
                return 0;
            }
            int beforeSize = data.tell();

            byte dataType = (byte)TABLE_SAVE_TYPE.eSaveAll;
            data.write(dataType);
            // write table name info
            if (!data.writeOne(mTableIndex))
                return 0;
            // field info index
            if (!mFieldIndex.Save(ref data))
            {
                Log("ERROR: save field data fail>" + mTableIndex);
                return 0;
            }
            // record 
            // record num
            uint recordCount = (uint)mRecordMap.Count;
            data.write(recordCount);
            // record data
            tFieldInfo[] fieldList = mFieldIndex.GetFieldList();
            foreach (var rv in mRecordMap)
            {
                DataRecord r = rv.Value;
                if (r.GetDataList().Length == fieldList.Length)
                {
                    for (int i = 0; i < fieldList.Length; ++i)
                    {
                        try
                        {
                            if (!fieldList[i].serialize(r.GetDataList(), ref data))
                            {
                                Log("Error : serialize record fail >>> " + rv.Key.ToString());
                                return 0;
                            }
                        }
                        catch (Exception e)
                        {
                            Log("Error : serialize record fail >>> " + e.ToString());
                            return 0;
                        }
                    }
                }
                else
                    Log("Error : serialize table have a record data count is not same of field count>>>" + rv.Key.ToString());
            }
            return data.tell() - beforeSize;
        }

        class SortRecord : IComparable<SortRecord>
        {
            public string key { get; set; }
            public DataRecord record { get; set; }

            public int CompareTo(SortRecord other)
            {
                return key.CompareTo(other.key);
            }
        }

        public bool SortSaveData(ref DataBuffer resultData, bool bSaveTableIndex = false)
        {
            byte dataType = (byte)TABLE_SAVE_TYPE.eSaveAll;
            resultData.write(dataType);

            if (bSaveTableIndex)
                resultData.writeString(mTableIndex);
            else
                resultData.writeString("");

            if (!GetField().Save(ref resultData))
            {
                Log("序列字段信息失败");
                return false;
            }

            int count = 0;
            List<SortRecord> recordList = new List<SortRecord>();
            foreach (DataRecord re in GetAllRecord().Values)
            {
                if (re == null)
                    continue;

                ++count;
                SortRecord r = new SortRecord();
                r.key = re[0];
                r.record = re;
                recordList.Add(r);
            }

            resultData.write((int)count);
            recordList.Sort();

            tFieldInfo[] fieldList = mFieldIndex.GetFieldList();
            foreach (SortRecord re in recordList)
            {
                DataRecord r = re.record;
                if (r.GetDataList().Length == fieldList.Length)
                {
                    for (int i = 0; i < fieldList.Length; ++i)
                    {
                        try
                        {
                            if (!fieldList[i].serialize(r.GetDataList(), ref resultData))
                            {
                                Log("Error : serialize record fail >>> " + re.key);
                                return false;
                            }
                        }
                        catch (Exception e)
                        {
                            Log("Error : serialize record fail >>> " + e.ToString());
                            return false;
                        }
                    }
                }
                else
                    Log("Error : serialize table have a record data count is not same of field count>>>" + re.key);
            }

            return true;
        }

        void Log(string strInfo)
        {
            LOG.log(strInfo);
        }

        public bool restore(ref DataBuffer data)
        {
            if (data == null)
            {
                EventCenter.Log(LOG_LEVEL.ERROR, "restore table data is null");
                return false;
            }
            byte dataType = (byte)TABLE_SAVE_TYPE.eSaveAll;
            if (!data.read(out dataType))
                return false;

            if (dataType == (byte)TABLE_SAVE_TYPE.eSaveAll)
            {
                // read table index;
                if (!data.readOne(out mTableIndex))
                    return false;

                ResetField();
                // read field
                mFieldIndex.Reset(this);
                if (!mFieldIndex.Restore(ref data))
                {
                    Log("ERROR: Field restore fail>" + mTableIndex);
                    return false;
                }
            }
            else if (dataType == (byte)TABLE_SAVE_TYPE.eOnlyUpdateRecordData)
            {
                LOG.log("ERROR: Table data type can not TABLE_SAVE_TYPE.eOnlyUpdateRecordData");
                return false;
            }

            if (mFieldIndex.Count() <= 0)
                return false;

            // read record
            uint recordCount;
            if (!data.read(out recordCount))
                return false;

            for (int i = 0; i < recordCount; ++i)
            {
                DataRecord re = NewRecord();
                if (!re.restore(ref data))
                    return false;
                string key = re.getObject(mIndexCol).ToString();
                mRecordMap[key] = re;
            }
            return true;
        }
    }

}
