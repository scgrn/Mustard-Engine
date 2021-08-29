workspace("Mustard")
	location("./build")
	
	configurations({"Debug", "Release" })
	
	platforms({
		"win64",
		"win32",
		"linux64",
		"linux86",
		"android",
		"web"
	})
	
--	check for 32bit platforms
--	error("32 bit builds are not currently supported")

	architecture("x86_64")

	language("C++")
	cppdialect("gnu++11")

	defines {
		"WIN32",
		"WITH_MINIAUDIO",
		
		-- for emscripten
		-- https://uncovergame.com/2015/01/21/porting-a-complete-c-game-engine-to-html5-through-emscripten/
		"LUA_COMPAT_ALL"
	}
	
	buildoptions  {
		"-Wno-pragmas",
		--"-fpermissive",		-- TODO: why did i need this?
		"-Wpsabi",
		"-msse2"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project("Mustard") ---------------------------------------------------------
	kind("StaticLib")

	staticruntime("on")
	--targetdir("bin/%{cfg.buildcfg}")
	targetdir("./bin")
	targetname("Mustard-%{cfg.buildcfg}")
	
	removeplatforms({"android", "web"})
	
	filter { "platforms:win64" }
		files {
			"**.h",
			"./src/**.cpp",
			"./vendor/glad/**.c",
			"./vendor/lua-5.3.5/**.c",
			"./vendor/tinyxml/**.cpp",
			"./vendor/zlib-1.2.11/*.c",

			"./vendor/soloud20200207/src/core/**.cpp",
			"./vendor/soloud20200207/src/audiosource/**.cpp",
			"./vendor/soloud20200207/src/audiosource/**.c",
			"./vendor/soloud20200207/src/filter**.cpp",
			
			--"./vendor/soloud20200207/src/backend/sdl/**.cpp",
			--"./vendor/soloud20200207/src/backend/sdl/**.c",

			"./vendor/soloud20200207/src/backend/miniaudio/**.cpp",
		}
		
		removefiles {
			"./src/assetCompiler.cpp",
			"./vendor/lua-5.3.5/src/lua.c",
			"./vendor/lua-5.3.5/src/luac.c",
			"./vendor/tinyxml/xmltest.cpp",
		}

		includedirs {
			"./src",
			"./vendor",
			"./vendor/glad",
			"./vendor/lua-5.3.5/src",
			"./vendor/SDL2-2.0.12/x86_64-w64-mingw32/include",
			"./vendor/SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2",
			"./vendor/soloud20200207/include",
			"./vendor/tinyxml",
			"./vendor/zlib-1.2.11"
		}
		
		-- TODO: pch shit doesn't work
		-- pchheader("./src/pch.h")
		-- pchsource("./src/pch.c")

		links {
			"mingw32",
			"opengl32",
			"SDL2main",
			"SDL2.dll"
		}
	 
project("AssetCompiler") ---------------------------------------------------------
	kind("ConsoleApp")
	cppdialect("gnu++17")

	targetdir("./bin")
	targetname("ABAC")
	
	files {
		"./src/assetCompiler.cpp",
		"./vendor/tinyxml/**.cpp",
		"./vendor/zlib-1.2.11/*.c"
	}

	removefiles {
		"./vendor/tinyxml/xmltest.cpp",
	}

	includedirs {
		"./vendor/tinyxml",
		"./vendor/zlib-1.2.11"
	}


include("projectTemplate")
