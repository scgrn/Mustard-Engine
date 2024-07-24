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

    @file tga.cpp
    @date 10.25.10

    Loads a TGA file! Currently supports 32 or 24 bit depths.

*/

//  http://gpwiki.org/index.php/LoadTGACpp

#include "../pch.h"

#include <cstring>

#include "tga.h"
#include "../core/fileSystem.h"
#include "../core/log.h"
#include "../math/math.h"

namespace AB {

static void swapRB(u8 *data, u32 width, u32 height, u32 bpp) {
    u8 *current = data;
    for (u32 index = 0; index != (width * height); index++) {
        u8 temp = *current;  //  save blue value
        *current = *(current + 2);      //  write red value into first pos
        *(current + 2) = temp;          //  write blue value to last pos
        current += (bpp / 8);
    }
}

static void flipImage(u8 *data, u32 width, u32 height, u32 bpp) {
    u8 bTemp;
    u8 *pLine1, *pLine2;
    u32 iLineLen, iIndex;

    iLineLen = width * (bpp / 8);
    pLine1 = data;
    pLine2 = &data[iLineLen * (height - 1)];

    for(; pLine1 < pLine2; pLine2 -= (iLineLen * 2)) {
        for (iIndex = 0; iIndex != iLineLen; pLine1++, pLine2++, iIndex++) {
            bTemp = *pLine1;
            *pLine1 = *pLine2;
            *pLine2 = bTemp;
        }
    }
}

static void premultiplyAlpha(u8 *data, u32 width, u32 height, u32 bpp) {
    u32 size = width * height;
    
    for (u32 i = 0; i < size; i++) {
        f32 r = data[i * 4 + 0] / 255.0f;
        f32 g = data[i * 4 + 1] / 255.0f;
        f32 b = data[i * 4 + 2] / 255.0f;
        f32 a = data[i * 4 + 3] / 255.0f;
        
        r *= a;
        g *= a;
        b *= a;
        
        data[i * 4 + 0] = round(r * 255.0f);
        data[i * 4 + 1] = round(g * 255.0f);
        data[i * 4 + 2] = round(b * 255.0f);
    }
}

u8* loadTGA(const std::string& filename, u32 &width, u32 &height, u32 &bpp) {
    DataObject dataObject(filename.c_str());

    //  read header
    u8* data = dataObject.getData();
    if (data[1] > 1) {
        ERR("Unsupported file type", 0);
    }

    // Encoding flag  1 = Raw indexed image
    //                2 = Raw RGB
    //                3 = Raw greyscale
    //                9 = RLE indexed
    //               10 = RLE RGB
    //               11 = RLE greyscale
    //               32 & 33 Other compression, indexed
    u32 encoding = data[2];
    if (encoding != 2 && encoding != 10) {
        ERR("Unsupported file type", 0);
    }

    //  get image size
    short x1, y1, x2, y2;
    memcpy(&x1, &data[8], 2);
    memcpy(&y1, &data[10], 2);
    memcpy(&x2, &data[12], 2);
    memcpy(&y2, &data[14], 2);

    width = (x2 - x1);
    height = (y2 - y1);
    if (width < 1 || height < 1) {
        ERR("Bad format", 0);
    }

    //  get bit depth
    bpp = data[16];
    LOG("\tWidth: %d, Height: %d, BPP: %d", width, height, bpp);

    if (data[17] > 32) {
        ERR("Interleaved data unsupported", 0);
    }

    //  calculate image size // and verify file size not suspect
    //    TODO: will this work for RLE images?
    u32 imageSize = (width * height * (bpp / 8));

    //  color map type. if this is 1, the image is indexed. screwy. bail.
    if (data[1] != 0) {
        ERR("Bad image format", 0);
    }

    //    allocate memory for image data
    u8* imageData = new u8[imageSize];
    if (imageData == NULL) {
        ERR("Unable to allocate memory for image", 0);
    }
    short offset = data[0] + 18;
    
    // unmapped RGB
    if (encoding == 2) {
        if (imageSize + 18 + data[0] > (u32)dataObject.getSize()) {
            ERR("Bad image format", 0);
        }
        memcpy(imageData, &data[offset], imageSize);
    }
    
    //    RLE RGB
    if (encoding == 10) {
        //    pixel size in bytes
        u32 pixelSize = bpp / 8;
        
        u8* current = &data[offset];
        u32 index = 0;

        // decode RLE data
        while (index < imageSize) {
            if (*current & 0x80) {
                //    run length chunk (high bit == 1)
                u8 runLength = *current - 127;
                current++;
                
                for (u32 loop = 0; loop != runLength; ++loop, index += pixelSize) {
                    memcpy(&imageData[index], current, pixelSize);
                }
                
                current += pixelSize;
            } else {
                //    raw chunk
                u8 chunkLength = *current + 1;
                current++;
                
                for (u32 loop = 0; loop != chunkLength; ++loop, index += pixelSize, current += pixelSize) {
                    memcpy(&imageData[index], current, pixelSize);
                }
            }
        }
    }
    
    swapRB(imageData, width, height, bpp);
    
    //  flip image
    if ((data[17] & 0x10)) {
        flipImage(imageData, width, height, bpp);
    }

    //    premultiply alpha
    premultiplyAlpha(imageData, width, height, bpp);

    return imageData;
}

//    TODO: profile this and see if it's faster to generate a BGR buffer rather than swizzle twice
void saveTGA(u8* data, const u32 width, const u32 height, const std::string& filename) {
    FILE *filePtr = fopen(filename.c_str(), "wb");
    if (!filePtr) {
        LOG("Couldn't create TGA file: %s", filename.c_str());
    }

    u8 TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
    u8 header[6] = { (u8)(width % 256), (u8)(width / 256),
        (u8)(height % 256), (u8)(height / 256), 32, 0};

    swapRB(data, width, height, 32);

    fwrite(TGAheader, sizeof(u8), 12, filePtr);
    fwrite(header, sizeof(u8), 6, filePtr);
    fwrite(data, sizeof(GLubyte), width * height * 4, filePtr);
    fclose(filePtr);
    
    swapRB(data, width, height, 32);
}

}   //  namespace
