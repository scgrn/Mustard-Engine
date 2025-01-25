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

    @file font.h
    @date 07.14.10

    Font rendering based on BMFont http://www.angelcode.com/products/bmfont/


*/

#ifndef AB_FONT_H
#define AB_FONT_H

#include <iostream>
#include <unordered_map>

#include "../core/assetManager.h"
#include "sprite.h"

namespace AB {

class Font : public Asset {
    public:
        enum Align {LEFT, RIGHT, CENTER};

        Font() {}
        virtual ~Font() {}

        /**
        *    Loads a font and its associated texture from file
        *
        *    @param filename XML font descriptor file -
        *        Passing "default1" creates a built-in 8x16 pixel font.
        *        Passing "default2" creates a built-in 8x8 pixel font.
        *        Passing "default3" creates a built-in 3x5 pixel font.
        */
        virtual void load(std::string const& filename);

        virtual void release();

        /**
        *    Renders a string on screen
        *
        *    @param renderGroup render group
        *    @param x screen X coordinate of string
        *    @param y screen Y coordinate of string
        *    @param scale scaling factor
        *    @param align alignment of string (LEFT, RIGHT, CENTER)
        *    @param string the string to print
        */
        void printString(RenderLayer *renderer, GLfloat x, GLfloat y, GLfloat scale, Align alignment, std::string const& string);

        /**
        *    Returns pixel-width of string
        *
        *    @param string source string
        *    @param scale scaling factor
        *
        *    @return length of string in pixels
        */
        int stringLength(std::string const& string, GLfloat scale);

        void setColor(float r, float g, float b, float a);

        class Character : public Sprite {
            public:
                Character(std::shared_ptr<Texture> texture, int x, int y, int width, int height, int xOffset, int yOffset, int xAdvance, float scaleW, float scaleH);
                virtual ~Character();

                int xOffset, yOffset, xAdvance;

            protected:
                Character() {}
        };

        float scaleW, scaleH;
        int height;

    protected:
        Character* chars[256];
        std::unordered_map<int, Character*> extendedChars;

        std::unordered_map<int, std::unordered_map<int, int> > kernings;
        std::shared_ptr<Texture> texture;
        
        Vec4 color;

        int lineHeight, base;

    private:
        void build8x8Default(bool stretch);
        void build3x5Default();
        std::unordered_map<unsigned char, bool> unknownCharWarnings;
};

}   //  namespace

#endif // FONT_H

