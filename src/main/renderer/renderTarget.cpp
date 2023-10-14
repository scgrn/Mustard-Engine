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

#include "renderTarget.h"
#include "../core/log.h"
#include "../core/window.h"

namespace AB {

extern Window window;

RenderTarget::RenderTarget(int width, int height, bool depthStencil) {
    this->width = width;
    this->height = height;
    
    hasDepthStencil = depthStencil;
    
    CALL_GL(glGenFramebuffers(1, &fbo));
    CALL_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    
    CALL_GL(glGenTextures(1, &texture));
    CALL_GL(glBindTexture(GL_TEXTURE_2D, texture));
    CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    //  TODO: needs stencil!
    if (hasDepthStencil) {
        /*
        CALL_GL(glGenRenderbuffers(1, &depthStencilBuffer));
        CALL_GL(glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer));
        CALL_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
        CALL_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer));
        */
        glGenTextures(1, &depthStencilBuffer);
        glBindTexture(GL_TEXTURE_2D, depthStencilBuffer);

        glTexImage2D(
          GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, 
          GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
        );

        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilBuffer, 0); 
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilBuffer, 0);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }    
    
    CALL_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    CALL_GL(glDrawBuffers(1, drawBuffers));
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        CALL_GL(glDeleteFramebuffers(1, &fbo));
        CALL_GL(glDeleteTextures(1, &texture));

        if (hasDepthStencil) {
            CALL_GL(glDeleteTextures(1, &depthStencilBuffer));
        }
        ERR("Couldn't create frame buffer!", 0);
    }

    CALL_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    CALL_GL(glBindTexture(GL_TEXTURE_2D, texture));
}

RenderTarget::~RenderTarget() {
    CALL_GL(glDeleteFramebuffers(1, &fbo));
    CALL_GL(glDeleteTextures(1, &texture));

    if (hasDepthStencil) {
        CALL_GL(glDeleteTextures(1, &depthStencilBuffer));
    }
}

void RenderTarget::clear() {
    CALL_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    if (hasDepthStencil) {
        CALL_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    } else {
        CALL_GL(glClear(GL_COLOR_BUFFER_BIT));
    }
}

void RenderTarget::begin() {
    CALL_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    CALL_GL(glViewport(0, 0, width, height));
}

void RenderTarget::end() {
    CALL_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    //    TODO: need to reset viewport here but we don't know the dimensions.
    //          maybe the renderer or window class should
    //          be responsible for setting / resetting render targets?
    window.resetViewport();
}

}
