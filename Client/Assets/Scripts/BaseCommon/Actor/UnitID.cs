using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public struct UnitID
{
    private static readonly byte msIDBit = 48;
    private static readonly byte msTypeBit = 16;
    
    public Int64				id;
    public ushort				type;

    
    public UnitID(int ntype = 0, Int64 uid = 0)
    {
        type = (ushort)ntype;
        id = uid;
    }

    public UnitID(UInt64 uValue)
    {
        UInt64 x = uValue >> msIDBit;
        type = (ushort) x;
        id = (ushort)( uValue & (~(x << msIDBit)) );
    }

    static public implicit operator UInt64(UnitID current)
    {
        UInt64 x = (UInt64)current.type;
        x = x << msIDBit;
        x += (UInt64)current.id;
        return x;
    }
    

    public string dump() 
    {
        string info = 
        $"[t: {type.ToString()}, id: {id.ToString()}]";
        return info;
    }
};