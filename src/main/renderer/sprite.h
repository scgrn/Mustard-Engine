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

#ifndef AB_SPRITE_H
#define AB_SPRITE_H

#include <memory>

#include "../core/assetManager.h"
#include "texture.h"
#include "image.h"
#include "renderLayer.h"

namespace AB {

class Sprite : public Asset {
    public:
        Sprite();
        /*
        Sprite(u32 textureHandle, u32 width, u32 height, f32 u1, f32 v1, f32 u2, f32 v2) :
            textureHandle(textureHandle), width(width), height(height), u1(u1), v1(v1), u2(u2), v2(v2) {}
        */
        virtual ~Sprite() {}

        virtual void load(std::string const& filename);
        virtual void release();

        Image* getImage() { return image; }

        /**
            This is called after a sprite has been added to a sprite atlas.
        */
        void adopt(std::shared_ptr<Texture> texture, f32 u1, f32 v1, f32 u2, f32 v2, b8 retainImage = false);

        void buildCollisionMask(u32 offsetX = 0, u32 offsetY = 0);
        void uploadToGPU(b8 retainImage = false);
        void render(RenderLayer *renderer, Vec3 pos, f32 rotation = 0.0f, Vec2 scale = Vec2(1.0f, 1.0f), Vec4 color = Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        std::shared_ptr<Texture> texture;

        u32 width, height;
        u32 atlasX, atlasY;
        f32 u1, v1, u2, v2;

        f32 radius;            //  radius of bounding circle

        f32 uSpan, vSpan;        //    percentage of actual size/padded size
        i32 halfX, halfY;        //  width and height / 2

        b8 *collisionMask;
        Image *image;

};

extern b8 collides(Sprite *s1, Vec2 pos1, f32 angle1, f32 scaleX1, f32 scaleY1,
    Sprite *s2, Vec2 pos2, f32 angle2, f32 scaleX2, f32 scaleY2);

}   //  namespace

#endif // SPRITE_H

