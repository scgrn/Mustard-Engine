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

#include "renderer.h"
#include "../core/log.h"
#include "renderLayer.h"
#include "renderTarget.h"

#ifdef WIN32
//  force use of discrete GPU
//  https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card/39047129
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#else
    //  TODO: handle this on Linux
#endif

namespace AB {
    
GLuint whiteTexture;
 
b8 Renderer::startup() {
    LOG("Renderer subsystem startup", 0);
    
    // create white texture
    CALL_GL(glGenTextures(1, &whiteTexture));
    if (!whiteTexture) {
        ERR("Couldn't create texture!", 0);
    }
    CALL_GL(glActiveTexture(GL_TEXTURE0));
    CALL_GL(glBindTexture(GL_TEXTURE_2D, whiteTexture));

    // TODO: test memset
    // memset(data, 4, 0xFF);
    unsigned char data[4];
    for (u32 i = 0; i < 4; i++) {
        data[i] = 0xFF;
    }

    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data));

    // create render state, uniform buffer, etc
    CALL_GL(glGenBuffers(1, &ubo));
    CALL_GL(glBindBuffer(GL_UNIFORM_BUFFER, ubo));
    CALL_GL(glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms), NULL, GL_STATIC_DRAW)); // allocate 152 bytes of memory (?)
    CALL_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    LOG_EXP(sizeof(uniforms));
    
    CALL_GL(glEnable(GL_BLEND));
    CALL_GL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
    
    layers.clear();
    //layers[0] = new RenderLayer();
    layers[0] = new RenderLayer(nullptr, nullptr, blend::identity(), true);
    //batchRenderers.insert(std::pair<int, RenderLayer>(0, new RenderLayer()));
    
    CALL_GL(glGenVertexArrays(1, &fullscreenQuadVAO));

    initialized = true;
    
    return true;
}

void Renderer::shutdown() {
    LOG("Renderer subsystem shutdown", 0);
    
    for (std::map<u32, RenderLayer*>::iterator i = layers.begin(); i != layers.end(); i++) {
        delete i->second;
    }
    for (std::map<u32, RenderTarget*>::iterator i = canvases.begin(); i != canvases.end(); i++) {
        delete i->second;
    }

    CALL_GL(glDeleteBuffers(1, &fullscreenQuadVAO));

    CALL_GL(glDeleteTextures(1, &whiteTexture));
    CALL_GL(glDeleteBuffers(1, &ubo));

    initialized = false;
}

void Renderer::clear(f32 r, f32 g, f32 b, f32 a) {
    CALL_GL(glClearColor(r, g, b, a));
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::renderFullscreenQuad() {
    CALL_GL(glBindVertexArray(fullscreenQuadVAO));
    CALL_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    CALL_GL(glBindVertexArray(0));
}

void Renderer::render(const Camera& camera) {
    for (std::map<u32, RenderLayer*>::reverse_iterator it = layers.rbegin(); it != layers.rend(); it++) {
        RenderLayer *renderLayer = it->second;
        renderLayer->render(camera);
    }
}

}

