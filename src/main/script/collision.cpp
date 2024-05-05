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
Collision detection functions
*/

#include "../pch.h"

#include "script.h"
#include "../renderer/sprite.h"

namespace AB {

extern Script script;
extern ResourceManager<Sprite> sprites;

/// Pixel-precise collision detection with rotation and scaling
// @function AB.collision.collides
// @param index1 Sprite 1 handle
// @param x1 Sprite 1 x position
// @param y1 Sprite 1 y position
// @param r1 Sprite 1 rotation
// @param sx1 Sprite 1 x scale
// @param sy1 Sprite 1 y scale
// @param index2 Sprite 2 handle
// @param x2 Sprite 2 x position
// @param y2 Sprite 2 y position
// @param r2 Sprite 2 rotation
// @param sx2 Sprite 2 x scale
// @param sy2 Sprite 2 y scale
// @return collision
static i32 luaCollides(lua_State* luaVM) {

    //  TODO: maybe check number of arguments?

    i32 f1 = (i32)lua_tonumber(luaVM, 1);
    f32 x1 = (f32)lua_tonumber(luaVM, 2);
    f32 y1 = (f32)lua_tonumber(luaVM, 3);
    f32 r1 = (f32)lua_tonumber(luaVM, 4);
    f32 sx1 = (f32)lua_tonumber(luaVM, 5);
    f32 sy1 = (f32)lua_tonumber(luaVM, 6);

    i32 f2 = (int)lua_tonumber(luaVM, 7);
    f32 x2 = (f32)lua_tonumber(luaVM, 8);
    f32 y2 = (f32)lua_tonumber(luaVM, 9);
    f32 r2 = (f32)lua_tonumber(luaVM, 10);
    f32 sx2 = (f32)lua_tonumber(luaVM, 11);
    f32 sy2 = (f32)lua_tonumber(luaVM, 12);

    extern b8 collides(Sprite *s1, Vec2 pos1, f32 angle1, f32 scaleX1, f32 scaleY1,
        Sprite *s2, Vec2 pos2, f32 angle2, f32 scaleX2, f32 scaleY2);
    
    b8 collision = collides(sprites.get(f1), Vec2(x1, y1), r1, sx1, sy1,
        sprites.get(f2), Vec2(x2, y2), r2, sx2, sy2);

    lua_pushboolean(luaVM, collision);

    return 1;
}

void registerCollisionFunctions() {
    static const luaL_Reg collisionFuncs[] = {
        { "collides", luaCollides},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "collision", collisionFuncs);
}

}
