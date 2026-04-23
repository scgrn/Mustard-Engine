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
   in a product, an acknowledgment in the product documentation would bea
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#include "../pch.h"

#include <cstring>

#include "image.h"
#include "tga.h"

#include "../core/fileSystem.h"
#include "../core/log.h"

namespace AB {

Image::Image(u32 width, u32 height) {
    this->width = width;
    this->height = height;
    imageSize = width * height * 4;

    data = new u8[imageSize];
}

Image::Image(const std::string& tgaFilename) {
    u32 bpp;

    data = loadTGA(tgaFilename, width, height, bpp);
    imageSize = width * height * 4;

    if (bpp == 24) {
        u8* rgba = new u8[imageSize];
        for (u32 i = 0; i < width * height; i++) {
            rgba[i * 4 + 0] = data[i * 3 + 0];
            rgba[i * 4 + 1] = data[i * 3 + 1];
            rgba[i * 4 + 2] = data[i * 3 + 2];
            rgba[i * 4 + 3] = 255;
        }
        delete[] data;
        data = rgba;
    }
}

Image::~Image() {
    if (data) {
        delete [] data;
        data = NULL;
    }
}

void Image::clear() {
    memset(data, 0, imageSize);
}

}
