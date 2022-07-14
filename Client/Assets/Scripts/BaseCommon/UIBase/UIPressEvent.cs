//----------------------------------------------
//            NGUI: Next-Gen UI kit
// Copyright © 2011-2014 Tasharen Entertainment
//----------------------------------------------

using UnityEngine;
using Logic;

/// <summary>
/// Sends a message to the remote object when something happens.
/// </summary>

[AddComponentMenu("NGUI/Interaction/Button Press Event (Legacy)")]
public class UIPressEvent : MonoBehaviour
{
    public NiceData mData = new ExNiceData();

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

  
    bool mbPress = false;

    public void Start()
    {
       

        mData.set("NAME", gameObject.name);
    }

    void OnPress(bool isPressed)
    {
        if (enabled)
        {
            if (isPressed && !mbPress)
                Send();
            //if (((isPressed && trigger == Trigger.OnPress) ||
            //     (!isPressed && trigger == Trigger.OnRelease))) Send();
        }
        mbPress = isPressed;
    }

    void OnClick() { mbPress = false; }

    //void OnDoubleClick() { if (enabled && trigger == Trigger.OnDoubleClick) Send(); }

    public virtual void Send()
    {
        string evtName = "Button_";
        evtName += gameObject.name;
        Debug.Log("------------> evtName=" + evtName);
        Logic.tEvent evt = Logic.EventCenter.Start(evtName);
        if (evt != null)
        {
            evt.setData(mData);
            evt.Start();
        }
        else
        {
            LOG.logError("No exist button event > " + evtName);
        }
    }

}
