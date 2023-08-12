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

#include "../pch.h"

#include <cassert>

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

#include "textureCache.h"
#include "../core/log.h"


namespace AB {
    
TextureCache::TextureCache() {
    frameID = 1;
    invalidate();
}
        
GLuint TextureCache::bindTexture(GLuint textureID, bool reserve) {
    int oldestIndex = 0;
    int oldestFrame = frameID;

    // for (int i = 1; i < min(numTextureUnitsAvailable, MAX_TEXTURE_UNITS); i++) {
    for (int i = 1; i < MAX_TEXTURE_UNITS; i++) {
        if (textureBindings[i].textureID == textureID) {
            textureBindings[i].lastFrame = frameID;

            return i;
        }

        if (textureBindings[i].lastFrame < oldestFrame) {
            oldestFrame = textureBindings[i].lastFrame;
            oldestIndex = i;
        }
    }
    
    if (oldestFrame == frameID) {
        ERR("OLDEST FRAME: %d / FRAMEID: %d", oldestFrame, frameID);
        return -1;
    }

    int unit = oldestIndex;
    assert(unit != -1);

    //    TODO: LOG_V
    // LOG("Loading texture into unit %d", unit);

    textureBindings[unit].textureID = textureID;
    textureBindings[unit].lastFrame = frameID;

    CALL_GL(glActiveTexture(GL_TEXTURE0 + unit));
    CALL_GL(glBindTexture(GL_TEXTURE_2D, textureID));

    return unit;
}

void TextureCache::advanceFrame() {
    frameID++;
}

void TextureCache::invalidate() {
    for (int i = 0; i < MAX_TEXTURE_UNITS; i++) {
        textureBindings[i].textureID = 0;
        textureBindings[i].lastFrame = 0;
    }
}

}
