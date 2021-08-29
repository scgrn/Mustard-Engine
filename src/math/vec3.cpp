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

#include <cmath>

#include "Vec3.h"

namespace AB {
	
Vec3::Vec3() {
	reset();
}

Vec3::Vec3(f32 x, f32 y, f32 z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vec3::set(f32 x, f32 y, f32 z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vec3::reset() {
	x = 0;
	y = 0;
	z = 0;
}

void Vec3::normalize() {
	f32 l = magnitude();

	if (l != 0.0f) {
		x = x / l;
		y = y / l;
		z = z / l;
	}
}

void Vec3::truncate(f32 length) {
	f32 l = magnitude();

	if (l > length) {
		x = (x / l) * length;
		y = (y / l) * length;
		z = (z / l) * length;
	}
}

f32 Vec3::magnitude() {
	return sqrt((x * x) + (y * y) + (z * z));
}

f32 Vec3::dotProduct(Vec3 v) {
	return (x * v.x) + (y * v.y) + (z * v.z);
}

Vec3 Vec3::crossProduct(Vec3 v) {
    Vec3 newVector;

    newVector.x = (y * v.z) - (z * v.y);
    newVector.y = (z * v.x) - (x * v.z);
    newVector.z = (x * v.y) - (y * v.x);

    return newVector;
}

}
