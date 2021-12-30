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

#ifndef AB_MATH_H
#define AB_MATH_H

#include <cmath>

namespace AB {
	
template<class T>
inline T min(T a, T b) {
    return a < b ? a : b;
}

template<class T>
inline T max(T a, T b) {
    return a > b ? a : b;
}

template<class T>
inline T clamp(T val) {
    return min(max(val, (T)0), (T)1);
}

template<class T>
inline T clamp(T val, T minVal, T maxVal) {
    return min(max(val, minVal), maxVal);
}

template<class T>
inline T round(T val) {
    return (val > 0.0) ? floor(val + 0.5) : ceil(val - 0.5);
}

float inline toRadians(float val) {
	return val * (M_PI / 180.0f);
}

float inline toDegrees(float val) {
	return val / (M_PI / 180.0f);
}

//  round up to next highest power of 2
//  from http://graphics.stanford.edu/~seander/bithacks.html
inline int nextPowerOfTwo(int v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

template<class T>
T greatestCommonDevisor(T a, T b) {
    return (b == 0) ? a : greatestCommonDevisor(b, a % b);
}

}	// namespace

#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
#include "random.h"
#include "perlin.h"

#endif
