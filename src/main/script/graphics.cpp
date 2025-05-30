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
#include "../renderer/renderer.h"

namespace AB {

extern Script script;
extern Window window;

extern AssetManager<Sprite> sprites;
extern AssetManager<Shader> shaders;

extern Renderer renderer;
extern Window window;
extern OrthographicCamera camera2d;

static Vec4 currentColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
static u32 spriteHandle = 1;
static u32 canvasHandle = 1;
static u32 currentRenderTarget = 0;

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
// @field SWAP_R_G ()
// @field SWAP_R_B ()
// @field SWAP_G_B ()
// @table colorTransforms

/// Sets the video mode. Only necessary upon changes to video mode. Reads a global table: videoConfig = { title, xRes, yRes, fullscreen, vsync }
// @function AB.graphics.resetVideo
static i32 luaResetVideo(lua_State* luaVM) {
    window.setVideoMode(NULL);

    return 0;
}

/// Gets desktop resolution
// @function AB.graphics.getDesktopResolution
// @return xRes X resolution
// @return yRes Y resolution
static i32 luaGetDesktopResolution(lua_State* luaVM) {
    Vec2 res = window.getDesktopResolution();
    
    lua_pushnumber(luaVM, res.x);
    lua_pushnumber(luaVM, res.y);

    return 2;
}

/// Clears the screen or current canvas
// @function AB.graphics.clear
// @param r (0.0) Red color component
// @param g (0.0) Green color component
// @param b (0.0) Blue color component
// @param a (1.0) Alpha component
static i32 luaClear(lua_State* luaVM) {
    f32 r = 0.0f;
    f32 g = 0.0f;
    f32 b = 0.0f;
    f32 a = 1.0f;
    
    if (lua_gettop(luaVM) >= 1) {
        r = (f32)lua_tonumber(luaVM, 1);
    }
    if (lua_gettop(luaVM) >= 2) {
        g = (f32)lua_tonumber(luaVM, 2);
    }
    if (lua_gettop(luaVM) >= 3) {
        b = (f32)lua_tonumber(luaVM, 3);
    }
    if (lua_gettop(luaVM) >= 4) {
        a = (f32)lua_tonumber(luaVM, 4);
    }
    renderer.clear(r, g, b, a);
    
    return 0;
}

///    Loads a sprite
// @function AB.graphics.loadSprite
// @param filename Filename
// @param collisionMask (true) If a collision mask should be created
// @param index (optional) Sprite index
// @return sprite handle
static i32 luaLoadSprite(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));

    b8 createMask = true;
    if (lua_gettop(luaVM) >= 2) {
        createMask = (b8)lua_toboolean(luaVM, 2);
    }

    i32 index;
    if (lua_gettop(luaVM) >= 3) {
        index = (i32)lua_tonumber(luaVM, 3);
    } else {
        index = spriteHandle;
        spriteHandle++;
    }

    sprites.mapAsset(index, filename);
    if (createMask) {
        sprites.get(index)->buildCollisionMask();
    }

    //  return handle
    lua_pushnumber(luaVM, index);

    return 1;
}

/// Loads a series of sprites from a spritesheet
// @function AB.graphics.loadAtlas
// @param filename Filename
// @param width Width of each sprite
// @param height Height of each sprite
// @param collisionMask (true) If a collision mask should be created
// @param index (optional) First sprite index
// @return first sprite handle
// @return number of sprites loaded
static i32 luaLoadAtlas(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));
    i32 width = (i32)lua_tonumber(luaVM, 2);
    i32 height = (i32)lua_tonumber(luaVM, 3);
    
    b8 createMask = true;
    if (lua_gettop(luaVM) >= 4) {
        createMask = (b8)lua_toboolean(luaVM, 4);
    }

    i32 index;
    i32 spritesLoaded;
    if (lua_gettop(luaVM) >= 5) {
        index = (i32)lua_tonumber(luaVM, 5);
        spritesLoaded = loadAtlas(filename, index, width, height, createMask);
    } else {
        index = spriteHandle;
        spritesLoaded = loadAtlas(filename, index, width, height, createMask);
        spriteHandle += spritesLoaded;
    }

    //  return handle and number of sprites loaded
    lua_pushnumber(luaVM, index);
    lua_pushnumber(luaVM, spritesLoaded);

    return 2;
}

