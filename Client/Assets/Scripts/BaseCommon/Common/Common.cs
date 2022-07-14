
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;
using System.Threading.Tasks;



public class Common
{


    public static void OpenWindow(string windowName, GameObject parentRoot, object param=null)
    {
        tWindow win = UIManager.OpenWindow(windowName, param, parentRoot);
       
    }


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

    //static int mStringCodePage = 936;
    static Encoding mUseCodePage = Encoding.UTF8; //GetEncoding(mStringCodePage);

    static public void GetFullPath(GameObject go, ref string path)
    {
        if (go.transform.parent != null)
        {
            GetFullPath(go.transform.parent.gameObject, ref path);
            path += "/";
            path += go.name;
        }
        else
            path = go.name;
    }
}



