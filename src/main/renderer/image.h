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

namespace AB {

class Image {
    public:
        Image(const int width, const int height);
        Image(const std::string& tgaFilename);
        ~Image();
        
        inline void pset(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
            //  TODO: implement alpha blending?

            int ofs = ((y * width) + x) * 4;

            data[ofs + 0] = r;
            data[ofs + 1] = g;
            data[ofs + 2] = b;
            data[ofs + 3] = a;
        }
        
        unsigned char *data;
        int width, height;
        int imageSize;    // in bytes

    protected:
        Image() {}
};

}

#endif
