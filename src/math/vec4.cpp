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

#include "Vec4.h"

namespace AB {
	
Vec4::Vec4() {
	reset();
}

Vec4::Vec4(f32 x, f32 y, f32 z, f32 w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Vec4::set(f32 x, f32 y, f32 z, f32 w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Vec4::reset() {
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

void Vec4::normalize() {
	f32 l = magnitude();

	if (l != 0.0f) {
		x = x / l;
		y = y / l;
		z = z / l;
	}
}

void Vec4::truncate(f32 length) {
	f32 l = magnitude();

	if (l > length) {
		x = (x / l) * length;
		y = (y / l) * length;
		z = (z / l) * length;
	}
}

f32 Vec4::magnitude() {
	return sqrt((x * x) + (y * y) + (z * z));
}

f32 Vec4::dotProduct(Vec4 v) {
	return (x * v.x) + (y * v.y) + (z * v.z);
}

Vec4 Vec4::crossProduct(Vec4 v) {
    Vec4 newVector;

    newVector.x = (y * v.z) - (z * v.y);
    newVector.y = (z * v.x) - (x * v.z);
    newVector.z = (x * v.y) - (y * v.x);

	
    return newVector;
}

}
