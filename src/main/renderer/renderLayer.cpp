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

#include "renderLayer.h"

static const int MAX_QUADS_PER_BATCH = 4096;

namespace AB {

// common quad mesh
GLfloat RenderLayer::quadVertices[] = {
    0.5f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
};
GLuint RenderLayer::quadElements[] = {
    0, 1, 2,
    2, 3, 0,
};

Shader RenderLayer::defaultBatchShader;
Shader RenderLayer::defaultShader;

TextureCache RenderLayer::textureCache;

static bool initialized = false;

RenderLayer::RenderLayer(Shader *batchShader, Shader *shader, Mat4 colorTransform, bool depthSorting) {
    //    load default shaders
    if (!initialized) {
        defaultBatchShader.load("shaders/instanced2d");
        defaultShader.load("shaders/default2d");
        
        initialized = true;
    }
    this->batchShader = batchShader == nullptr ? &defaultBatchShader : batchShader;
    this->shader = shader == nullptr ? &defaultShader : shader;

    // create VAO
    CALL_GL(glGenVertexArrays(1, &batchVAO));
    CALL_GL(glBindVertexArray(batchVAO));
    
    // create vertex VBO
    CALL_GL(glGenBuffers(1, &batchVBO));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, batchVBO));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, &quadVertices[0], GL_STATIC_DRAW));

    // set vertex attribute pointer
    CALL_GL(glEnableVertexAttribArray(0));
    CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    CALL_GL(glVertexAttribDivisor(0, 0));    // default buy hey clarity      

    // create EBO
    CALL_GL(glGenBuffers(1, &batchEBO));
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchEBO));
    CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, &quadElements[0], GL_STATIC_DRAW));
    
    // create IBO
    CALL_GL(glGenBuffers(1, &batchIBO));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, batchIBO));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Quad) * MAX_QUADS_PER_BATCH, NULL, GL_DYNAMIC_DRAW));
    
    // set instance attribute pointers
    
    //        position
    CALL_GL(glEnableVertexAttribArray(1));
    CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)0));
    CALL_GL(glVertexAttribDivisor(1, 1));  

    //        size
    CALL_GL(glEnableVertexAttribArray(2));
    CALL_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 3)));
    CALL_GL(glVertexAttribDivisor(2, 1));  
    
    //        scale
    CALL_GL(glEnableVertexAttribArray(3));
    CALL_GL(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 5)));
    CALL_GL(glVertexAttribDivisor(3, 1));  

    //        rotation
    CALL_GL(glEnableVertexAttribArray(4));
    CALL_GL(glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 7)));
    CALL_GL(glVertexAttribDivisor(4, 1));  

    //        uv
    CALL_GL(glEnableVertexAttribArray(5));
    CALL_GL(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 8)));
    CALL_GL(glVertexAttribDivisor(5, 1));  

    //        texture unit
    CALL_GL(glEnableVertexAttribArray(6));
    CALL_GL(glVertexAttribIPointer(6, 1, GL_INT, sizeof(Quad), (void*)(sizeof(GLfloat) * 12)));
    CALL_GL(glVertexAttribDivisor(6, 1));  

    //        color
    CALL_GL(glEnableVertexAttribArray(7));
    CALL_GL(glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Quad), (void*)(sizeof(GLfloat) * 12 + sizeof(GLint))));
    CALL_GL(glVertexAttribDivisor(7, 1));
    
    quadBatch.reserve(MAX_QUADS_PER_BATCH);
    quadBatch.clear();
    
    this->colorTransform = colorTransform;
    this->depthSorting = depthSorting;
    
    //    ------------ non-batch

    // allocate resources for immediate-mode emulation
    CALL_GL(glGenVertexArrays(1, &VAO));
    CALL_GL(glBindVertexArray(VAO));
    CALL_GL(glGenBuffers(1, &VBO));

    currentColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    setLineWidth(1.0f);
}

RenderLayer::~RenderLayer() {
    glDeleteBuffers(1, &batchVAO);
    glDeleteBuffers(1, &batchEBO);
    glDeleteBuffers(1, &batchVBO);
    glDeleteBuffers(1, &batchIBO);

    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void RenderLayer::setLineWidth(float width) {
    CALL_GL(glLineWidth(width));
    halfWidth = width / 2.0f;
}

void RenderLayer::renderQuad(const Quad& quad) {
    quadBatch.push_back(quad);
}

void RenderLayer::flush(int begin, int end) {
    //  massive sinkhole on chemical road
    CALL_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, (end - begin + 1) * sizeof(Quad), &quadBatch[begin]));
    CALL_GL(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, end - begin + 1));
    textureCache.advanceFrame();
}

