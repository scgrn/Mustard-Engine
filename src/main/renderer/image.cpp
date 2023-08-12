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

#include "image.h"
#include "tga.h"

#include "../core/fileSystem.h"
#include "../core/log.h"

namespace AB {

Image::Image(int width, int height) {
    this->width = width;
    this->height = height;
    imageSize = width * height * 4;

    data = new unsigned char[imageSize];
}

Image::Image(const std::string& tgaFilename) {
    int bpp;
    
    data = loadTGA(tgaFilename, width, height, bpp);
    
    //    TODO: pad alpha if 24bit. who knows what happens if we don't?
    
    imageSize = width * height * 4;
}

Image::~Image() {
    if (data) {
        delete [] data;
        data = NULL;
    }
}

}
