//Auto generate script for config table

using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;

public class Config
{
    static public void Init()
    {
        LuaList.Init();
    }
}


public class LuaList
{
    static public  NiceTable msTable;
    static public  DataRecord msEmptyRecord;
    DataRecord  mRecord;

    public LuaList(int key){ mRecord = msTable.GetRecord(key); }
    public LuaList(string key){ mRecord = msTable.GetRecord(key); }
    public bool exist(){ return mRecord!=null; } 

    public Int32 INDEX { get { if (mRecord!=null) return (Int32)mRecord.getObject(0); return (Int32)msEmptyRecord.getObject(0); } }
    public String LUA { get { if (mRecord!=null) return (String)mRecord.getObject(1); return (String)msEmptyRecord.getObject(1); } }

    static public void Init()
    {
        msTable = TableManager.GetTable("LuaList");
        if (msTable!=null)
        {
            if (Check())  { msEmptyRecord = msTable.NewRecord();  LOG.log("LuaList init config succeed"); } else LOG.logError("LuaList config check fail");
        }
        else
          LOG.logError("No exist config table >LuaList");
    }
    static public bool Check()
    {
        FieldIndex field = msTable.GetField();
        if (field.Count() != 2) { LOG.logError("LuaList field count != 2"); return false; }
        string[] fieldNameList = { "INDEX", "LUA" };
        for (int i = 0; i < fieldNameList.Length; ++i)
        {
            string key = fieldNameList[i];
            if (field.GetField(key) == null) { LOG.logError("LuaList No exist field >" + key); return false; }
            if (field.GetField(key).getPosition() != i) { LOG.logError(string.Format("LuaList field {0} col !=  {1}", key, i)); return false; }
        }
        return true;
    }
}