/**

zlib License

(C) 2020 Andrew Krause

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

/**

    @file misc.h
    @author Andrew Krause - contact@alienbug.net
    @date 08.11.11

    Miscellaneous utility functions.

*/

#ifndef AB_MISC_H
#define AB_MISC_H

#include <iostream>
#include <sstream>
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

float inline round(float val) {
    return floor(val + 0.5f);
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
inline std::string toString(T val, bool groupDigits = true) {
	std::ostringstream o;
    o << val;
    std::string s(o.str());

    if (groupDigits) {
        unsigned int offset1 = 3;
        unsigned int offset2 = 3;
        if (val < 0) {
            offset2++;   // account for "-"
        }
        for (int i = 0; i < 5; i++) {
            if (s.length() > offset2) {
                s.insert(s.length() - offset1, ",");
                offset1 += 4;
                offset2 += 4;
            }
        }
    }
    return s;
}

}   //  namespace

#endif  //  VISAGE_MISC_H
