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

#ifndef AB_RENDER_LAYER_H
#define AB_RENDER_LAYER_H

#include "camera.h"
#include "shader.h"
#include "textureCache.h"

namespace AB {

extern GLuint whiteTexture;

class RenderLayer {
    public:
        // a 64 svelte bytes
        // TODO: constructor to set defaults
        struct Quad {
            Vec3 pos;
            Vec2 size;
            Vec2 scale;
            float rotation;        // radians
            Vec4 uv;            // {u1, v1, u2, v2}
            GLint textureID;    // set to 0 for white texture
            Vec4 color;        
        };

        RenderLayer(Shader *batchShader = &defaultBatchShader, Shader *shader = &defaultShader, Mat4 colorTransform = Mat4(), bool depthSorting = false);
        virtual ~RenderLayer();
        
        virtual void render(const Camera& camera);

        //    state
        void setLineWidth(float width);
        void setColor(Vec4 color) { currentColor = color; }

        //    immediate mode emulation
        void begin(GLenum mode, GLuint texture = whiteTexture);
        void addVertex(float x, float y, float z = -1.0f);
        void addVertex(float x, float y, float u, float v);
        void addVertex(float x, float y, float r, float g, float b, float a);
        void addVertex(float x, float y, float u, float v, float r, float g, float b, float a);
        //  TODO: should probably also support normals.
        void end();

        //    submits a quad to the batch renderer
        void renderQuad(const Quad& quad);
        
        //    these are queued and render after the quad batch
        void renderTri(float x1, float y1, float x2, float y2, float x3, float y3, bool full = true);
        void renderRectangle(float x1, float y1, float x2, float y2, bool full = true);
        void renderCircle(float x, float y, float radius, bool full = true, int segments = 32);
        void renderArc(float x, float y, float radius, float angle1, float angle2, int segments = 10);
        void renderRoundedRectangle(float x, float y, float w, float h, float radius, bool full = true, int segments = 8);
        void renderLines(float endpoints[], int lineCount); // {x1, y1, x2, y2, ...}
        
        std::vector<Quad> quadBatch;
        Shader *batchShader;
        Shader *shader;

        bool depthSorting;        //    only applies to batch renderer
        
        Mat4 colorTransform;

        static Shader defaultBatchShader;
        static Shader defaultShader;

        static TextureCache textureCache;

    protected:
        // common quad mesh
        static GLfloat quadVertices[];
        static GLuint quadElements[];

    protected:
        //    for std::sort
        static inline bool cmp(const Quad& a, const Quad& b) {
            return a.textureID < b.textureID;
        }

        static inline bool cmpDepth(const Quad& a, const Quad& b) {
            if (a.pos.z == b.pos.z) {
                return a.textureID < b.textureID;
            } else {
                return a.pos.z < b.pos.z;
            }
        }
        
        void flush(int begin, int end);
        void renderBatch(const Camera& camera);

        //    non-batch rendering stuff
        struct RenderItem {
            std::vector<GLfloat> vertices;   // x, y, z, u, v, r, g, b, a
            GLenum mode;
            GLuint texture;
        };
        std::vector<RenderItem> renderItems;
        RenderItem currentRenderItem;

        /// what about all this stuff?/
        //  TODO: move these into render state in renderer.h
        Vec4 currentColor;
        float halfWidth;

        GLuint batchVAO;        //    vertex array object
        GLuint batchVBO;        //    vertex buffer
        GLuint batchEBO;        //    element buffer
        GLuint batchIBO;        //    per-instance vertex buffer

        GLuint VAO;        //    vertex array object
        GLuint VBO;        //    vertex buffer

};

}

#endif
