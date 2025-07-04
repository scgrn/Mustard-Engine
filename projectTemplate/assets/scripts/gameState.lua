declare("GameState", {})
GameState.__index = GameState

declare("setGameState", function(newState)
    if (newState ~= nil) then
        newState:start()
        currentState = newState
    else
        error("INVALID STATE!")
    end
end)

function GameState:new()
    local this = {
        start = function() end,
        cancel = function() end,
        click = function(x,y) end,
        keyPressed = function(key) end,
        backPressed = function() end,
        update = function() end,
        render = function() end
    }
    setmetatable(this,self)
    return this
end

declare("currentState", GameState:new())

