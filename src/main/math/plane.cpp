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

#include "plane.h"
#include "matrix.h"

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

bool findIntersection(Plane const& p1, Plane const& p2, Plane const& p3, Vec3& out) {
    Mat4 a;

    a.data2d[0][0] = p1.normal.x;
    a.data2d[0][1] = p1.normal.y;
    a.data2d[0][2] = p1.normal.z;
    a.data2d[0][3] = 0;

    a.data2d[1][0] = p2.normal.x;
    a.data2d[1][1] = p2.normal.y;
    a.data2d[1][2] = p2.normal.z;
    a.data2d[1][3] = 0;

    a.data2d[2][0] = p3.normal.x;
    a.data2d[2][1] = p3.normal.y;
    a.data2d[2][2] = p3.normal.z;
    a.data2d[2][3] = 0;

    a.data2d[3][0] = 0;
    a.data2d[3][1] = 0;
    a.data2d[3][2] = 0;
    a.data2d[3][3] = 1;

    Vec4 b(p1.d, p2.d, p3.d, 1);

    Mat4 inv;
    if (!inverse(a, inv)) {
        return false;
    }
    Vec4 result = inv * b;

    out = Vec3(result.x, result.y, result.z);

    return true;
}

}   //  namespace
