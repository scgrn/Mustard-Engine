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

#include "Vec2.h"
#include "misc.h"

namespace AB {
	
Vec2::Vec2() {
	reset();
}

Vec2::Vec2(f32 x, f32 y) {
	this->x = x;
	this->y = y;
}

void Vec2::set(f32 x, f32 y) {
	this->x = x;
	this->y = y;
}

void Vec2::reset() {
	x = 0;
	y = 0;
}

void Vec2::normalize() {
	f32 l = magnitude();

	if (l != 0.0f) {
		x = x / l;
		y = y / l;
	}
}

void Vec2::truncate(f32 length) {
	f32 l = magnitude();

	if (l > length) {
		x = (x / l) * length;
		y = (y / l) * length;
	}
}

void Vec2::rotate(f32 angle) {
    f32 tx = x;
    f32 ty = y;
    f32 ta = toRadians(angle);

    x = ty * sin(ta) + tx * cos(ta);
    y = ty * cos(ta) - tx * sin(ta);
}

f32 Vec2::magnitude() {
	return sqrt((x * x) + (y * y));
}

f32 Vec2::dotProduct(Vec2 v) {
	return (x * v.x) + (y * v.y);
}

}
