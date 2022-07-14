using System.Collections;
using System.IO;
using System.Collections.Generic;
using System;
using Logic;

//-------------------------------------------------------------------------
// 用于制作生成资源包
// NOTE: 目前客户端使用的是简单表格索引方式资源包， 对应服务器的 EasyResourcesPack
//-------------------------------------------------------------------------
public class ResourcesPack
{
    string mPackFileName;
    Stream mPackDataStream;
    public NiceTable mResourceIndex;

    bool mbZip = false;
    bool mbEnrypt = false;

    public delegate void ExportProcess(Int64 alreadySize, Int64 totalSize); 
    
	public ResourcesPack(){}

    ~ResourcesPack()
    {
        close();
    }

    public bool load()
    {
        if (mPackDataStream == null)
            return false;
        try
        {
            if (mPackDataStream.Seek(0, SeekOrigin.Begin) < 0)
            {
                close();
                return false;
            }
            byte[] val = new byte[sizeof(int)];
            if (mPackDataStream.Read(val, 0, sizeof(int)) != sizeof(int))
            {
                close();
                return false;
            }

            int indexPos = BitConverter.ToInt32(val, 0);

            if (mPackDataStream.Read(val, 0, sizeof(int)) != sizeof(int))
            {
                close();
                return false;
            }

            int size = BitConverter.ToInt32(val, 0)-indexPos;
            if (size<=0)
            {
                close();
                return false;
            }

            DataBuffer d = new DataBuffer(size);

			if (mPackDataStream.Seek((long)indexPos, SeekOrigin.Begin)<0)
            {
                close();
                return false;
            }
            if (mPackDataStream.Read(d.mData, 0, size)!=size)
            {
                close();
                return false;
            }

            mResourceIndex = new NiceTable();
            if (!mResourceIndex.restore(ref d))
            {
                close();
                return false;
            }

            mPackDataStream.Seek(sizeof(int)*2, SeekOrigin.Begin);
            mbZip = mPackDataStream.ReadByte()!=0;
            mbEnrypt = mPackDataStream.ReadByte()!=0;

#if UNITY_EDITOR
            //mResourceIndex.SaveTable("d:/test_read_pack.csv");
#endif
           return true;

        }
        catch
        {
            LOG.logError("Init Read from pack error");            
            close();              
            return false;
        }
    }

    public void close()
    {
        if (mPackDataStream != null)
            mPackDataStream.Close();
        mPackDataStream = null;
    }
    public virtual bool load(string packetName, DataBuffer packData)
    {
        close();
        mPackFileName = packetName;
        mPackDataStream = new MemoryStream(packData.getData());

        return load();
    }

    public virtual bool load(string packetName, byte[] packData)
    {
        close();
        mPackFileName   = packetName;
        mPackDataStream = new MemoryStream(packData);

        return load();
    }
    
	public virtual bool load( string packFileName )
    {
        mPackFileName = packFileName;
        try
        {
            mPackDataStream = File.OpenRead(mPackFileName); // new FileStream(mPackFileName, FileMode.Open, FileAccess.Read, FileShare.Read);
        }
        catch
        {
            LOG.logError("Resources pack is not exist > " + mPackFileName);
            return false;
        }
		return load();
	}
    
    public virtual bool load( string packFileName, Stream reader )
    {
        mPackFileName   = packFileName;
        mPackDataStream = reader;
        return load();
    }

    public DataBuffer loadResource(string pathFilename)
	{
		string szPathFilename = pathFilename.ToLower();
        szPathFilename.Replace('\\', '/');
        DataRecord r = mResourceIndex.GetRecord(szPathFilename);
        if (r != null)
        {
            int p = r["POS"];
            int size = r["SIZE"];

            if (p >= 0 && size > 0)
            {
				if (mPackDataStream.Seek((long)p, SeekOrigin.Begin)>=0)
				{
                	DataBuffer d = new DataBuffer(size);
                    if (mPackDataStream.Read(d.mData, 0, size) == size)
                    {
                        //??? if (mbEnrypt)
                        //    CommonParam.decrypt_common(d.mData, size, 0);                        
                        d.seek(size);
                        return d;
                    }
				}
            }
        }

		return null;
	}

    public int Export(string destPath)
    {
        int resultCount = 0;
        foreach (DataRecord vRe in mResourceIndex.GetAllRecord().Values)
        {
            string fieldIndex = (string)vRe.getObject(0);
            DataBuffer d = loadResource(fieldIndex);
            if (d != null)
            {
                FileStream f = new FileStream(destPath + fieldIndex, FileMode.Create);
                f.Write(d.mData, 0, d.size());
                f.Close();

                ++resultCount;
            }
            else
            {
                LOG.logWarn($"Resource load fail : {fieldIndex}");
            }
        }
        return resultCount;
    }
    
    public int loadResourceEx(string pathFilename, ref DataBuffer destBuffer)
    {
        string szPathFilename = pathFilename.ToLower();
        szPathFilename.Replace('\\', '/');
        DataRecord r = mResourceIndex.GetRecord(szPathFilename);
        if (r != null)
        {
            int p    = r["POS"];
            int size = r["SIZE"];

            if (p >= 0 && size > 0)
            {
                if (mPackDataStream.Seek((long)p, SeekOrigin.Begin)>=0)
                {
                    //DataBuffer d = new DataBuffer(size);
                    if (destBuffer.mData.Length<size)
                        destBuffer._resize(size);
                    if (mPackDataStream.Read(destBuffer.mData, 0, size) == size)
                    {
                        //??? if (mbEnrypt)
                        //    CommonParam.decrypt_common(d.mData, size, 0);                        
                        destBuffer.seek(size);
                        return size;
                    }
                }
            }
        }

        return 0;
    }

