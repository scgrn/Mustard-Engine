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

/**

    @file perlin.h
    @author Andrew Krause - contact@alienbug.net
    @date 05.14.19

    Perlin noise

*/

#ifndef AB_PERLIN_H
#define AB_PERLIN_H

namespace AB {

class PerlinNoise {
    public:
        PerlinNoise();
        PerlinNoise(u32 seed);
        ~PerlinNoise();

        f32 noise(f32 x, f32 y, f32 z, i32 octaves, f32 persistence);

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
