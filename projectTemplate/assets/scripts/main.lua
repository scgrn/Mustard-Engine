--  define globals for luacheck
std = {
  globals = {
    "AB",
  }
}

function AB.loadConfig()
    videoConfig = {
        title = "MUSTARD PROJECT",
        xRes = 800,
        yRes = 600,
        xOffset = 0,
        yOffset = 0,
        xScale = 1,
        yScale = 1,
        fullscreen = false,
        vsync = true,
    }
end

function AB.init()
    font = AB.font.loadFont("default1")
end

function AB.update()
    if (AB.input.keyPressed(AB.input.scancodes.ESCAPE)) then
        AB.system.quit()
    end
end

function AB.render()
    AB.font.setColor(font, 1, 219 / 255, 88 / 255, 1, 0)
    AB.font.printString(0, font, 400, 300, 2, AB.font.CENTER, "Mustard")
end

