using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using UnityEngine;
using UnityEditor;
using UnityEngine.UI;

[CustomEditor(typeof(WindowEdit))]

[ExecuteInEditMode]

public class WindowTool : Editor
{
    public override void OnInspectorGUI()
    {        
        WindowEdit winEdit = (WindowEdit)target;
        //绘制一个窗口
        winEdit.mWindowScriptPath = EditorGUILayout.TextField("窗口脚本路径",
                winEdit.mWindowScriptPath);
        //绘制一个贴图槽
        //test.texture = EditorGUILayout.ObjectField("增加一个贴图", test.texture, typeof(Texture), true) as Texture;

        if (GUILayout.Button("生成窗口脚本", GUILayout.Width(100f)))
        {
            GenerateC4(winEdit.gameObject);
        }

         string c4FileName = winEdit.mWindowScriptPath + winEdit.name + ".cs";
        if (File.Exists(c4FileName))
        {
            if (GUILayout.Button("打开脚本", GUILayout.Width(100f)))
            {
                UnityEngine.Object obj = AssetDatabase.LoadAssetAtPath(c4FileName, typeof(MonoScript));
                if (obj != null)
                    AssetDatabase.OpenAsset(obj);
                //System.Diagnostics.Process.Start("C:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\Common7\\IDE\\devenv.exe", c4FileName);
            }
        }        

        winEdit.mWindowLuaPath = EditorGUILayout.TextField("窗口LUA脚本路径",
                winEdit.mWindowLuaPath);
        if (GUILayout.Button("生成窗口LUA脚本", GUILayout.Width(100f)))
        {
            GenerateLua(winEdit.gameObject);
        }

        string luaFileName = winEdit.mWindowLuaPath + winEdit.name + ".lua";

        if (File.Exists(luaFileName))
        {
            if (GUILayout.Button("打开LUA脚本", GUILayout.Width(100f)))
            {
                System.Diagnostics.Process.Start("notepad.exe", luaFileName);
            }
        }
    }

    public string mWindowName = "";

