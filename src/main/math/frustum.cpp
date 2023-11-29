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

#include "frustum.h"
#include "math.h"

namespace AB {

b8 Frustum::pointInFrustum(Vec3 &p) {
    for(i32 i = 0; i < 6; i++) {
        if (plane[i].getSignedDistanceToPoint(p) < 0.0f) {
            return false;
        }
    }
    return true;
}

b8 Frustum::sphereInFrustum(Vec3 &p, f32 radius) {
    for(i32 i = 0; i < 6; i++) {
        if (plane[i].getSignedDistanceToPoint(p) < -radius) {
            return false;
        }
    }
    return true;
}

b8 Frustum::boxInFrustum(AABB &box) {
/*
    i32 result = INSIDE;
    for(i32 i = 0; i < 6; i++) {
        Vec3 vertexP = box.getVertexP(pl[i].normal); 
        if (pl[i].distance(vertexP) < 0.0f) {
            return OUTSIDE;
        } else {
            Vec3 vertexN = box.getVertexN(pl[i].normal);
            if (pl[i].distance(vertexN) < 0.0f) {
                result =  INTERSECT;
            }
        }
    }
    return(result);
*/
    return false;
}

}   //  namespace