void RenderLayer::renderBatch(const Camera& camera) {
    //    set transformation uniforms
    batchShader->bind();
    batchShader->setMat4("projection", camera.projectionMatrix);

    //    it renders
    
    // enable VAO
    CALL_GL(glBindVertexArray(batchVAO));
    
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, batchIBO));
    
    //    TODO: don't bind shaders redundantly!
    batchShader->bind();
    
    // set colorTransform uniform
    batchShader->setMat4("colorTransform", colorTransform);
    
    // sort quadBatch
    std::stable_sort(quadBatch.begin(), quadBatch.end(), depthSorting ? cmpDepth : cmp);
    
    //    iterate renderbatch, set textureID to texture unit for each Quad before populating batchIBO
    int begin = 0;
    int end = -1;

    for (int i = 0; i < quadBatch.size();) {
        //    need pointer
        Quad &quad = quadBatch[i];
        
        if (quad.textureID == 0) {
            quad.textureID = whiteTexture;
        }
        unsigned int ret = textureCache.bindTexture(quad.textureID);
        
        if (ret == -1) {
            //    no texture slots available. render and loop again without advancing index.
            //  TODO: test this
            flush(begin, end);
            begin = end + 1;
        } else {
            quad.textureID = ret;
            end++;

            //    reach max batch size, flush em
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
    quadBatch.clear();

    // disable VAO and IBO
    CALL_GL(glBindVertexArray(0));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void RenderLayer::render(const Camera& camera) {
    renderBatch(camera);
    
    //    set transformation uniforms
    shader->bind();
    shader->setMat4("projection", camera.projectionMatrix);

    // set colorTransform uniform
    shader->setMat4("colorTransform", colorTransform);

    CALL_GL(glBindVertexArray(VAO));

    for (auto renderItem : renderItems) {
        // bind texture
        u32 slot = textureCache.bindTexture(renderItem.texture);
        shader->setInt("Texture", slot);
        
        CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
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
    textureCache.advanceFrame();
}

void RenderLayer::begin(GLenum mode, GLuint texture) {
    currentRenderItem.vertices.clear();
    currentRenderItem.mode = mode;
    currentRenderItem.texture = texture;
}

void RenderLayer::addVertex(float x, float y, float z) {
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

void RenderLayer::addVertex(float x, float y, float u, float v) {
    currentRenderItem.vertices.push_back(x);
    currentRenderItem.vertices.push_back(y);
    currentRenderItem.vertices.push_back(-1);
    currentRenderItem.vertices.push_back(u);
    currentRenderItem.vertices.push_back(v);
    currentRenderItem.vertices.push_back(currentColor.r);
    currentRenderItem.vertices.push_back(currentColor.g);
    currentRenderItem.vertices.push_back(currentColor.b);
    currentRenderItem.vertices.push_back(currentColor.a);
}

void RenderLayer::addVertex(float x, float y, float r, float g, float b, float a) {
    currentRenderItem.vertices.push_back(x);
    currentRenderItem.vertices.push_back(y);
    currentRenderItem.vertices.push_back(-1);
    currentRenderItem.vertices.push_back(0.0f); // u
    currentRenderItem.vertices.push_back(0.0f); // v
    currentRenderItem.vertices.push_back(r);
    currentRenderItem.vertices.push_back(g);
    currentRenderItem.vertices.push_back(b);
    currentRenderItem.vertices.push_back(a);
}

void RenderLayer::addVertex(float x, float y, float u, float v, float r, float g, float b, float a) {
    currentRenderItem.vertices.push_back(x);
    currentRenderItem.vertices.push_back(y);
    currentRenderItem.vertices.push_back(-1);
    currentRenderItem.vertices.push_back(u);
    currentRenderItem.vertices.push_back(v);
    currentRenderItem.vertices.push_back(r);
    currentRenderItem.vertices.push_back(g);
    currentRenderItem.vertices.push_back(b);
    currentRenderItem.vertices.push_back(a);
}

void RenderLayer::end() {
    renderItems.push_back(currentRenderItem);
}

void RenderLayer::renderTri(float x1, float y1, float x2, float y2, float x3, float y3, bool full) {
    begin(full ? GL_TRIANGLES : GL_LINE_LOOP);
        addVertex(x1, y1);
        addVertex(x2, y2);
        addVertex(x3, y3);
    end();
}

void RenderLayer::renderRectangle(float x1, float y1, float x2, float y2, bool full) {
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

void RenderLayer::renderCircle(float x, float y, float radius, bool full, int segments) {
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

void RenderLayer::renderArc(float x, float y, float radius, float angle1, float angle2, int segments) {
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

void RenderLayer::renderRoundedRectangle(float x, float y, float w, float h, float radius, bool full, int segments) {
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

void RenderLayer::renderLines(float endpoints[], int lineCount) {
    begin(GL_LINES);
        for (int i = 0; i < lineCount * 2; i++) {
            addVertex(endpoints[i * 2], endpoints[i * 2 + 1]);
        }
    end();
}

}
