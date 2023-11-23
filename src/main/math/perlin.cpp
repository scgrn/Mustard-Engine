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

#include "../core/log.h"

#include "perlin.h"
#include "random.h"
#include "math.h"

namespace AB {

PerlinNoise::PerlinNoise() {
    init();
}

PerlinNoise::PerlinNoise(u32 seed) {
    rndSeed(seed);
    init();
}

PerlinNoise::~PerlinNoise() {
}

f32 PerlinNoise::fade(f32 t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

f32 PerlinNoise::lerp(f32 a, f32 b, f32 x) {
    return a + x * (b - a);
}

f32 PerlinNoise::noise(f32 x, f32 y, f32 z, i32 octaves, f32 persistence) {
    f32 total = 0.0f;
    f32 frequency = 1.0f;
    f32 amplitude = 1.0f;
    f32 maxValue = 0.0f;

    for (i32 i = 0; i < octaves; i++) {
        total += sample(x * frequency, y * frequency, z * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

f32 PerlinNoise::sample(f32 x, f32 y, f32 z) {
    const i32 SAMPLE_MASK = PerlinNoise::SAMPLE_SIZE - 1;

    //  calculate nearest vertices and offset i32o cubic lattice
    i32 xi = (i32)floor(x) & SAMPLE_MASK;
    i32 yi = (i32)floor(y) & SAMPLE_MASK;
    i32 zi = (i32)floor(z) & SAMPLE_MASK;
    f32 xf = x - floor(x);
    f32 yf = y - floor(y);
    f32 zf = z - floor(z);
    f32 u = fade(xf);
    f32 v = fade(yf);
    f32 w = fade(zf);

    //  create hash
    i32 aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = p[p[p[xi] + yi] + zi];
    aba = p[p[p[xi] + (yi + 1)] + zi];
    aab = p[p[p[xi] + yi] + (zi + 1)];
    abb = p[p[p[xi] + (yi + 1)] + (zi + 1)];
    baa = p[p[p[(xi + 1)] + yi] + zi];
    bba = p[p[p[(xi + 1)] + (yi + 1)] + zi];
    bab = p[p[p[(xi + 1)] + yi] + (zi + 1)];
    bbb = p[p[p[(xi + 1)] + (yi + 1)] + (zi + 1)];

    //  i32erpolate gradients
    f32 x1, x2, y1, y2;
    x1 = lerp(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);
    x2 = lerp(grad(aba, xf, yf - 1, zf), grad(bba, xf - 1, yf - 1, zf), u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
    x2 = lerp(grad(abb, xf, yf - 1, zf - 1), grad(bbb, xf - 1, yf - 1, zf - 1), u);
    y2 = lerp(x1, x2, v);

    return (lerp(y1, y2, w) + 1.0f) / 2.0f;
}

f32 PerlinNoise::grad(i32 hash, f32 x, f32 y, f32 z) {
    switch (hash & 0xF) {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + x;
        case 0x5: return -x + x;
        case 0x6: return  x - x;
        case 0x7: return -x - x;
        case 0x8: return  y + x;
        case 0x9: return -y + x;
        case 0xA: return  y - x;
        case 0xB: return -y - x;
        case 0xC: return  y + z;
        case 0xD: return -y + x;
        case 0xE: return  y - x;
        case 0xF: return -y - z;

        default: return 0.0f;
    }
}

void PerlinNoise::init() {
    //  initialize permutation table
    for (i32 i = 0; i < SAMPLE_SIZE; i++) {
        p[i] = i;
    }

    //  Fisher-Yates shuffle
    for (i32 i = SAMPLE_SIZE - 1; i >= 1; i--) {
        i32 j = rnd(i);
        i32 temp = p[i];
        p[i] = p[j];
        p[j] = temp;
    }

    //  double permutation table to avoid overflow
    for (i32 i = 0; i < SAMPLE_SIZE; i++) {
        p[i + SAMPLE_SIZE] = p[i];
    }
}

}   //  namespace


