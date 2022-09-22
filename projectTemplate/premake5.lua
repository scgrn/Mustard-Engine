local project = {
	name = "MUSTARD PROJECT"
}

workspace(project.name)
	location("./build")

	configurations({"Debug", "Development", "Release"})
	
	platforms({
		"win64",
	})

	architecture("x86_64")

	language("C++")
	cppdialect("gnu++11")

	defines {
		"WIN32",
		"WINDOWS_VERSION",
		"WITH_SDL2",
		"LUA_COMPAT_ALL"
	}
	
	buildoptions  {
		"-Wno-pragmas",
		"-Wpsabi",
		"-msse2"
	}

	linkoptions {
		"-static-libstdc++",
		"-static-libgcc",
		--"-static"
	}

	filter({ "configurations:Debug or Development" })
		defines { "DEBUG" }
		kind("ConsoleApp")
		symbols "On"

	filter("configurations:Release")
		defines { "NDEBUG" }
		kind("WindowedApp")
		optimize "On"
		
project(project.name) ---------------------------------------------------------
	removeplatforms({"android", "web"})
	
	targetdir("./bin/%{cfg.buildcfg}")
	
	targetname(project.name)

	staticruntime("on")
	
	filter { "platforms:win64" }
		files {
			"./src/**.cpp",
		}

		includedirs {
			"./src",
			"../../Mustard/src",

			"../../Mustard/vendor",
			"../../Mustard/vendor/lua-5.3.5/src",
			"../../Mustard/vendor/SDL2-2.0.12/x86_64-w64-mingw32/include",
			"../../Mustard/vendor/SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2",
			"../../Mustard/vendor/soloud20200207/include",
			"../../Mustard/vendor/tinyxml",
			"../../Mustard/vendor/zlib-1.2.11"
		}
		
		libdirs {
			"/projects/Mustard/bin/",
			"/projects/Mustard/vendor/SDL2-2.0.12/x86_64-w64-mingw32/lib/",

			"../../Mustard/bin/**",
			"../../Mustard/vendor/SDL2-2.0.12/x86_64-w64-mingw32/lib/**",
		}
		
		links {
			"mingw32",
			"opengl32",
			"SDL2main",
			"SDL2",
		}

		filter { "configurations:Debug" } 
			links { "Mustard-win64-Debug:static" }

		filter { "configurations:Development" } 
			links { "Mustard-win64-Development:static" }

		filter { "configurations:Release" } 
			links { "Mustard-win64-Release:static" }

