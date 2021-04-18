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
Graphics state and rendering
*/

#include "../pch.h"

#include "script.h"
#include "../core/window.h"
#include "../renderer/spriteAtlas.h"

namespace AB {

extern Script script;
extern Window window;

//	not sure if i like these here...
ResourceManager<Sprite> sprites;
ResourceManager<Shader> shaders;
std::map<int, BatchRenderer> batchRenderers;

//----------------------------------------------------------------- Graphics state --------------------------------
static int luaResetVideo(lua_State* luaVM) {
    window.setVideoMode(NULL);

    return 0;
}

//----------------------------------------------------------------- Graphics ------------------------------------------

static int luaLoadSprite(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    std::string filename = std::string(lua_tostring(luaVM, 2));

    bool createMask = true;
    if (lua_gettop(luaVM) >= 3) {
        createMask = (bool)lua_toboolean(luaVM, 3);
    }

    sprites.mapResource(index, filename);
    if (createMask) {
        sprites.get(index)->buildCollisionMask();
    }

    return 0;
}

// renderSprite(index, x, y, z, r = 0, sx = 1, sy = 1, additive = false)
static int luaRenderSprite(lua_State* luaVM) {
    int frame = (int)lua_tonumber(luaVM, 1);
    float x = (float)lua_tonumber(luaVM, 2);
    float y = (float)lua_tonumber(luaVM, 3);

    float z = -1.0f;
    float angle = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (lua_gettop(luaVM) >= 4) {
		z = (float)lua_tonumber(luaVM, 4);
    }
    if (lua_gettop(luaVM) >= 5) {
		angle = (float)lua_tonumber(luaVM, 5);
    }
    if (lua_gettop(luaVM) >= 6) {
		scaleX = (float)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
		scaleY = (float)lua_tonumber(luaVM, 7);
    } else {
        scaleY = scaleX;
    }
/*
    bool batch = false;
    if (lua_gettop(luaVM) >= 8) {
        batch = (bool)lua_toboolean(luaVM, 8);
    }

    graphics->spriteBatchAlpha->add(graphics->sprites.get(frame), x, y, z, angle, scaleX, scaleY);
    if (!batch) {
        graphics->spriteBatchAlpha->render();
        graphics->spriteBatchAlpha->clear();
    }
*/
    return 0;
}


static int luaAddToAtlas(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    addToAtlas(sprites.get(index));

    return 0;
}

/// Builds a sprite atlas. Call this after several calls to AB.graphics.addToAtlas
// @function AB.graphics.buildAtlas
static int luaBuildAtlas(lua_State* luaVM) {
    buildAtlas();

    return 0;
}

/// Returns width of a sprite
// @param index Sprite index
// @return width
// @function AB.graphics.spriteWidth
static int luaSpriteWidth(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    int width = sprites.get(index)->width;

    lua_pushinteger(luaVM, width);

    return 1;
}

/// Returns height of a sprite
// @param index Sprite index
// @return height
// @function AB.graphics.spriteHeight
static int luaSpriteHeight(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    int height = sprites.get(index)->height;

    lua_pushinteger(luaVM, height);

    return 1;
}

/// Creates a new rendering layer. (BatchRenderer internally). Layers are renderered back to front, largest indices first. A default layer of 0 is provided
// @param index Layer index
// @function AB.graphics.addLayer
static int luaAddLayer(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

	return 0;
}

void registerGraphicsFunctions() {
    static const luaL_Reg graphicsFuncs[] = {
        { "resetVideo", luaResetVideo},

        { "loadSprite", luaLoadSprite},
        { "addToAtlas", luaAddToAtlas},
        { "buildAtlas", luaBuildAtlas},

        { "renderSprite", luaRenderSprite},
        { "spriteWidth", luaSpriteWidth},
        { "spriteHeight", luaSpriteHeight},
		
		// { "colorTransform", luaColorTransform},
		// { "loadShader", luaLoadShader},
		{ "addLayer", luaAddLayer},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "graphics", graphicsFuncs);
}

}
