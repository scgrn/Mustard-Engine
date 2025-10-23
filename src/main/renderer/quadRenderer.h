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
};

class QuadRenderer : public RenderLayer {
    public:
        static constexpr u32 MAX_VERTICES = 65536;

        QuadRenderer();
        ~QuadRenderer();

        void addQuad(Quad3d& quad, GLuint textureID);
        void render(PerspectiveCamera &camera);

        static AB::Shader quadShader;

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