    void GenerateC4(GameObject winObject)
    {
        mWindowName = winObject.name;

        Dictionary<string, UIObject> varList = new Dictionary<string, UIObject>();
        string path = "";
        _GetAllValue(path, winObject, ref varList);
        
        WindowEdit winEdit = (WindowEdit)target;
        Logic.UpdateResourceMgr.CreateDirctory(winEdit.mWindowScriptPath);
        string luaFileName = winEdit.mWindowScriptPath + winEdit.name + ".cs";

        string strLuaScript = "// Auto generate for " + winEdit.name + "\r\n";
        strLuaScript += @"using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;";

        strLuaScript += "\r\n\r\npublic class " + winEdit.name + " : tWindow\r\n{\r\n";
        //strLuaScript += "        public string mWindowName = \"" + mWindowName +"\";\r\n";
        strLuaScript += "    public override string getPrefabName(){ return \"" + mWindowName + "\" ; }\r\n\r\n";
        foreach (var vk in varList)
        {
            if (vk.Value.mComType == "ButtonEvent")
                continue;
            if (string.IsNullOrEmpty(vk.Value.mComType))
                strLuaScript += "    protected Transform " + vk.Key + ";\r\n";
            else
            {                
                strLuaScript += "    protected "+vk.Value.mComType + " " + vk.Key + ";\r\n";                
            }
        }

        string memberStr = "";
        string initStr = "";
        string memberInit = "";

        var uiCtrl = winObject.GetComponent<ReferenceCollector>();
        if (uiCtrl != null)
        {
            /// 加上控件集合中的变量
            var uiDataList = uiCtrl.data;

            uiDataList.ForEach(t =>
            {
                var memberName = t.key;
                var typeStr = t.gameObject.GetType();

                memberStr += string.Format("    protected {0} {1} = null;\r\n", typeStr, memberName);
                initStr += string.Format("        {0} = rc.Get<{1}>(\"{2}\");\r\n", memberName, typeStr, memberName);
                initStr += _GenerateCheckCode(memberName);

                memberInit += string.Format("        {0}=null;\r\n", memberName);
            });
        }

        strLuaScript += memberStr;

        strLuaScript += "\r\n    public override void OnInit()\r\n    {\r\n";

        foreach (var vk in varList)
        {
            var v = vk.Value;
            if (v.mComType=="ButtonEvent")
            {
                strLuaScript += string.Format("        // {0}\r\n", v.uiObj.name);
                strLuaScript += string.Format("        RegisterButtonWhenNoRegister(\"{0}\");\r\n", v.uiObj.name);
            }
        }
        strLuaScript += "    }\r\n\r\n";

        strLuaScript += "\r\n    public override bool InitUI()\r\n    {\r\n"; //        string parentName = GameCommon.GetFullName(mGameObjUI, mGameObjUI.name);\r\n\r\n";

        strLuaScript += "        var rc = mGameObjUI.GetComponent<ReferenceCollector>();\r\n\r\n";

        // 记录所有已经导出的 GameObject, 避免重复 
        Dictionary<string, string> goList = new Dictionary<string, string>();

        foreach (var vk in varList)
        {
            var v = vk.Value;
            if (v.mComType == "ButtonEvent")
                continue;

            string existParent = "";
            if (!goList.TryGetValue(v.mParentPath, out existParent))
            {
                // 生成父的临时
                string str = _GenerateParentGoMember(v.mParentPath, ref goList);
                strLuaScript += str;
            }

            //existParent = goList[v.mParentPath];
            if (!goList.TryGetValue(v.mParentPath, out existParent))
            {
                LOG.logError("No exist parent : " + v.mParentPath);
                continue;
            }

            if (string.IsNullOrEmpty(v.mComType))
            {
                if (goList.ContainsKey(v.uiObjPath+"/"))
                    continue;

                
                strLuaScript += string.Format( "            {0} = {1}.Find(\"{2}\");\r\n", vk.Key, existParent, v.uiObj.name);
                goList[v.uiObjPath + "/"] = vk.Key;
                strLuaScript += _GenerateCheckCode(vk.Key);
            }
            else
            {
                strLuaScript += _GenerateParentGoMember(v.uiObjPath, ref goList);
                string goMember = "";
                if (!goList.TryGetValue(v.uiObjPath+"/", out goMember))
                {
                    LOG.logError("No exist game object : " + v.uiObjPath+"/");
                    continue;
                }
                
                strLuaScript += string.Format("            {0} = {1}.GetComponent<{2}>();\r\n", vk.Key, goMember, v.mComType);
                strLuaScript += _GenerateCheckCode(vk.Key);                
            }
        }
        strLuaScript += "\r\n";
        strLuaScript += initStr;

        strLuaScript += "        return true;\r\n";

        strLuaScript += "    }\r\n\r\n";

        strLuaScript += "    protected override void OnDestroy()\r\n    {\r\n";
        foreach (var vk in varList)
        {
            if (vk.Value.mComType == "ButtonEvent")
                continue;
           
            strLuaScript += string.Format( "        {0} = null;\r\n", vk.Key);   
        }
        strLuaScript += memberInit;

        strLuaScript += "    }\r\n";

        strLuaScript += "\r\n}\r\n";
        
        try
        {
            FileStream fs = new FileStream(luaFileName, FileMode.Create);
            byte[] data = Common.GetBytes(strLuaScript);
            fs.Write(data, 0, data.Length);
            fs.Close();
            AssetDatabase.Refresh();

            LOG.log("Succeed generate lua script " + luaFileName);
        }
        catch (System.Exception e)
        {
            LOG.logError("Create table file fail >" + luaFileName + ", Error>" + e.ToString());
        }
        catch
        {
            LOG.logError("Create table file fail >" + luaFileName);
        }
    }

    string _GenerateParentGoMember(string path, ref Dictionary<string, string> goList)
    {
        string scriptString = "";
        string[] pathList = path.Split( '/');
        string nowPath = "";
        string nowName = "";
        for (int i=0; i< pathList.Length; ++i)
        {            
            string s = pathList[i];
            if (string.IsNullOrEmpty(s))
                continue;

            nowName += "_";
            nowName += s;
            string path2 = nowPath;            
            path2 += s;
            path2 += "/";
            if (goList.ContainsKey(path2))
            {
                nowPath += s;
                nowPath += "/";
                continue;
            }
            string p = "mGameObjUI.transform";
            if (!string.IsNullOrEmpty(nowPath))
            {
                if (!goList.TryGetValue(nowPath, out p))
                {
                    LOG.logError("No exist parent : " + nowPath);
                    nowPath += s;
                    nowPath += "/";
                    continue;
                }
            }
            scriptString += string.Format("        Transform _{0} = {1}.Find(\"{2}\");\r\n", nowName, p, s);
            scriptString += _GenerateCheckCode("_" + nowName);
            //nowPath += "/";
            nowPath += s;
            nowPath += "/";
            goList[nowPath] = "_" + nowName;
            
        }
        return scriptString;
    }

