using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

using Logic;



public class DataBuffer
{
    public byte[] mData = null;
    protected int mCurrent = 0;

    public DataBuffer()
    {
        mData = new byte[8];
        mCurrent = 0;
    }

    public DataBuffer(int initSize)
    {
        if (initSize <= 0)
            initSize = 64;
        if (initSize > 160 * 1024 * 1024)
        {
            LOG.logError("DataBuffer init data more then 160MB, return fail");
        }
        else
        {
            mData = new byte[initSize];
        }
    }

    public DataBuffer(byte[] data)
    {
        mData = data;
        mCurrent = 0;
    }

    public override string ToString()
    {
        if (mData == null)
            return "Size 0";
        return "Size " + mData.Length.ToString();
    }

    public virtual bool saveTable(NiceTable table)
    {
        DataBuffer d = this;
        return table.serialize(ref d) > 0;
    }
    public virtual bool saveRecord(DataRecord record)
    {
        FieldIndex f = record.GetFieldIndex();
        DataBuffer d = this;
        if (f != null)
            for (int i = 0; i < f.Count(); ++i)
            {
                tFieldInfo info = f.GetField(i);
                if (!info.serialize(record.GetDataList(), ref d))
                {
                    LOG.logError("记录数据序列化失败");
                    return false;
                }
            }
        return true;
    }

    public virtual NiceTable restoreTable(FieldIndex field)
    {
        NiceTable t = new NiceTable();
        DataBuffer d = this;
        if (t.restore(ref d))
            return t;
        return null;
    }

    public virtual DataRecord restoreRecord(NiceTable table)
    {
        DataRecord re = table.NewRecord();
        DataBuffer d = this;
        if (re.restore(ref d))
        {
            table.AppendRecord(re, true);
            return re;
        }
        return null;
    }

    public int size() { return mData.Length; }

    public void clear(bool bFreeBuffer)
    {
        if (bFreeBuffer)
            mData = null;
        mCurrent = 0;
        _setZero((uint)0);
    }

    public void _setZero(uint startPos)
    {
        for (uint i = startPos; i < (uint)mData.Length; ++i)
        {
            mData[i] = 0;
        }
    }

    public void _setData(byte[] data)
    {
        mData = data;
        mCurrent = 0;
    }

    public void _resize(int size)
    {
        if (mData.Length == size)
            return;
        int x = mData.Length;
        if (size < x)
            x = size;
        byte[] temp = new byte[size];
        //??? !!! 可以不需要初始,为了效率
        //for (int i = 0; i < size; ++i)
        //{
        //    temp[i] = 0;
        //}
        Array.Copy(mData, temp, x);
        mData = temp;
        if (mCurrent > size)
            mCurrent = size;
    }

    public void resizeTwoLength(int size)
    {
        int s = mData.Length;
        if (s <= 0)
            s = 1;
        do
        {
            s *= 2;
        } while (s < size);

        _resize(s);
    }

    public byte[] getData() { return mData; }

    public int tell() { return mCurrent; }

    public bool seek(int size)
    {
        if (size <= mData.Length)
        {
            mCurrent = size;
            return true;
        }
        return false;
    }

    public bool end() { return mCurrent >= mData.Length; }


    public int write(byte[] scr, int len)
    {
        int reLen = len;
        if (reLen > scr.Length)
            reLen = scr.Length;
        if (mCurrent + reLen > mData.Length)
            resizeTwoLength(mCurrent + reLen);

        //for (int i = 0; i < reLen; ++i)
        //{
        //    mData[mCurrent + i] = scr[i];
        //}
        Array.Copy(scr, 0, mData, mCurrent, reLen);

        mCurrent += reLen;
        return reLen;
    }

    public int write(byte[] scr, int scrIndex, int len)
    {
        if (scrIndex < 0
            || len <= 0
            || scrIndex + len > scr.Length)
            return 0;

        int reLen = len;

        if (mCurrent + reLen > mData.Length)
            resizeTwoLength(mCurrent + reLen);

        Array.Copy(scr, scrIndex, mData, mCurrent, reLen);

        mCurrent += reLen;

        return reLen;
    }