/// Adds a sprite into the current atlas queue. This should be done at startup before any rendering occurs. Sprites that are commonly
// used together can be batched into a single atlas to improve performance by minimizing texture switches.
// @param index Sprite index
// @function AB.graphics.addToAtlas
static i32 luaAddToAtlas(lua_State* luaVM) {
    i32 index = (i32)lua_tonumber(luaVM, 1);
    addToAtlas(sprites.get(index));

    return 0;
}

/// Builds a sprite atlas. Call this after several calls to AB.graphics.addToAtlas
// @function AB.graphics.buildAtlas
static i32 luaBuildAtlas(lua_State* luaVM) {
    buildAtlas();

    return 0;
}

/// Defines a sprite from an atlas texture
// @function AB.graphics.defineSpriteFromAtlas
// @param atlasIndex Sprite atlas index
// @param x X coordinate of sprite
// @param y Y coordinate of sprite
// @param width Sprite width
// @param height Sprite height
// @param index (optional) Sprite index
// @param collisionMask (true) If a collision mask should be created
// @return sprite handle
static i32 luaDefineSpriteFromAtlas(lua_State* luaVM) {
    i32 atlasIndex = (i32)lua_tonumber(luaVM, 1);

    i32 x = (i32)lua_tonumber(luaVM, 2);
    i32 y = (i32)lua_tonumber(luaVM, 3);
    i32 width = (i32)lua_tonumber(luaVM, 4);
    i32 height = (i32)lua_tonumber(luaVM, 5);

    i32 index;
    if (lua_gettop(luaVM) >= 6) {
        index = (i32)lua_tonumber(luaVM, 6);
    } else {
        index = spriteHandle;
        spriteHandle++;
    }

    b8 createMask = true;
    if (lua_gettop(luaVM) >= 7) {
        createMask = (b8)lua_toboolean(luaVM, 4);
    }

    Sprite *atlas = sprites.get(atlasIndex);
    f32 u1 = x / (f32)(atlas->width);
    f32 v1 = y / (f32)(atlas->height);
    f32 u2 = (x + width) / (f32)(atlas->width);
    f32 v2 = (y + height)  / (f32)(atlas->height);

    sprites.mapAsset(index, ".");
    defineSpriteFromAtlas(atlasIndex, u1, v1, u2, v2, index, createMask);

    lua_pushnumber(luaVM, index);

    return 1;
}

///    Queues a sprite to be renderer on the screen or current canvas.
// @function AB.graphics.renderSprite
// @param layer Rendering layer. A default layer of 0 is provided
// @param index Sprite index
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
// @param scaleX (1) Scale X
// @param scaleY (scaleX) Scale Y
static i32 luaRenderSprite(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);
    i32 index = (i32)lua_tonumber(luaVM, 2);
    f32 x = (f32)lua_tonumber(luaVM, 3);
    f32 y = (f32)lua_tonumber(luaVM, 4);

    x = floor(x + 0.5f);
    y = floor(y + 0.5f);

    f32 z = -1.0f;
    f32 angle = 0.0f;
    f32 scaleX = 1.0f;
    f32 scaleY = 1.0f;
    
    if (lua_gettop(luaVM) >= 5) {
        z = (f32)lua_tonumber(luaVM, 5);
    }
    if (lua_gettop(luaVM) >= 6) {
        angle = (f32)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
        scaleX = (f32)lua_tonumber(luaVM, 7);
    }
    if (lua_gettop(luaVM) >= 8) {
        scaleY = (f32)lua_tonumber(luaVM, 8);
    } else {
        scaleY = scaleX;
    }

    RenderLayer *batchRenderer = reinterpret_cast<RenderLayer*>(renderer.layers[layer]);
    sprites.get(index)->render(batchRenderer, Vec3(x, y, z), angle, Vec2(scaleX, scaleY), currentColor);

    return 0;
}

