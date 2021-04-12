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
#include "../renderer/spriteAtalas.h"

namespace AB {

extern Script script;

//----------------------------------------------------------------- Graphics state --------------------------------

#if 0

static int luaResetVideo(lua_State* luaVM) {
    window.setVideoMode(NULL);

    return 0;
}

static int luaResizeCanvas(lua_State* luaVM) {
    int width = (int)lua_tonumber(luaVM, 1);
    int height = (int)lua_tonumber(luaVM, 2);

    graphics->resizeFBO(width, height);

    return 0;
}

static int luaSetDefaultShader(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    graphics->setDefaultShader(index);

    return 0;
}

static int luaSetCanvasShader(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    graphics->setPostProcessShader(index);

    return 0;
}

static int luaInvalidateTextureCache(lua_State* luaVM) {
    graphics->invalidateTextureCache();

    return 0;
}

static int luaSetColor(lua_State* luaVM) {
    float r = (float)lua_tonumber(luaVM, 1) / 255.0f;
    float g = (float)lua_tonumber(luaVM, 2) / 255.0f;
    float b = (float)lua_tonumber(luaVM, 3) / 255.0f;

    float a = 1.0f;
    if (lua_gettop(luaVM) >= 4) {
        a = (float)lua_tonumber(luaVM, 4) / 255.0f;
    }

    graphics->setColor(r, g, b, a);

    return 0;
}

static int luaSetLineWidth(lua_State* luaVM) {
    float width = (float)lua_tonumber(luaVM, 1);
    CALL_GL(glLineWidth(width));
    graphics->setLineWidth(width);

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

    graphics->sprites.mapResource(index, filename);
    if (createMask) {
        graphics->sprites.get(index)->buildCollisionMask();
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

    bool batch = false;
    if (lua_gettop(luaVM) >= 8) {
        batch = (bool)lua_toboolean(luaVM, 8);
    }

    graphics->spriteBatchAlpha->add(graphics->sprites.get(frame), x, y, z, angle, scaleX, scaleY);
    if (!batch) {
        graphics->spriteBatchAlpha->render();
        graphics->spriteBatchAlpha->clear();
    }

    return 0;
}

// renderSpriteUV(index, x, y, z, w, h, u1, v1, u2, v2, r = 0, sx = 1, sy = 1, additive = false)
static int luaRenderSpriteUV(lua_State* luaVM) {
    int frame = (int)lua_tonumber(luaVM, 1);
    float x = (float)lua_tonumber(luaVM, 2);
    float y = (float)lua_tonumber(luaVM, 3);
    float z = (float)lua_tonumber(luaVM, 4);

    float w = (float)lua_tonumber(luaVM, 5);
    float h = (float)lua_tonumber(luaVM, 6);
    float u1 = (float)lua_tonumber(luaVM, 7);
    float v1 = (float)lua_tonumber(luaVM, 8);
    float u2 = (float)lua_tonumber(luaVM, 9);
    float v2 = (float)lua_tonumber(luaVM, 10);

    float angle = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (lua_gettop(luaVM) >= 11) {
		angle = (float)lua_tonumber(luaVM, 11);
    }
    if (lua_gettop(luaVM) >= 12) {
		scaleX = (float)lua_tonumber(luaVM, 12);
    }
    if (lua_gettop(luaVM) >= 13) {
		scaleY = (float)lua_tonumber(luaVM, 13);
    } else {
        scaleY = scaleX;
    }

    Sprite *sprite = graphics->sprites.get(frame);
    if (sprite->texture.get() == 0) {
        sprite->uploadToGPU();
    }
    GLuint tex = sprite->texture.get()->glHandle;

    graphics->spriteBatchAlpha->add(tex, x, y, z, w, h, u1, v1, u2, v2, angle, scaleX, scaleY);

    return 0;
}

static int luaAddToAtlas(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    addToAtlas(graphics->sprites.get(index));

    return 0;
}

static int luaBuildAtlas(lua_State* luaVM) {
    buildAtlas();

    return 0;
}

static int luaSpriteWidth(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    int width = graphics->sprites.get(index)->width;

    lua_pushinteger(luaVM, width);

    return 1;
}

static int luaSpriteHeight(lua_State* luaVM) {
    int index = (int)lua_tonumber(luaVM, 1);
    int height = graphics->sprites.get(index)->height;

    lua_pushinteger(luaVM, height);

    return 1;
}

static int luaBeginBatch(lua_State* luaVM) {
    //  TODO: maybe pass primitive type from lua?
    graphics->preRenderVBO();

    return 0;
}

static int luaEndBatch(lua_State* luaVM) {
    graphics->postRenderVBO(GL_TRIANGLES);

    return 0;
}

static int luaRenderTri(lua_State* luaVM) {
    float x1 = (float)lua_tonumber(luaVM, 1);
    float y1 = (float)lua_tonumber(luaVM, 2);
    float x2 = (float)lua_tonumber(luaVM, 3);
    float y2 = (float)lua_tonumber(luaVM, 4);
    float x3 = (float)lua_tonumber(luaVM, 5);
    float y3 = (float)lua_tonumber(luaVM, 6);

    graphics->renderTri(x1, y1, x2, y2, x3, y3);

    return 0;
}

static int luaRenderQuad(lua_State* luaVM) {
    float x = (float)lua_tonumber(luaVM, 1);
    float y = (float)lua_tonumber(luaVM, 2);
    float w = (float)lua_tonumber(luaVM, 3);
    float h = (float)lua_tonumber(luaVM, 4);

    if (h < 0.001f || w < 0.001f) {
        return 0;
    }

    bool full = true;
    if (lua_gettop(luaVM) >= 5) {
        full = (bool)lua_toboolean(luaVM, 5);
    }

    bool batch = false;
    if (lua_gettop(luaVM) >= 6) {
        batch = (bool)lua_toboolean(luaVM, 6);
    }

    graphics->renderQuad(x, y, x + w, y + h, full, batch);


    return 0;
}

static int luaRenderCircle(lua_State* luaVM) {
    float x = (float)lua_tonumber(luaVM, 1);
    float y = (float)lua_tonumber(luaVM, 2);
    float radius = (float)lua_tonumber(luaVM, 3);

    if (lua_gettop(luaVM) >= 4) {
        int segments = (int)lua_tonumber(luaVM, 4);
        graphics->renderCircle(x, y, radius, segments);
    } else {
        graphics->renderCircle(x, y, radius);
    }

    return 0;
}

static int luaRenderArc(lua_State* luaVM) {
    float x = (float)lua_tonumber(luaVM, 1);
    float y = (float)lua_tonumber(luaVM, 2);
    float radius = (float)lua_tonumber(luaVM, 3);
    float angle1 = (float)lua_tonumber(luaVM, 4);
    float angle2 = (float)lua_tonumber(luaVM, 5);

    graphics->renderArc(x, y, radius, angle1, angle2);

    return 0;
}

static int luaSetScissor(lua_State* luaVM) {
    if (lua_gettop(luaVM) >= 4) {
        int x = (int)lua_tonumber(luaVM, 1);
        int y = (int)lua_tonumber(luaVM, 2);
        int w = (int)lua_tonumber(luaVM, 3);
        int h = (int)lua_tonumber(luaVM, 4);

        graphics->setScissor(x, y, w, h);
    } else {
        graphics->setScissor();
    }

    return 0;
}

static int luaFlushGraphics(lua_State* luaVM) {
    graphics->spriteBatchAlpha->render();
    graphics->spriteBatchAlpha->clear();

    return 0;
}

#endif

void registerGraphicsFunctions() {
	/*
    static const luaL_Reg graphicsFuncs[] = {
        { "resetVideo", luaResetVideo},
        { "resizeCanvas", luaResizeCanvas},
        { "setDefaultShader", luaSetDefaultShader},
        { "setCanvasShader", luaSetCanvasShader},
        { "invalidateTextureCache", luaInvalidateTextureCache},

        { "loadSprite", luaLoadSprite},
        { "addToAtlas", luaAddToAtlas},
        { "buildAtlas", luaBuildAtlas},

        // renderSprite(index, x, y, z = 1, r = 0, sx = 1, sy = 1, additive = false)
        { "renderSprite", luaRenderSprite},
        { "renderSpriteUV", luaRenderSpriteUV},
        { "spriteWidth", luaSpriteWidth},
        { "spriteHeight", luaSpriteHeight},
        { "collides", luaCollides},

        { "setColor", luaSetColor},
        { "setLineWidth", luaSetLineWidth},

        { "beginBatch", luaBeginBatch},
        { "endBatch", luaEndBatch},
        { "renderTri", luaRenderTri},
        { "renderQuad", luaRenderQuad},
        { "renderCircle", luaRenderCircle},
        { "renderArc", luaRenderArc},
        { "setScissor", luaSetScissor},
        { "flushGraphics", luaFlushGraphics},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "graphics", graphicsFuncs);
	*/
}

}
