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

#ifndef AB_PERLIN_H
#define AB_PERLIN_H

#include <vector>

#include "vector.h"

namespace AB {

//  fbm
struct NoiseParams {
    f32 scale = 1.0f;
    i32 octaves = 8.0f;
    f32 persistence = 0.5f;
    f32 lacunarity = 2.0f;
};

class PerlinNoise {
    public:
        PerlinNoise();
        PerlinNoise(u32 seed);
        ~PerlinNoise();

        f32 noise(Vec3 pos, NoiseParams params);
        std::vector<f32> generateNoiseMap(Vec2i size, NoiseParams params, u32 seed = 0, Vec2 offset = {0, 0});

    protected:
        static const int SAMPLE_SIZE = 256;

        void init();
        f32 fade(f32 t);
        f32 lerp(f32 a, f32 b, f32 x);
        f32 sample(f32 x, f32 y, f32 z);
        f32 grad(int hash, f32 x, f32 y, f32 z);

        i32 p[SAMPLE_SIZE * 2];
};

}   //  namespace

#endif // AB_PERLIN_H