/// Returns size of a sprite
// @function AB.graphics.spriteSize
// @param index Sprite index
// @return width
// @return height
// @usage width, height = AB.graphics.spriteSize(1)
static i32 luaSpriteSize(lua_State* luaVM) {
    i32 index = (i32)lua_tonumber(luaVM, 1);
    i32 width = sprites.get(index)->width;
    i32 height = sprites.get(index)->height;

    lua_pushinteger(luaVM, width);
    lua_pushinteger(luaVM, height);

    return 2;
}

///    Queues a quad to be rendered on the screen or current canvas.
// @function AB.graphics.renderQuad
// @param layer Rendering layer. A default layer of 0 is provided
// @param width Width
// @param height Height
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
static i32 luaRenderQuad(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);
    f32 width = (f32)lua_tonumber(luaVM, 2);
    f32 height = (f32)lua_tonumber(luaVM, 3);
    f32 x = (f32)lua_tonumber(luaVM, 4);
    f32 y = (f32)lua_tonumber(luaVM, 5);

    f32 z = -1.0f;
    f32 angle = 0.0f;

    if (lua_gettop(luaVM) >= 6) {
        z = (f32)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
        angle = (f32)lua_tonumber(luaVM, 7);
    }
    
    RenderLayer::Quad quad;
    
    quad.pos = Vec3(x, y, z);
    quad.size = Vec2(width, height);
    quad.scale = Vec2(1.0f, 1.0f);
    quad.rotation = angle;
    quad.uv = Vec4(0, 0, 1, 1);
    quad.textureID = 0;
    quad.color = currentColor;

    renderer.layers[layer]->renderQuad(quad);

    return 0;
}

/// Queues a triangle to be rendered on the screen or current canvas.
// @function AB.graphics.renderTri
// @param layer Rendering layer. A default layer of 0 is provided
// @param x1 X1 position
// @param y1 Y1 position
// @param x2 X2 position
// @param y2 Y2 position
// @param x3 X3 position
// @param y3 Y3 position
static i32 luaRenderTri(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);
    f32 x1 = (f32)lua_tonumber(luaVM, 2);
    f32 y1 = (f32)lua_tonumber(luaVM, 3);
    f32 x2 = (f32)lua_tonumber(luaVM, 4);
    f32 y2 = (f32)lua_tonumber(luaVM, 5);
    f32 x3 = (f32)lua_tonumber(luaVM, 6);
    f32 y3 = (f32)lua_tonumber(luaVM, 7);

    renderer.layers[layer]->setColor(currentColor);     // TODO: move this to renderer.state
    renderer.layers[layer]->renderTri(x1, y1, x2, y2, x3, y3);
    
    return 0;
}


///    Queues an arc to be rendered on the screen or current canvas.
// @function AB.graphics.renderArc
// @param layer Rendering layer. A default layer of 0 is provided
// @param x X position
// @param y Y position
// @param radius Radius
// @param angle1 Start angle in degrees
// @param angle2 End angle in degrees
// @param segments (8) Number of arc segments
static i32 luaRenderArc(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);
    f32 x = (f32)lua_tonumber(luaVM, 2);
    f32 y = (f32)lua_tonumber(luaVM, 3);
    f32 radius = (f32)lua_tonumber(luaVM, 4);
    f32 angle1 = (f32)lua_tonumber(luaVM, 5);
    f32 angle2 = (f32)lua_tonumber(luaVM, 6);
    
    i32 segments = 8;
    if (lua_gettop(luaVM) >= 7) {
        segments = (i32)lua_tonumber(luaVM, 7);
    }

    renderer.layers[layer]->setColor(currentColor);     // TODO: move this to renderer.state
    renderer.layers[layer]->renderArc(x, y, radius, angle1, angle2, segments);
    
    return 0;
}


