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
#include "../core/log.h"
#include "../misc/misc.h"

namespace AB {

extern Script script;
extern Input input;

/// Keyboard scancodes
// @field A 4
// @field B 5
// @field C 6
// @field D 7
// @field E 8
// @field F 9
// @field G 10
// @field H 11
// @field I 12
// @field J 13
// @field K 14
// @field L 15
// @field M 16
// @field N 17
// @field O 18
// @field P 19
// @field Q 20
// @field R 21
// @field S 22
// @field T 23
// @field U 24
// @field V 25
// @field W 26
// @field X 27
// @field Y 28
// @field Z 29
// @field NUM_1 30
// @field NUM_2 31
// @field NUM_3 32
// @field NUM_4 33
// @field NUM_5 34
// @field NUM_6 35
// @field NUM_7 36
// @field NUM_8 37
// @field NUM_9 38
// @field NUM_0 39
// @field RETURN 40
// @field ESCAPE 41
// @field BACKSPACE 42
// @field TAB 43
// @field SPACE 44
// @field MINUS 45
// @field EQUALS 46
// @field LEFTBRACKET 47
// @field RIGHTBRACKET 48
// @field BACKSLASH 49
// @field NONUSHASH 50
// @field SEMICOLON 51
// @field APOSTROPHE 52
// @field GRAVE 53
// @field COMMA 54
// @field PERIOD 55
// @field SLASH 56
// @field CAPSLOCK 57
// @field F1 58
// @field F2 59
// @field F3 60
// @field F4 61
// @field F5 62
// @field F6 63
// @field F7 64
// @field F8 65
// @field F9 66
// @field F10 67
// @field F11 68
// @field F12 69
// @field PRINTSCREEN 70
// @field SCROLLLOCK 71
// @field PAUSE 72
// @field INSERT 73
// @field HOME 74
// @field PAGEUP 75
// @field DELETE 76
// @field END 77
// @field PAGEDOWN 78
// @field RIGHT 79
// @field LEFT 80
// @field DOWN 81
// @field UP 82
// @field NUMLOCKCLEAR 83
// @field KP_DIVIDE 84
// @field KP_MULTIPLY 85
// @field KP_MINUS 86
// @field KP_PLUS 87
// @field KP_ENTER 88
// @field KP_1 89
// @field KP_2 90
// @field KP_3 91
// @field KP_4 92
// @field KP_5 93
// @field KP_6 94
// @field KP_7 95
// @field KP_8 96
// @field KP_9 97
// @field KP_0 98
// @field KP_PERIOD 99
// @field NONUSBACKSLASH 100
// @field APPLICATION 101
// @field POWER 102
// @field KP_EQUALS 103
// @field F13 104
// @field F14 105
// @field F15 106
// @field F16 107
// @field F17 108
// @field F18 109
// @field F19 110
// @field F20 111
// @field F21 112
// @field F22 113
// @field F23 114
// @field F24 115
// @field EXECUTE 116
// @field HELP 117
// @field MENU 118
// @field SELECT 119
// @field STOP 120
// @field AGAIN 121
// @field UNDO 122
// @field CUT 123
// @field COPY 124
// @field PASTE 125
// @field FIND 126
// @field MUTE 127
// @field VOLUMEUP 128
// @field VOLUMEDOWN 129
// @field KP_COMMA 133
// @field KP_EQUALSAS400 134
// @field INTERNATIONAL1 135
// @field INTERNATIONAL2 136
// @field INTERNATIONAL3 137
// @field INTERNATIONAL4 138
// @field INTERNATIONAL5 139
// @field INTERNATIONAL6 140
// @field INTERNATIONAL7 141
// @field INTERNATIONAL8 142
// @field INTERNATIONAL9 143
// @field LANG1 144
// @field LANG2 145
// @field LANG3 146
// @field LANG4 147
// @field LANG5 148
// @field LANG6 149
// @field LANG7 150
// @field LANG8 151
// @field LANG9 152
// @field ALTERASE 153
// @field SYSREQ 154
// @field CANCEL 155
// @field CLEAR 156
// @field PRIOR 157
// @field RETURN2 158
// @field SEPARATOR 159
// @field OUT 160
// @field OPER 161
// @field CLEARAGAIN 162
// @field CRSEL 163
// @field EXSEL 164
// @field KP_00 176
// @field KP_000 177
// @field THOUSANDSSEPARATOR 178
// @field DECIMALSEPARATOR 179
// @field CURRENCYUNIT 180
// @field CURRENCYSUBUNIT 181
// @field KP_LEFTPAREN 182
// @field KP_RIGHTPAREN 183
// @field KP_LEFTBRACE 184
// @field KP_RIGHTBRACE 185
// @field KP_TAB 186
// @field KP_BACKSPACE 187
// @field KP_A 188
// @field KP_B 189
// @field KP_C 190
// @field KP_D 191
// @field KP_E 192
// @field KP_F 193
// @field KP_XOR 194
// @field KP_POWER 195
// @field KP_PERCENT 196
// @field KP_LESS 197
// @field KP_GREATER 198
// @field KP_AMPERSAND 199
// @field KP_DBLAMPERSAND 200
// @field KP_VERTICALBAR 201
// @field KP_DBLVERTICALBAR 202
// @field KP_COLON 203
// @field KP_HASH 204
// @field KP_SPACE 205
// @field KP_AT 206
// @field KP_EXCLAM 207
// @field KP_MEMSTORE 208
// @field KP_MEMRECALL 209
// @field KP_MEMCLEAR 210
// @field KP_MEMADD 211
// @field KP_MEMSUBTRACT 212
// @field KP_MEMMULTIPLY 213
// @field KP_MEMDIVIDE 214
// @field KP_PLUSMINUS 215
// @field KP_CLEAR 216
// @field KP_CLEARENTRY 217
// @field KP_BINARY 218
// @field KP_OCTAL 219
// @field KP_DECIMAL 220
// @field KP_HEXADECIMAL 221
// @field LCTRL 224
// @field LSHIFT 225
// @field LALT 226
// @field LGUI 227
// @field RCTRL 228
// @field RSHIFT 229
// @field RALT 230
// @field RGUI 231
// @field MODE 257
// @field AUDIONEXT 258
// @field AUDIOPREV 259
// @field AUDIOSTOP 260
// @field AUDIOPLAY 261
// @field AUDIOMUTE 262
// @field MEDIASELECT 263
// @field WWW 264
// @field MAIL 265
// @field CALCULATOR 266
// @field COMPUTER 267
// @field AC_SEARCH 268
// @field AC_HOME 269
// @field AC_BACK 270
// @field AC_FORWARD 271
// @field AC_STOP 272
// @field AC_REFRESH 273
// @field AC_BOOKMARKS 274
// @field BRIGHTNESSDOWN 275
// @field BRIGHTNESSUP 276
// @field DISPLAYSWITCH 277
// @field KBDILLUMTOGGLE 278
// @field KBDILLUMDOWN 279
// @field KBDILLUMUP 280
// @field EJECT 281
// @field SLEEP 282
// @field APP1 283
// @field APP2 284
// @field AUDIOREWIND 285
// @field AUDIOFASTFORWARD 286
// @table scanCodes

/// Gamepad buttons
// @field A
// @field B
// @field X
// @field Y
// @field BACK
// @field GUIDE
// @field START
// @field LEFTSTICK
// @field RIGHTSTICK
// @field LEFTSHOULDER
// @field RIGHTSHOULDER
// @field DPAD_UP
// @field DPAD_DOWN
// @field DPAD_LEFT
// @field DPAD_RIGHT
// @field LSTICK_UP
// @field LSTICK_DOWN
// @field LSTICK_LEFT
// @field LSTICK_RIGHT
// @field RSTICK_UP
// @field RSTICK_DOWN
// @field RSTICK_LEFT
// @field RSTICK_RIGHT
// @field LTRIGGER
// @field RTRIGGER
// @table gamepadButtons

/// Gamepad axes
// @field LEFT_X
// @field LEFT_Y
// @field RIGHT_X
// @field RIGHT_Y
// @field TRIGGER_LEFT
// @field TRIGGER_RIGHT
// @table gamepadAxes

//----------------------------------------------------------------- INPUT --------------------------------------

// generate lua scancode constant list for documentation here

///    Set mouse cursor visibility
// @function AB.input.showCursor
// @param visible (true) Mouse cursor visible
static int luaShowCursor(lua_State* luaVM) {
    bool visible = (bool)lua_toboolean(luaVM, 1);
    input.showMouseCursor(visible);

    return 0;
}

/// Gets current mouse state
// @function AB.input.getMouseState
// @return X position
// @return Y position
// @return Left mouse button
// @return Middle mouse button
// @return Right mouse button
// @usage x, y, left, middle, right = AB.input.getMouseState()
static int luaGetMouseState(lua_State* luaVM) {
    Vec2 pos = input.getMousePosition();
    lua_pushinteger(luaVM, (int)pos.x);
    lua_pushinteger(luaVM, (int)pos.y);
    
    for (int i = 0; i < 3; i++) {
        lua_pushboolean(luaVM, input.isMousePressed(i));
    }
    
    return 5;
}

/// Sets mouse position
// @function AB.input.setMousePosition
// @param x X position
// @param y Y position
static int luaSetMousePosition(lua_State* luaVM) {
    int x = (int)lua_tonumber(luaVM, 1);
    int y = (int)lua_tonumber(luaVM, 2);
    
    input.setMousePosition(Vec2(x, y));

    return 0;
}

/// Checks if mouse button was pressed this frame
// @function AB.input.mouseWasPressed
// @param button Button index (1-2)
// @return pressed
static int luaMouseWasPressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1) - 1;
    lua_pushboolean(luaVM, input.wasMousePressed(button));

    return 1;
}

