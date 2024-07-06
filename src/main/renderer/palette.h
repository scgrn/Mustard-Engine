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

#ifndef AB_PALETTE_H
#define AB_PALETTE_H

#include "../core/assetManager.h"

#include "texture.h"

namespace AB {

class Palette : public Asset {
    public:
        struct Color {
            uint8_t r, g, b;
        };
        
        struct LABColor {
            float L, a, b;
        };
        
        virtual void load(std::string const& filename);
        virtual void release();
        
        Texture *closestColor, *secondClosestColor;
        std::vector<Color> palette;
        std::vector<LABColor> LABpalette;
        
    private:
        LABColor RGBtoLAB(Color color);
        void findClosestColors(Color target, uint32_t &closest, uint32_t &secondClosest); 
    
};

}

#endif
