

-- 新手引导基本节点
-- 检查开启对话条件
-- 保存引导数据（进度数据）
-- 重启后恢复引导


gJsonData = {}

TutorialNode = class(LogicNode)
TutorialNode.mFinished = false;
TutorialNode.mStarted = false;


function TutorialNode:GetShowDialogAsyncFunc()
    return CS.GameCommon.WaitDialog;
end

function TutorialNode:ShowDialog()
    AwaitFunc(self:GetShowDialogAsyncFunc());
end

function TutorialNode:CheckStart()
    return true;
end


function TutorialNode:Save()
    local data = {}
    data.mName = self:GetName();
    data.mFinished = self.mFinished;
    data.mStarted = self.mStarted;

    gJsonData[self:GetName] = data;
    local strData = json.encode(gJsonData);
    CS.GameCommon.SaveJson("");
end


function TutorialNode:Restor()

end


function TutorialNode:run()

    if (self.mFinished) then
        self:Finish()
        return;
    end
    
    if (self.mStarted) then
        self:ShowDialog();    

    elseif (self:CheckStart()) then
    
        self.mStarted = true;
        self:Save();
        self:ShowDialog();

    end

    self.Save();
end


-- 保存或读取引导数据（JSON格式）
function LoadTutorial()
    local strData = CS.GameCommon.LoadJson();
    gJsonData = json.decode(strData);
end