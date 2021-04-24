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

#include "skybox.h"
#include "image.h"

#include "../input/input.h"	// TODO: remove!

namespace AB {

float rx = 0.0f;
float ry = 0.0f;

extern Input input;

Skybox::Skybox(std::vector<std::string> faces) {

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	
	if (faces.size() != 6) {
		ERR("Skyboxes have SIX (%d) faces. Six!", 6);
	}
	
	//	initialize VAO
    CALL_GL(glGenVertexArrays(1, &vao));
    CALL_GL(glGenBuffers(1, &vbo));
    CALL_GL(glBindVertexArray(vao));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW));
    CALL_GL(glEnableVertexAttribArray(0));
    CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	//	load cubemap
	CALL_GL(glGenTextures(1, &glHandle));
	CALL_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle));
	
    for (int i = 0; i < faces.size(); i++) {
		Image image(faces[i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    }
	
	//	TODO: do we want linear?
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
	//	load shader
	shader.load("shaders/skybox");
	shader.bind();
    shader.setInt("skybox", 0);
}
	
Skybox::~Skybox() {
	glDeleteTextures(1, &glHandle);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vao);
}

void Skybox::beginScene(const Camera& camera) {
	shader.bind();
	
	//	TODO: should come from camera!
	float fov = 90.0f;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::rotate(glm::mat4(1.0), ry, glm::vec3(0.0f, 1.0f, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, rx, glm::vec3(1.0f, 0.0f, 0.0f));
	
	const float SPEED = 0.1f;
	if (input.isPressed(79)) {
		ry += SPEED;
	}
	if (input.isPressed(80)) {
		ry -= SPEED;
	}
	if (input.isPressed(82)) {
		rx += SPEED;
	}
	if (input.isPressed(81)) {
		rx -= SPEED;
	}
	
	shader.setMat4("projection", projectionMatrix);
	shader.setMat4("view", viewMatrix);
}

void Skybox::endScene() {
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

}
