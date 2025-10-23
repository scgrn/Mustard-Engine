/**

zlib License

(C) 2025 Andrew Krause

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

#include "quadRenderer.h"
#include "../math/math.h"

namespace AB {

Shader QuadRenderer::quadShader;

static bool initialized = false;

QuadRenderer::QuadRenderer() {
    if (!initialized) {
        quadShader.load("shaders/quadRenderer");

        initialized = true;
    }

    CALL_GL(glGenVertexArrays(1, &batchVAO));
    CALL_GL(glBindVertexArray(batchVAO));

    CALL_GL(glGenBuffers(1, &batchVBO));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, batchVBO));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

    //  position (location 0)
    CALL_GL(glEnableVertexAttribArray(0));
    CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, px)));

    //  normal (location 1)
    CALL_GL(glEnableVertexAttribArray(1));
    CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, nx)));

    //  UV (location 2)
    CALL_GL(glEnableVertexAttribArray(2));
    CALL_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, u)));

    //  color (location 3)
    CALL_GL(glEnableVertexAttribArray(3));
    CALL_GL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, r)));

    CALL_GL(glBindVertexArray(0));
}

QuadRenderer::~QuadRenderer() {
    quadShader.release();

    CALL_GL(glDeleteBuffers(1, &batchVAO));
    CALL_GL(glDeleteBuffers(1, &batchVBO));
}

void QuadRenderer::addQuad(Quad3d& quad, GLuint textureID) {
    static const int indices[6] = { 0, 1, 2, 2, 3, 0};

    batches[textureID].reserve(batches[textureID].size() + 6);

    //  computer surface normal
    AB::Vec3 edge1 = quad.v[1] - quad.v[0];
    AB::Vec3 edge2 = quad.v[2] - quad.v[0];
    AB::Vec3 cross = crossProduct(edge2, edge1);
    AB::Vec3 normal = normalize(cross);

    for (u32 i = 0; i < 6; i++) {
        Vertex v;
        u32 index = indices[i];
        v.px = quad.v[index].x;
        v.py = quad.v[index].y;
        v.pz = quad.v[index].z;
        v.nx = normal.x;
        v.ny = normal.y;
        v.nz = normal.z;
        v.u = quad.uv[index].u;
        v.v = quad.uv[index].v;
        v.r = quad.color.r;
        v.g = quad.color.g;
        v.b = quad.color.b;
        v.a = quad.color.a;
        batches[textureID].push_back(v);
    }
}

void QuadRenderer::render(PerspectiveCamera &camera) {
    quadShader.bind();
    quadShader.setMat4("uProjView", camera.viewProjectionMatrix);
    quadShader.setMat4("uView", camera.viewMatrix);

    CALL_GL(glBindVertexArray(batchVAO));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, batchVBO));

    for (auto& [textureID, verts] : batches) {
        if (verts.empty()) {
            continue;
        }

        CALL_GL(glBindTexture(GL_TEXTURE_2D, textureID));

        u32 vertexCount = min((u32)verts.size(), MAX_VERTICES);
        CALL_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), verts.data()));
        CALL_GL(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexCount));
    }
    CALL_GL(glBindVertexArray(0));
    batches.clear();
}

}

