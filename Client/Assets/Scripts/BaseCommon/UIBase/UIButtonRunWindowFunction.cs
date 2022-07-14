using UnityEngine;
using System.Collections;

public class UIButtonRunWindowFunction : ButtonEvent
{
	public string WindowName;
	public string WindowFunctionName;


	public override void Send ()
	{
        NiceData butData = null; // GameCommon.GetButtonData(gameObject);
		tWindow win = UIManager.GetWindow(WindowName);
		if (win!=null)
		{
			System.Reflection.MethodInfo method = win.GetType().GetMethod(WindowFunctionName);
			if (method!=null)
			{
				object[] p = new object[1];
				p[0] = butData;
				try{
					method.Invoke(win, p);
				}
				catch (System.Exception e)
				{
					LOG.logError(e.ToString());
				}
			}
			else
				LOG.logError("Window function no exist >"+WindowFunctionName);
		}
		else
			LOG.logError("Window no exist >"+WindowName);
	}
}
