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
Font loading and rendering. Currently only supports <a href="http://www.angelcode.com/products/bmfont/">AngelCode BM font format</a>
*/

#include "../pch.h"

#include "script.h"
#include "../renderer/font.h"

namespace AB {

extern Script script;
extern ResourceManager<Font> fonts;
extern std::map<int, BatchRenderer*> batchRenderers;

//----------------------------------------------------------------- Text functions --------------------------------

///	Loads a font
// @param index Font index
// @param filename Filename
// @function AB.font.loadFont
static int luaLoadFont(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    std::string filename = std::string(lua_tostring(luaVM, 2));

    fonts.mapResource(fontIndex, filename);

    return 0;
}

/// Prints a string
// @param layer Rendering layer
// @param index Font index
// @param x X position
// @param y Y position
// @param scale Scale
// @param alignment Alignment (AB.font.LEFT, AB.font.CENTER, AB.font.RIGHT)
// @param str String to print
// @function AB.font.printString
static int luaPrintString(lua_State* luaVM) {
	int layer = (int)lua_tonumber(luaVM, 1);
    int fontIndex = (int)lua_tonumber(luaVM, 2);
    float x = (float)lua_tonumber(luaVM, 3);
    float y = (float)lua_tonumber(luaVM, 4);
    float scale = (float)lua_tonumber(luaVM, 5);
    int alignment = (int)lua_tonumber(luaVM, 6);
    std::string str = std::string(lua_tostring(luaVM, 7));
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

    fonts.get(fontIndex)->printString(batchRenderers[layer], x, y, scale, align, str);

	return 0;
}

///	Calculates width of string
// @param index Font index
// @param str String
// @param scale Scale
// @return Width of string in pixels
// @function AB.font.stringLength
static int luaStringLength(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    std::string str = std::string(lua_tostring(luaVM, 2));
    float scale = (float)lua_tonumber(luaVM, 3);

	//	why
    // scale /= 36.0f;

	lua_pushnumber(luaVM, fonts.get(fontIndex)->stringLength(str, scale));

    return 1;
}

void registerFontFunctions() {
    static const luaL_Reg fontFuncs[] = {
        { "loadFont", luaLoadFont},
        { "printString", luaPrintString},
        { "stringLength", luaStringLength},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "font", fontFuncs);
	
	script.execute("AB.font.LEFT = 1");
	script.execute("AB.font.CENTER = 2");
	script.execute("AB.font.RIGHT = 3");
}

}
