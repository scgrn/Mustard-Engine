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

#ifndef AB_SHADER_H
#define AB_SHADER_H

#include <unordered_map>

#include "../core/resourceManager.h"
#include "../math/math.h"

namespace AB {

class Shader : public Resource {
    public:
        Shader();
        virtual ~Shader();

        virtual void load(std::string const& filename);
        virtual void release();
        
        void bind();

        void setInt(const std::string& name, int value);
        void setIntArray(const std::string& name, int* values, uint32_t count);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const Vec2& value);
        void setVec3(const std::string& name, const Vec3& value);
        void setVec4(const std::string& name, const Vec4& value);
        void setMat3(const std::string& name, const Mat3& matrix);
        void setMat4(const std::string& name, const Mat4& matrix);
        
        GLuint getProgram() { return shaderProgram; }
        
    protected:
        static std::string getHeader();

        GLint getUniformLocation(const std::string& name) const;
        mutable std::unordered_map<std::string, GLint> uniformLocations;

        GLuint shaderProgram;
};

}   //  namespace

#endif // SHADER_H
