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

#endif  
