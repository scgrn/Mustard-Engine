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

#ifndef AB_MODEL_H
#define AB_MODEL_H

#include "../core/assetManager.h"
#include "../math/math.h"

namespace AB {

class Model : public Asset {
    public:
        static void setLoadTransform(Mat4 transform);
        static void resetLoadTransform();

        virtual void load(std::string const& filename);
        virtual void release();
        virtual ~Model() {}

        void render();


        //void render(Visage::Vector3 pos, Visage::Vector3 forward, Visage::Vector3 up);
        float getRadius() { return radius; }

    protected:
        static Mat4 loadTransform;

        struct PackedVertex{
            Vec3 position;
            Vec2 uv;
            Vec3 normal;

            bool operator<(const PackedVertex that) const{
                return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
            };
        };

        bool loadOBJ(std::string const& filename,
            std::vector<Vec3> &outVertices,
            std::vector<Vec2> &outUVs,
            std::vector<Vec3> &outNormals);

        bool getSimilarVertexIndex(PackedVertex &packed,
            std::map<PackedVertex, unsigned short> &vertexToOutIndex,
            unsigned short &result);

        void indexVBO(std::vector<Vec3> &inVertices,
            std::vector<Vec2> &inUVs,
            std::vector<Vec3> &inNormals,

            std::vector<unsigned short> &outIndices,
            std::vector<Vec3> &outVertices,
            std::vector<Vec2> &outUVs,
            std::vector<Vec3> &outNormals);

        GLuint vertexBuffer, uvBuffer, normalBuffer, elementBuffer;
        GLuint vertexArrayID;

        bool textured;
        float radius;
        int indicesSize;
};

}   //  namespace

#endif  //  AB_MODEL_H

