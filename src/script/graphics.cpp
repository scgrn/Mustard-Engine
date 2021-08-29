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

extern ResourceManager<Sprite> sprites;
extern ResourceManager<Shader> shaders;
extern std::map<int, BatchRenderer*> batchRenderers;

//----------------------------------------------------------------- Graphics state --------------------------------
static int luaResetVideo(lua_State* luaVM) {
    window.setVideoMode(NULL);

    return 0;
}

//----------------------------------------------------------------- Graphics ------------------------------------------

///	Loads a sprite
// @param index Sprite index
// @param filename Filename
// @function AB.graphics.loadSprite
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

///	Queues a sprite to be renderer on the screen or current canvas.
// @param layer Rendering layer. A default layer of 0 is provided
// @param index Sprite index
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
// @param scaleX (1) Scale X
// @param scaleY (scaleX) Scale Y
// @param r (1.0) Red component
// @param g (1.0) Green component
// @param b (1.0) Blue component
// @param a (1.0) Alpha component
// @function AB.graphics.renderSprite

// renderSprite(batchIndex, index, x, y, z, r = 0, sx = 1, sy = 1, additive = false)
static int luaRenderSprite(lua_State* luaVM) {
    int layer = (int)lua_tonumber(luaVM, 1);
    int index = (int)lua_tonumber(luaVM, 2);
    float x = (float)lua_tonumber(luaVM, 3);
    float y = (float)lua_tonumber(luaVM, 4);

    float z = -1.0f;
    float angle = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
	
	float r = 1.0f;
	float b = 1.0f;
	float g = 1.0f;
	float a = 1.0f;

    if (lua_gettop(luaVM) >= 5) {
		z = (float)lua_tonumber(luaVM, 5);
    }
    if (lua_gettop(luaVM) >= 6) {
		angle = (float)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
		scaleX = (float)lua_tonumber(luaVM, 7);
    }
    if (lua_gettop(luaVM) >= 8) {
		scaleY = (float)lua_tonumber(luaVM, 8);
    } else {
        scaleY = scaleX;
    }

    if (lua_gettop(luaVM) >= 9) {
		r = (float)lua_tonumber(luaVM, 9);
    }
    if (lua_gettop(luaVM) >= 10) {
		g = (float)lua_tonumber(luaVM, 10);
    }
    if (lua_gettop(luaVM) >= 11) {
		b = (float)lua_tonumber(luaVM, 11);
    }
    if (lua_gettop(luaVM) >= 12) {
		a = (float)lua_tonumber(luaVM, 12);
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
	
	//	TODO: need to support scaleY in quad renderer
	sprites.get(index)->render(batchRenderers[layer], Vec3(x, y, z), angle, scaleX, Vec4(r, g, b, a));

    return 0;
}

/// Adds a sprite into the current atlas queue. This should be done at startup before any rendering occurs. Sprites that are commonly
// used together can be batched into a single atlas to improve performance by minimizing texture switches.
// @param index Sprite index
// @function AB.graphics.addToAtlas
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

///	Queues a quad to be renderer on the screen or current canvas.
// @param layer Rendering layer. A default layer of 0 is provided
// @param width Width
// @param height Height
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
// @param r (1.0) Red color component
// @param g (1.0) Green color component
// @param b (1.0) Blue color component
// @param a (1.0) Alpha component
// @function AB.graphics.renderQuad
static int luaRenderQuad(lua_State* luaVM) {
    int layer = (int)lua_tonumber(luaVM, 1);
    float width = (float)lua_tonumber(luaVM, 2);
    float height = (float)lua_tonumber(luaVM, 3);
    float x = (float)lua_tonumber(luaVM, 4);
    float y = (float)lua_tonumber(luaVM, 5);

    float z = -1.0f;
    float angle = 0.0f;
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    if (lua_gettop(luaVM) >= 6) {
		z = (float)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
		angle = (float)lua_tonumber(luaVM, 7);
    }
    if (lua_gettop(luaVM) >= 8) {
		r = (float)lua_tonumber(luaVM, 8);
    }
    if (lua_gettop(luaVM) >= 9) {
		g = (float)lua_tonumber(luaVM, 9);
    }
    if (lua_gettop(luaVM) >= 10) {
		b = (float)lua_tonumber(luaVM, 10);
    }
    if (lua_gettop(luaVM) >= 11) {
		a = (float)lua_tonumber(luaVM, 11);
    }
	
	//	TODO: need to support scaleY in quad renderer
	// sprites.get(index)->render(batchRenderers[layer], Vec3(x, y, z), angle, scaleX);

    return 0;
}

/*
///	Creates a rendering canvas.
// @param index
// @param width
// @param height
// @function AB.graphics.createCanvas
static int luaCreateCanvas(lua_State* luaVM) {
	return 0;
}

///	Uses a canvas for rendering. Pass 0 to restore default frameBuffer.
// @param index
// @function AB.graphics.useCanvas
static int luaUseCanvas(lua_State* luaVM) {
	return 0;
}
*/
/// Creates a new rendering layer. (BatchRenderer internally). Layers are renderered back to front, largest indices first.
// A default layer of 0 is provided
// @param index Layer index
// @function AB.graphics.createLayer
static int luaCreateLayer(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

	//	TODO: pass in colorTransform, depthSorting
	batchRenderers[index] = new BatchRenderer();

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
		{ "renderQuad", luaRenderQuad},
		
		//{ "createCanvas", luaCreateCanvas},
		//{ "useCanvas", luaUseCanvas},

		// { "colorTransform", luaColorTransform},
		// { "loadShader", luaLoadShader},
		{ "createLayer", luaCreateLayer},
		
        { NULL, NULL }
    };
    script.registerFuncs("AB", "graphics", graphicsFuncs);
}

}