    public int ExportEx(string destPath, ref DataBuffer tempBuffer, /*ExportProcess processFun,*/ ref Int64 alreadySize, Int64 totalSize)
    {
        int resultCount = 0;
        foreach (DataRecord vRe in mResourceIndex.GetAllRecord().Values)
        {
            string     fieldIndex = (string)vRe.getObject(0);
            int size          = loadResourceEx(fieldIndex, ref tempBuffer);
            if (size>0)
            {
                FileStream f = new FileStream(destPath + fieldIndex, FileMode.Create);
                f.Write(tempBuffer.mData, 0, size);
                f.Close();
                alreadySize += size;
                //processFun(alreadySize, totalSize);
                ++resultCount;
            }
            else
            {
                LOG.logWarn($"Resource load fail : {fieldIndex}");
            }
        }
        return resultCount;
    }

	static bool _EncryptData(DataBuffer scrData, int length){ return true; }
	static bool _DecryptData(DataBuffer scrData, int length){ return true; }


}


//-------------------------------------------------------------------------
// 用于制作生成资源包
// NOTE: 生成的是使用表格方式索引的资源包, 目前客户端使用的资源包
//-------------------------------------------------------------------------

public class EasyGenerateResourcesPack
{
    class SortBuffer : IComparable<SortBuffer>
    {
        public string key { set; get; }
        public DataBuffer buffer { set; get; }

        public int CompareTo(SortBuffer other)
        {
            return key.CompareTo(other.key);
        }
    }

    List<SortBuffer> mTempFileList = new List<SortBuffer>();
	bool mbZip = false;
	bool mEncrypt = false;

	public EasyGenerateResourcesPack()
	{
        mbZip = false;
		mEncrypt = false;
	}

    public EasyGenerateResourcesPack(bool bZip, bool bEncrypt)
	{
        mbZip = bZip;
		mEncrypt = bEncrypt;
	}


	public void InitReset(bool bZip, bool bEncrypt)
	{
        // 目前不支持压缩和加密
        //mbZip = bZip;
        mEncrypt = bEncrypt;
		mTempFileList.Clear();
	}

    // NOTE: 文件数据当前游标位置在有效数据结尾,  resourceData .tell() 表示资源的大小 
	public void AppendResourceData(string pathFileName, DataBuffer resourceData)
	{
		string path = pathFileName;
		path = path.ToLower();
		path.Replace('\\', '/');
		DataBuffer d = _EncryptOrZipData(resourceData);

        //if (mTempFileList.ContainsKey(path))
        //{
        //    LOG.logWarn("Pack resource already exist >" + pathFileName);
        //    mTempFileList.Remove(path);
        //}
        SortBuffer buff = new SortBuffer();
        buff.key = path;
        buff.buffer = d;
		mTempFileList.Add(buff);
	}

    
    public virtual bool GeneratePack(string szDestPathFileName, out string errorInfo, out DataBuffer pResultPackData, bool bDebug = false)
    {
        pResultPackData = new DataBuffer(1024);
        DataBuffer pDestData = pResultPackData;

        pDestData.seek(sizeof(int)*2);
        pDestData.write(mbZip);
        pDestData.write(mEncrypt);

        NiceTable indexTable = new NiceTable();
        indexTable.SetField("INDEX", FIELD_TYPE.FIELD_STRING, 0);
        indexTable.SetField("POS", FIELD_TYPE.FIELD_INT, 1);
        indexTable.SetField("SIZE", FIELD_TYPE.FIELD_INT, 2);

        mTempFileList.Sort();
        //LOG.log("----------------------------------------------------------------------");
        foreach (var kV in mTempFileList)
        {
            DataBuffer resData = kV.buffer;
            DataRecord indexRe = indexTable.CreateRecord(kV.key);
            indexRe.set("POS", pDestData.tell());
            indexRe.set("SIZE", resData.tell());
            //LOG.LogFormat("{0} {1} size={2} MD5 {3}", kV.key, pDestData.tell(), resData.tell(), GameCommon.MakeMD5(resData.getData(), resData.tell()));
            pDestData.write(resData.getData(), resData.tell());
        }
        //LOG.log("----------------------------------------------------------------------");
        int indexPos = pDestData.tell();

        if (!indexTable.SortSaveData(ref pDestData))
        {
            LOG.logError("Save index table fail >" + szDestPathFileName);
            errorInfo = "Save index table fail >" + szDestPathFileName;
            return false;
        }

        int size = pDestData.tell();
        pDestData.seek(0);
        pDestData.write(indexPos);
        pDestData.write(size);
        pDestData.seek(size);

        if (bDebug)
            indexTable.SaveTable("Test_Resource.csv");

        if (File.Exists(szDestPathFileName))
            File.Delete(szDestPathFileName);

        FileStream fs = new FileStream(szDestPathFileName, FileMode.CreateNew);
        fs.Write(pDestData.getData(), 0, pDestData.tell());
        fs.Close();

        errorInfo = "";
        return true;

    }
	
	DataBuffer _EncryptOrZipData(DataBuffer scrData)
    {
        if (mEncrypt)
        {
            //??? GameCommon.encrypt_common(scrData.getData(), scrData.size(), 0, 0);
        }
        return scrData; 
    }

}