/// Checks if a mouse button is currently being held
// @function AB.input.mousePressed
// @param button Button index (1-3)
// @return pressed
static int luaMousePressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1) - 1;
    lua_pushboolean(luaVM, input.isMousePressed(button));

    return 1;
}

/// Checks if a mouse button was released this frame
// @function AB.input.mouseWasReleased
// @param button Button index (1-3)
// @return released
static int luaMouseWasReleased(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1) - 1;
    lua_pushboolean(luaVM, input.wasMouseReleased(button));

    return 1;
}

/// Checks if a keyboard key was pressed this frame
// @function AB.input.keyWasPressed
// @param scanCode Key scancode
// @return pressed
// @see scanCodes
static int luaKeyWasPressed(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasKeyPressed(scanCode));

    return 1;
}

/// Checks if a keyboard key is being held
// @function AB.input.keyPressed
// @param scanCode Key scancode
// @return pressed
// @usage pressed = AB.input.keyPressed(AB.input.scancodes.RETURN)
// @see scanCodes
static int luaKeyPressed(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.isKeyPressed(scanCode));

    return 1;
}

/// Checks if a keyboard key was released this frame
// @function AB.input.keyWasReleased
// @param scanCode Key scancode
// @return released
// @see scanCodes
static int luaKeyWasReleased(lua_State* luaVM) {
    int scanCode = (int)lua_tonumber(luaVM, 1);
    lua_pushboolean(luaVM, input.wasKeyReleased(scanCode));

    return 1;
}

