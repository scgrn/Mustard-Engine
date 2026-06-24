/**

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

**/

#include "../pch.h"

#include "random.h"
#include "../core/log.h"

#include <stdint.h>
#include <ctime>

namespace AB {

PRNG globalPRNG;

void rndSeed(u32 seed) { globalPRNG.reseed(seed); }

f64 rnd() { return globalPRNG.rnd(); }
u32 rnd(u32 n) { return globalPRNG.rnd(n); }
i32 rnd(i32 lb, i32 ub) { return globalPRNG.rnd(lb, ub); }
f32 rndf(f32 lb, f32 ub) { return globalPRNG.rndf(lb, ub); }

PRNG::PRNG() {
    u32 t = time(NULL);
    u32 f = 0;

    //  flip bits
    for (int i = 0; i < 32; i++) {
        f |= ((t >> i) & 1) << (31-i);
    }

    reseed(f);
}

PRNG::PRNG(u32 seed) {
    reseed(seed);
}

u32 PRNG::randomUint32() {
    state.x = 69069U * state.x + 123U;
    state.y ^= state.y << 13U;
    state.y ^= state.y >> 17U;
    state.y ^= state.y << 5U;

    return state.x + state.y;
}

void PRNG::reseed(u32 seed) {
    state.x = seed;
    state.y = 362436069U;
}

f64 PRNG::rnd() {
    return randomUint32() * (1.0 / 4294967296.0); // 2 ^ 32 − 1
}

u32 PRNG::rnd(u32 n) {
    return (u32)(rnd() * (f64)n);
}

i32 PRNG::rnd(i32 lb, i32 ub) {
    return lb + (i32)(rnd() * (ub - lb + 1));
}

f32 PRNG::rndf(f32 lb, f32 ub) {
    return lb + rnd() * (ub - lb);
}

}   //  namespace



