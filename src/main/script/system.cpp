/*

zlib License

(C) 2020 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

/**
System functions
*/

#include "../pch.h"

#include "../core/log.h"
#include "../core/application.h"
#include "../core/fileSystem.h"
#include "../core/window.h"

#include "script.h"

namespace AB {
    
extern Script script;
extern Window window;
extern FileSystem fileSystem;

extern void quit();

//----------------------------------------------------------------- System functions --------------------------------

/// Writes a message to the log
// @param message
// @function AB.system.log
static int luaLog(lua_State* luaVM) {
    std::string str = std::string(lua_tostring(luaVM, 1));
    LOG("LUA: %s", str.c_str());

    return 0;
}

/// Loads and executes a script
// @param filename
// @param forceLocal (false) If false, prepends "scripts//"
// @function AB.system.loadScript
static int luaLoadScript(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));

    bool forceLocal = false;
    if (lua_gettop(luaVM) >= 2) {
        forceLocal = (bool)lua_toboolean(luaVM, 2);
    }
    if (!forceLocal) {
#ifdef DEBUG
        filename = "scripts/" + filename;
#else
        filename = "scripts/compiled" + filename + 'c';
#endif
    }

    DataObject dataObject(filename.c_str(), forceLocal);

    int error = luaL_loadbuffer(luaVM, (const char*)dataObject.getData(), dataObject.getSize(), filename.c_str());
    if (error) {
        std::string errorMsg = lua_tostring(luaVM, -1);
        lua_pop(luaVM, 1);

        LOG("Lua Error: %s", errorMsg.c_str());
#ifdef ANDROID
        extern void reportError(std::string errorMessage);
        reportError(errorMsg);
#else
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", errorMsg.c_str(), NULL);
#endif
   }

    lua_pushvalue(luaVM, -1);
    error = lua_pcall(luaVM, 0, 0, 0);
    if (error) {
        std::string errorMsg = lua_tostring(luaVM, -1);
        lua_pop(luaVM, 1);

        LOG("Lua Error: %s", errorMsg.c_str());
#ifdef ANDROID
        extern void reportError(std::string errorMessage);
        reportError(errorMsg);
#else
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", errorMsg.c_str(), NULL);
#endif

    }

    return 1;
}

/// Loads string from file
// @param key
// @return data
// @function AB.system.loadData
static int luaLoadData(lua_State* luaVM) {
    std::string key = std::string(lua_tostring(luaVM, 1));

    if (key.empty()) {
        LOG("Lua Error: FILE NOT FOUND", 0);
        script.luaError = true;
        return 0;
    }

    std::string value = fileSystem.loadData(key);
    lua_pushstring(luaVM, value.c_str());

    return 1;
}

/// Saves string to file
// @param key
// @param value
// @param compile (false) obfuscates by compiling to Lua bytecode
// @function AB.system.saveData
static int luaSaveData(lua_State* luaVM) {
    //    TODO: compile switch
    std::string key = std::string(lua_tostring(luaVM, 1));
    std::string value = std::string(lua_tostring(luaVM, 2));
    
    fileSystem.saveData(key, value);

    return 0;
}

///    Gets milliseconds since program start
// @function AB.system.getTime
static int luaGetTime(lua_State* luaVM) {
    lua_pushinteger(luaVM, SDL_GetTicks());

    return 1;
}

///    Resynchronise engine timing. Good to call after long-running operations
// @function AB.system.resync
static int luaResync(lua_State* luaVM) {
    extern bool resync;

    resync = true;

    return 0;
}

/// Opens a URL in the system's default browser
// @param URL
// @function AB.system.openURL
static int luaOpenURL(lua_State* luaVM) {
    ERR("Unimplemented!", 0);
    return 0;
}

/// Returns the current operating system
// @return OS string
// @function AB.system.getOS
static int luaGetOS(lua_State* luaVM) {
    ERR("Unimplemented!", 0);
    return 0;
}

/// Checks if engine is running in debug mode
// @function AB.system.debugMode
// @return debugMode
static int luaDebugMode(lua_State* luaVM) {
#ifdef DEBUG
    lua_pushboolean(luaVM, 1);
#else
    lua_pushboolean(luaVM, 0);
#endif
    return 1;
}

/// Exits the program
// @function AB.system.quit
static int luaQuit(lua_State* luaVM) {
    quit();

    return 0;
}

void registerSystemFunctions() {
    static const luaL_Reg systemFuncs[] = {
        { "log", luaLog},
        { "loadScript", luaLoadScript},
        { "loadData", luaLoadData},
        { "saveData", luaSaveData},
        { "getTime", luaGetTime},
        { "resync", luaResync},
        { "openURL", luaOpenURL},
        { "getOS", luaGetOS},
        { "debugMode", luaDebugMode},
        { "quit", luaQuit},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "system", systemFuncs);
}

}
