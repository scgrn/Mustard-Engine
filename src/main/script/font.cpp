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
Font loading and rendering. Currently only supports <a href="http://www.angelcode.com/products/bmfont/">AngelCode BM font format</a> with XML descriptor
*/

#include "../pch.h"

#include "script.h"
#include "../renderer/font.h"
#include "../renderer/renderer.h"

namespace AB {

extern Script script;
extern AssetManager<Font> fonts;
extern Renderer renderer;

static int fontHandle = 1;

//----------------------------------------------------------------- Text functions --------------------------------

///    Loads a font
// @function AB.font.loadFont
// @param filename Filename.<br/>
// &nbsp;&nbsp;&nbsp;&nbsp;Pass in "default1" for a built-in 8x16 font<br/>
// &nbsp;&nbsp;&nbsp;&nbsp;Pass in "default2" for a built-in 8x8 font<br/>
// &nbsp;&nbsp;&nbsp;&nbsp;Pass in "default3" for a built-in 3x5 font<br/>
// @param index (optional) Font index
// @return font index
static int luaLoadFont(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));
    
    int index;
    if (lua_gettop(luaVM) >= 2) {
        index = (int)lua_tonumber(luaVM, 2);
    } else {
        index = fontHandle;
        fontHandle++;
    }

    fonts.mapAsset(index, filename);

    //  return handle
    lua_pushnumber(luaVM, index);

    return 1;
}

/// Prints a string
// @param layer Rendering layer
// @param index Font index
// @param x X position
// @param y Y position (baseline)
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

    RenderLayer *batchRenderer = reinterpret_cast<RenderLayer*>(renderer.layers[layer]);

    fonts.get(fontIndex)->printString(batchRenderer, x, y, scale, align, str);

    return 0;
}

///    Calculates width of string
// @param index Font index
// @param str String
// @param scale Scale
// @return Width of string in pixels
// @function AB.font.stringLength
static int luaStringLength(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    std::string str = std::string(lua_tostring(luaVM, 2));
    float scale = (float)lua_tonumber(luaVM, 3);

    //    why
    // scale /= 36.0f;

    lua_pushnumber(luaVM, fonts.get(fontIndex)->stringLength(str, scale));

    return 1;
}

/// Sets font (premultiplied) color
// @param index Font index
// @param r (1.0) Red color component
// @param g (1.0) Green color component
// @param b (1.0) Blue color component
// @param opacity (1.0) Opacity
// @param additivity (0.0) Additivity
// @function AB.font.setColor
static int luaSetColor(lua_State* luaVM) {
    int fontIndex = (int)lua_tonumber(luaVM, 1);
    
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float opacity = 1.0f;
    float additivity = 0.0f;
    
    if (lua_gettop(luaVM) >= 2) {
        r = (float)lua_tonumber(luaVM, 2);
    }
    if (lua_gettop(luaVM) >= 3) {
        g = (float)lua_tonumber(luaVM, 3);
    }
    if (lua_gettop(luaVM) >= 4) {
        b = (float)lua_tonumber(luaVM, 4);
    }
    if (lua_gettop(luaVM) >= 5) {
        opacity = (float)lua_tonumber(luaVM, 5);
    }
    if (lua_gettop(luaVM) >= 6) {
        additivity = (float)lua_tonumber(luaVM, 6);
    }
    
    fonts.get(fontIndex)->setColor(r * opacity, g * opacity, b * opacity, 1.0f - additivity);
    
    return 0;
}

void registerFontFunctions() {
    static const luaL_Reg fontFuncs[] = {
        { "loadFont", luaLoadFont},
        { "printString", luaPrintString},
        { "stringLength", luaStringLength},
        { "setColor", luaSetColor},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "font", fontFuncs);
    
    script.execute("AB.font.LEFT = 1");
    script.execute("AB.font.CENTER = 2");
    script.execute("AB.font.RIGHT = 3");
}

}