/// Gets a string representation of a key
// @function AB.input.keyName
// @param scanCode Key scancode
// @param uppercase (false) Convert to uppercase
// @return name
// @see scanCodes
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

/// Gets a string representation of a gamepad button
// @function AB.input.buttonName
// @param button Button number
// @return name
// @see gamepadButtons
static int luaButtonName(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);

    std::string name;
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
        case input.BUTTON_DPAD_UP: name = "D-pad up"; break;
        case input.BUTTON_DPAD_DOWN: name = "D-pad down"; break;
        case input.BUTTON_DPAD_LEFT: name = "D-pad left"; break;
        case input.BUTTON_DPAD_RIGHT: name = "D-pad right"; break;
        
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
        
        default: name = "UNKNOWN";
    }
    lua_pushstring(luaVM, name.c_str());

    return 1;
}

/// Gets a string representation of a gamepad axis
// @function AB.input.axisName
// @param axis Axis number
// @return name
// @see gamepadAxes
static int luaAxisName(lua_State* luaVM) {
    int axis = (int)lua_tonumber(luaVM, 1);

    std::string name;
    switch (axis) {
        case input.AXIS_LEFT_X: name = "Left stick X"; break;
        case input.AXIS_LEFT_Y: name = "Left stick Y"; break;
        case input.AXIS_RIGHT_X: name = "Right stick X"; break;
        case input.AXIS_RIGHT_Y: name = "Right stick Y"; break;
        case input.AXIS_TRIGGER_LEFT: name = "Left trigger"; break;
        case input.AXIS_TRIGGER_RIGHT: name = "Right trigger"; break;

        default: name = "UNKNOWN";
    }
    lua_pushstring(luaVM, name.c_str());

    return 1;
}

