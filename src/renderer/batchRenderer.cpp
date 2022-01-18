/**

zlib License

(C) 2021 Andrew Krause

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

#include "batchRenderer.h"

static const int MAX_QUADS_PER_BATCH = 4096;

namespace AB {

// common quad mesh
GLfloat BatchRenderer::quadVertices[] = {
	0.5f,  0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
};
GLuint BatchRenderer::quadElements[] = {
	0, 1, 2,
	2, 3, 0,
};

Shader BatchRenderer::defaultShader;
static bool defaultShaderLoaded = false;

BatchRenderer::BatchRenderer(Shader *shader, Mat4 colorTransform, bool depthSorting) {
	//	load default shader
	if (!defaultShaderLoaded) {
		defaultShader.load("shaders/default2d");
		defaultShader.bind();
		defaultShaderLoaded = true;
	}

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
	CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Quad) * MAX_QUADS_PER_BATCH, NULL, GL_DYNAMIC_DRAW));
	
	// set instance attribute pointers
	
	//		position
    CALL_GL(glEnableVertexAttribArray(1));
	CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)0));
	CALL_GL(glVertexAttribDivisor(1, 1));  

	//		size
    CALL_GL(glEnableVertexAttribArray(2));
	CALL_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 3)));
	CALL_GL(glVertexAttribDivisor(2, 1));  
	
	//		scale
    CALL_GL(glEnableVertexAttribArray(3));
	CALL_GL(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 5)));
	CALL_GL(glVertexAttribDivisor(3, 1));  

	//		rotation
    CALL_GL(glEnableVertexAttribArray(4));
	CALL_GL(glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 7)));
	CALL_GL(glVertexAttribDivisor(4, 1));  

	//		uv
    CALL_GL(glEnableVertexAttribArray(5));
	CALL_GL(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 8)));
	CALL_GL(glVertexAttribDivisor(5, 1));  

	//		texture unit
    CALL_GL(glEnableVertexAttribArray(6));
	CALL_GL(glVertexAttribIPointer(6, 1, GL_INT, sizeof(Quad), (void*)(sizeof(GLfloat) * 12)));
	CALL_GL(glVertexAttribDivisor(6, 1));  

	//		color
    CALL_GL(glEnableVertexAttribArray(7));
	CALL_GL(glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 12 + sizeof(GLint))));
	CALL_GL(glVertexAttribDivisor(7, 1));
	
	renderBatch.reserve(MAX_QUADS_PER_BATCH);
	renderBatch.clear();
	
	this->shader = shader == nullptr ? &defaultShader : shader;
	this->colorTransform = colorTransform;
	this->depthSorting = depthSorting;
}

BatchRenderer::~BatchRenderer() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void BatchRenderer::renderQuad(const Quad& quad) {
	renderBatch.push_back(quad);
}

void BatchRenderer::flush(int begin, int end) {
	//  massive sinkhole on chemical road
	CALL_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, (end - begin + 1) * sizeof(Quad), &renderBatch[begin]));
	CALL_GL(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, end - begin + 1));
	Renderer::textureCache.advanceFrame();
}

void BatchRenderer::render(const Camera& camera) {
	//	set transformation uniforms
	shader->bind();
	shader->setMat4("projection", camera.projectionMatrix);

	//	it renders
	
	// enable VAO
	CALL_GL(glBindVertexArray(vao));
	
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, ibo));
	
	//	TODO: don't bind shaders redundantly!
	shader->bind();
	
	// set colorTransform uniform
	shader->setMat4("colorTransform", colorTransform);
	
	// sort renderBatch
	std::sort(renderBatch.begin(), renderBatch.end(), depthSorting ? cmpDepth : cmp);
	
	//	iterate renderbatch, set textureID to texture unit for each Quad before populating ibo
	int begin = 0;
	int end = -1;

	for (int i = 0; i < renderBatch.size();) {
		//	need pointer
		Quad &quad = renderBatch[i];
		
		if (quad.textureID == 0) {
			quad.textureID = Renderer::whiteTexture;
		}
		unsigned int ret = Renderer::textureCache.bindTexture(quad.textureID);
		
		if (ret == -1) {
			//	no texture slots available. render and loop again without advancing index.
			//  TODO: test this
			flush(begin, end);
			begin = end + 1;
		} else {
			quad.textureID = ret;
			end++;

			//	reach max batch size, flush em
			if ((end - begin + 1) >= MAX_QUADS_PER_BATCH) {
				flush(begin, end);
				begin = end + 1;
				end = begin - 1;
			}
			i++;
		}
	}
	
	if (end >= begin) {
		flush(begin, end);
	}
	
	//  this here open ballet is for errrone
	renderBatch.clear();

	// disable VAO and IBO
	CALL_GL(glBindVertexArray(0));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

}
