using System.Collections;
using System.Collections.Generic;
using System.IO;

//using ICSharpCode.SharpZipLib.GZip;


public class ZipTool
{
    public static byte[] CompressGZip(byte[] rawData)
    {
        return null;
        // MemoryStream ms = new MemoryStream();
        // GZipOutputStream compressedzipStream = new GZipOutputStream(ms);
        // compressedzipStream.Write(rawData, 0, rawData.Length);
        // compressedzipStream.Close();
        // return ms.ToArray();
    }

    public static byte[] UnGZip(byte[] byteArray, string info = "")
    {
        try
        {
            // using (GZipInputStream gzi = new GZipInputStream(new MemoryStream(byteArray)))
            // {
            //     MemoryStream re = new MemoryStream(50000);
            //     int count;
            //     byte[] data = new byte[50000];
            //     while ((count = gzi.Read(data, 0, data.Length)) != 0)
            //     {
            //         re.Write(data, 0, count);
            //     }
            //     re.SetLength(re.Position);
            //     byte[] overarr = re.ToArray();
            //     return overarr;
            // }
        }
        catch(System.Exception e)
        {
            LOG.logError("Unzip data file : " + info + " , Error : " +e.ToString());
        }
        return null;
    }

}
