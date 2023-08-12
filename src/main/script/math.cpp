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
    float x = (float)lua_tonumber(luaVM, 1);
    float y = (float)lua_tonumber(luaVM, 2);
    float z = (float)lua_tonumber(luaVM, 3);
    int octaves = (int)lua_tonumber(luaVM, 4);
    float persistence = (float)lua_tonumber(luaVM, 5);

    lua_pushnumber(luaVM, perlinNoise.noise(x, y, z, octaves, persistence));

    return 1;
}

void registerMathFunctions() {
    static const luaL_Reg mathFuncs[] = {
        { "randomSeed", luaRandomSeed},
        { "random", luaRandom},
        { "noiseSeed", luaNoiseSeed},
        { "noise", luaNoise},

        { NULL, NULL }
    };
    script.registerFuncs("AB", "math", mathFuncs);
}

}