    string _GenerateCheckCode(string memberName)
    {
        return string.Format("                if ({0}==null)", memberName) + " {" +string.Format(" LOG.logError(\"No exist : {0}\"); return false; ", memberName) +" }\r\n";
    }

    void GenerateLua(GameObject winObject)
    {
        mWindowName = winObject.name;
        Dictionary<string, UIObject> varList = new Dictionary<string, UIObject>();
        string path = "";
        _GetAllValue(path, winObject, ref varList, true);
        
        WindowEdit winEdit = (WindowEdit)target;
        string luaFileName = winEdit.mWindowLuaPath + winEdit.name+".lua";
        
        string strLuaScript = "-- Auto generate for " + winEdit.name + "\r\n" + "\r\n";
        strLuaScript += winEdit.name + " = class(base_window);\r\n\r\n";

        strLuaScript += "function " + winEdit.name + " : getPrefab()   \r\n\r\n";
        strLuaScript += string.Format("    return \"{0}\";\r\n", mWindowName);
        strLuaScript += "end\r\n\r\n";

        strLuaScript += "function " + winEdit.name + " : OnInit()   \r\n\r\n";
        foreach (var vk in varList)
        {
            if (vk.Value.mComType != "ButtonEvent")
                continue;
            strLuaScript += string.Format("   self:CheckRegisterButton( \"{0}\");\r\n", vk.Value.uiObj.name);
        }
        strLuaScript += "end\r\n\r\n";

        // 根据控制集合生成变量
        string memberStr = "";
        string initStr = "";
        string memberInit = "";

        var uiCtrl = winObject.GetComponent<ReferenceCollector>();
        if (uiCtrl != null)
        {
            /// 加上控件集合中的变量
            var uiDataList = uiCtrl.data;

            uiDataList.ForEach(t =>
            {
                var memberName = t.key;
                var typeStr = t.gameObject.GetType();

                memberStr += string.Format("    self.{0} = nil;\r\n", memberName);
                initStr += string.Format("        self.{0} = self.win:GetControl(\"{2}\");\r\n", memberName, typeStr, memberName);
                initStr += string.Format("                if (self.{0}==nil) then ", memberName) + string.Format(" LOG.logError(\"No exist : {0}\"); return false; ", memberName) + " end\r\n";

                memberInit += string.Format("        self.{0}=nil;\r\n", memberName);
            });
        }

        strLuaScript += "function " + winEdit.name + ":InitUI(win)   \r\n\r\n";
        strLuaScript += initStr;
        strLuaScript += "    return true;\r\nend\r\n\r\n";

        strLuaScript += "function " + winEdit.name + ":OnDestroy(win)   \r\n\r\n";
        strLuaScript += memberInit;
        strLuaScript += "end\r\n\r\n";

        //strLuaScript += "function " + winEdit.name + ":InitUI(win)\r\n    local parentName = GameCommon.GetFullName(win.mGameObjUI, win.mGameObjUI.name);\r\n\r\n";
        //foreach (var vk in varList)
        //{
        //    string objTag = vk.Value.uiObj.tag;
        //    if (objTag == "GameObject")
        //        strLuaScript += "    self." + vk.Key + " = GameObject.Find(parentName..\"/" + vk.Value.uiObjPath + "\");\r\n";
        //    else
        //        strLuaScript += "    self." + vk.Key + " = GameObject.Find(parentName..\"/" + vk.Value.uiObjPath + "\"):GetComponent(\"" + objTag + "\");\r\n";
        //}
        //strLuaScript += "end\r\n";

        try
        {           
            FileStream fs = new FileStream(luaFileName, FileMode.Create);
            byte[] data = Common.GetBytes(strLuaScript);
            fs.Write(data, 0,  data.Length);
            fs.Close();
            LOG.log("Succeed generate lua script " + luaFileName);
        }
        catch (System.Exception e)
        {
            LOG.logError("Create table file fail >" + luaFileName + ", Error>" + e.ToString());
        }
        catch
        {
            LOG.logError("Create table file fail >" + luaFileName);
        }
    }

