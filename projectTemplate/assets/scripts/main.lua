function AB.loadConfig()
	videoConfig = {
		title = "AB SAMPLE PROJECT",
		xRes = 800,
		yRes = 600,
		fullscreen = false,
		vsync = true,
	}
end

function AB.init()
	font = AB.font.loadFont("default1")
end

function AB.update()
end

function AB.render()
	AB.font.printString(0, font, 400, 300, 2, AB.font.CENTER, "Hello, world!")
end

