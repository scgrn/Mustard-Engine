function loadConfig()
	videoConfig = {
		title = TITLE or "AB SAMPLE PROJECT",
		xRes = 800,
		yRes = 600,
		fullscreen = false,
		vsync = true,
	}
end

function AB.init()
	--AB.graphics.loadSprite(1, "gfx/tv1.tga")
end

function AB.update()
end

function AB.render()
	--AB.graphics.setColor(128, 0, 128, 255)
	--AB.graphics.renderQuad(100, 100, 200, 200)
--[[	
	AB.graphics.setColor(255, 255, 255, 255)
	AB.graphics.renderSprite(1, 200, 200, -1)
	AB.graphics.renderQuad(100,100,50,50)
]]
end

