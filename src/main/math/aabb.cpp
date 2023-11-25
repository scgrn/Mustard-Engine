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

#include "aabb.h"

namespace AB {

AABB::AABB(Vec3 &corner, f32 x, f32 y, f32 z) {
    setBox(corner, x, y, z);
}

AABB::AABB(void) {
    corner = Vec3();

    x = 1.0f;
    y = 1.0f;
    z = 1.0f;
}

AABB::~AABB() {}

void AABB::setBox(Vec3 &corner, f32 x, f32 y, f32 z) {
    this->corner = corner;

    if (x < 0.0f) {
        x = -x;
        this->corner.x -= x;
    }
    if (y < 0.0f) {
        y = -y;
        this->corner.y -= y;
    }
    if (z < 0.0f) {
        z = -z;
        this->corner.z -= z;
    }

    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3 AABB::getVertexP(Vec3 &normal) {
    Vec3 res = corner;

    if (normal.x > 0.0f) {
        res.x += x;
    }

    if (normal.y > 0.0f) {
        res.y += y;
    }

    if (normal.z > 0.0f) {
        res.z += z;
    }

    return(res);
}

Vec3 AABB::getVertexN(Vec3 &normal) {
    Vec3 res = corner;

    if (normal.x < 0.0f) {
        res.x += x;
    }

    if (normal.y < 0.0f) {
        res.y += y;
    }

    if (normal.z < 0.0f) {
        res.z += z;
    }

    return(res);
}

}   //  namespace

