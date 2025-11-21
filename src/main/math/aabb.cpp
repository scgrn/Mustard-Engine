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

#include "../pch.h"

#include "aabb.h"

namespace AB {

AABB::AABB(Vec3 centerPosition, Vec3 size) {
    this->centerPosition = centerPosition;
    this->size = size;
    calculateExtents();
}

AABB::AABB() {
    centerPosition = Vec3(0.0f, 0.0f, 0.0f);
    size = Vec3(1.0f, 1.0f, 1.0f);

    min = Vec3(-0.5f, -0.5f, -0.5f);
    max = Vec3( 0.5f,  0.5f,  0.5f);
}

void AABB::calculateExtents() {
    min = centerPosition - (size / 2.0f);
    max = centerPosition + (size / 2.0f);
}

void AABB::calculateCenter() {
    size = max - min;
    centerPosition = (size / 2.0f) + min;
}

b8 AABB::collides(const AABB &other) {
    return(
        max.x > other.min.x &&
        min.x < other.max.x &&
        max.y > other.min.y &&
        min.y < other.max.y &&
        max.z > other.min.z &&
        min.z < other.max.z);
}

std::array<AABB, 8> AABB::subdivide() {
    std::array<AABB, 8> result;

    for (int i = 0; i < 8; i++) {
        result[i].min = Vec3(
            (i & 1) ? centerPosition.x : min.x,
            (i & 2) ? centerPosition.y : min.y,
            (i & 4) ? centerPosition.z : min.z);

        result[i].max = AB::Vec3(
            (i & 1) ? max.x : centerPosition.x,
            (i & 2) ? max.y : centerPosition.y,
            (i & 4) ? max.z : centerPosition.z);

        result[i].calculateCenter();
    }

    return result;
}

}   //  namespace

