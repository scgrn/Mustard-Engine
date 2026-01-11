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
Pseudo-random number generation and Perlin noise functions
*/

#include "../pch.h"

#include "script.h"
#include "../math/math.h"
#include "../math/random.h"
#include "../math/perlin.h"

namespace AB {

extern Script script;

static PerlinNoise perlinNoise;

//----------------------------------------------------------------- PRNG / NOISE --------------------------------------

/// Seeds the pseudo-random number generator
// @param seed (optional) Seeds the PRNG for deterministic ouput
// @function AB.math.randomSeed
static int luaRandomSeed(lua_State* luaVM) {
    if (lua_gettop(luaVM) >= 1) {
        u32 seed = (u32)lua_tointeger(luaVM, 1);
        rndSeed(seed);
    } else {
        rndSeed();
    }

    return 0;
}

/// Generates a random floating point number
// @return A random floating point number [0..1]
// @function AB.math.random

/// Generates a random integer
// @param upperBound
// @return A random integer [1..upperBound]
// @function AB.math.random

/// Generates a random integer
// @param lowerBound
// @param upperBound
// @return A random integer [lowerBound..upperBound]
// @function AB.math.random
static int luaRandom(lua_State* luaVM) {
    if (lua_gettop(luaVM) == 1) {
        u32 n = (u32)lua_tointeger(luaVM, 1);
        lua_pushinteger(luaVM, rnd(n) + 1);
    } else if (lua_gettop(luaVM) >= 2) {
        i32 lb = (i32)lua_tointeger(luaVM, 1);
        i32 ub = (i32)lua_tointeger(luaVM, 2);
        lua_pushinteger(luaVM, rnd(lb, ub));
    } else {
        lua_pushnumber(luaVM, rnd());
    }

    return 1;
}

/// Seeds the Perlin noise generator
// @param seed (optional) Seeds the Perlin noise generator for deterministic ouput
// @function AB.math.noiseSeed
static int luaNoiseSeed(lua_State *luaVM) {
    u32 seed = (u32)lua_tointeger(luaVM, 1);
    perlinNoise = PerlinNoise(seed);

    return 0;
}

/// Generates Perlin noise
// @param x X-coordinate
// @param y Y-coordinate
// @param z Z-coordinate
// @param octaves Octaves
// @param persistence Persistence
// @return noise
// @function AB.math.noise
static int luaNoise(lua_State* luaVM) {
    f32  x = (f32 )lua_tonumber(luaVM, 1);
    f32  y = (f32 )lua_tonumber(luaVM, 2);
    f32  z = (f32 )lua_tonumber(luaVM, 3);
    i32 octaves = (i32)lua_tonumber(luaVM, 4);
    f32  persistence = (f32 )lua_tonumber(luaVM, 5);

    lua_pushnumber(luaVM, perlinNoise.noise(x, y, z, octaves, persistence));

    return 1;
}

/// Normalizes an angle
// @param angle Angle in radians
// @return Angle normalized to [-math.pi .. math.pi]
// @function AB.math.normalizeAngle
static int luaNormalizeAngle(lua_State* luaVM) {
    f32 angle = (f32)lua_tonumber(luaVM, 1);
    
    lua_pushnumber(luaVM, normalizeAngle(angle));
    
    return 1;
}

/// Calculates the distance between two points in 2D space
// @param x1 X-coordinate of first point
// @param y1 Y-coordinate of first point
// @param x2 X-coordinate of second point
// @param y2 Y-coordinate of second point
// @return distance
// @function AB.math.distance
static int luaDistance(lua_State* luaVM) {
    f32  x1 = (f32 )lua_tonumber(luaVM, 1);
    f32  y1 = (f32 )lua_tonumber(luaVM, 2);
    f32  x2 = (f32 )lua_tonumber(luaVM, 3);
    f32  y2 = (f32 )lua_tonumber(luaVM, 4);

    lua_pushnumber(luaVM, distance(Vec2(x1, y1), Vec2(x2, y2)));

    return 1;
}

/// Calculates the distance from a point to a line segment in 2D space
// @param x1 X-coordinate of line segment first endpoint
// @param y1 Y-coordinate of line segment first endpoint
// @param x2 X-coordinate of line segment second endpoint
// @param y2 Y-coordinate of line segment second endpoint
// @param x3 X-coordinate of point
// @param y3 Y-coordinate of point
// @return distance
// @function AB.math.distPointToLine
static int luaDistPointToLine(lua_State* luaVM) {
    f32  x1 = (f32 )lua_tonumber(luaVM, 1);
    f32  y1 = (f32 )lua_tonumber(luaVM, 2);
    f32  x2 = (f32 )lua_tonumber(luaVM, 3);
    f32  y2 = (f32 )lua_tonumber(luaVM, 4);
    f32  x3 = (f32 )lua_tonumber(luaVM, 5);
    f32  y3 = (f32 )lua_tonumber(luaVM, 6);

    lua_pushnumber(luaVM, distPointToLine(Vec2(x1, y1), Vec2(x2, y2), Vec2(x3, y3)));

    return 1;
}

/// Determines if two line segments intersect in 2D space
// @param x1 X-coordinate of first line segment first endpoint
// @param y1 Y-coordinate of first line segment first endpoint
// @param x2 X-coordinate of first line segment second endpoint
// @param y2 Y-coordinate of first line segment second endpoint
// @param x3 X-coordinate of second line segment first endpoint
// @param y3 Y-coordinate of second line segment first endpoint
// @param x4 X-coordinate of second line segment second endpoint
// @param y4 Y-coordinate of second line segment second endpoint
// @return intersects
// @function AB.math.lineSegmentIntersection
static int luaLineSegmentIntersection(lua_State* luaVM) {
    f32  x1 = (f32 )lua_tonumber(luaVM, 1);
    f32  y1 = (f32 )lua_tonumber(luaVM, 2);
    f32  x2 = (f32 )lua_tonumber(luaVM, 3);
    f32  y2 = (f32 )lua_tonumber(luaVM, 4);
    f32  x3 = (f32 )lua_tonumber(luaVM, 5);
    f32  y3 = (f32 )lua_tonumber(luaVM, 6);
    f32  x4 = (f32 )lua_tonumber(luaVM, 7);
    f32  y4 = (f32 )lua_tonumber(luaVM, 8);

    lua_pushnumber(luaVM, lineSegmentIntersection(Vec2(x1, y1), Vec2(x2, y2), Vec2(x3, y3), Vec2(x4, y4)));

    return 1;
}


void registerMathFunctions() {
    static const luaL_Reg mathFuncs[] = {
        { "randomSeed", luaRandomSeed},
        { "random", luaRandom},
        { "noiseSeed", luaNoiseSeed},
        { "noise", luaNoise},
        { "normalizeAngle", luaNormalizeAngle},
        { "distance", luaDistance},
        { "distPointToLine", luaDistPointToLine},
        { "lineSegmentIntersection", luaLineSegmentIntersection},
        
        { NULL, NULL }
    };
    script.registerFuncs("AB", "math", mathFuncs);
}

}
