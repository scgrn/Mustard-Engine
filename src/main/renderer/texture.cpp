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


#include "../pch.h"

#include "texture.h"
#include "tga.h"
#include "../math/math.h"
#include "../core/log.h"

namespace AB {

GLenum Texture::minFilter = GL_NEAREST;
GLenum Texture::magFilter = GL_NEAREST;
GLenum Texture::wrapMode = GL_REPEAT;

Texture::Texture() {
    glHandle = 0;
}

Texture::Texture(u32 width, u32 height) {
    CALL_GL(glGenTextures(1, &glHandle));
    if (!glHandle) {
        ERR("Couldn't create texture!", 0);
    }

    CALL_GL(glActiveTexture(GL_TEXTURE0));
    CALL_GL(glBindTexture(GL_TEXTURE_2D, glHandle));

    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));

    this->width = width;
    this->height = height;
}

Texture::Texture(std::shared_ptr<Image> image) {
    init(image);
}

void Texture::init(std::shared_ptr<Image> image) {
    u32 rWidth = image->width;
    u32 rHeight = image->height;
    width = nextPowerOfTwo(rWidth);
    height = nextPowerOfTwo(rHeight);

    u2 = ((f32)rWidth - 0.01f) / (f32)width;
    v2 = ((f32)rHeight - 0.01f) / (f32)height;

    //  create new image padded to ^2
    u8 *data = NULL;
    data = new u8[width * height * 4];
    memset(data, 0, width * height * 4);
    u8 *imageData = NULL;
    imageData = image->data;

    for (u32 y = 0; y < rHeight; y++) {
        memcpy(&data[(rHeight - y - 1) * width * 4], &imageData[y * rWidth * 4], rWidth * 4);
    }

    //  create OGL texture
    CALL_GL(glGenTextures(1, &glHandle));
    if (!glHandle) {
        ERR("Couldn't create texture!", 0);
    }
    CALL_GL(glBindTexture(GL_TEXTURE_2D, glHandle));

    //    copy the pixel data over to the new OGL texture
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));

    CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

    if (data) {
        delete [] data;
        data = NULL;
    }
}

Texture::Texture(std::string const& filename) {
    auto image = std::make_shared<Image>(filename);
    init(image);
}

Texture::~Texture() {
    glDeleteTextures(1, &glHandle);
}

}   //  namespace