///    Queues a rounded rectangle to be rendered on the screen or current canvas.
// @function AB.graphics.renderRoundedRectangle
// @param layer Rendering layer. A default layer of 0 is provided
// @param x X position
// @param y Y position
// @param width Width
// @param height Height
// @param radius Corner radius
// @param full (true) Filled in or outline
// @param segments (8) Number of corner segments
static i32 luaRenderRoundedRectangle(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);
    f32 x = (f32)lua_tonumber(luaVM, 2);
    f32 y = (f32)lua_tonumber(luaVM, 3);
    f32 width = (f32)lua_tonumber(luaVM, 4);
    f32 height = (f32)lua_tonumber(luaVM, 5);
    f32 radius = (f32)lua_tonumber(luaVM, 6);

    b8 full = true;
    if (lua_gettop(luaVM) >= 7) {
        full = (b8)lua_toboolean(luaVM, 7);
    }

    i32 segments = 8;
    if (lua_gettop(luaVM) >= 8) {
        segments = (i32)lua_tonumber(luaVM, 8);
    }

    renderer.layers[layer]->setColor(currentColor);     // TODO: move this to renderer.state
    renderer.layers[layer]->renderRoundedRectangle(x, y, width, height, radius, full, segments);

    return 0;
}

/// Renders a list of line segments
// @function AB.graphics.renderLines
// @param layer Rendering layer. A default layer of 0 is provided
// @param lines[] array of line segment endpoints in form {x1, y1, x2, y2, ...}
static i32 luaRenderLines(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);

    if (!lua_istable(luaVM, -1)) {
        LOG("Not a table!", 0);
    }
    
    //  make sure array is %4
    size_t length = lua_rawlen(luaVM, -1);
    if (length % 4 != 0) {
        LOG("Vertex size is not multiple of four!", 0);
    }

    // https://dav3.co/blog/looping-through-lua-table-in-c/
    f32 endpoints[length];
    for (u32 i = 0; i <= length; i++) {
        lua_pushinteger(luaVM, i + 1);
        
        lua_gettable(luaVM, -2);
        if (lua_type(luaVM, -1) == LUA_TNIL) {
            break;
        }
        endpoints[i] = luaL_checknumber(luaVM, -1);
        lua_pop(luaVM, 1);
    }
    
    renderer.layers[layer]->setColor(currentColor);     // TODO: move this to renderer.state
    renderer.layers[layer]->renderLines(endpoints, length / 4);

    return 0;
}

/// Sets current (premultiplied) color
// @function AB.graphics.setColor
// @param r (1.0) Red color component
// @param g (1.0) Green color component
// @param b (1.0) Blue color component
// @param opacity (1.0) Opacity
// @param additivity (0.0) Additivity
static i32 luaSetColor(lua_State* luaVM) {
    f32 r = 1.0f;
    f32 g = 1.0f;
    f32 b = 1.0f;
    f32 opacity = 1.0f;
    f32 additivity = 0.0f;
    
    if (lua_gettop(luaVM) >= 1) {
        r = (f32)lua_tonumber(luaVM, 1);
    }
    if (lua_gettop(luaVM) >= 2) {
        g = (f32)lua_tonumber(luaVM, 2);
    }
    if (lua_gettop(luaVM) >= 3) {
        b = (f32)lua_tonumber(luaVM, 3);
    }
    if (lua_gettop(luaVM) >= 4) {
        opacity = (f32)lua_tonumber(luaVM, 4);
    }
    if (lua_gettop(luaVM) >= 5) {
        additivity = (f32)lua_tonumber(luaVM, 5);
    }
    
    currentColor = Vec4(r * opacity, g * opacity, b * opacity, 1.0f - additivity);

    return 0;
}

///    Creates a rendering canvas.
// @function AB.graphics.createCanvas
// @param width Width of canvas
// @param height Height of canvas
// @param index (optional) Handle to canvas
// @return canvas handle
static i32 luaCreateCanvas(lua_State* luaVM) {
    i32 width = (i32)lua_tonumber(luaVM, 1);
    i32 height = (i32)lua_tonumber(luaVM, 2);

    i32 index;
    if (lua_gettop(luaVM) >= 3) {
        index = (i32)lua_tonumber(luaVM, 3);
    } else {
        index = canvasHandle;
        canvasHandle++;
    }
    renderer.canvases[index] = new RenderTarget(width, height);

    //  return handle
    lua_pushnumber(luaVM, index);

    return 1;
}

