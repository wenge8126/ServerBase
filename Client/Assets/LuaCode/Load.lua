
luaList = {
    "Window",
    "Config",
    "LogicNode",
    -- "OpenWindowTutorial",
    --"TestLuaEvent"
}

---@module **Tutorial/OnWindowOpen
---@module **Tutorial/Tutorial*
---@module **base/base
luaTutorialList = {
    "base/base",
    "Tutorial/TutorialBase",
    "Tutorial/OnWindowOpen",
    "Tutorial/Tutorial27",
    "Tutorial/Tutorial30"
}

function load()
    for i, v in ipairs(luaList) do
        CS.LuaMain.LoadLua(v)
    end

    -- for i, v in ipairs(luaTutorialList) do
    --     CS.LuaMain.LoadLua(v)
    -- end
end

load()
