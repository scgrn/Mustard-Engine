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

//#define AB_PALETTE_TEST

#include "../pch.h"

#include "palette.h"
#include "tga.h"
#include "../math/math.h"

namespace AB {

void Palette::load(std::string const& filename) {
    LOG("Loading palette <%s>", filename.c_str());

    DataObject dataObject(filename.c_str());
    std::string paletteData = std::string((const char*)dataObject.getData(), dataObject.getSize());
    
    std::stringstream ss(paletteData);
    std::string hexColor;
    while(std::getline(ss, hexColor, '\n')) {
        Color color;
        sscanf(hexColor.c_str(), "%02hhx %02hhx %02hhx", &color.r, &color.g, &color.b);
        palette.push_back(color);
    }
    LOG("%d colors loaded", palette.size());
    
    //    TODO: remove all magic numbers from up in here
    
    //    generate palette lookup images
    Image closestColorImage(512, 512);
    Image secondClosestColorImage(512, 512);
#ifdef AB_PALETTE_TEST
    Image targetColorImage(512, 512);
#endif

    for (u64 i = 0; i < palette.size(); i++) {
        LABpalette.push_back(RGBtoLAB(palette[i]));
    }
    
    for (u8 r = 0; r < 64; r++) {
        for (u8 g = 0; g < 64; g++) {
            for (u8 b = 0; b < 64; b++) {
                u32 closest, secondClosest;
                findClosestColors(Color{(u8)(r * 4), (u8)(g * 4), (u8)(b * 4)}, closest, secondClosest);
                
                // map RGB to UV
                i32 i = r + (g * 64) + (b * 64 * 64);
                i32 u = i / 512;
                i32 v = i % 512;
                
                i32 offset = ((v * 512) + u) * 4;
                closestColorImage.data[offset + 0] = palette[closest].r;
                closestColorImage.data[offset + 1] = palette[closest].g;
                closestColorImage.data[offset + 2] = palette[closest].b;
                closestColorImage.data[offset + 3] = 255;
                
                secondClosestColorImage.data[offset + 0] = palette[secondClosest].r;
                secondClosestColorImage.data[offset + 1] = palette[secondClosest].g;
                secondClosestColorImage.data[offset + 2] = palette[secondClosest].b;
                secondClosestColorImage.data[offset + 3] = 255;

#ifdef AB_PALETTE_TEST
                // test!
                targetColorImage.data[offset + 0] = r * 4;
                targetColorImage.data[offset + 1] = g * 4;
                targetColorImage.data[offset + 2] = b * 4;
                targetColorImage.data[offset + 3] = 255;
#endif
            }
        }
    }
#ifdef AB_PALETTE_TEST
    saveTGA(targetColorImage.data, 512, 512, "paletteTest1.tga");
    saveTGA(closestColorImage.data, 512, 512, "paletteTest2.tga");
    saveTGA(secondClosestColorImage.data, 512, 512, "paletteTest3.tga");

    LABColor c = RGBtoLAB(Color{128, 12, 65});
    LOG_EXP(c.L);
    LOG_EXP(c.a);
    LOG_EXP(c.b);
#endif
    
    closestColor = new Texture(&closestColorImage);
    secondClosestColor = new Texture(&secondClosestColorImage);
}

void Palette::release() {
    delete closestColor;
    delete secondClosestColor;
}

//    goinked from http://www.easyrgb.com/en/math.php
Palette::LABColor Palette::RGBtoLAB(Color color) {
    //    convert RGB to XYZ
    f32 r = (color.r / 255.0f);
    f32 g = (color.g / 255.0f);
    f32 b = (color.b / 255.0f);

    if (r > 0.04045f) {
        r = pow(((r + 0.055f) / 1.055f), 2.4f);
    } else {
        r /= 12.92f;
    }
    r *= 100.0f;

    if (g > 0.04045f) {
        g = pow(((g + 0.055f) / 1.055f), 2.4f);
    } else {
        g /= 12.92f;
    }
    g *= 100.0f;

    if (b > 0.04045f) {
        b = pow(((b + 0.055f) / 1.055f), 2.4f);
    } else {
        b /= 12.92f;
    }
    b *= 100.0f;
    
    f32 x = r * 0.4124f + g * 0.3576f + b * 0.1805f;
    f32 y = r * 0.2126f + g * 0.7152f + b * 0.0722f;
    f32 z = r * 0.0193f + g * 0.1192f + b * 0.9505f;
    
    //    convert XYZ to CIE-L*ab
    LABColor lab;
    
    //    reference values: Observer= 2°, Illuminant= D65
    x /= 95.047f;
    y /= 100.0f;
    z /= 108.883f;

    if (x > 0.008856f) {
        x = pow(x, (1.0f / 3.0f));
    } else {
        x = (7.787f * x) + (16.0f / 116.0f);
    }

    if (y > 0.008856f) {
        y = pow(y, (1.0f / 3.0f));
    } else {
        y = (7.787f * y) + (16.0f / 116.0f);
    }

    if (z > 0.008856f) {
        z = pow(z, (1.0f / 3.0f));
    } else {
        z = (7.787f * z) + (16.0f / 116.0f);
    }

    lab.L = (116.0f * y) - 16.0f;
    lab.a = 500.0f * (x - y);
    lab.b = 200.0f * (y - z);

    return lab;
}

void Palette::findClosestColors(Color target, u32 &closest, u32 &secondClosest) {    
    closest = 0;
    secondClosest = 0;
    
    //    squared distance
    f32 closestDistance = FLT_MAX;
    f32 secondClosestDistance = FLT_MAX;
    
    LABColor color = RGBtoLAB(target);
    
    for (u64 i = 0; i < palette.size(); i++) {
        f32 L = abs(LABpalette[i].L - color.L);
        f32 a = abs(LABpalette[i].a - color.a);
        f32 b = abs(LABpalette[i].b - color.b);
        
        f32 distance = (L * L + a * a + b * b);
        
        if (distance < closestDistance) {
            secondClosest = closest;
            secondClosestDistance = closestDistance;
            
            closest = i;
            closestDistance = distance;
        } else if (distance < secondClosestDistance) {
            secondClosest = i;
            secondClosestDistance = distance;
        }
    }
}

}