///    Deletes a canvas.
// @function AB.graphics.deleteCanvas
// @param index Handle to canvas
static i32 luaDeleteCanvas(lua_State* luaVM) {
    u32 index = (u32)lua_tonumber(luaVM, 1);

    std::map<u32, AB::RenderTarget*>::iterator it;
    it = renderer.canvases.find(index);
    delete renderer.canvases[index];
    renderer.canvases.erase(it);

    return 0;
}

///    Uses a canvas for rendering. Pass 0 to restore default frameBuffer.
// @function AB.graphics.useCanvas
// @param index
static i32 luaUseCanvas(lua_State* luaVM) {
    u32 index = (u32)lua_tonumber(luaVM, 1);

    renderer.render(camera2d);
    
    if (currentRenderTarget != 0) {
        renderer.canvases[currentRenderTarget]->end();
    }
    
    currentRenderTarget = index;
    if (currentRenderTarget != 0) {
        if (renderer.canvases.find(currentRenderTarget) == renderer.canvases.end()) {
            ERR("Render target not created: %d", currentRenderTarget);
        }
        renderer.canvases[currentRenderTarget]->begin();

        //    TODO: not sure about setting the projection this way.. (y coords are flipped)
        camera2d.setProjection(0, renderer.canvases[currentRenderTarget]->width, 0, renderer.canvases[currentRenderTarget]->height);
    } else {
        camera2d.setProjection(0, window.currentMode.xRes, window.currentMode.yRes, 0);
    }
    
    return 0;
}

///    Queues a canvas's texture to be renderer on the screen or another canvas.
// @function AB.graphics.renderCanvas
// @param layer Rendering layer. A default layer of 0 is provided
// @param index Canvas index
// @param x X position
// @param y Y position
// @param z (-1) Z position
// @param angle (0) Rotation
// @param scaleX (1) Scale X
// @param scaleY (scaleX) Scale Y
static i32 luaRenderCanvas(lua_State* luaVM) {
    u32 layer = (u32)lua_tonumber(luaVM, 1);
    u32 index = (u32)lua_tonumber(luaVM, 2);
    f32 x = (f32)lua_tonumber(luaVM, 3);
    f32 y = (f32)lua_tonumber(luaVM, 4);

    f32 z = -1.0f;
    f32 angle = 0.0f;
    f32 scaleX = 1.0f;
    f32 scaleY = 1.0f;
    
    if (lua_gettop(luaVM) >= 5) {
        z = (f32)lua_tonumber(luaVM, 5);
    }
    if (lua_gettop(luaVM) >= 6) {
        angle = (f32)lua_tonumber(luaVM, 6);
    }
    if (lua_gettop(luaVM) >= 7) {
        scaleX = (f32)lua_tonumber(luaVM, 7);
    }
    if (lua_gettop(luaVM) >= 8) {
        scaleY = (f32)lua_tonumber(luaVM, 8);
    } else {
        scaleY = scaleX;
    }

    RenderTarget *canvas = renderer.canvases[index];
    RenderLayer *batchRenderer = reinterpret_cast<RenderLayer*>(renderer.layers[layer]);
    
    RenderLayer::Quad quad;
    
    quad.pos = Vec3(x, y, z);
    quad.size = Vec2(canvas->width, canvas->height);
    quad.scale = Vec2(scaleX, scaleY);
    quad.rotation = angle;
    quad.uv = Vec4(0, 0, 1, 1);
    quad.textureID = canvas->texture;
    quad.color = currentColor;

    batchRenderer->renderQuad(quad);
    
    return 0;
}

