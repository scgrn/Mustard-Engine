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

    @file tga.h
    @author Andrew Krause - contact@alienbug.net
    @date 10.25.10

    Loads TGA files. Currently supports 32 or 24 bit depths.

*/

#ifndef AB_TGA_H
#define AB_TGA_H

#include <string>

namespace AB {

u8* loadTGA(const std::string& filename, u32 &width, u32 &height, u32 &bpp);
void saveTGA(u8* data, const u32 width, const u32 height, const std::string& filename);

}   // namespace

#endif  //  AB_TGA_H

