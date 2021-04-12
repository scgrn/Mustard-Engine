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
Input polling
*/

#include "../pch.h"

#include "script.h"
#include "../input/input.h"

namespace AB {

extern Script script;
extern Input input;

//----------------------------------------------------------------- INPUT --------------------------------------
// TODO:		setMousePosition(x, y)

// generate lua scancode constant list for documentation here

static int luaShowCursor(lua_State* luaVM) {
    bool enabled = (bool)lua_toboolean(luaVM, 1);
    input.showCursor(enabled);

    return 0;
}

static int luaGetMouseState(lua_State* luaVM) {
    int x, y;

    Uint32 buttons = SDL_GetMouseState(&x, &y);

    lua_pushinteger(luaVM, x);
    lua_pushinteger(luaVM, y);
	
	//	TODO: these should come from input subsystem. why does script even know about SDL.
    lua_pushboolean(luaVM, buttons & SDL_BUTTON(SDL_BUTTON_LEFT));
    lua_pushboolean(luaVM, buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE));
    lua_pushboolean(luaVM, buttons & SDL_BUTTON(SDL_BUTTON_RIGHT));

    return 5;
}

static int luaKeyWasPressed(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasPressed(scanCode));

    return 1;
}

static int luaKeyPressed(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.isPressed(scanCode));

    return 1;
}

static int luaKeyWasReleased(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasReleased(scanCode));

    return 1;
}

static int luaKeyName(lua_State* luaVM) {
    //  TODO: should i be concerned that this complains about the cast?

    SDL_Scancode scancode = (SDL_Scancode)lua_tonumber(luaVM, 1);
    bool upCase = false;
    if (lua_gettop(luaVM) >= 2) {
        upCase = (bool)lua_toboolean(luaVM, 2);
    }

    std::string name = SDL_GetKeyName(SDL_GetKeyFromScancode(scancode));
    if (upCase) {
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    }

    lua_pushstring(luaVM, name.c_str());

    return 1;
}

static int luaButtonName(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);

    std::string name = "UNKNOWN";
    /*
    if (button < SDL_CONTROLLER_BUTTON_MAX) {
        name = SDL_GameControllerGetStringForButton(button);
    } else {
        // TODO: custom axis - button conversion thinger
        name = "UNIMPLEMENTED";
    }
    // SDL_CONTROLLER_BUTTON_DPAD_UP
    */

    lua_pushstring(luaVM, name.c_str());

    return 1;
}

static int luaGamepadPressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);

    int index = 0;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    }

    if (button < SDL_CONTROLLER_BUTTON_MAX) {
        lua_pushboolean(luaVM, input.gamepadIsPressed(index, button));
    } else {
        // TODO: custom axis - button conversion thinger

                //... wait no, input subsystem should handle that transparently.
    }

    return 1;
}

static int luaNumGamepads(lua_State* luaVM) {
    lua_pushinteger(luaVM, input.numGamepads());

    return 1;
}

void registerInputFunctions() {
    static const luaL_Reg inputFuncs[] = {
        { "getMouseState", luaGetMouseState},
        { "showCursor", luaShowCursor},
        { "keyWasPressed", luaKeyWasPressed},
        { "keyPressed", luaKeyPressed},
        { "keyWasReleased", luaKeyWasReleased},
        { "keyName", luaKeyName},
        { "buttonName", luaButtonName},
        { "gamepadPressed", luaGamepadPressed},
        { "numGamepads", luaNumGamepads},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "input", inputFuncs);
}

}