//    BONUS BONER BARBECUE

/// Checks if a gamepad button was pressed this frame
// @function AB.input.gamepadWasPressed
// @param button Button number
// @param index (0) Gamepad index
// @return pressed
// @see gamepadButtons
// @usage pressed = AB.input.gamepadWasPressed(AB.input.gamepadButtons.START)
static int luaGamepadWasPressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);

    int index = 0;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    }

    if (button >= 0 && button < input.BUTTON_MAX) {
        lua_pushboolean(luaVM, input.gamepadWasPressed(index, button));
    } else {
        LOG("Invalid button: %d", button);
        lua_pushboolean(luaVM, 0);
    }

    return 1;
}

/// Checks if a gamepad button is currently being pressed
// @function AB.input.gamepadPressed
// @param button Button number
// @param index (0) Gamepad index
// @return pressed
// @see gamepadButtons
static int luaGamepadPressed(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);

    int index = 0;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    }

    if (button >= 0 && button < input.BUTTON_MAX) {
        lua_pushboolean(luaVM, input.gamepadIsPressed(index, button));
    } else {
        LOG("Invalid button: %d", button);
        lua_pushboolean(luaVM, 0);
    }

    return 1;
}

/// Checks if a gamepad button was released this frame
// @function AB.input.gamepadWasReleased
// @param button Button number
// @param index (0) Gamepad index
// @return pressed
// @see gamepadButtons
static int luaGamepadWasReleased(lua_State* luaVM) {
    int button = (int)lua_tonumber(luaVM, 1);

    int index = 0;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    }

    if (button >= 0 && button < input.BUTTON_MAX) {
        lua_pushboolean(luaVM, input.gamepadWasReleased(index, button));
    } else {
        LOG("Invalid button: %d", button);
        lua_pushboolean(luaVM, 0);
    }

    return 1;
}

/// Gets current value of a gamepad axis, accounting for deadzone
// @function AB.input.gamepadGetAxis
// @param axis Axis index
// @param index (0) Gamepad index
// @return position
// @see gamepadAxes
static int luaGamepadGetAxis(lua_State* luaVM) {
    int axis = (int)lua_tonumber(luaVM, 1);

    int index = 0;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    }

    if (axis >= 0 && axis < input.AXIS_MAX) {
        lua_pushnumber(luaVM, input.gamepadAxis(index, axis));
    } else {
        LOG("Invalid axis: %d", axis);
        lua_pushnumber(luaVM, 0);
    }
    
    return 1;
}

/// Gets current number of gamepads attached to the system
// @function AB.input.numGamepads
// @return numGamepads
static int luaNumGamepads(lua_State* luaVM) {
    lua_pushinteger(luaVM, input.getNumGamepads());

    return 1;
}

/// Gets name of gamepad
// @function AB.input.getGamepadName
// @return gamepad name
static int luaGetGamepadName(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    lua_pushstring(luaVM, input.getGamepadName(index).c_str());

    return 1;
}

/// Tracks which input was used last (gamepad or keyboard/mouse) for UI hints
// @function AB.input.showGamepadControls
// @return showGamepadControls
static int luaShowGamepadControls(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.showGamepadControls);
    
    return 1;
}

