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

namespace AB {

GLenum Skybox::filter = GL_NEAREST;

void Skybox::init() {
    constexpr f32 skyboxVertices[] = {
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
    
    //    initialize VAO
    CALL_GL(glGenVertexArrays(1, &vao));
    CALL_GL(glGenBuffers(1, &vbo));
    CALL_GL(glBindVertexArray(vao));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW));
    CALL_GL(glEnableVertexAttribArray(0));
    CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    CALL_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle));
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
    //    load shader
    shader.load("shaders/skybox");
    shader.bind();
    shader.setInt("skybox", 0);
}

Skybox::Skybox(std::vector<std::string> faces) {
    if (faces.size() != 6) {
        ERR("Skyboxes have SIX (%d) faces. Six!", 6);
    }

    //    load cubemap
    CALL_GL(glGenTextures(1, &glHandle));
    CALL_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle));
    
    for (u32 i = 0; i < faces.size(); i++) {
        Image image(faces[i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.width, image.height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    }

    init();
}

Skybox::Skybox(std::string cubemap) {
    Image image(cubemap);

    u32 faceW = image.width / 4;
    u32 faceH = image.height / 3;

    if (image.width % 4 != 0 || image.height % 3 != 0) {
        ERR("Cubemap dimensions must be divisible by 4 and 3.");
    }

    CALL_GL(glGenTextures(1, &glHandle));
    CALL_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle));

    static const u32 cells[6][2] = {
        {2, 1},  //  +X
        {0, 1},  //  -X
        {1, 0},  //  +Y
        {1, 2},  //  -Y
        {1, 1},  //  +Z
        {3, 1},  //  -Z
    };

    for (u32 i = 0; i < 6; i++) {
        u32 cx = cells[i][0];
        u32 cy = cells[i][1];
        u8* imageData = new u8[faceW * faceH * 4];
        for (u32 y = 0; y < faceH; y++) {
            u32 srcOfs = ((cy * faceH + y) * image.width + cx * faceW) * 4;
            memcpy(&imageData[y * faceW * 4], &image.data[srcOfs], faceW * 4);
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
            faceW, faceH, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        delete[] imageData;
    }

    init();
}
    
Skybox::~Skybox() {
    shader.release();

    glDeleteTextures(1, &glHandle);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Skybox::render(const PerspectiveCamera& camera) {
    shader.bind();
    
    Mat4 viewMatrix = camera.viewMatrix;
    viewMatrix = Mat4(Mat3(viewMatrix)); // remove translation from the view matrix

    shader.setMat4("projection", camera.projectionMatrix);
    shader.setMat4("view", viewMatrix);

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

}
