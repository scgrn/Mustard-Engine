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
    Xorshift PRNG
    11.28.18
*/

#ifndef AB_RANDOM_H
#define AB_RANDOM_H

namespace AB {

void rndSeed();
void rndSeed(u32 seed);

//  TODO: document ranges (inclusive, etc)
f64 rnd();
u32 rnd(u32 n);
i32 rnd(i32 lb, i32 ub);

}   //  namespace

#endif // AB_RANDOM_H
