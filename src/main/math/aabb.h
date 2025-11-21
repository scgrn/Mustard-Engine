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

#include <array>

#include "vector.h"

namespace AB {

class AABB {
    public:
        AABB(Vec3 centerPosition, Vec3 size);
        AABB();
        ~AABB() {}

        void calculateExtents();
        void calculateCenter();

        b8 collides(const AABB &other);
        std::array<AABB, 8> subdivide();

        Vec3 centerPosition, size;
        Vec3 min, max;

};

}   //  namespace

#endif