/// Vibrates!
// @function AB.input.vibrate
// @param strength Vibration strength [0..1]
// @param duration Duration in milliseconds
// @param index (0) Gamepad index (0 for mobile device)
static int luaVibrate(lua_State* luaVM) {
    float strength = (float)lua_tonumber(luaVM, 1);
    int duration = (int)lua_tonumber(luaVM, 2);

    int index = 0;
    if (lua_gettop(luaVM) >= 3) {
        index = (int)lua_tonumber(luaVM, 3);
    }
    
#ifdef ANDROID
#else
    input.vibrate(index, strength, duration);
#endif

    return 0;
}

/// Checks if a menu up event has occured
// @function AB.input.menuUp
// @return pressed
static int luaMenuUp(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.menuUp());
    return 1;
}

/// Checks if a menu down event has occured
// @function AB.input.menuDown
// @return pressed
static int luaMenuDown(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.menuDown());
    return 1;
}

/// Checks if a menu left event has occured
// @function AB.input.menuLeft
// @return pressed
static int luaMenuLeft(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.menuLeft());
    return 1;
}

/// Checks if a menu right event has occured
// @function AB.input.menuRight
// @return pressed
static int luaMenuRight(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.menuRight());
    return 1;
}

/// Checks if a menu select event has occured
// @function AB.input.menuSelect
// @return pressed
static int luaMenuSelect(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.menuSelect());
    return 1;
}

/// Checks if a menu back event has occured
// @function AB.input.menuBack
// @return pressed
static int luaMenuBack(lua_State* luaVM) {
    lua_pushboolean(luaVM, input.menuBack());
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
        { "axisName", luaAxisName},
        { "gamepadWasPressed", luaGamepadWasPressed}, 
        { "gamepadPressed", luaGamepadPressed},
        { "gamepadWasReleased", luaGamepadWasReleased},
        { "gamepadGetAxis", luaGamepadGetAxis},
        { "numGamepads", luaNumGamepads},
        { "getGamepadName", luaGetGamepadName},
        { "showGamepadControls", luaShowGamepadControls},

        { "vibrate", luaVibrate},
        
        { "menuUp", luaMenuUp},
        { "menuDown", luaMenuDown},
        { "menuLeft", luaMenuLeft},
        { "menuRight", luaMenuRight},
        { "menuSelect", luaMenuSelect},
        { "menuBack", luaMenuBack},
        
        { NULL, NULL }
    };
    script.registerFuncs("AB", "input", inputFuncs);

    std::string command = "AB.input.scancodes = {";
#define X(a, b) command += #a; command += "=" + toString(b) + ",";
#include "../input/scancodes.h"
#undef X
    command += "}";
    script.execute(command);

    script.execute("AB.input.gamepadButtons={"
        "A=0,"
        "B=1,"
        "X=2,"
        "Y=3,"
        "BACK=4,"
        "GUIDE=5,"
        "START=6,"
        "LEFTSTICK=7,"
        "RIGHTSTICK=8,"
        "LEFTSHOULDER=9,"
        "RIGHTSHOULDER=10,"
        "DPAD_UP=11,"
        "DPAD_DOWN=12,"
        "DPAD_LEFT=13,"
        "DPAD_RIGHT=14,"
        
        "LSTICK_UP=16,"
        "LSTICK_DOWN=17,"
        "LSTICK_LEFT=18,"
        "LSTICK_RIGHT=19,"
        "RSTICK_UP=20,"
        "RSTICK_DOWN=21,"
        "RSTICK_LEFT=22,"
        "RSTICK_RIGHT=23,"
        "LTRIGGER=24,"
        "RTRIGGER=25}");

    script.execute("AB.input.gamepadAxes={"
        "LEFT_X=0,"
        "LEFT_Y=1,"
        "RIGHT_X=2,"
        "RIGHT_Y=3,"
        "TRIGGER_LEFT=4,"
        "TRIGGER_RIGHT=5}");
}

}
