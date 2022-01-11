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

#include <stdint.h>
#include <ctime>

namespace AB {

typedef struct {
    u32 x;
    u32 y;
} RandomState;

static RandomState state = {2282008U, 362436069U};

static u32 randomUint32() {
    state.x = 69069U * state.x + 123U;
    state.y ^= state.y << 13U;
    state.y ^= state.y >> 17U;
    state.y ^= state.y << 5U;

    return state.x + state.y;
}

void rndSeed() {
    u32 t = time(NULL);
    u32 f = 0;

    //  flip bits
    for (int i = 0; i < 32; i++) {
        f |= ((t >> i) & 1) << (31-i);
    }

    rndSeed(f);
}

void rndSeed(u32 seed) {
    state.x = seed;
    state.y = 362436069U;
}

f64 rnd() {
    return randomUint32() * (1.0 / 4294967296.0); // 2^32 − 1
}

u32 rnd(u32 n) {
    return rnd() * n;   // TODO: looks weird. test.
}

i32 rnd(i32 lb, i32 ub) {
  return lb + (i32)(rnd() * (ub - lb + 1));
}

}   //  namespace