    public int write(byte[] scr)
    {
        if (mCurrent + scr.Length > mData.Length)
            resizeTwoLength(mCurrent + scr.Length);
        //for (int i = 0; i < scr.Length; ++i)
        //{
        //    mData[mCurrent + i] = scr[i];
        //}
        scr.CopyTo(mData, mCurrent);
        mCurrent += scr.Length;
        return scr.Length;
    }

    public void write(byte val)
    {
        if (mCurrent + 1 > mData.Length)
            resizeTwoLength(mCurrent + 1);
        mData[mCurrent++] = val;
    }

    public void write(bool bVal)
    {
        write(bVal ? (byte)1 : (byte)0);
    }

    public void write(int val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }
    public void write(uint val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }
    public void write(short val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }
    public void write(float val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }
    public void write(double val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }
    public void write(ushort val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }

    public void write(UInt64 val)
    {
        byte[] temp = BitConverter.GetBytes(val);
        write(temp, temp.Length);
    }

    virtual public bool writeOne(string strVal)
    {
#if SAVE_STRING_LENGTH
        if (strVal == null)
        {
            write((byte)0);
            return true;
        }
        byte[] str = StaticDefine.GetBytes(strVal);
        if (str.Length > 255)
            return false;
       
        byte d = (byte)(str.Length & 0xff);
        write(d);
        write(str);

        return true;
#else
        return writeString(strVal);
#endif
    }

    virtual public bool writeString(string strVal)
    {
        if (strVal == null)
        {
            write((ushort)0);
            return true;
        }
        byte[] str = StaticDefine.GetBytes(strVal);
        if (str.Length > 65535)
        {
            LOG.logError("writeString >65535 >" + strVal);
            return false;
        }
        ushort len = (ushort)(str.Length & 0xFFFF);
        //byte[] d = BitConverter.GetBytes(len);
        write(len);

        write(str);

        return true;
    }

    public byte[] read(uint size)
    {
        if (mCurrent + size <= mData.Length)
        {
            byte[] temp = new byte[size];
            for (int i = 0; i < size; ++i)
            {
                temp[i] = mData[mCurrent + i];
            }
            mCurrent += (int)size;
            return temp;
        }
        return null;
    }

    public bool read(out short dest)
    {
        if (mCurrent + 2 <= mData.Length)
        {
            dest = BitConverter.ToInt16(mData, mCurrent);
            mCurrent += 2;
            return true;
        }
        dest = 0;
        return false;
    }

    public bool read(out byte dest)
    {
        if (mCurrent + 1 <= mData.Length)
        {
            dest = mData[mCurrent++];
            return true;
        }
        dest = 0;
        return false;
    }

    public bool read(out bool dest)
    {
        byte val;
        if (read(out val))
        {
            dest = val != 0;
            return true;
        }
        dest = false;
        return false;
    }

    public bool read(out int dest)
    {
        if (mCurrent + 4 <= mData.Length)
        {
            dest = BitConverter.ToInt32(mData, mCurrent);
            mCurrent += 4;
            return true;
        }
        dest = 0;
        return false;
    }
    public bool read(out uint dest)
    {
        if (mCurrent + sizeof(int) <= mData.Length)
        {
            dest = BitConverter.ToUInt32(mData, mCurrent);
            mCurrent += sizeof(uint);
            return true;
        }
        dest = 0;
        return false;
    }
    public bool read(out ushort dest)
    {
        if (mCurrent + sizeof(ushort) <= mData.Length)
        {
            dest = BitConverter.ToUInt16(mData, mCurrent);
            mCurrent += sizeof(ushort);
            return true;
        }
        dest = 0;
        return false;
    }
    public bool read(out float dest)
    {
        if (mCurrent + 4 <= mData.Length)
        {
            dest = BitConverter.ToSingle(mData, mCurrent);
            mCurrent += 4;
            return true;
        }
        dest = 0;
        return false;
    }

    public bool read(out double dest)
    {
        if (mCurrent + 8 <= mData.Length)
        {
            dest = BitConverter.ToDouble(mData, mCurrent);
            mCurrent += 8;
            return true;
        }
        dest = 0;
        return false;
    }

    public bool read(out UInt64 dest)
    {
        if (mCurrent + sizeof(UInt64) <= mData.Length)
        {
            dest = BitConverter.ToUInt64(mData, mCurrent);
            mCurrent += sizeof(UInt64);
            return true;
        }
        dest = 0;
        return false;
    }

