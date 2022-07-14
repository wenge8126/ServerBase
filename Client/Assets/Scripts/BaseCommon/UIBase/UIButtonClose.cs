//----------------------------------------------
//            NGUI: Next-Gen UI kit
// Copyright © 2011-2014 Tasharen Entertainment
//----------------------------------------------

using UnityEngine;
using Logic;
using System;

/// <summary>
/// Sends a message to the remote object when something happens.
/// </summary>

[AddComponentMenu("NGUI/Interaction/UIButton Close (Legacy)")]
public class UIButtonClose : MonoBehaviour
{
    public string mCloseWindowName;

    public Action mCloseHandle = null;

    public enum Trigger
    {
        OnClick,
        OnMouseOver,
        OnMouseOut,
        OnPress,
        OnRelease,
        OnDoubleClick,
    }

    public Trigger trigger = Trigger.OnClick;
    public bool includeChildren = false;

    

    void Start()
    {
       

    }

    void OnHover(bool isOver)
    {
        if (enabled)
        {
            if (((isOver && trigger == Trigger.OnMouseOver) ||
                 (!isOver && trigger == Trigger.OnMouseOut))) Send();
        }
    }

    void OnPress(bool isPressed)
    {
        if (enabled)
        {
            if (((isPressed && trigger == Trigger.OnPress) ||
                 (!isPressed && trigger == Trigger.OnRelease))) Send();
        }
    }

    void OnClick() { if (enabled && trigger == Trigger.OnClick) Send(); }

    void OnDoubleClick() { if (enabled && trigger == Trigger.OnDoubleClick) Send(); }

    void Send()
    {
        tWindow win = UIManager.GetWindow(mCloseWindowName);
        if (win != null)
        {
            tWindow.mPlayOpenSound = false;
            if (win.mCfgCloseSound == 0)
            {
                //int soundCfgIndex = DataCenter.mGlobalConfig.GetData("CloseWindow", "VALUE");
                //Sound.PlayAudioInMainCamera(soundCfgIndex, "_CloseWin");
            }
            
            if (mCloseHandle != null)
            {
                mCloseHandle();
                mCloseHandle = null;
            }
            else
            {
                    UIManager.CloseWindow(mCloseWindowName);
            }

            tWindow.mPlayOpenSound = true;
        }
        else
            LOG.logError(string.Format("{0} :have not factory", mCloseWindowName));
    }
}
