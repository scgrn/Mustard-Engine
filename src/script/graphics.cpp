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
#include "../renderer/renderTarget.h"

namespace AB {

extern Script script;
extern Window window;

extern ResourceManager<Sprite> sprites;
extern ResourceManager<Shader> shaders;
extern std::map<int, BatchRenderer*> batchRenderers;
extern std::map<int, RenderTarget*> canvases;

extern Renderer renderer;
extern Window window;
extern OrthographicCamera camera2d;

static Vec4 currentColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
static int spriteHandle = 1;
static int canvasHandle = 1;
static int currentRenderTarget = 0;

/// Color transforms
// @field NONE ()
// @field INVERT ()
// @field MULTIPLY (r, g, b)
// @field SCREEN (r, g, b)
// @field COLOR_FILL (r, g, b, a)
// @field LINEAR_DODGE (r, g, b)
// @field LINEAR_BURN (r, g, b)
// @field COLOR_DODGE (r, g, b)
// @field COLOR_BURN (r, g, b)
// @field TINTED_MONOCHROME (r, g, b, a)
// @field HUE_SHIFT (theta)
// @field SATURATE (value)
// @field BRIGHTNESS (value)
// @field DARKNESS (value)
// @field CONTRAST (value)
// @table colorTransforms

//----------------------------------------------------------------- Graphics state --------------------------------
static int luaResetVideo(lua_State* luaVM) {
    window.setVideoMode(NULL);

    return 0;
}

//----------------------------------------------------------------- Graphics ------------------------------------------

///	Loads a sprite
// @function AB.graphics.loadSprite
// @param filename Filename
// @param collisionMask (true) If a collision mask should be created
// @param index (optional) Sprite index
// @return sprite handle
static int luaLoadSprite(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));

    bool createMask = true;
    if (lua_gettop(luaVM) >= 2) {
        createMask = (bool)lua_toboolean(luaVM, 2);
    }

	int index;
    if (lua_gettop(luaVM) >= 3) {
        index = (int)lua_tonumber(luaVM, 3);
    } else {
		index = spriteHandle;
		spriteHandle++;
	}

    sprites.mapResource(index, filename);
    if (createMask) {
        sprites.get(index)->buildCollisionMask();
    }

	//  return handle
	lua_pushnumber(luaVM, index);

    return index;
}