    public bool read(out string dest)
    {
        return readString(out dest);
    }

    virtual public bool readOne(out string dest)
    {
#if SAVE_STRING_LENGTH
        byte b;
        if (!read(out b))
        {
            dest = "";
            return false;
        }

        if (b == 0)
        {
            dest = "";
            return true;
        }
            
        if (mCurrent + b <= mData.Length)
        {
            dest = StaticDefine.GetString(mData, mCurrent, (int)b);
            mCurrent += b;
            return true;
        }
        
        dest = "";
        return false;
#else
        return readString(out dest);
#endif
    }

    virtual public bool readString(out string dest)
    {
        ushort b;
        if (!read(out b))
        {
            dest = "";
            return false;
        }

        if (b == 0)
        {
            dest = "";
            return true;
        }

        if (mCurrent + b <= mData.Length)
        {
            dest = StaticDefine.GetString(mData, mCurrent, (int)b);
            mCurrent += b;
            return true;
        }
        dest = "";
        return false;
    }


    public bool read(ref DataBuffer destBuffer, int readSize, int destBeginPos)
    {
        if (mData == null || readSize > size() - tell())
            return false;

        if (destBuffer.size() - destBeginPos < readSize)
            destBuffer._resize((int)(destBeginPos + readSize));
        try
        {
            Array.Copy(mData, tell(), destBuffer.mData, destBeginPos, (int)readSize);
            mCurrent += readSize;
            return true;
        }
        catch (Exception e)
        {
            LOG.log(e.ToString());
            return false;
        }
    }

    public bool read(ref byte[] destBuffer, int readSize)
    {
        if (mData == null || readSize > size() - tell() || destBuffer.Length < readSize)
            return false;

        Array.Copy(mData, tell(), destBuffer, 0, (int)readSize);
        mCurrent += readSize;
        return true;
    }

    // 如果目标Buffer太小，会修改为足够大
    public int readData(ref DataBuffer destBuffer)
    {
        int len = 0;
        if (read(out len))
        {
            if (len > 0)
            {
                if (len > destBuffer.size())
                    destBuffer._resize(len);

                if (!read(ref destBuffer.mData, len))
                    return 0;
            }
            return len;
        }
        return 0;
    }

    public bool writeData(ref DataBuffer scrData, int size = 0)
    {
        if (size <= 0 || size > scrData.size())
            size = scrData.size();

        write(size);
        if (size > 0)
            return write(scrData.mData, size) == size;

        return true;
    }

    // write string to debug string
    public bool _writeString(string strValue)
    {
        byte[] strData = StaticDefine.GetBytes(strValue);
        return write(strData) == strData.Length;
    }

    public bool writeStringArray(ref string[] scrArray)
    {
        write((int)scrArray.Length);
        for (int i = 0; i < scrArray.Length; ++i)
        {
            writeString(scrArray[i]);
        }
        return true;
    }

    public bool readStringArray(out string[] resultArray)
    {
        int count = 0;
        if (read(out count))
        {
            resultArray = new string[count];
            for (int i = 0; i < count; ++i)
            {
                if (!readString(out resultArray[i]))
                    return false;
            }
            return true;
        }
        resultArray = null;
        return false;
    }

    public bool writeIntArray(ref int[] scrArray)
    {
        write((int)scrArray.Length);
        for (int i = 0; i < scrArray.Length; ++i)
        {
            write(scrArray[i]);
        }
        return true;
    }

    public bool readIntArray(out int[] resultArray)
    {
        int count = 0;
        if (read(out count))
        {
            resultArray = new int[count];
            for (int i = 0; i < count; ++i)
            {
                if (!read(out resultArray[i]))
                    return false;
            }
            return true;
        }
        resultArray = null;
        return false;
    }

    public bool writeUintArray(ref uint[] scrArray)
    {
        write((int)scrArray.Length);
        for (int i = 0; i < scrArray.Length; ++i)
        {
            write(scrArray[i]);
        }
        return true;
    }

    public bool readUintArray(out uint[] resultArray)
    {
        int count = 0;
        if (read(out count))
        {
            resultArray = new uint[count];
            for (int i = 0; i < count; ++i)
            {
                if (!read(out resultArray[i]))
                    return false;
            }
            return true;
        }
        resultArray = null;
        return false;
    }
}
