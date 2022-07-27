

using System;
using System.Threading.Tasks;
using Logic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;
using LitJson;

public class LoginActor : Actor
{
    public override void Start()
    {
        Log("LoginActor : Start");
        
        UIManager.Register<LoginWindow>("LoginWindow");
        // 先显示
        var win = UIManager.OpenWindow("LoginWindow") as LoginWindow;
        win.mLoginActor = this;
    }

    public async Task ConnectLoginServer(string ip, int port, Int64 wDBID, string token)
    {
        bool bOk = await MainStart.mNet.AsyncConnect(ip, port, 10000);
        LOG.log($"Connect {bOk.ToString()}");
        if (bOk)
        {
            // 发送token
            var msg = new CS_RequestLogin();
            msg.mID = (byte)CS_MSG_ID.eMsg_ClientRequestLogin;
            msg.mDBID = wDBID;
            msg.mToken = token;
            var response = await msg.AsyncRequest(MainStart.mNet);
            if (response != null)
            {
                response.dump("ok=========");
                if (response["error"] == 0)
                {
                    MainStart.mCurrentActor = MainStart.mActorMgr.CreateActor((int)EActorType.Actor_Client, wDBID);
                }
            }
            else
            {
                LOG.log("Request fail, response null");
            }
        }
        else
        {
            LOG.logError($"Connect login server [{ip}:{port.ToString()}] fail");
        }
        Destory();
    }

    public override void OnDestory()
    {
        UIManager.CloseWindow("LoginWindow");
    }
}




public class LoginWindow : login_window
{
    public LoginActor mLoginActor;
    
    public override void OnOpen()
    {
        edit_account.text = "wenge3";
    }
    
    public async Task but_start(GameObject d)
    {
        LOG.log("99999999999 :: "+d.ToString());
        
        string result = await GameCommon.HttpRequest($"http://127.0.0.1:1080/?CMD=CHECK&ACCOUNT={edit_account.text}&PASSWORD=999888");
        if (string.IsNullOrEmpty(result))
        {
            LOG.logError("Request check account fail");
            return;
        }
        
        LOG.log("Check result : "+result.ToString());

        NiceData resultData = GameCommon.FullJson(result);
        resultData.dump("YYYYYYYYYYYYY");

        Int64 v = resultData["DBID"];
        LOG.log(" &&&&&&& DBID : "+v.ToString());

        int error = resultData["error"];
        if (error == 0)
        {
            await mLoginActor.ConnectLoginServer(resultData["LOGIN_IP"], resultData["PORT"],  v, resultData["TOKEN"]);
        }
        else
        {
            LOG.logError("Check account error : "+error.ToString());
        }
    }
}