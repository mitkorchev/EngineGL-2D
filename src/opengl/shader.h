#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "../common/common.h"

#include "uniform_data.h"

/*
*   Shader class
*   Represents a named program object with several attached shaders to it.
*   This means a vertex and a fragment shader.
* 
*   For the purposes of creating this framework,
*   An OpenGL *program* object, which contains several attached *shader* objects
*   within it, will be called a *shader* for convenience.
*/

class Shader {

    unsigned int m_ProgramID = 0;
    std::string m_ShaderName;

private:    //  UniformName -> UniformLocation map + type 

    struct UniformLocation {
        int location;
        GLenum type;
    };
    
    std::unordered_map<std::string, UniformLocation> m_UniformLocationMap;

    void InitialiseUniformLocationMap();

    std::unordered_map<std::string, unsigned int> m_UniformBlockLocationMap;

    void InitialiseUniformBlockLocationMap();

    bool IsUniformBlock(
        const char* _uniformBlockName
    );

private:

    int m_ModelMatrixUniformLocation = -100;
    int m_ViewMatrixUniformLocation = -100;
    int m_ProjectionMatrixUniformLocation = -100;

    size_t c_StandardUniformCount = 3;
    std::string c_StandardUniformNames[3] = {
        "u_Model",
        "u_View",
        "u_Projection"
    };

    bool IsStandardUniform(
        const char* _uniformName
    );

public:     

    Shader() {}
    
    Shader(
        uint32_t programId,
        const std::string& shaderName
    );

    void ApplyUniforms(
        const UniformDataVector* _uniformArray
    ) const;

    unsigned int GetUniformBlockLocation(
        const char* _uniformBlockName
    ) const;

public:

    void SetStandardModel(
        const glm::mat4& _model
    ) const;

    void SetStandardView(
        const glm::mat4& _view
    ) const;

    void SetStandardProjection(
        const glm::mat4& _projection
    ) const;

    void SetIntArray(
        const char* _uniformName,
        const int* _array,
        int _arraySize
    ) const;

    void SetFloatArray(
        const char* _uniformName,
        const float* _array,
        int _arraySize
    ) const;

    void SetMat4(
        const char* _uniformName,
        const glm::mat4& _uniformValue
    ) const;

    void SetFloat(
        const char* _uniformName,
        const float _uniformValue
    ) const;

    void SetInt(
        const char* _uniformName,
        const int _uniformValue
    ) const;

    void SetVec2(
        const char* _uniformName,
        const glm::vec2 _uniformValue
    ) const;

    const void UseShader() const;

public:

    int GetUniformLocation(
        const char* _uniformName
    ) const;

    const unsigned int GetShaderId() const;
    const std::string& GetName() const;

};

#define GLCall(x) GLClearError();\
    x;\
    DEBUG_ASSERT(GLLogCall(#x, __FILE__, __LINE__), "OpenGL Error from [%s]", #x)

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        return false;
    }
    return true;
}