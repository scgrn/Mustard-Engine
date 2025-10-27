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

AABB::AABB(Vec3 centerPosition, Vec3 size) {
    this->centerPosition = centerPosition;
    this->size = size;
    calculateExtents();
}

AABB::AABB(void) {
    centerPosition = Vec3(0.0f, 0.0f, 0.0f);
    size = Vec3(1.0f, 1.0f, 1.0f);

    min = Vec3(-0.5f, -0.5f, -0.5f);
    max = Vec3( 0.5f,  0.5f,  0.5f);
}

void AABB::calculateExtents(void) {
    min = centerPosition - (size / 2.0f);
    max = centerPosition + (size / 2.0f);
}

b8 AABB::collides(const AABB &other) {
    return(
        max.x > other.min.x &&
        min.x < other.max.x &&
        maxy > other.min.y &&
        min.y < other.max.y &&
        maxz > other.min.z &&
        min.z < other.max.z);
}

}   //  namespace

