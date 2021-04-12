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

//  force use of discrete GPU
//  https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card/39047129
extern "C" 
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace AB {

static const int MAX_QUADS_PER_BATCH = 4096;

bool Renderer::startup() {
	LOG("Renderer subsystem startup", 0);

	// common quad mesh
	GLfloat quadVertices[] = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};
	GLuint quadElements[] = {
		0, 1, 2,
		2, 3, 0,
	};
	
	//	load default shader
	defaultShader.load("shaders/default2d");
	defaultShader.bind();
	
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


	// create VAO
    CALL_GL(glGenVertexArrays(1, &vao));
	CALL_GL(glBindVertexArray(vao));
	
	// create vertex VBO
    CALL_GL(glGenBuffers(1, &vbo));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, &quadVertices[0], GL_STATIC_DRAW));

	// set vertex attribute pointer
    CALL_GL(glEnableVertexAttribArray(0));
	CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
	CALL_GL(glVertexAttribDivisor(0, 0));	// default buy hey clarity  	

	// create EBO
	CALL_GL(glGenBuffers(1, &ebo));
	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, &quadElements[0], GL_STATIC_DRAW));
	
	// create IBO
	CALL_GL(glGenBuffers(1, &ibo));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, ibo));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(RenderQuad) * MAX_QUADS_PER_BATCH, NULL, GL_DYNAMIC_DRAW));

	// set instance attribute pointers
	
	//		position
    CALL_GL(glEnableVertexAttribArray(1));
	CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderQuad), (void*)0));
	CALL_GL(glVertexAttribDivisor(1, 1));  

	//		size
    CALL_GL(glEnableVertexAttribArray(2));
	CALL_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderQuad), (void*)(sizeof(GLfloat) * 3)));
	CALL_GL(glVertexAttribDivisor(2, 1));  
	
	//		scale
    CALL_GL(glEnableVertexAttribArray(3));
	CALL_GL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(RenderQuad), (void*)(sizeof(GLfloat) * 5)));
	CALL_GL(glVertexAttribDivisor(3, 1));  

	//		rotation
    CALL_GL(glEnableVertexAttribArray(4));
	CALL_GL(glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(RenderQuad), (void*)(sizeof(GLfloat) * 6)));
	CALL_GL(glVertexAttribDivisor(4, 1));  

	//		uv
    CALL_GL(glEnableVertexAttribArray(5));
	CALL_GL(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(RenderQuad), (void*)(sizeof(GLfloat) * 7)));
	CALL_GL(glVertexAttribDivisor(5, 1));  

	//		texture unit
    CALL_GL(glEnableVertexAttribArray(6));
	CALL_GL(glVertexAttribIPointer(6, 1, GL_INT, sizeof(RenderQuad), (void*)(sizeof(GLfloat) * 11)));
	CALL_GL(glVertexAttribDivisor(6, 1));  

	//		color
    CALL_GL(glEnableVertexAttribArray(7));
	CALL_GL(glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(RenderQuad), (void*)(sizeof(GLfloat) * 11 + sizeof(GLint))));
	CALL_GL(glVertexAttribDivisor(7, 1));
	

	// create render state, uniform buffer, etc
	CALL_GL(glGenBuffers(1, &ubo));
	CALL_GL(glBindBuffer(GL_UNIFORM_BUFFER, ubo));
	CALL_GL(glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms), NULL, GL_STATIC_DRAW)); // allocate 152 bytes of memory (?)
	CALL_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	LOG_EXP(sizeof(uniforms));
	
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	
	initialized = true;
	
	return true;
}

void Renderer::shutdown() {
	LOG("Renderer subsystem shutdown", 0);
	
	glDeleteTextures(1, &whiteTexture);

    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &ubo);

	initialized = false;
}

void Renderer::beginScene(const Camera& camera) {
	assert(!inScene);

	//	set transformation uniforms
	defaultShader.bind();
	defaultShader.setMat4("projection", camera.getProjectionMatrix());
	
	//	default renderGroup
	defineRenderGroup(0, &defaultShader);

	inScene = true;
}

void Renderer::defineRenderGroup(int index, Shader *shader, glm::mat4 colorTransform, bool depthSorting) {
	renderGroups[index].shader = shader;
	renderGroups[index].colorTransform = colorTransform;
	renderGroups[index].depthSorting = depthSorting;
	
	//renderGroups[index].renderBatch.clear();	// do we want this?
	renderGroups[index].renderBatch.reserve(MAX_QUADS_PER_BATCH);
}

void Renderer::renderQuad(const int renderGroup, const RenderQuad& quad) {
	assert(inScene);

	renderGroups[renderGroup].renderBatch.push_back(quad);
}

void Renderer::flush(RenderGroup *renderGroup, int begin, int end) {
	//  massive sinkhole on chemical road
	CALL_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, (end - begin + 1) * sizeof(RenderQuad), &renderGroup->renderBatch[begin]));
	CALL_GL(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, end - begin + 1));
	textureCache.advanceFrame();
}

void Renderer::endScene() {
	assert(inScene);
	
	//	it renders

	// enable VAO
	CALL_GL(glBindVertexArray(vao));
	
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, ibo));
	
	for (const auto& it : renderGroups) {
		RenderGroup renderGroup = it.second;

		//	TODO: don't bind shaders redundantly!
		renderGroup.shader->bind();

		//LOG("NEW RENDER GROUP: %d", renderGroup.renderBatch.size());

		// set colorTransform uniform
		renderGroup.shader->setMat4("colorTransform", renderGroup.colorTransform);
		
		// sort renderBatch
		std::sort(renderGroup.renderBatch.begin(), renderGroup.renderBatch.end(),
			renderGroup.depthSorting ? cmpDepth : cmp);
		
		//	iterate renderbatch, set textureID to texture unit for each renderQuad before populating ibo
		int begin = 0;
		int end = -1;

		for (int i = 0; i < renderGroup.renderBatch.size();) {

			//	need pointer
			RenderQuad &quad = renderGroup.renderBatch[i];
			
			if (quad.textureID == 0) {
				quad.textureID = whiteTexture;
			}
			unsigned int ret = textureCache.bindTexture(quad.textureID);
			
			if (ret == -1) {
				//	no texture slots available. render and loop again without advancing index.
				//  TODO: test this
				flush(&renderGroup, begin, end);
				begin = end + 1;
			} else {
				quad.textureID = ret;
				end++;

				//	reach max batch size, flush em
				if ((end - begin + 1) >= MAX_QUADS_PER_BATCH) {
					flush(&renderGroup, begin, end);
					begin = end + 1;
					end = begin - 1;
				}
				i++;
			}
		}
		
		if (end >= begin) {
			flush(&renderGroup, begin, end);
		}
		
		//  this here open ballet is for errrone
		renderGroup.renderBatch.clear();
    }
	
	// disable VAO and IBO
	CALL_GL(glBindVertexArray(0));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	
	renderGroups.clear();
	inScene = false;
}
		
}

