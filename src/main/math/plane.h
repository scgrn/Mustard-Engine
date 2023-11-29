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

#ifndef AB_PLANE_H
#define AB_PLANE_H

#include "vector.h"

namespace AB {

class Plane {
    public:
        Plane(Vec3 point, Vec3 normal);
        Plane(void) {}
        ~Plane() {}

        void setNormalAndPoint(Vec3& point, Vec3& normal);
        void setCoefficients(f32 a, f32 b, f32 c, f32 d);
        f32 getSignedDistanceToPoint(Vec3& point);

        Vec3 normal;

        //  negative distance from origin the to the nearest point in the plane
        f32 d;
};

}   //  namespace

#endif

