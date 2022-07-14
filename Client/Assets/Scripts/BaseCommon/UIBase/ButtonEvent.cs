//----------------------------------------------
//            NGUI: Next-Gen UI kit
// Copyright © 2011-2014 Tasharen Entertainment
//----------------------------------------------

using UnityEngine;
using UnityEngine.UI;
using Logic;

/// <summary>
/// Sends a message to the remote object when something happens.
/// </summary>

//[AddComponentMenu("NGUI/Interaction/UIButton Event (Legacy)")]
public class ButtonEvent : MonoBehaviour
{
    public NiceData mData = new ExNiceData();

    public string mWindowName = "None";

	public enum Trigger
	{
		OnClick,
		OnMouseOver,
		OnMouseOut,
		OnPress,
		OnRelease,
		OnDoubleClick,
        OnDrag,
	}

	public Trigger trigger = Trigger.OnClick;
	public bool includeChildren = false;
	
	
    bool mbLongPress;
    bool mbNeedLongPress;
    float mCurTime;
    float mSpaceTime = 0.5f;

   // bool mbIsSmallTipsButton = false;
    public bool mPlaySound = true;

	public void Start () 
    {       
        //mData.set("_NAME_", gameObject.name);
        //mData.set("BUTTON", gameObject);

        Button but = GetComponent<Button>();
        if (but)
        {
            but.onClick.AddListener(Send);
        }
        else
            LOG.logError("No find Button Script");
    }
	
	//void OnEnable () { if (mStarted) OnHover(UICamera.IsHighlighted(gameObject)); }
	
	protected void OnHover (bool isOver)
	{
		if (enabled)
		{
			if (((isOver && trigger == Trigger.OnMouseOver) ||
			     (!isOver && trigger == Trigger.OnMouseOut))) Send();
		}
	}
	
	void OnPress (bool isPressed)
	{
		if (enabled)
		{
            //if (!mPlaySound) mPlaySound = true;
            OnPressed(isPressed);

			if (((isPressed && trigger == Trigger.OnPress) ||
			     (!isPressed && trigger == Trigger.OnRelease))) Send();

            if (!isPressed)
            {
                mbLongPress = false;
            }
            else
            {
//                 if (mData.get("SMALL_TIPS", out mbIsSmallTipsButton))
//                 {
//                     if (mbIsSmallTipsButton)
//                         SmallTipsWindow.SetOnLongPressTime(0.1f);
//                 }
            }
		}
	}

    public virtual void OnPressed(bool b) { }
    
	
    //void OnSelect (bool isSelected)
    //{
    //    if (enabled && (!isSelected || UICamera.currentScheme == UICamera.ControlScheme.Controller))
    //        OnHover(isSelected);
    //}

    void OnClick() { if (enabled && trigger == Trigger.OnClick) Send(); }
    void OnDrag() { if (enabled && trigger == Trigger.OnDrag) Send(); }
    void OnDoubleClick() { if (enabled && trigger == Trigger.OnDoubleClick) Send(); }

    void OnLongPress()
    {
        mbNeedLongPress = mData.get("NEED_LONG_PRESS");
        if (enabled && mbNeedLongPress)
        {
            mbLongPress = true;
            mSpaceTime = mData.get("SPACE_TIME");
            if (mSpaceTime <= 0)
                mSpaceTime = 0.5f;

            //Debug.LogError(mbLongPress + " >>NEED_LONG_PRESS == " + (bool)mData.get("NEED_LONG_PRESS"));
        }

//         if (mbIsSmallTipsButton)
//         {
//             int type = mData.get("TYPE");
//             int index = mData.get("INDEX");
//             if(type != 0)
//                 UI.OpenSamllTipsWindow(type, index);
//         }
    }
    void Update()
    {
        if (mbLongPress && mbNeedLongPress && Time.realtimeSinceStartup > mCurTime) 
        {
            mCurTime = Time.realtimeSinceStartup + mSpaceTime;
            Send(); 
        }
    }

   
	public virtual void Send ()
	{
        if (mPlaySound)
        {
            //int soundCfgIndex = DataCenter.mGlobalConfig.GetData("ClickBtn", "VALUE");
            //tWindow.mClickBtnSound = Sound.PlayAudioInMainCamera(soundCfgIndex, "_ClickBtn");
        }

        string evtName = "Button_";
        evtName += mWindowName + "_" + gameObject.name;
        //Debug.Log("------------> evtName=" + evtName);
        Logic.tEvent evt = Logic.EventCenter.Start(evtName);
        if (evt != null)
        {
            evt.setData(mData);             
            evt.Start(gameObject);
        }
        else
		{
			LOG.logError("No exist button event > "+evtName);
		}
	}

}
