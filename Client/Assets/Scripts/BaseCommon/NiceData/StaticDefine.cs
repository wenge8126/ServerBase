using System;
using System.Text;
using Logic;


//--------------------------------------------------------------------------------
// 基本字段类型
// 此类型定义值必须要与C++代码中的进行对应
//--------------------------------------------------------------------------------
public enum FIELD_TYPE
{
    FIELD_NULL = 0,
    FIELD_INT,
    FIELD_FLOAT,
    FIELD_DOUBLE,
    FIELD_STRING,
    FIELD_BOOL,
    FIELD_BYTE,
    FIELD_SHORT,
    FIELD_INT64,
    FIELD_INT_RELATION,
    FIELD_STRING_RELATION,
    FIELD_DATA,
    FIELD_TABLE,
    FIELD_RECORD,
    FIELD_NICEDATA,
    FIELD_EVENT,
    FIELD_SKIP_1,
    FIELD_SKIP_2,
    FIELD_UINT64,
    FIELD_CHAR_STRING,
    FIELD_NET_CONNECT,
//	FIELD_BINARY,
    FIELD_DB_TABLE,
    FIELD_ARRAYDATA,
    FIELD_DBNICE,					// DB或表格中使用的具有共用名称索引的数据, 特点是只保存名称索引ID与数据
    FIELD_CLOUD_NICE,			// 保存NiceData > UInt64 key 索引
    FIELD_SQLDATA,					// 即时保存或读取SQL数据, 只用于DB记录字段
    FIELD_TYPE_MAX
};


