#ifndef SHADER_H
#define SHADER_H

#include "openglincludes.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define NB_SHADER_TYPES 16

enum RenderingMethod
{
    DEFFERED,
    FORWARD
};

struct ShaderType
{
    GLboolean texture;
    GLboolean normal;
    GLboolean specular;
    GLboolean animated;
    GLuint index;

    void setValues(const GLboolean &t, const GLboolean &n, const GLboolean &s, const GLboolean &a, const GLuint &i)
    {
        texture = t;
        normal = n;
        specular = s;
        animated = a;
        index = i;
    }
};

ShaderType getShaderType(const GLboolean &is_animated, const GLuint &diffuse_size, const GLuint &specular_size, const GLuint &normal_size);

class Shader
{
public:
    Shader();
    Shader(const ShaderType &shader_type, RenderingMethod rendering_method, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);
    Shader(const std::string &vertexPath, const std::string &fragmentPath, std::string path = "");

    void init(const ShaderType &shader_type, RenderingMethod r,  const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights);
    void init(const std::string &vertexPath, const std::string &fragmentPath, std::string path = "");

    void reload();
    void use();

    //  Getters
    inline GLuint getProgram() const{return m_program;}

    //  Setters
    inline void setNbDirLights(const GLuint &nb_dirlights){m_nb_dirlights = nb_dirlights;}
    inline void setNbPointLights(const GLuint &nb_pointlights){m_nb_pointlights = nb_pointlights;}
    inline void setNbSpotLights(const GLuint &nb_spotlights){m_nb_spotlights = nb_spotlights;}

private:
    GLboolean compileSourceCode(const GLchar *v_shader_code, const GLchar *f_shader_code);
    GLboolean loadSourceFromFiles(std::string &vertex_code, std::string &fragment_code);
    void generateShaderCode(const ShaderType &shader_type, RenderingMethod r, std::string &vertex_code, std::string &fragment_code);

    GLuint m_program;

    std::string m_vertex_saved_path;
    std::string m_fragment_saved_path;

    GLuint m_nb_dirlights;
    GLuint m_nb_pointlights;
    GLuint m_nb_spotlights;

    GLboolean m_initialised;
};

#endif
