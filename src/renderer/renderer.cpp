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
#include "batchRenderer.h"

//  force use of discrete GPU
//  https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card/39047129
extern "C" 
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace AB {

GLuint Renderer::whiteTexture;
TextureCache Renderer::textureCache;

extern std::map<int, BatchRenderer*> batchRenderers;

// common quad mesh
GLfloat Renderer::quadVertices[] = {
	0.5f,  0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
};
GLuint Renderer::quadElements[] = {
	0, 1, 2,
	2, 3, 0,
};
		
bool Renderer::startup() {
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
    for (int i = 0; i < 4; i++) {
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
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	
	batchRenderers.clear();
	batchRenderers[0] = new BatchRenderer();
	//batchRenderers.insert(std::pair<int, BatchRenderer>(0, new BatchRenderer()));
	
    CALL_GL(glGenVertexArrays(1, &fullscreenQuadVAO));

	initialized = true;
	
	return true;
}

void Renderer::shutdown() {
	LOG("Renderer subsystem shutdown", 0);
	
	for (std::map<int, BatchRenderer*>::iterator i = batchRenderers.begin(); i != batchRenderers.end(); i++) {
		delete i->second;
	}

    glDeleteBuffers(1, &fullscreenQuadVAO);

	glDeleteTextures(1, &whiteTexture);
    glDeleteBuffers(1, &ubo);

	initialized = false;
}

void Renderer::beginScene(const Camera& camera) {
	assert(!inScene);

	inScene = true;
}
/*
void Renderer::defineRenderGroup(int index, Shader *shader, glm::mat4 colorTransform, bool depthSorting) {
	renderGroups[index].shader = shader;
	renderGroups[index].colorTransform = colorTransform;
	renderGroups[index].depthSorting = depthSorting;
	
	//renderGroups[index].renderBatch.clear();	// do we want this?
	renderGroups[index].renderBatch.reserve(MAX_QUADS_PER_BATCH);
}
*/

void Renderer::clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::renderFullscreenQuad() {
	CALL_GL(glBindVertexArray(fullscreenQuadVAO));
	CALL_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

void Renderer::renderQuad(const Quad& quad) {
	ERR("NOT IMPLEMENTED!", 0);
}

void Renderer::endScene() {
	assert(inScene);
	
	//	it renders
	
	inScene = false;
}

void Renderer::renderBatches(const Camera& camera) {
    for (std::map<int, BatchRenderer*>::reverse_iterator it = batchRenderers.rbegin(); it != batchRenderers.rend(); it++) {
		BatchRenderer *batchRenderer = it->second;
		
		if (batchRenderer->renderBatch.size() > 0) {
			batchRenderer->beginScene(camera);
			batchRenderer->endScene();
		}
	}
}

}