/// Creates a new rendering layer. (RenderLayer internally). Layers are renderered back to front, largest indices first.
// A default layer of 0 is provided (with depth sorting)
// @function AB.graphics.createLayer
// @param index Layer index
// @param depthSorting (false) Whether this layer needs to support depth sorting
// @return layer index
static i32 luaCreateLayer(lua_State* luaVM) {
    u32 index = (u32)lua_tonumber(luaVM, 1);
    b8 depthSorting = false;
    if (lua_gettop(luaVM) >= 2) {
        depthSorting = (b8)lua_toboolean(luaVM, 2);
    }
    renderer.layers[index] = new RenderLayer(nullptr, nullptr, blend::identity(), depthSorting);

    lua_pushnumber(luaVM, index);

    return 1;
}

///    Removes a rendering layer
// @function AB.graphics.removeLayer
// @param index Layer index
static i32 luaRemoveLayer(lua_State* luaVM) {
    u32 index = (u32)lua_tonumber(luaVM, 1);

    auto iterator = renderer.layers.find(index);
    renderer.layers.erase(iterator);

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
static i32 luaAddColorTransform(lua_State* luaVM) {
    u32 index = (u32)lua_tonumber(luaVM, 1);
    i32 colorTransform = (i32)lua_tonumber(luaVM, 2);
    
    f32 r = 1.0f;
    f32 g = 1.0f;
    f32 b = 1.0f;
    f32 a = 1.0f;
    
    if (lua_gettop(luaVM) >= 3) {
        r = (f32)lua_tonumber(luaVM, 3);
    }
    if (lua_gettop(luaVM) >= 4) {
        g = (f32)lua_tonumber(luaVM, 4);
    }
    if (lua_gettop(luaVM) >= 5) {
        b = (f32)lua_tonumber(luaVM, 5);
    }
    if (lua_gettop(luaVM) >= 6) {
        a = (f32)lua_tonumber(luaVM, 6);
    }
    
    f32 theta = r;
    f32 value = r;

    Mat4 transform;
    switch (colorTransform) {
        case 0: transform = blend::identity(); break;
        case 1: transform = blend::invert(); break;
        case 2: transform = blend::multiply(r, g, b); break;
        case 3: transform = blend::screen(r, g, b); break;
        case 4: transform = blend::colorFill(r, g, b, a); break;
        case 5: transform = blend::linearDodge(r, g, b); break;
        case 6: transform = blend::linearBurn(r, g, b); break;
        case 7: transform = blend::colorDodge(r, g, b); break;
        case 8: transform = blend::colorBurn(r, g, b); break;
        case 9: transform = blend::tintedMonochrome(r, g, b, a); break;
        case 10: transform = blend::hueShift(theta); break;
        case 11: transform = blend::saturate(value); break;
        case 12: transform = blend::brightness(value); break;
        case 13: transform = blend::darkness(value); break;
        case 14: transform = blend::contrast(value); break;
        case 15: transform = blend::swapRG(); break;
        case 16: transform = blend::swapRB(); break;
        case 17: transform = blend::swapGB(); break;
        
        default: break;
    }

    renderer.layers[index]->colorTransform = renderer.layers[index]->colorTransform * transform;
    
    return 0;
}

/// Sets line width per layer.
// @function AB.graphics.setLineWidth
// @param (0) layer Layer. A default layer of 0 is provided
// @param width (1) Line width
static i32 luaSetLineWidth(lua_State* luaVM) {
    u32 layerIndex = 0;
    i32 width = 1;

    if (lua_gettop(luaVM) >= 1) {
        layerIndex = (i32)lua_tonumber(luaVM, 1);
    }
    if (lua_gettop(luaVM) >= 2) {
        width = (i32)lua_tonumber(luaVM, 2);
    }

    RenderLayer *lineRenderer = reinterpret_cast<RenderLayer*>(renderer.layers[layerIndex]);
    lineRenderer->setLineWidth(width);

    return 0;
}


/// Resets any color transformation for layer.
// @function AB.graphics.resetColorTransforms
// @param index Layer index
static i32 luaResetColorTransforms(lua_State* luaVM) {
    u32 index = (u32)lua_tonumber(luaVM, 1);
    
    RenderLayer *batchRenderer = reinterpret_cast<RenderLayer*>(renderer.layers[index]);
    batchRenderer->colorTransform = blend::identity();
    
    return 0;
}

/// Loads a shader program. ".vert" will be appended to the filename for the vertex shader,
// and ".frag" will be appended for the fragment shader.
// @function AB.graphics.loadShader
// @param filename Shader filename
// @param index Shader index
static i32 luaLoadShader(lua_State* luaVM) {
    std::string filename = std::string(lua_tostring(luaVM, 1));
    u32 index = (u32)lua_tonumber(luaVM, 2);

    AB::shaders.mapAsset(index, filename);
    
    return 0;
}

/// Sets a layer's shader
// @function AB.graphics.setShader
// @param layerIndex Layer index
// @param shaderIndex Shader index
static i32 luaSetShader(lua_State* luaVM) {
    i32 layerIndex = (i32)lua_tonumber(luaVM, 1);
    i32 shaderIndex = (i32)lua_tonumber(luaVM, 2);

    renderer.layers[layerIndex]->shader = shaders.get(shaderIndex);
    
    return 0;
}

/// Sets a layer's batch shader
// @function AB.graphics.setBatchShader
// @param layerIndex Layer index
// @param shaderIndex Shader index
static i32 luaSetBatchShader(lua_State* luaVM) {
    i32 layerIndex = (i32)lua_tonumber(luaVM, 1);
    i32 shaderIndex = (i32)lua_tonumber(luaVM, 2);

    renderer.layers[layerIndex]->batchShader = shaders.get(shaderIndex);
    
    return 0;
}

/// Performs any pending rendering operations
// @function AB.graphics.flushGraphics
static i32 luaFlushGraphics(lua_State* luaVM) {
    renderer.render(camera2d);
    
    return 0;
}

void registerGraphicsFunctions() {
    static const luaL_Reg graphicsFuncs[] = {
        { "resetVideo", luaResetVideo},
        { "getDesktopResolution", luaGetDesktopResolution},
        { "clear", luaClear},
            
        { "loadSprite", luaLoadSprite},
        { "loadAtlas", luaLoadAtlas},
        { "addToAtlas", luaAddToAtlas},
        { "buildAtlas", luaBuildAtlas},
        { "defineSpriteFromAtlas", luaDefineSpriteFromAtlas},

        { "renderSprite", luaRenderSprite},
        { "spriteSize", luaSpriteSize},
        { "renderQuad", luaRenderQuad},
        { "renderTri", luaRenderTri},
        { "renderArc", luaRenderArc},
        { "renderLines", luaRenderLines},
        { "renderRoundedRectangle", luaRenderRoundedRectangle},

        { "setLineWidth", luaSetLineWidth},
        { "setColor", luaSetColor},
        { "createCanvas", luaCreateCanvas},
        { "deleteCanvas", luaDeleteCanvas},
        { "useCanvas", luaUseCanvas},
        { "renderCanvas", luaRenderCanvas}, 

        { "createLayer", luaCreateLayer},
        { "removeLayer", luaRemoveLayer},
        
        { "addColorTransform", luaAddColorTransform},
        { "resetColorTransforms", luaResetColorTransforms},

        { "loadShader", luaLoadShader},
        { "setShader", luaSetShader},
        { "setBatchShader", luaSetBatchShader},
        
        { "flushGraphics", luaFlushGraphics},
        
        { NULL, NULL }
    };
    script.registerFuncs("AB", "graphics", graphicsFuncs);

    script.execute("AB.graphics.colorTransforms = {"
        "NONE = 0,"
        "INVERT = 1,"
        "MULTIPLY = 2,"
        "SCREEN = 3,"
        "COLOR_FILL = 4,"
        "LINEAR_DODGE = 5,"
        "LINEAR_BURN = 6,"
        "COLOR_DODGE = 7,"
        "COLOR_BURN = 8,"
        "TINTED_MONOCHROME = 9,"
        "HUE_SHIFT = 10,"
        "SATURATE = 11,"
        "BRIGHTNESS = 12,"
        "DARKNESS = 13,"
        "CONTRAST = 14,"
        "SWAP_R_G = 15,"
        "SWAP_R_B = 16,"
        "SWAP_G_B = 17}");
}

}
