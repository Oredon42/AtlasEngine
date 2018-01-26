#ifndef SHADER_H
#define SHADER_H

#include <glew.h>
#include <glm/glm.hpp>

#include <unordered_map>

class Shader
{
private:
    struct ShaderDefine
    {
        bool bEnabled;
        std::string strText;
    };
    static constexpr char* GLSL_VERSION = "#version 330 core";

public:
    Shader();
    Shader(const std::string &strVertexPath, const std::string &strFragmentPath, const std::string &strGeometryPath = "");

    void init(const std::string &strVertexPath, const std::string &strFragmentPath, const std::string &strGeometryPath = "");
    void reload();

    inline void use() const;
    inline void release() const{glUseProgram(0);}

    inline void resetActiveTexture() {m_iActiveTexture = 0;}
    inline void activateNextTexture() {++m_iActiveTexture;}

    //  Getters
    inline GLuint getProgram() const{return m_uiProgram;}
    inline GLenum getActiveTextureEnum() const{return static_cast<GLenum>(m_iActiveTexture) + GL_TEXTURE0;}
    inline GLint getActiveTextureInt() const{return m_iActiveTexture;}

    //  Setters

    //  Defines
    //  strKey -> text to write in the shader
    //  bEnabled -> write in shader if true
    void setFragmentDefineEnabled(const std::string &strKey, const bool &bEnabled);
    void setVertexDefineEnabled(const std::string &strKey, const bool &bEnabled);
    void setGeometryDefineEnabled(const std::string &strKey, const bool &bEnabled);
    void setFragmentDefineText(const std::string &strKey, const std::string &strText);
    void setVertexDefineText(const std::string &strKey, const std::string &strText);
    void setGeometryDefineText(const std::string &strKey, const std::string &strText);

    //  Uniforms
    inline void addUniformLocation(const std::string &strUniformName);

    //  use Shader before calling following methods
    inline void setIntUniform(const std::string &strUniformName, const int &iValue);
    inline void setFloatUniform(const std::string &strUniformName, const float &fValue);
    inline void setVec2Uniform(const std::string &strUniformName, const glm::vec2 &vValue);
    inline void setVec3Uniform(const std::string &strUniformName, const glm::vec3 &vValue);
    inline void setVec4Uniform(const std::string &strUniformName, const glm::vec4 &vValue);
    inline void setMat3Uniform(const std::string &strUniformName, const glm::mat3 &vValue);
    inline void setMat4Uniform(const std::string &strUniformName, const glm::mat4 &mValue);

private:
    GLuint m_uiProgram;
    GLint m_iActiveTexture;
    bool m_bInitialised;

    std::string m_strVertexPath;
    std::string m_strFragmentPath;
    std::string m_strGeometryPath;

    std::unordered_map<std::string, ShaderDefine> m_mapFragmentDefines;
    std::unordered_map<std::string, ShaderDefine> m_mapVertexDefines;
    std::unordered_map<std::string, ShaderDefine> m_mapGeometryDefines;

    std::unordered_map<std::string, GLint> m_mapUniformsLocations;

    //  Private methods
    bool compileSourceCode(const std::string &strVertexShaderSource, const std::string &strFragmentShaderSource, const std::string &strGeometryShaderData = "");
    bool loadSourceFromFiles(std::string &strVertexShaderSource, std::string &strFragmentShaderSource, std::string &strGeometryShaderSource);
};

#endif // SHADER_H
