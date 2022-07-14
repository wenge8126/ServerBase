using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Logic;

namespace LogicHotfix.UIBase
{
    public class UIWindowRegister
    {
        //考虑到这个是频繁变动的文件，为了让SVN更新颗粒度更小，单独从Logic里抽离出来 
        //这里注册Window
        public static void Register()
        {
            //UIManager.Register<TestWindow>("TestWindow");
        }

    }
}
