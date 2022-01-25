/**

zlib License

(C) 2022 Andrew Krause

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

#include "geometryRenderer.h"

namespace AB {

Shader GeometryRenderer::defaultShader;
static bool defaultShaderLoaded = false;

GeometryRenderer::GeometryRenderer(Shader *shader, Mat4 colorTransform) {
	//	load default shader
	if (!defaultShaderLoaded) {
		defaultShader.load("shaders/default2d");
		defaultShader.bind();
		defaultShaderLoaded = true;
	}
	
    // allocate resources for immediate-mode emulation
    CALL_GL(glGenVertexArrays(1, &vao));
    CALL_GL(glBindVertexArray(vao));
    CALL_GL(glGenBuffers(1, &vbo));

	currentColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    setLineWidth(1.0f);

	this->shader = shader == nullptr ? &defaultShader : shader;
	this->colorTransform = colorTransform;
}

GeometryRenderer::~GeometryRenderer() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void GeometryRenderer::setLineWidth(float width) {
    CALL_GL(glLineWidth(width));
    halfWidth = width / 2.0f;
}

void GeometryRenderer::render(const Camera& camera) {
	//	set transformation uniforms
	shader->bind();
	shader->setMat4("projection", camera.projectionMatrix);

	CALL_GL(glBindVertexArray(vao));

	for (auto renderItem : renderItems) {
		// bind texture
		u32 slot = Renderer::textureCache.bindTexture(renderItem.texture);
		shader->setInt("Texture", slot);
		
		CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * renderItem.vertices.size(), &renderItem.vertices[0], GL_DYNAMIC_DRAW));

		// vertices
		CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0));
		CALL_GL(glEnableVertexAttribArray(0));

		// texture coords
		CALL_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));
		CALL_GL(glEnableVertexAttribArray(1));

		// colors
		CALL_GL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat))));
		CALL_GL(glEnableVertexAttribArray(2));

		// render!
		CALL_GL(glDrawArrays(renderItem.mode, 0, renderItem.vertices.size() / 9));
	}

    CALL_GL(glBindVertexArray(0));
	
	renderItems.clear();
	Renderer::textureCache.advanceFrame();
}

void GeometryRenderer::begin(GLenum mode, GLuint texture) {
	currentRenderItem.vertices.clear();
	currentRenderItem.mode = mode;
	currentRenderItem.texture = texture;
}

void GeometryRenderer::addVertex(float x, float y, float z) {
    currentRenderItem.vertices.push_back(x);
    currentRenderItem.vertices.push_back(y);
    currentRenderItem.vertices.push_back(z);
    currentRenderItem.vertices.push_back(0.0f); // u
    currentRenderItem.vertices.push_back(0.0f); // v
    currentRenderItem.vertices.push_back(currentColor.r);
    currentRenderItem.vertices.push_back(currentColor.g);
    currentRenderItem.vertices.push_back(currentColor.b);
    currentRenderItem.vertices.push_back(currentColor.a);
}

void GeometryRenderer::end() {
	renderItems.push_back(currentRenderItem);
}

void GeometryRenderer::renderTri(float x1, float y1, float x2, float y2, float x3, float y3, bool full) {
    begin(full ? GL_TRIANGLES : GL_LINE_LOOP);
		addVertex(x1, y1);
		addVertex(x2, y2);
		addVertex(x3, y3);
    end();
}

void GeometryRenderer::renderRectangle(float x1, float y1, float x2, float y2, bool full) {
    if (full) {
		begin(GL_TRIANGLES);
			addVertex(x1,y1);
			addVertex(x2,y1);
			addVertex(x1,y2);
			addVertex(x1,y2);
			addVertex(x2,y1);
			addVertex(x2,y2);
		end();
    } else {
        renderRectangle(x1 + halfWidth, y1 - halfWidth, x2 - halfWidth, y1 + halfWidth);
        renderRectangle(x2 - halfWidth, y1 - halfWidth, x2 + halfWidth, y2 + halfWidth);
        renderRectangle(x1 + halfWidth, y2 - halfWidth, x2 - halfWidth, y2 + halfWidth);
        renderRectangle(x1 - halfWidth, y1 - halfWidth, x1 + halfWidth, y2 + halfWidth);
    }
}

void GeometryRenderer::renderCircle(float x, float y, float radius, bool full, int segments) {
	begin(full ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
		//  center
		if (full) {
			addVertex(x,y);
		}
		
		for (int i = 0; i <= segments; i++) {
			float theta = (M_PI * 2 / segments) * i;
			addVertex(cos(theta) * radius + x, sin(theta) * radius + y);
		}
	end();
}

void GeometryRenderer::renderArc(float x, float y, float radius, float angle1, float angle2, int segments) {
	begin(GL_TRIANGLE_FAN);
		//  center
		addVertex(x,y);

		float rad1 = toRadians(angle1);
		float rad2 = toRadians(angle2);

		if (rad1 > rad2) {
			float temp = rad1;
			rad1 = rad2;
			rad2 = temp;
		}

		float theta = rad1;
		float inc = (rad2 - rad1) / (segments);
		for (int i = 0; i <= segments; i++) {
			addVertex(cos(theta) * radius + x, y - (sin(theta) * radius));
			theta += inc;
		}
	end();
}

void GeometryRenderer::renderRoundedRectangle(float x, float y, float w, float h, float radius, bool full, int segments) {
	begin(full ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
		//  center
		if (full) {
			addVertex(x + w / 2.0f, y + h / 2.0f);
		}

		//  top right
		for (int i = 0; i <= segments; i++ ) {
			float theta = (M_PI / 2.0f) / segments * i;
			float cx = cos(theta) * radius;
			float cy = sin(theta) * radius;
			addVertex(x + w - radius + cx, y + radius - cy);
		}

		//  top left
		for (int i = 0; i <= segments; i++ ) {
			float theta = (M_PI / 2.0f) + (M_PI / 2.0f) / segments * i;
			float cx = cos(theta) * radius;
			float cy = sin(theta) * radius;
			addVertex(x + radius + cx, y + radius - cy);
		}

		//  bottom left
		for (int i = 0; i <= segments; i++ ) {
			float theta = M_PI + (M_PI / 2.0f) / segments * i;
			float cx = cos(theta) * radius;
			float cy = sin(theta) * radius;
			addVertex(x + radius + cx, y + h - radius - cy);
		}

		//  bottom right
		for (int i = 0; i <= segments; i++ ) {
			float theta = (M_PI * 1.5f) + (M_PI / 2.0f) / segments * i;
			float cx = cos(theta) * radius;
			float cy = sin(theta) * radius;
			addVertex(x + w - radius + cx, y + h - radius - cy);
		}

		if (full) {
			addVertex(x + w, y + radius);
		}
	end();
}

}
