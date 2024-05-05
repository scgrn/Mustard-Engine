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

//    represents a 32bit RGBA image in system memory

#ifndef AB_IMAGE_H
#define AB_IMAGE_H

#include <string>

#include "../types.h"

namespace AB {

class Image {
    public:
        Image(const u32 width, const u32 height);
        Image(const std::string& tgaFilename);
        ~Image();
        
        inline void pset(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a = 255) {
            //  TODO: implement alpha blending?

            u32 ofs = ((y * width) + x) * 4;

            data[ofs + 0] = r;
            data[ofs + 1] = g;
            data[ofs + 2] = b;
            data[ofs + 3] = a;
        }
        
        u8 *data;
        u32 width, height;
        u32 imageSize;    // in bytes

    protected:
        Image() {}
};

}

#endif
