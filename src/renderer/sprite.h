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

#include "glm/glm.hpp"

#include "../core/resourceManager.h"
#include "texture.h"
#include "image.h"
#include "batchRenderer.h"

namespace AB {

class Sprite : public Resource {
    public:
        Sprite();
        /*
        Sprite(int textureHandle, int width, int height, float u1, float v1, float u2, float v2) :
            textureHandle(textureHandle), width(width), height(height), u1(u1), v1(v1), u2(u2), v2(v2) {}
        */
        virtual ~Sprite() {}

        virtual void load(std::string const& filename);
		virtual void release();

		Image* getImage() { return image; }

		/**
            This is called after a sprite has been added to a sprite atlas.
		*/
        void adopt(std::shared_ptr<Texture> texture, float u1, float v1, float u2, float v2);

		void buildCollisionMask();
		void uploadToGPU();
		void render(BatchRenderer *renderer, glm::vec3 pos, float rotation = 0.0f, float scale = 1.0f, glm::vec4 color = glm::vec4(1.0f));
		
		std::shared_ptr<Texture> texture;

        int width, height;
        int atlasX, atlasY;
        float u1, v1, u2, v2;

		float radius;			//  radius of bounding circle

		float uSpan, vSpan;		//	percentage of actual size/padded size
		int halfX, halfY;		//  width and height / 2

        bool *collisionMask;

    protected:
        Image *image;

};

extern bool collides(Sprite *s1, glm::vec2 pos1, float angle1, float scaleX1, float scaleY1,
    Sprite *s2, glm::vec2 pos2, float angle2, float scaleX2, float scaleY2);

}   //  namespace

#endif // SPRITE_H