public struct StaticDefine
{
    static public bool debug = false;            // use debug mode
    /*static public int IntType = 0;
    static public float FloatType = 0.0f;
    static public bool BoolType = false;
    static public string StringType = "";
    static public NiceTable mTable = new NiceTable();
    static public NiceData mNiceData = new ExNiceData();
	 */
    static public string MakeMD5(string scrString)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] bytValue, bytHash;
        bytValue = StaticDefine.GetBytes(scrString);
        bytHash = md5.ComputeHash(bytValue);
        md5.Clear();

        string result = "";
        for (int i = 0; i < bytHash.Length; i++)
        {
            result += bytHash[i].ToString("X").PadLeft(2, '0');
        }
        return result.ToLower();
    }

    static public string MakeMD5(byte[] scrData, int size)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] bytHash;

        bytHash = md5.ComputeHash(scrData, 0, size);
        md5.Clear();

        string result = "";
        for (int i = 0; i < bytHash.Length; i++)
        {
            result += bytHash[i].ToString("X").PadLeft(2, '0');
        }
        return result.ToLower();
    }
    //--------------------------------------------------------------------------------
    static public Type ToObjectType(FIELD_TYPE eType)
    {
        switch (eType)
        {
            case FIELD_TYPE.FIELD_INT:
                return typeof(int);
            case FIELD_TYPE.FIELD_SHORT:
                return typeof(short);
            case FIELD_TYPE.FIELD_FLOAT:
                return typeof(float);
            case FIELD_TYPE.FIELD_STRING:
                return typeof(float);
            case FIELD_TYPE.FIELD_BOOL:
                return typeof(bool);
            case FIELD_TYPE.FIELD_BYTE:
                return typeof(byte);
            case FIELD_TYPE.FIELD_TABLE:
                return typeof(NiceTable);
            case FIELD_TYPE.FIELD_NICEDATA:
                return typeof(NiceData);
            case FIELD_TYPE.FIELD_DATA:
                return typeof(DataBuffer);

            case FIELD_TYPE.FIELD_INT64:
                return typeof(Int64);
            case FIELD_TYPE.FIELD_UINT64:
                return typeof(UInt64);

            case FIELD_TYPE.FIELD_DOUBLE:
                return typeof(double);
        }
        Type t = null;
        return t;
    }
    //--------------------------------------------------------------------------------

    static public string ToStringType(FIELD_TYPE eType)
    {
        switch (eType)
        {
            case FIELD_TYPE.FIELD_INT:
                return "INT";
            case FIELD_TYPE.FIELD_SHORT:
                return "SHORT";
            case FIELD_TYPE.FIELD_FLOAT:
                return "FLOAT";
            case FIELD_TYPE.FIELD_STRING:
                return "STRING";
            case FIELD_TYPE.FIELD_BOOL:
                return "BOOL";
            case FIELD_TYPE.FIELD_BYTE:
                return "BYTE";
            case FIELD_TYPE.FIELD_UINT64:
                return "UINT64";
            
            case FIELD_TYPE.FIELD_INT64:
                return "INT64";

            case FIELD_TYPE.FIELD_NICEDATA:
                return "NICEDATA";

            case FIELD_TYPE.FIELD_DATA:
                return "DATA";

            case FIELD_TYPE.FIELD_TABLE:
                return "TABLE";

            case FIELD_TYPE.FIELD_DOUBLE:
                return "DOUBLE";

            case FIELD_TYPE.FIELD_RECORD:
                return "RECORD";
        }
        return "";
    }
    //--------------------------------------------------------------------------------

    static public FIELD_TYPE ToType(string strType)
    {
        switch (strType)
        {
            case "INT":
                return FIELD_TYPE.FIELD_INT;

            case "SHORT":
                return FIELD_TYPE.FIELD_SHORT;

            case "FLOAT":
                return FIELD_TYPE.FIELD_FLOAT;

            case "STRING":
                return FIELD_TYPE.FIELD_STRING;

            case "BOOL":
                return FIELD_TYPE.FIELD_BOOL;
            case "BYTE":
                return FIELD_TYPE.FIELD_BYTE;
            case "INT64":
                return FIELD_TYPE.FIELD_INT64;
            case "UINT64":
                return FIELD_TYPE.FIELD_UINT64;

            case "DATA":
                return FIELD_TYPE.FIELD_DATA;

            case "TABLE":
                return FIELD_TYPE.FIELD_TABLE;

        }
        return FIELD_TYPE.FIELD_NULL;
    }
    //--------------------------------------------------------------------------------
    static public int GetType(object obj)
    {
        if (obj == null)
            return (int)FIELD_TYPE.FIELD_NULL;

        if (obj.GetType() == typeof(int))
            return (int)FIELD_TYPE.FIELD_INT;
        else if (obj.GetType() == typeof(short))
            return (int)FIELD_TYPE.FIELD_SHORT;
        else if (obj.GetType() == typeof(float))
            return (int)FIELD_TYPE.FIELD_FLOAT;
        else if (obj.GetType() == typeof(double))
            return (int)FIELD_TYPE.FIELD_DOUBLE;
        else if (obj.GetType() == typeof(string))
            return (int)FIELD_TYPE.FIELD_STRING;
        else if (obj.GetType() == typeof(bool))
            return (int)FIELD_TYPE.FIELD_BOOL;
        else if (obj.GetType() == typeof(byte))
            return (int)FIELD_TYPE.FIELD_BYTE;
        else if (obj is NiceTable)
            return (int)FIELD_TYPE.FIELD_TABLE;
        else if (obj is tNiceData)
            return (int)FIELD_TYPE.FIELD_NICEDATA;
        else if (obj is DataBuffer)
            return (int)FIELD_TYPE.FIELD_DATA;
        else if (obj.GetType() == typeof(UInt64))
            return (int)FIELD_TYPE.FIELD_UINT64;
        else if (obj.GetType() == typeof(Int64))
            return (int) FIELD_TYPE.FIELD_INT64;

        return (int)FIELD_TYPE.FIELD_NULL;
    }
    //--------------------------------------------------------------------------------
    static void Add(ref uint x, uint y, ref uint flag)
    {
        UInt64 t64 = (UInt64)x + y;
        if ((t64 >> 32) > 0)
            flag = 1;
        else
            flag = 0;
        x = (uint)t64;
    }

    static void Adc(ref uint x, uint y, ref uint flag)
    {
        UInt64 t64 = (UInt64)x + y + flag;
        if ((t64 >> 32) > 0)
            flag = 1;
        else
            flag = 0;
        x = (uint)t64;
    }

    static void Mul(ref uint one, ref uint two, ref uint flag)
    {
        UInt64 t64 = (UInt64)one * two;
        two = (uint)(t64 >> 32);
        one = (uint)t64;
        if (two == 0)
            flag = 0;
        else
            flag = 1;
    }

    static void Mov(ref uint x, uint val)
    {
        x = val;
    }

    static void Xor(ref uint x, uint val)
    {
        x = x ^ val;
    }

    static void Rol(ref uint v, ref uint flag)
    {
        flag = v >> (32 - 1);
        v = v << 1;
        v += flag;
        //flag = v >> 31;
        //flag = flag != 0 ? 1 : 0;
    }
    // set use code page when get id from string .
    static void SetCodePage(int codepage)
    {
        Encoding en = null;
        try
        {
            en = Encoding.GetEncoding(codepage);
        }
        catch
        {
            if (null == en)
            {
                throw new Exception("not find string use code page.");
            }
        }
        mUseCodePage = en;
        mStringCodePage = codepage;
    }

    // get data from string at seting code page.
    static public bool GetBytes(string strVal, ref byte[] destData, int destPos)
    {
        if (strVal == null)
            return false;
        byte[] d = Encoding.Default.GetBytes(strVal);
        byte[] re = Encoding.Convert(Encoding.Default, mUseCodePage, d);
        if (re.Length > destData.Length - destPos)
            return false;

        re.CopyTo(destData, destPos);
        //mUseCodePage.GetBytes(strVal, 0, strVal.Length, destData, 0);
        return true;
    }

    static public byte[] GetBytes(string strVal)
    {
        if (null == strVal)
            return null;
        return mUseCodePage.GetBytes(strVal);

        //byte[] d = Encoding.Default.GetBytes(strVal);
        //byte[] re = Encoding.Convert(Encoding.Default, mUseCodePage, d);		
        //return re;
    }

    static public string GetString(byte[] scrData, int indexPos, int length)
    {
        return mUseCodePage.GetString(scrData, indexPos, length);

        //if (scrData.Length<=0)
        //    return "";
        //byte[] d = Encoding.Convert(mUseCodePage, Encoding.Default, scrData, indexPos, length);
        //return Encoding.Default.GetString(d);

    }

    static int mStringCodePage = 936;
    static Encoding mUseCodePage = Encoding.UTF8; //GetEncoding(mStringCodePage);
    static uint[] m = new uint[70 * 4];

    static public int StringID(string strVal)
    {
        int id = StringID(GetBytes(strVal));
        string info = " make id = ";
        info += id.ToString();
        //LOG.log(info);*/
        return id;
    }

    static public int StringID(byte[] strUTF8Data)
    {
        if (strUTF8Data == null || strUTF8Data.Length <= 0 || strUTF8Data.Length > 260 * 4)
            return 0;

        int mDebug = 0;

        int i = 0;
        int c = 0;
        while (i + 4 <= strUTF8Data.Length)
        {
            m[c] = BitConverter.ToUInt32(strUTF8Data, i);
            i += 4;
            ++c;

            ++mDebug;
        }
        if (i < strUTF8Data.Length)
        {
            byte[] temp = new byte[4];
            // ut8.CopyTo(temp, i, ut8.Length-i);
            int x = 0;
            for (x = 0; x < 4; ++x)
            {
                if (i < strUTF8Data.Length)
                    temp[x] = strUTF8Data[i];
                else
                    temp[x] = 0;
                ++i;

                ++mDebug;
            }
            m[c] = BitConverter.ToUInt32(temp, 0);
            ++c;
        }
        i = c;
        // must use strncpy because strncpy do full zero for last char , but strncpy_s not done. by Ywg 20110612
        //strncpy( (char *)m, str, i * 4 );  

        uint v = 0;
        //for (i=0;i<TEMP_BUFFER_SIZE && m[i];i++) ;
        m[i++] = 0x9BE74448;
        m[i++] = 0x66F42C48;
        v = 0xF4FA8928;
        //---------------------------------------------------------

        uint eax = 0;
        uint ebx = 0;

        uint edx = 0;
        uint esi = 0;
        uint edi = 0;

        uint j = 0;

        uint f = 0;

        {
            Mov(ref esi, 0x37A8470E);
            Mov(ref edi, 0x7758B42B);

            j = 0;

            _loop:
            Mov(ref ebx, 0x267B0B11);
            Rol(ref v, ref f);
            Xor(ref ebx, v);

            Mov(ref eax, m[j]);
            Mov(ref edx, ebx);

            //1
            Xor(ref esi, eax);
            Xor(ref edi, eax);

            //2
            Add(ref edx, edi, ref f);
            edx = edx | 0x2040801;
            edx = edx & 0xBFEF7FDF;
            Mov(ref eax, esi);
            Mul(ref eax, ref edx, ref f);

            //3
            Adc(ref eax, edx, ref f);
            //4
            Mov(ref edx, ebx);
            Adc(ref eax, 0, ref f);

            //5
            Add(ref edx, esi, ref f);

            edx = edx | 0x804021;
            edx = edx & 0x7DFEFBFF;
            //6
            Mov(ref esi, eax);
            Mov(ref eax, edi);
            Mul(ref eax, ref edx, ref f);

            //7
            Add(ref edx, edx, ref f);
            Adc(ref eax, edx, ref f);
            if (f == 0)
                goto _skip;
            //8
            Add(ref eax, 2, ref f);

            ++mDebug;

            _skip:
            j++;
            Mov(ref edi, eax);
            if (j < i)
                goto _loop;

            //9
            Xor(ref esi, edi);
            Mov(ref v, esi);
        }

        return (int)v;

    }

}
