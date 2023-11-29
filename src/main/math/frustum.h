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

#ifndef AB_FRUSTUM_H
#define AB_FRUSTUM_H

#include "aabb.h"
#include "plane.h"

namespace AB {

class Frustum {
    public:
        Frustum() {}
        ~Frustum() {}
        
        b8 pointInFrustum(Vec3 &p);
        b8 sphereInFrustum(Vec3 &p, f32 radius);
        b8 boxInFrustum(AABB &box);

        Plane plane[6];

    private:
        enum {
            TOP = 0,
            BOTTOM,
            LEFT,
            RIGHT,
            NEAR,
            FAR
        };

};

}   //  namespace

#endif

