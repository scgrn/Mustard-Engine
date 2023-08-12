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

#ifndef AB_TEXTURE_H
#define AB_TEXTURE_H

#ifdef ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else

#ifndef __EMSCRIPTEN__
#include "glad/glad.h"
#else
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#endif
#include <SDL2/SDL.h>
#endif

#include "image.h"

namespace AB {

class Texture {
    public:
        Texture();
        Texture(int width, int height);
        Texture(Image *image);
        Texture(std::string const& filename);

        virtual ~Texture();

        GLuint glHandle;        //  handle to OpenGL texture object
        GLuint textureUnit; // not used...?

        static void setMinFilter(GLenum filter) { Texture::minFilter = filter; }
        static void setMagFilter(GLenum filter) { Texture::magFilter = filter; }

        int width, height;        //  size padded to nearest 2^
        float u2, v2;

        static GLenum minFilter, magFilter;

    protected:
        void init(Image *image);
};

}   //  namespace

#endif
