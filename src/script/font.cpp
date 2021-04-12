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
Font loading and rendering
*/

#include "../pch.h"

#include "script.h"

namespace AB {

extern Script script;

//----------------------------------------------------------------- Text functions --------------------------------

#if 0

static int luaLoadFont(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    std::string filename = std::string(lua_tostring(luaVM, 2));

    AB::graphics->fonts.mapResource(fontIndex, filename);

    return 0;
}

static int luaPrintString(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    float x = (float)lua_tonumber(luaVM, 2);
    float y = (float)lua_tonumber(luaVM, 3);
    float scale = (float)lua_tonumber(luaVM, 4);
    int alignment = (int)lua_tonumber(luaVM, 5);
    std::string str = std::string(lua_tostring(luaVM, 6));
/*
    //  adjust to match love2d font metrics
    scale /= 36.0f;
    y += scale * 32.0f;
    alignment++;
*/
	Font::Align align;
	switch (alignment) {
		case 1: align = Font::LEFT; break;
		case 2: align = Font::CENTER; break;
		case 3: align = Font::RIGHT; break;
		default: align = Font::LEFT;
	}

    graphics->fonts.get(fontIndex)->printString(x, y, scale, align, str);

	return 0;
}

static int luaStringLength(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    std::string str = std::string(lua_tostring(luaVM, 2));
    float scale = (float)lua_tonumber(luaVM, 3);

    // scale /= 36.0f;

	lua_pushnumber(luaVM, graphics->fonts.get(fontIndex)->stringLength(str, scale));

    return 1;
}
#endif

void registerFontFunctions() {
	/*
    static const luaL_Reg fontFuncs[] = {
        { "loadFont", luaLoadFont},
        { "printString", luaPrintString},
        { "stringLength", luaStringLength},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "font", fontFuncs);
	*/
}

}
