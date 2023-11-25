/**

zlib License

(C) 2023 Andrew Krause

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

#ifndef AB_AABB_H
#define AB_AABB_H

#include "vector.h"

namespace AB {

class AABB {
    public:
        AABB(Vec3 &corner, f32 x, f32 y, f32 z);
        AABB(void);
        ~AABB();

        void setBox(Vec3 &corner, f32 x, f32 y, f32 z);

        //  for use in frustum computations
        Vec3 getVertexP(Vec3 &normal);
        Vec3 getVertexN(Vec3 &normal);

        Vec3 corner;
        f32 x, y, z;
};

}   //  namespace

#endif
