


LogicData = class(BaseEvent);
LogicData.mDebug = "Start";
LogicData.mStep = 0;


RegisterEvent("LogicData")

LogicNode = class(BaseEvent);
LogicNode.mAutoFinish = true;
LogicNode.mData = Start("LogicData");


function LogicNode:run( )
	LOG.log(self:GetName().." run nothing !");
end

function LogicNode:Do(  )
    --LOG.log("Do "..tostring(self.mData));
    self.mData.mStep = self.mData.mStep + 1;
    self.mData.mDebug =  self.mData.mDebug.." > "..tostring(self.mData.mStep).." : "..self:GetName();
    LOG.log(self:GetName().. " >>> Ready run ...")
	self:WaitTime(0.1);
end
 
 function LogicNode:OnTimeOver(  )
    LOG.log(self:GetName().. " >>> start Async");
	local co = coroutine.create(self.Async)
    coroutine.resume(co, self);
end

function LogicNode:Async( )
	self:run();
    if (self.mAutoFinish) then
        LOG.log(self:GetName().. " >>> finish ");
        self:Finish();
    end
end


function LogicNode:Next( )
	return "None";
end

function LogicNode:OnFinish( )
	local name = self:Next();
    
    if (name~=nil and name~="") then
        LOG.log(self:GetName().. " >>> next : "..name);
        StartLogic(name, self.mData);
    else
        LOG.log(" === Over : "..self.mData.mDebug);
    end
end

function LogicNode:Dump( )
	LOG.log("=== "..self.mData.mDebug);
end



function StartLogic(loginName, logicData)
	local logicEvent = Start(loginName);
    if (logicEvent==nil) then        
        return nil;
    end
    
    logicEvent.mData = logicData;
    logicEvent:Do();
    return logicEvent;
end

Node1 = class(LogicNode)

function Node1:run()
    LOG.log("Node1 run");
    AwaitFunc(CS.MainStart.WaitTest);
     LOG.log("Node1 run finish");
end

RegisterEvent("Node1");

local evt = Start("Node1");
evt.mData = Start("LogicData");
evt:Do();

print("ddd");
LOG.log("FFFF");