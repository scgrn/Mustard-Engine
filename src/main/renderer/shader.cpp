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
#include "shader.h"

#include "../core/log.h"
#include "../core/fileSystem.h"

namespace AB {

extern FileSystem fileSystem;

Shader::Shader() {}

Shader::~Shader() {}

void Shader::load(std::string const& filename) {
    const int INFO_LOG_LENGTH = 1024;

    LOG("Loading shader <%s>", filename.c_str());

    DataObject vertexDataObject = fileSystem.loadAsset(filename + ".vert");
    std::string vertexSource = getHeader() + std::string((const char*)vertexDataObject.getData(), vertexDataObject.getSize());
    LOG("VERTEX SHADER SOURCE: %s", vertexSource.c_str());

    DataObject fragmentDataObject = fileSystem.loadAsset(filename + ".frag");
    std::string fragmentSource = getHeader() + std::string((const char*)fragmentDataObject.getData(), fragmentDataObject.getSize());
    LOG("FRAGMENT SHADER SOURCE: %s", fragmentSource.c_str());

    //  compile vertex shader
    GLuint vertexShader;
    CALL_GL(vertexShader = glCreateShader(GL_VERTEX_SHADER));
    
    const char* vertexSourceC = vertexSource.c_str();
    CALL_GL(glShaderSource(vertexShader, 1, &vertexSourceC, NULL));
    CALL_GL(glCompileShader(vertexShader)); // here

    GLint success;
    GLchar infoLog[INFO_LOG_LENGTH];
    CALL_GL(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
    if (!success) {
        CALL_GL(glGetShaderInfoLog(vertexShader, INFO_LOG_LENGTH, NULL, infoLog));
        ERR("%s: Vertex shader compilation failed: %s", filename.c_str(), infoLog);
    }

    // compile fragment shader
    GLuint fragmentShader;
    CALL_GL(fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));

    const char* fragmentSourceC = fragmentSource.c_str();
    CALL_GL(glShaderSource(fragmentShader, 1, &fragmentSourceC, NULL));
    CALL_GL(glCompileShader(fragmentShader));

    CALL_GL(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));
    if (!success) {
        CALL_GL(glGetShaderInfoLog(fragmentShader, INFO_LOG_LENGTH, NULL, infoLog));
        ERR("%s: Fragment shader compilation failed: %s", filename.c_str(), infoLog);
    }

    // shader program
    CALL_GL(shaderProgram = glCreateProgram());
    CALL_GL(glAttachShader(shaderProgram, vertexShader));
    CALL_GL(glAttachShader(shaderProgram, fragmentShader));
    CALL_GL(glLinkProgram(shaderProgram));

    CALL_GL(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success));
    if (!success) {
        CALL_GL(glGetProgramInfoLog(shaderProgram, INFO_LOG_LENGTH, NULL, infoLog));
        ERR("%s: Shader program compilation failed: %s", filename.c_str(), infoLog);
    }
    CALL_GL(glUseProgram(shaderProgram));

    GLint textureSamplers[16];
    for (GLint i = 0; i < 16; i++) {
        textureSamplers[i] = i;
    }
    GLint textureSamplersLoc;
    CALL_GL(textureSamplersLoc = glGetUniformLocation(shaderProgram, "textureSamplers"));
    if (textureSamplersLoc != -1) {
        CALL_GL(glUniform1iv(textureSamplersLoc, 16, textureSamplers));
    }

    CALL_GL(glDeleteShader(vertexShader));
    CALL_GL(glDeleteShader(fragmentShader));

    LOG("Shader compilation successful.", 0);
}

void Shader::release() {
    CALL_GL(glDeleteProgram(shaderProgram));
}

void Shader::bind() {
    CALL_GL(glUseProgram(shaderProgram));
}

void Shader::setInt(const std::string& name, int value) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniform1i(location, value));
}

void Shader::setIntArray(const std::string& name, int* values, uint32_t count) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniform1iv(location, count, values));
}

void Shader::setFloat(const std::string& name, float value) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniform1f(location, value));
}

void Shader::setVec2(const std::string& name, const Vec2& value) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniform2f(location, value.x, value.y));
}

void Shader::setVec3(const std::string& name, const Vec3& value) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::setVec4(const std::string& name, const Vec4& value) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void Shader::setMat3(const std::string& name, const Mat3& matrix) {
    GLint location = getUniformLocation(name);
    CALL_GL(glUniformMatrix3fv(location, 1, GL_FALSE, (f32*)(&matrix)));
}

void Shader::setMat4(const std::string& name, const Mat4& matrix) {
    GLint location = getUniformLocation(name);
    //CALL_GL(glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(matrix)));
    CALL_GL(glUniformMatrix4fv(location, 1, GL_FALSE, (f32*)(&matrix)));
}

GLint Shader::getUniformLocation(const std::string& name) const {
    if (uniformLocations.find(name) != uniformLocations.end()) {
        return uniformLocations[name];
    }
    CALL_GL(uniformLocations[name] = glGetUniformLocation(shaderProgram, name.c_str()));
    return uniformLocations[name];
}

std::string Shader::getHeader() {
    std::string header;

#if defined(__EMSCRIPTEN__) || defined(ANDROID)
    header =
        "#version 300 es\n\n"
        "precision mediump float;\n"
        "precision lowp int;\n\n"
        "#line 0\n";
#else
    header =
        "#version 420 core\n\n"
        "#define precision\n"
        "#define lowp\n"
        "#define mediump\n"
        "#define highp\n\n"
        "#line 0\n";  // s/b -1
#endif

    return header;
}

}   //  namespace

