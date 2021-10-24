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

// generate lua scancode constant list for documentation here

static int luaShowCursor(lua_State* luaVM) {
    bool visible = (bool)lua_toboolean(luaVM, 1);
    input.showMouseCursor(visible);

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

static int luaSetMousePosition(lua_State* luaVM) {
    int x = (int)lua_tonumber(luaVM, 1);
    int y = (int)lua_tonumber(luaVM, 2);
	
	input.setMousePosition(Vec2(x, y));

    return 0;
}

static int luaMouseWasPressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasMousePressed(button));

    return 1;
}

static int luaMousePressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.isMousePressed(button));

    return 1;
}

static int luaMouseWasReleased(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasKeyReleased(button));

    return 1;
}

static int luaKeyWasPressed(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasKeyPressed(scanCode));

    return 1;
}

static int luaKeyPressed(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.isKeyPressed(scanCode));

    return 1;
}

static int luaKeyWasReleased(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasKeyReleased(scanCode));

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
	
	switch (button) {
		case input.BUTTON_A: name = "A"; break;
		case input.BUTTON_B: name = "B"; break;
		case input.BUTTON_X: name = "X"; break;
		case input.BUTTON_Y: name = "Y"; break;
		case input.BUTTON_BACK: name = "Back"; break;
		case input.BUTTON_GUIDE: name = "Guide"; break;
		case input.BUTTON_START: name = "Start"; break;
		case input.BUTTON_LEFTSTICK: name = "Left stick"; break;
		case input.BUTTON_RIGHTSTICK: name = "Right stick"; break;
		case input.BUTTON_LEFTSHOULDER: name = "Left shoulder"; break;
		case input.BUTTON_RIGHTSHOULDER: name = "Right shoulder"; break;
		case input.BUTTON_DPAD_UP: name = "Dpad up"; break;
		case input.BUTTON_DPAD_DOWN: name = "Dpad down"; break;
		case input.BUTTON_DPAD_LEFT: name = "Dpad left"; break;
		case input.BUTTON_DPAD_RIGHT: name = "Dpad right"; break;
		
		case input.BUTTON_MAX: name = "wut"; break;
		
		case input.BUTTON_LSTICK_UP: name = "Left stick up"; break;
		case input.BUTTON_LSTICK_DOWN: name = "Left stick down"; break;
		case input.BUTTON_LSTICK_LEFT: name = "Left stick left"; break;
		case input.BUTTON_LSTICK_RIGHT: name = "Left stick right"; break;
		case input.BUTTON_RSTICK_UP: name = "Right stick up"; break;
		case input.BUTTON_RSTICK_DOWN: name = "Right stick down"; break;
		case input.BUTTON_RSTICK_LEFT: name = "Right stick left"; break;
		case input.BUTTON_RSTICK_RIGHT: name = "Right stick right"; break;
		case input.BUTTON_LTRIGGER: name = "Left trigger"; break;
		case input.BUTTON_RTRIGGER: name = "Right trigger"; break;	
	}
	
    lua_pushstring(luaVM, name.c_str());

    return 1;
}

//	BONUS BONER BARBECUE
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
    lua_pushinteger(luaVM, input.getNumGamepads());

    return 1;
}

void registerInputFunctions() {
    static const luaL_Reg inputFuncs[] = {
        { "getMouseState", luaGetMouseState},
        { "showCursor", luaShowCursor},
		{ "setMousePosition", luaSetMousePosition},
		{ "mouseWasPressed", luaMouseWasPressed},
		{ "mousePressed", luaMousePressed},
		{ "mouseWasReleased", luaMouseWasReleased},
		
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