///	Queues a sprite to be renderer on the screen or current canvas.
// @function AB.graphics.renderSprite
// @param layer Rendering layer. A default layer of 0 is provided
// @param index Sprite index
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
// @param scaleX (1) Scale X
// @param scaleY (scaleX) Scale Y
static int luaRenderSprite(lua_State* luaVM) {
    int layer = (int)lua_tonumber(luaVM, 1);
    int index = (int)lua_tonumber(luaVM, 2);
    float x = (float)lua_tonumber(luaVM, 3);
    float y = (float)lua_tonumber(luaVM, 4);

    float z = -1.0f;
    float angle = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
	
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

	//	TODO: need to support scaleY in quad renderer
	sprites.get(index)->render(batchRenderers[layer], Vec3(x, y, z), angle, scaleX, currentColor);

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

/// Returns size of a sprite
// @function AB.graphics.spriteSize
// @param index Sprite index
// @return width
// @return height
// @usage width, height = AB.graphics.spriteSize(1)
static int luaSpriteSize(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    int width = sprites.get(index)->width;
    int height = sprites.get(index)->height;

    lua_pushinteger(luaVM, width);
	lua_pushinteger(luaVM, height);

    return 2;
}

///	Queues a quad to be renderer on the screen or current canvas.
// @function AB.graphics.renderQuad
// @param layer Rendering layer. A default layer of 0 is provided
// @param width Width
// @param height Height
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
static int luaRenderQuad(lua_State* luaVM) {
    int layer = (int)lua_tonumber(luaVM, 1);
    float width = (float)lua_tonumber(luaVM, 2);
    float height = (float)lua_tonumber(luaVM, 3);
    float x = (float)lua_tonumber(luaVM, 4);
    float y = (float)lua_tonumber(luaVM, 5);

    float z = -1.0f;
    float angle = 0.0f;

    if (lua_gettop(luaVM) >= 6) {
		z = (float)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
		angle = (float)lua_tonumber(luaVM, 7);
    }
	
	//	TODO: need to support scaleY in quad renderer
	// sprites.get(index)->render(batchRenderers[layer], Vec3(x, y, z), angle, scaleX);

    return 0;
}

/// Sets current color
// @function AB.graphics.setColor
// @param r (1.0) Red color component
// @param g (1.0) Green color component
// @param b (1.0) Blue color component
// @param a (1.0) Alpha component
static int luaSetColor(lua_State* luaVM) {
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;
	
    if (lua_gettop(luaVM) >= 1) {
		r = (float)lua_tonumber(luaVM, 1);
    }
    if (lua_gettop(luaVM) >= 2) {
		g = (float)lua_tonumber(luaVM, 2);
    }
    if (lua_gettop(luaVM) >= 3) {
		b = (float)lua_tonumber(luaVM, 3);
    }
    if (lua_gettop(luaVM) >= 4) {
		a = (float)lua_tonumber(luaVM, 4);
    }
	currentColor = Vec4(r, g, b, a);

	return 0;
}

///	Creates a rendering canvas.
// @function AB.graphics.createCanvas
// @param width Width of canvas
// @param height Height of canvas
// @param index (optional) Handle to canvas
// @return canvas handle
static int luaCreateCanvas(lua_State* luaVM) {
    int width = (int)lua_tonumber(luaVM, 1);
    int height = (int)lua_tonumber(luaVM, 2);

	int index;
    if (lua_gettop(luaVM) >= 3) {
        index = (int)lua_tonumber(luaVM, 3);
    } else {
		index = canvasHandle;
		canvasHandle++;
	}
	canvases[index] = new RenderTarget(width, height);

	//  return handle
	lua_pushnumber(luaVM, index);

	return 1;
}

///	Uses a canvas for rendering. Pass 0 to restore default frameBuffer.
// @function AB.graphics.useCanvas
// @param index
static int luaUseCanvas(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

	if (currentRenderTarget != 0) {
		canvases[currentRenderTarget]->end();
	}
	
	currentRenderTarget = index;
	if (currentRenderTarget != 0) {
		canvases[currentRenderTarget]->begin();
		camera2d.setProjection(0, canvases[currentRenderTarget]->width, canvases[currentRenderTarget]->height, 0);
	} else {
		camera2d.setProjection(0, window.currentMode.xRes, window.currentMode.yRes, 0);
	}
	
	return 0;
}

///	Queues a canvas's texture to be renderer on the screen or another canvas.
// @function AB.graphics.renderCanvas
// @param layer Rendering layer. A default layer of 0 is provided
// @param index Canvas index
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
// @param scaleX (1) Scale X
// @param scaleY (scaleX) Scale Y
static int luaRenderCanvas(lua_State* luaVM) {
    int layer = (int)lua_tonumber(luaVM, 1);
    int index = (int)lua_tonumber(luaVM, 2);
    float x = (float)lua_tonumber(luaVM, 3);
    float y = (float)lua_tonumber(luaVM, 4);

    float z = -1.0f;
    float angle = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
	
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

	RenderTarget *canvas = canvases[index];
	BatchRenderer *renderer = batchRenderers[layer];
	
	BatchRenderer::Quad quad;
	
	quad.pos = Vec3(x, y, z);
	quad.size = Vec2(canvas->width, canvas->height);
	quad.scale = scaleX;
	quad.rotation = angle;
	quad.uv = Vec4(0, 0, 1, 1);
	quad.textureID = canvas->texture;
	quad.color = currentColor;

	renderer->renderQuad(quad);
	
	return 0;
}

/// Creates a new rendering layer. (BatchRenderer internally). Layers are renderered back to front, largest indices first.
// A default layer of 0 is provided
// @function AB.graphics.createLayer
// @param index Layer index
// @param depthSorting (false) Whether this layer needs to support depth sorting
static int luaCreateLayer(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    bool depthSorting = false;
	if (lua_gettop(luaVM) >= 2) {
		depthSorting = (float)lua_toboolean(luaVM, 2);
    }
	batchRenderers[index] = new BatchRenderer(nullptr, blend::identity(), depthSorting);

	return 0;
}

///	Removes a rendering layer
// @function AB.graphics.removeLayer
// @param index Layer index
static int luaRemoveLayer(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

	auto iterator = batchRenderers.find(index);
	batchRenderers.erase(iterator);

	return 0;
}


/// Adds a color transform for layer. Chained with any previous color transforms
// @function AB.graphics.addColorTransform
// @param index Layer index
// @param transform Color transform
// @param r Red (will also be interpreted as theta or value)
// @param g Green
// @param b Blue
// @param a Alpha
// @usage AB.graphics.addColorTransform(0, AB.graphics.colorTransforms.MULTIPLY, 0.75, 0.25, 0.5)
// @usage AB.graphics.addColorTransform(0, AB.graphics.colorTransforms.SATURATE, 0.5)
// @see colorTransforms
static int luaAddColorTransform(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);

	// TODO
	
	return 0;
}

/// Resets any color transformation for layer.
// @function AB.graphics.resetColorTransforms
// @param index Layer index
static int luaResetColorTransforms(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
	
	//	TODO
	
	return 0;
}

/// Performs any pending rendering operations
// @function AB.graphics.flushGraphics
static int luaFlushGraphics(lua_State* luaVM) {
	renderer.renderBatches(camera2d);
	
	return 0;
}

void registerGraphicsFunctions() {
    static const luaL_Reg graphicsFuncs[] = {
        { "resetVideo", luaResetVideo},

        { "loadSprite", luaLoadSprite},
        { "addToAtlas", luaAddToAtlas},
        { "buildAtlas", luaBuildAtlas},

        { "renderSprite", luaRenderSprite},
        { "spriteSize", luaSpriteSize},
		{ "renderQuad", luaRenderQuad},

		{ "setColor", luaSetColor},
		{ "createCanvas", luaCreateCanvas},
		{ "useCanvas", luaUseCanvas},
		{ "renderCanvas", luaRenderCanvas}, 

		{ "addColorTransform", luaAddColorTransform},
		{ "resetColorTransforms", luaResetColorTransforms},

		// { "loadShader", luaLoadShader},
		{ "createLayer", luaCreateLayer},
		{ "removeLayer", luaRemoveLayer},
		
		{ "flushGraphics", luaFlushGraphics},
		
        { NULL, NULL }
    };
    script.registerFuncs("AB", "graphics", graphicsFuncs);
}

}
