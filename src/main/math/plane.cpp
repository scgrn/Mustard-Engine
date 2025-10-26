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

#include "plane.h"

namespace AB {

Plane::Plane(Vec3 point, Vec3 normal) {
    this->normal = normalize(normal);
    d = dotProduct(this->normal, point);
}

Plane::Plane(Vec3 p1, Vec3 p2, Vec3 p3) {
    Vec3 v1 = p2 - p1;
    Vec3 v2 = p3 - p1;
    Vec3 cross = crossProduct(v1, v2);
    this->normal = normalize(cross);
    d = -dotProduct(this->normal, p1);
}

void Plane::setCoefficients(f32 a, f32 b, f32 c, f32 d) {
    normal = Vec3(a, b, c);

    //  not calling the normalize function to avoid calculating the magnitude twice
    f32 length = magnitude(normal);
    if (length != 0) {
        normal = Vec3(a / length, b / length, c / length);
        this->d = d / length;
    }
}

f32 Plane::getSignedDistanceToPoint(Vec3& point) {
    return dotProduct(normal, point) - d;
}

}   //  namespace

