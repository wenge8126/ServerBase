
    using System.Threading.Tasks;
    using UnityEngine;

    public class MessageWindow : message_window
    {
        public static MessageWindow Instance
        {
            get
            {
                if (!Logic.EventCenter.Instance.ExistEventFactory("MessageWindow"))
                    UIManager.Register<MessageWindow>("MessageWindow");
                MessageWindow window = UIManager.GetWindow("MessageWindow") as MessageWindow;
                return window;
            }
        }
        
        public void SetInfo(string info)
        {
            if (exception_txt != null)
                exception_txt.text = info;
        }

        public void SetTitle(string title)
        {
            if (title_txt != null)
                title_txt.text = title;
        }

        public async Task WaitClose(int milSecond, bool bNeedRestart)
        {
            if (btn_base!=null)
                btn_base.gameObject.SetActive(!bNeedRestart);
            if (btn_txt!=null)
                btn_txt.gameObject.SetActive(!bNeedRestart);
            
            if (bNeedRestart && wait_txt != null)
            {
                int time = (int)(milSecond * 0.001);
                await GameCommon.LoopCheck(1000, milSecond, () =>
                {
                    wait_txt.text = $"游戏将在 {Mathf.Clamp(time--, 0, time)} 秒后重启";
                    return false;
                });
                //wait_txt.text = $"游戏将在 {string.Format("{0:F0}", (float)milSecond*0.001)} 秒后重启";
            }
            else
                await Task.Delay(milSecond);
            
            Close();

            if (bNeedRestart)
                GameCommon.RestartUnity();
        }

        public void but_base(GameObject go)
        {
            Close();
        }
    }

