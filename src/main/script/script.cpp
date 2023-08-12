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

#include "../pch.h"

#include "script.h"
#include "../core/log.h"

namespace AB {

void registerSystemFunctions();
void registerMathFunctions();
void registerGraphicsFunctions();
void registerCollisionFunctions();
void registerFontFunctions();
void registerInputFunctions();
void registerAudioFunctions();
void registerLocalizationFunctions();

static int luaDummyFunc(lua_State* luaVM) {
    return 0;
}

void Script::registerFuncs(std::string const& parent, std::string const& name, const luaL_Reg *funcs) {
    LOG("\tRegistering lua table: %s", name.c_str());

    if (!parent.empty()) {
        lua_getglobal(luaVM, parent.c_str());

        lua_newtable(luaVM);
        luaL_setfuncs(luaVM, funcs, 0);
        lua_pushvalue(luaVM, -1);
        lua_setfield(luaVM, -3, name.c_str()); // AB.[name] = table
        lua_remove(luaVM, -2); // AB

        lua_remove(luaVM, -1); // module
    } else {
        lua_newtable(luaVM);
        luaL_setfuncs(luaVM, funcs, 0);
        lua_pushvalue(luaVM, -1);
        lua_setglobal(luaVM, name.c_str());
        lua_remove(luaVM, -1); // module
    }
}

bool Script::startup() {
    // init lua VM and load libraries
    LOG("Scripting subsystem startup", 0);

    luaVM = luaL_newstate();
    if (!luaVM) {
       throw std::string("Error initializing Lua VM");
    }
    luaL_openlibs(luaVM);

    luaError = false;

    //  register callbacks
    static const luaL_Reg callbackFuncs[] = {
        { "onToggleFullscreen", luaDummyFunc},
        { "onFocusLost", luaDummyFunc},
        { "onFocusGained", luaDummyFunc},
        { "onWindowClose", luaDummyFunc},

        { "onKeyPressed", luaDummyFunc},
        { "onKeyReleased", luaDummyFunc},

        { "onBackPressed", luaDummyFunc},
        { "onTouchPressed", luaDummyFunc},
        { "onTouchReleased", luaDummyFunc},
        { "onTouchMoved", luaDummyFunc},

        { "onMousePressed", luaDummyFunc},
        { "onMouseReleased", luaDummyFunc},
        { "onMouseMoved", luaDummyFunc},
        { "onMouseWheelMoved", luaDummyFunc},

        { "onGamepadConnected", luaDummyFunc},
        { "onGamepadDisconnected", luaDummyFunc},
        { "onGamepadPressed", luaDummyFunc},
        { "onGamepadReleased", luaDummyFunc},
        { "onGamepadAxisMoved", luaDummyFunc},

        { "init", luaDummyFunc},
        { "update", luaDummyFunc},
        { "render", luaDummyFunc},

        { NULL, NULL }
    };
    registerFuncs("", "AB", callbackFuncs);
    
    registerSystemFunctions();
    registerMathFunctions();
    registerGraphicsFunctions();
    registerCollisionFunctions();
    registerFontFunctions();
    registerInputFunctions();
    registerAudioFunctions();
    registerLocalizationFunctions();
    
#ifdef DEBUG
    extern int luaPrint (lua_State *L);
    lua_register(luaVM, "print", luaPrint);
#endif

    // run startup scripts
    execute("AB.system.loadScript('main.lua')");
    execute("AB.loadConfig()");

    // check stack is balanced
    assert(lua_gettop(luaVM) == 0);

    initialized = true;

    return true;
}

static int traceback(lua_State *luaVM) {
    if (!lua_isstring(luaVM, 1)) { /* 'message' not a string? */
        return 1;  /* keep it intact */
    }
    lua_getglobal(luaVM, "debug");
    if (!lua_istable(luaVM, -1)) {
        lua_pop(luaVM, 1);
        return 1;
    }
    lua_getfield(luaVM, -1, "traceback");
        if (!lua_isfunction(luaVM, -1)) {
        lua_pop(luaVM, 2);
        return 1;
    }
    lua_pushvalue(luaVM, 1);  /* pass error message */
    lua_pushinteger(luaVM, 2);  /* skip this function and traceback */
    lua_call(luaVM, 2, 1);  /* call debug.traceback */

    return 1;
}

void Script::execute(std::string command) {
    if (!luaError) {
        lua_pushcfunction(luaVM, traceback);
        if (luaL_loadstring(luaVM, command.c_str()) || lua_pcall(luaVM, 0, LUA_MULTRET, lua_gettop(luaVM) - 1)) {
            std::string errorMsg = lua_tostring(luaVM, -1);
            lua_pop(luaVM, 2);

            LOG("Lua Error: %s", errorMsg.c_str());

#ifdef ANDROID
            extern void reportError(std::string errorMessage);
            reportError(errorMsg);
#else
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", errorMsg.c_str(), NULL);
#endif

            luaError = true;
        }
        lua_pop(luaVM, 1); /* remove debug.traceback from the stack */
    }
}

void Script::shutdown() {
    if (luaVM) {
        lua_close(luaVM);
        luaVM = 0;
    }

    LOG("Scripting subsystem shutdown", 0);
}

}   //   namespace

