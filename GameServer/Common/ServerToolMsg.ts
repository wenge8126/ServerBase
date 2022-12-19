//Auto genereate msg data code		

import BaseMsg from "../Base/BaseMsg"
import NiceData, { AData } from "../Base/NiceData"


export default class tBaseMsg extends BaseMsg
{
    public static CreateData(className) :tBaseMsg
    {
        switch (className)
        {
            case "TR_UpdateSaveResource":
                return new TR_UpdateSaveResource();
            case "RT_ReponseResult":
                return new RT_ReponseResult();
            case "CR_LoadResource":
                return new CR_LoadResource();
            case "RC_ResponseResource":
                return new RC_ResponseResource();
        }
        return null;
    }
}

//  工具更新保存资源 > ResourcesActor
export class TR_UpdateSaveResource  extends tBaseMsg
{
    public static msMsgName = "TR_UpdateSaveResource";

    public  GetMsgName():string { return  "TR_UpdateSaveResource"; }

    public mIndexName : string = "";	// STRING	
    public mMD5 : string = "";	// STRING	
    public mResourceData : egret.ByteArray = null;	// DATA	

    public GetType(memberName:string):AData
    {
        switch (memberName)
        {
            case "mIndexName":  return NiceData._getType(4);		// STRING
            case "mMD5":  return NiceData._getType(4);		// STRING
            case "mResourceData":  return NiceData._getType(11);		// DATA
        }
        return null;
    }

    public NewArrayElement(arrayName:string): BaseMsg
    {
        switch (arrayName)
        {
        }
        return null;
    }

    public IsMsg(memberName) :boolean
    {
        switch (memberName)
        {
        }
        return false;
    }

    public ResetClear()
    {
        this.mIndexName = "";
        this.mMD5 = "";
        this.mResourceData = null;
    }
}

export class RT_ReponseResult  extends tBaseMsg
{
    public static msMsgName = "RT_ReponseResult";

    public  GetMsgName():string { return  "RT_ReponseResult"; }

    public mErrorCode : number = 0;	// INT	
    public mInfo : string = "";	// STRING	

    public GetType(memberName:string):AData
    {
        switch (memberName)
        {
            case "mErrorCode":  return NiceData._getType(1);		// INT
            case "mInfo":  return NiceData._getType(4);		// STRING
        }
        return null;
    }

    public NewArrayElement(arrayName:string): BaseMsg
    {
        switch (arrayName)
        {
        }
        return null;
    }

    public IsMsg(memberName) :boolean
    {
        switch (memberName)
        {
        }
        return false;
    }

    public ResetClear()
    {
        this.mErrorCode = 0;
        this.mInfo = "";
    }
}

//  请求获取资源数据
export class CR_LoadResource  extends tBaseMsg
{
    public static msMsgName = "CR_LoadResource";

    public  GetMsgName():string { return  "CR_LoadResource"; }

    public mIndexName : string = "";	// STRING	
    public mMD5 : string = "";	// STRING	

    public GetType(memberName:string):AData
    {
        switch (memberName)
        {
            case "mIndexName":  return NiceData._getType(4);		// STRING
            case "mMD5":  return NiceData._getType(4);		// STRING
        }
        return null;
    }

    public NewArrayElement(arrayName:string): BaseMsg
    {
        switch (arrayName)
        {
        }
        return null;
    }

    public IsMsg(memberName) :boolean
    {
        switch (memberName)
        {
        }
        return false;
    }

    public ResetClear()
    {
        this.mIndexName = "";
        this.mMD5 = "";
    }
}

export class RC_ResponseResource  extends tBaseMsg
{
    public static msMsgName = "RC_ResponseResource";

    public  GetMsgName():string { return  "RC_ResponseResource"; }

    public mErrorCode : number = 0;	// INT	
    public mResourceData : egret.ByteArray = null;	// DATA	

    public GetType(memberName:string):AData
    {
        switch (memberName)
        {
            case "mErrorCode":  return NiceData._getType(1);		// INT
            case "mResourceData":  return NiceData._getType(11);		// DATA
        }
        return null;
    }

    public NewArrayElement(arrayName:string): BaseMsg
    {
        switch (arrayName)
        {
        }
        return null;
    }

    public IsMsg(memberName) :boolean
    {
        switch (memberName)
        {
        }
        return false;
    }

    public ResetClear()
    {
        this.mErrorCode = 0;
        this.mResourceData = null;
    }
}

