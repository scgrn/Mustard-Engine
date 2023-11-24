/**

zlib License

(C) 2021 Andrew Krause

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

Plane::Plane(Vec3 &v1, Vec3 &v2, Vec3 &v3) {
    setPoints(v1, v2, v3);
}

Plane::Plane(void) {}
Plane::~Plane() {}

void Plane::setPoints(Vec3 &v1, Vec3 &v2, Vec3 &v3) {
    Vec3 aux1, aux2;

    aux1 = v1 - v2;
    aux2 = v3 - v2;

    normal = crossProduct(aux2, aux1);

    normal = normalize(normal);
    point = v2;
    d = -(dotProduct(normal, point));
}

void Plane::setNormalAndPoint(Vec3 &normal, Vec3 &point) {
    this->normal = normalize(normal);
    d = -(dotProduct(this->normal, point));
}

void Plane::setCoefficients(f32 a, f32 b, f32 c, f32 d) {
    // set the normal vector
    normal = Vec3(a, b, c);
    //compute the length of the vector
    f32 length = magnitude(normal);
    // normalize the vector
    normal = Vec3(a / length, b / length, c / length);
    // and divide d by th length as well
    this->d = d / length;
}

f32 Plane::distance(Vec3 &p) {
    return (d + dotProduct(normal, p));
}

void Plane::print() {}

}   //  namespace

