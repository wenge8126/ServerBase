-- Auto generate lua for config table


--[[
LuaList
{
    INDEX : INT
    LUA : STRING
} --]]

function LuaList(key)
    local record = TableManager.GetTable("LuaList"):GetRecord(key);
    local config = { };
    config.INDEX = record:getObject(0);
    config.LUA = record:getObject(1);
    return config;
end