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

#ifndef AB_QUAD_RENDERER_H
#define AB_QUAD_RENDERER_H

#include "renderLayer.h"

//  Intended to serve as a base for rendering static level geometry in
//  conjunction with some higher-level VSD algorithm

namespace AB {

struct Quad3d {
    AB::Vec3 v[4];
    AB::Vec2 uv[4];
    AB::Vec4 color;
    GLuint textureID;
    AB::Vec3 normal;
    AB::Vec3 min, max;

    void calculateNormal() {
        AB::Vec3 edge1 = v[1] - v[0];
        AB::Vec3 edge2 = v[2] - v[0];
        AB::Vec3 cross = crossProduct(edge2, edge1);
        normal = normalize(cross);
    }

    void calculateExtents() {
        min = v[0];
        max = v[0];
        for (int i = 1; i < 4; i++) {
            min.x = AB::min(min.x, v[i].x);
            min.y = AB::min(min.y, v[i].y);
            min.z = AB::min(min.z, v[i].z);

            max.x = AB::max(max.x, v[i].x);
            max.y = AB::max(max.y, v[i].y);
            max.z = AB::max(max.z, v[i].z);
        }
    }
};

class QuadRenderer : public RenderLayer {
    public:
        static constexpr u32 MAX_VERTICES = 65536;

        QuadRenderer(Shader *quadShader = &defaultQuadShader);
        ~QuadRenderer();

        void setFog(AB::Vec3 color = AB::Vec3(0.0f, 0.0f, 0.0f), f32 density = 0.15f);

        void addQuad(Quad3d& quad);
        void render(PerspectiveCamera &camera);

        Shader *quadShader;
        static AB::Shader defaultQuadShader;

    private:
        struct Vertex {
            f32 px, py, pz;
            f32 nx, ny, nz;
            f32 u, v;
            f32 r, g, b, a;
        };

        std::unordered_map<GLuint, std::vector<Vertex>> batches;

};

}

#endif