    struct UIObject
    {
        public GameObject uiObj;
        public string uiObjPath;
        public string mComType;
        public string mParentPath;
    }

    void _GetAllValue(string path, GameObject uiObject, ref Dictionary<string, UIObject> resultList, bool bLua = false)
    {
        Transform tran = null;
        for (int i = 0; i < uiObject.transform.childCount; i++)
        {
            tran = uiObject.transform.GetChild(i);

            var compList = tran.GetComponents<MonoBehaviour>();
            for (int m=0; m< compList.Length; ++m)
            {
                if (compList[m] == null)
                    DestroyImmediate(compList[m]);
            }

            if (true)
            {
                var but = tran.GetComponent<Button>();
                //if (tran.tag == "ButtonEvent")
                if (but != null)
                {
                    ButtonEvent com = tran.GetComponent<ButtonEvent>();
                    if (com == null)
                    {            
                        com = tran.gameObject.AddComponent<ButtonEvent>();
                        //com.mILScriptName = "ButtonEvent";
                    }
                    com.mWindowName = mWindowName;
                    var butEventName = mWindowName + "_" + tran.name;
                    if (resultList.ContainsKey(butEventName))
                    {
                        LOG.logError("Button name repeat : " + butEventName);
                    }

                    UIObject obj;
                    obj.uiObj = tran.gameObject;
                    obj.uiObjPath = path + tran.name;
                    obj.mComType = "ButtonEvent";
                    obj.mParentPath = path;
                    resultList[butEventName] = obj;
                }
            }

            if (tran.tag != "Untagged")
            {
                string name = tran.gameObject.name;
                string val = path + name;
                string temp = path;
                name = temp.Replace("/", "_") + name;
                if (resultList.ContainsKey(name))
                {
                    //string temp = path;
                    //name = temp.Replace("/", "__") + name;

                    LOG.logError("Repeat name : " + name);
                    continue;
                }

                if (tran.tag != "GameObject")
                {                
                    //name = uiObject.name + "_" + name;
                    if (tran.tag == "All")
                    {
                        {                            
                            UIObject obj;
                            obj.uiObj = tran.gameObject;
                            obj.uiObjPath = val;
                            obj.mComType = "";
                            obj.mParentPath = path;
                            resultList["mGo_" + name] = obj;
                        }
                        MonoBehaviour[] comList = tran.gameObject.GetComponents<MonoBehaviour>();
                        for (int m=0; m<comList.Length; ++m)
                        {
                            if (comList[m]==null || comList[m].GetType().Name == "ButtonEvent")
                                continue;

                            UIObject obj;
                            obj.uiObj = tran.gameObject;
                            obj.uiObjPath = val;
                            obj.mComType = comList[m].GetType().Name;
                            obj.mParentPath = path;

                            string memberName  = "m" + obj.mComType +"_" + name;
                            resultList[memberName] = obj;                            
                        }
                    }
                    else
                    {
                        var com = tran.GetComponent(tran.tag);
                        if (com == null)
                        {
                            LOG.logWarn(name + " No exist component : " + tran.tag);
                            continue;
                        }
                        if (tran.tag == "ButtonEvent")
                            continue;
                        name = "m"+ tran.tag + "_" + name;
                        
                        UIObject obj;
                        obj.uiObj = tran.gameObject;
                        obj.uiObjPath = val;
                        obj.mComType = tran.tag;
                        obj.mParentPath = path;
                        resultList[name] = obj;
                    }
                }
                else
                {
                    name = "mGo_" + name;
                    UIObject obj;
                    obj.uiObj = tran.gameObject;
                    obj.uiObjPath = val;
                    obj.mComType = "";
                    obj.mParentPath = path;
                    resultList[name] = obj;
                }
                
            }
            _GetAllValue(path + tran.gameObject.name + "/", tran.gameObject, ref resultList, bLua);            
        }
    }
    
    [MenuItem("GameObject/CopyPath #a", priority = 12)]
    private static void NewMenuOption()
    {
        if (Selection.activeGameObject)
        {
            string s = GetTransPath(Selection.activeGameObject.transform);
            Debug.Log("[CopyPath]:  " + s);
            
        }
    }

    /// <summary>
    /// 获得GameObject在Hierarchy中的完整路径
    /// </summary>
    public static string GetTransPath(Transform trans)
    {
        if (!trans.parent)
        {
            return trans.name;

        }
        return GetTransPath(trans.parent) + "/" + trans.name;
    }
}
