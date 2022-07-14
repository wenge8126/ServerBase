using UnityEngine;
using System;
using System.Collections;
using Logic;


public class tWindow : BaseEvent
{
    static public Camera mUICamera;
    static public bool mPlayOpenSound = true;
    //static public AudioSource mClickBtnSound = null;
    static public GameObject msUIRoot;

    public delegate void WinFunction(tNiceData butData);
    public System.Reflection.MethodInfo mAsyncFunMethod;

    public GameObject mGameObjUI;
    public string mWinPrefabName = "";
    public string mBundlename = "";
    public string mRootName = "";
    public string mAnchor = "";
    public string mWinName = "";
    public bool mIsRecordOpen = false;
    public bool mIsCache = false;

    //AudioSource mOpenSound = null;

    /// <summary>
    /// 点击关闭 回调
    /// </summary>
    public Action mClickCloseCallBack;

    public int mCfgOpenSound = 0;
    public int mCfgCloseSound = 0;
    

    public void Create()
    {
        mWinPrefabName = getPrefabName();
        InitConfig();
        Init();
        OnInit();
    }
    public virtual string getPrefabName() { return "None"; }

    public virtual void InitConfig() { }

    public virtual void Init() { }

    public virtual void Register() { }

    public virtual bool InitUI() { return true; }

    public virtual void OnInit() { }
    protected virtual void OnDestroy() { }

    public virtual void Destroy()
    {
        if (mGameObjUI != null)
        {
            GameObject.Destroy(mGameObjUI);
            OnDestroy();
        }
        mGameObjUI = null;
    }

    public virtual GameObject GetRoot() { return msUIRoot; }

    
    public virtual void Open(object param, GameObject parentRoot = null)
    {
        //if (Guide.msMainGuide.mbNowOpen && !mbCanOpwnGuide)
        //{
        //    LOG.logWarn("Guide window opening, then can not open other window");
        //    return;
        //}

        if (parentRoot == null)
            parentRoot = GetRoot();

        if (parentRoot == null)
        {
            var root = GameObject.Find("LogicMain/UIRoot");
            if (root != null)
                parentRoot = root;

            if (parentRoot == null)
            {
                LOG.logError("parentRoot is nil");
                return;
            }
        }
      
        if (mGameObjUI == null)
        {
            if (mWinPrefabName != "")
            {
                if (mAnchor == "")
                    mAnchor = "CenterAnchor";
                try
                {
                    mGameObjUI = GameCommon.LoadUIPrefabs(mWinPrefabName, parentRoot);
                    if (mGameObjUI != null)
                    {
                        mGameObjUI.name = mWinPrefabName.Substring(mWinPrefabName.IndexOf("/") + 1);
                        if (!InitUI())
                        {
                            LOG.logError(mWinName + " : Init fail, check ui member is init");
                            return;
                        }
                        Register();
                    }
                }
                catch
                {
                    LOG.logError("Load window fail >" + mWinPrefabName);
                }
            }
        }

        if (mGameObjUI != null)
        {
            if (mIsRecordOpen)
                UIManager.AppendOpenWindow(mWinName);

            mGameObjUI.SetActive(true);
            //PlayOpenSound();
        }

        if (IsOpen())
        {
            OnOpen();
        }
    }

    public virtual void Close()
    {
        if (!IsOpen())
            return;

        //if (mOpenSound != null)
        //{
        //    mOpenSound.Stop();
        //    GameObject.Destroy(mOpenSound);
        //    mOpenSound = null;
        //}

        OnClose();

        if (mGameObjUI != null)
        {
            mGameObjUI.SetActive(false);
        }

        mClickCloseCallBack = null;
    }

    public virtual bool HoldControl() { return true; }
    public virtual void OnOpen()
    {
        if (mAsyncFunMethod == null)
            mAsyncFunMethod = this.GetType().GetMethod("Async");
        if (mAsyncFunMethod!=null)
        {
            mAsyncFunMethod.Invoke(this, null);
        }
    }
    public virtual void OnClose() { }
    public virtual bool IsOpen() { if (mGameObjUI != null) return mGameObjUI.activeInHierarchy; return false; }
    public virtual bool Refresh(object param) { return true; }

    public virtual void WaitUpdate(float spaceTime) { StartUpdate(); }

    public virtual void RegisterButtonEvent(string buttonName, string funName)
    {
        EventCenter.StaticRegister("Button_" +getPrefabName()+"_"+ buttonName, new WindowEventFactory(this, funName));
    }

    public virtual void RegisterButtonEvent(string buttonName, WinFunction function)
    {
        EventCenter.StaticRegister("Button_" +getPrefabName() + "_"+buttonName, new WindowFunctionEventFactory(this, function));
    }
    /// <summary>
    /// 如果没有注册, 进行注册默认函数
    /// </summary>
    public virtual void RegisterButtonWhenNoRegister(string buttonName)
    {
        string butEventName = "Button_" + getPrefabName() + "_"+ buttonName;
        if (!EventCenter.Instance.ExistEventFactory(butEventName))
        {
            RegisterButtonEvent(buttonName, buttonName);
        }
    }

    public virtual void OpenFuncVisibleButton(GameObject buttonObj, bool visible)
    {
        buttonObj.SetActive(visible);
    }

    public virtual void OpenFuncButtonStartTween(GameObject buttonObj) { }

    public void CacheOpen()
    {
        if (mGameObjUI == null)
        {
            if (mWinPrefabName != "")
            {
                if (mAnchor == "")
                    mAnchor = "CenterAnchor";
                try
                {
                    //??? mGameObjUI = GameCommon.LoadUIPrefabs(mWinPrefabName, mBundlename, mRootName, mAnchor);
                    if (mGameObjUI != null)
                        mGameObjUI.name = mWinPrefabName.Substring(mWinPrefabName.IndexOf("/") + 1);

                    if (mGameObjUI.activeSelf)
                        this.StartCoroutine(WaitForEndOfFrame(() => { mGameObjUI.SetActive(false); }));
                }
                catch
                {
                    LOG.logError("Load window fail >" + mWinPrefabName);
                }
            }
        }
    }

    IEnumerator WaitForEndOfFrame(Action callBack)
    {
        yield return new WaitForEndOfFrame();
        if (callBack != null)
            callBack();
    }

    public GameObject GetMGameObjUI()
    {
        return mGameObjUI;
    }

    public GameObject GetSub(string objectName)
    {
        return GameCommon.FindObject(mGameObjUI, objectName);
    }

    public T GetComponentInChildren<T>(string objectName) where T : Component
    {
        GameObject obj = GetSub(objectName);
        if (obj != null)
            return obj.GetComponentInChildren<T>();

        return null;
    }

    public T GetComponent<T>(string objectName) where T : Component
    {
        GameObject obj = GetSub(objectName);
        if (obj != null)
            return obj.GetComponent<T>();

        return null;
    }

    public object GetControl(string controlName)
    {
        if (mGameObjUI == null)
            return null;
        ReferenceCollector rc = mGameObjUI.GetComponent<ReferenceCollector>();
        if (rc == null)
            return null;
        return rc.GetObject(controlName);
    }



    public virtual int GetCurrentTabTypeValue()
    {
        throw new NotImplementedException();
    }

    public virtual bool IsTabOn(int tabTypeValue)
    {
        throw new NotImplementedException();
    }

    public virtual void SetTabType(int tabTypeValue)
    {
        throw new NotImplementedException();
    }

    public virtual void DisposeTabCancelTokens()
    {
        throw new NotImplementedException();
    }
}

