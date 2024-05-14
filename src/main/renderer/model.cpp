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

#include "model.h"

namespace AB {

Mat4 Model::loadTransform = Mat4();

void Model::setLoadTransform(Mat4 transform) {
    Model::loadTransform = transform;
}

void Model::resetLoadTransform() {
    Model::loadTransform = Mat4();
}

bool Model::loadOBJ(std::string const& filename,
    std::vector<Vec3> &outVertices,
    std::vector<Vec2> &outUVs,
    std::vector<Vec3> &outNormals) {

    DataObject dataObject(filename.c_str());

    std::vector<Vec3> tempVertices;
    std::vector<Vec3> tempNormals;
    std::vector<Vec2> tempUVs;

    std::vector<u32> vertexIndices, uvIndices, normalIndices;

    LOG("Loading OBJ file %s", filename.c_str());

    const char* data = (char*)dataObject.getData();
    if (data == NULL) {
        ERR("COULDN'T LOAD %s", filename.c_str());
        return false;
    }
    std::stringstream input((std::string(data)));     // most vexing parse


    textured = false;
    radius = 0.0f;

    std::string buffer;
    while (getline(input, buffer)) {
        std::istringstream line(buffer);
        std::string line_t;
/*
        if (buffer.substr(0,1) == "o") {
            line >> line_t >> m->name;
        }
        else if (buffer.substr(0,6) == "mtllib") {
            line >> line_t >> m->mtllibFile;
        }
*/
        /* else */ if (buffer.substr(0,2) == "vn") {
            f64 f1, f2, f3;
            line >> line_t >> f1 >> f2 >> f3;
            tempNormals.push_back(Vec3((f32)f1, (f32)f2, (f32)f3) );
        }
        else if (buffer.substr(0,2) == "vt") {
            f64 f1, f2;
            line >> line_t >> f1 >> f2;
            tempUVs.push_back(Vec2((f32)f1, (f32)f2) );
            textured = true;
        }
        else if (buffer.substr(0,1) == "v") {
            f64 f1, f2, f3;
            line >> line_t >> f1 >> f2 >> f3;

            Vec4 vertex = loadTransform * Vec4((f32)f1, (f32)f2, (f32)f3, 1.0f);
            tempVertices.push_back((Vec3)vertex);

            //f32 newRadius = sqrt((f1 * f1) + (f2 * f2) + (f3 * f3));
            f32 newRadius = ((f1 * f1) + (f2 * f2) + (f3 * f3));
            radius = std::max(radius, newRadius);
        }
        else if (buffer.substr(0,1) == "f" && buffer.substr(0,2) != "vt") {
            std::string element;
            line >> line_t;

            while (line >> element) {
                i32 vi, vti, vni;
                std::string::size_type spos = 0;
                std::string::size_type epos;

                epos = element.find('/');
                if (epos == std::string::npos) {
                    std::istringstream vind(element);
                    vind >> vi;
                    vertexIndices.push_back(vi - 1);
                    continue;
                }
                std::istringstream vind(element.substr(spos, epos));
                vind >> vi;
                vertexIndices.push_back(vi - 1);

                spos = epos+1;
                epos = element.find('/', spos);
                if (epos == std::string::npos) {
                    std::istringstream vtind(element.substr(spos));
                    vtind >> vti;
                    uvIndices.push_back(vti - 1);
                    continue;
                }
                std::istringstream vtind(element.substr(spos, epos-spos));
                std::istringstream vnind(element.substr(epos+1));

                vtind >> vti; vnind >> vni;

                uvIndices.push_back(vti - 1);
                normalIndices.push_back(vni - 1);
            }
        }
    }

    
    if (textured) {
        for (u32 i = 0; i < vertexIndices.size(); i++) {
            outVertices.push_back(tempVertices[vertexIndices[i]]);
            outUVs.push_back(tempUVs[uvIndices[i]]);
            outNormals.push_back(tempNormals[normalIndices[i]]);
        }
    } else {
        for (u32 i = 0; i < vertexIndices.size(); i++) {
            outVertices.push_back(tempVertices[vertexIndices[i]]);
            outNormals.push_back(tempNormals[normalIndices[i]]);
        }
    }

    LOG("Loaded.", 0);
    
    return true;
}

bool Model::getSimilarVertexIndex(PackedVertex &packed,
    std::map<PackedVertex, unsigned short> &vertexToOutIndex,
    unsigned short &result) {

    std::map<PackedVertex, unsigned short>::iterator it = vertexToOutIndex.find(packed);
    if (it == vertexToOutIndex.end()) {
        return false;
    } else {
        result = it->second;
        return true;
    }
}

void Model::indexVBO(std::vector<Vec3> &inVertices,
    std::vector<Vec2> &inUVs,
    std::vector<Vec3> &inNormals,

    std::vector<unsigned short> &outIndices,
    std::vector<Vec3> &outVertices,
    std::vector<Vec2> &outUVs,
    std::vector<Vec3> &outNormals) {

    std::map<PackedVertex, unsigned short> vertexToOutIndex;

    LOG_EXP(inVertices.size());
    
    PackedVertex packed;
    for (u32 i = 0; i < inVertices.size(); i++) {
        if (textured) {
            packed = {inVertices[i], inUVs[i], inNormals[i]};
        } else {
            packed = {inVertices[i], Vec2(), inNormals[i]};
        }
        
        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

        if (found) { // A similar vertex is already in the VBO, use it instead !
            outIndices.push_back(index);
        } else { // If not, it needs to be added in the output data.
            outVertices.push_back(inVertices[i]);
            if (textured) {
                outUVs.push_back(inUVs[i]);
            } else {
                outUVs.push_back(Vec2());
            }
        
            outNormals.push_back(inNormals[i]);
            unsigned short newIndex = (unsigned short)outVertices.size() - 1;
            outIndices.push_back(newIndex);
            vertexToOutIndex[packed] = newIndex;
        }
    }
}

void Model::load(std::string const& filename) {
    std::vector<Vec3> vertices;
    std::vector<Vec2> UVs;
    std::vector<Vec3> normals;

    bool res = loadOBJ(filename, vertices, UVs, normals);

    if (!res) {
        // TODO: handle error!
        LOG("ERROR", 0);
    }

    std::vector<unsigned short> indices;
    std::vector<Vec3> indexedVertices;
    std::vector<Vec2> indexedUVs;
    std::vector<Vec3> indexedNormals;

    indexVBO(vertices, UVs, normals, indices, indexedVertices, indexedUVs, indexedNormals);

    // create VAO
    CALL_GL(glGenVertexArrays(1, &vertexArrayID));
    CALL_GL(glBindVertexArray(vertexArrayID));

    // Load it into a VBO
    CALL_GL(glGenBuffers(1, &vertexBuffer));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(Vec3), &indexedVertices[0], GL_STATIC_DRAW));

    if (textured) {
        CALL_GL(glGenBuffers(1, &uvBuffer));
        CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, uvBuffer));
        CALL_GL(glBufferData(GL_ARRAY_BUFFER, indexedUVs.size() * sizeof(Vec2), &indexedUVs[0], GL_STATIC_DRAW));
    }
    
    CALL_GL(glGenBuffers(1, &normalBuffer));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, normalBuffer));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(Vec3), &indexedNormals[0], GL_STATIC_DRAW));

    CALL_GL(glGenBuffers(1, &elementBuffer));
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer));
    CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW));

    indicesSize = indices.size();
}

void Model::release() {
    CALL_GL(glDeleteBuffers(1, &vertexBuffer));
    CALL_GL(glDeleteBuffers(1, &uvBuffer));
    CALL_GL(glDeleteBuffers(1, &normalBuffer));
    CALL_GL(glDeleteBuffers(1, &elementBuffer));
    CALL_GL(glDeleteVertexArrays(1, &vertexArrayID));
}

void Model::render() {
    CALL_GL(glBindVertexArray(vertexArrayID));

    // 1rst attribute buffer : vertices
    CALL_GL(glEnableVertexAttribArray(0));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

    // 2nd attribute buffer : UVs
    CALL_GL(glEnableVertexAttribArray(1));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, uvBuffer));
    CALL_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    // 3rd attribute buffer : normals
    CALL_GL(glEnableVertexAttribArray(2));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, normalBuffer));
    CALL_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

    // Index buffer
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer));

    // draw tris
    CALL_GL(glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, (void*)0));

    CALL_GL(glDisableVertexAttribArray(0));
    CALL_GL(glDisableVertexAttribArray(1));
    CALL_GL(glDisableVertexAttribArray(2));

    CALL_GL(glBindVertexArray(0));
}

}   //  namespace
