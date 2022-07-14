using UnityEngine;
using System.Collections;
using System.Reflection;
using System;

public class UIPressItem : MonoBehaviour
{

    bool mCanDoEvent = false;
    float lasttime = 0;
    // Use this for initialization
    void Start()
    {
        lasttime = Time.time;
    }

    protected virtual void OnLongPress()
    {
        //long press
        mCanDoEvent = true;
        Debug.LogWarning("long press-------------------");
    }


    protected virtual void OnPress(bool ispress)
    {
        //mouse up
        if (!ispress) Debug.LogWarning("mouse up-------------------");
        mCanDoEvent = false;
    }


    // Update is called once per frame
    void Update()
    {
        if (mCanDoEvent)
        {
            if (Time.time - lasttime > 0.05)
            {
                lasttime = Time.time;
                StartCoroutine(InvokeAdd());
            }
        }
    }
    IEnumerator InvokeAdd()
    {
        yield return new WaitForSeconds(0.05f);
        //string name = gameObject.name;
        //if (name.Substring(name.Length-3, 3) == "sub")
        //{
        //    ShopWindow.Instance.SubPropBtn(null);
        //}
        //if (name.Substring(name.Length-3, 3) == "add")
        //{
        //    ShopWindow.Instance.AddPropBtn(null);
        //}

    }
}
