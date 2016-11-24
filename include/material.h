#ifndef MATERIAL_H
#define MATERIAL_H

#ifndef OGL_DEF
#define OGL_DEF
#ifdef __APPLE__
    #include <QGLWidget>
#elif __linux__
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#elif _WIN32
    #include "lib/glew/glew.h"
#endif
#endif

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#include "shader.h"

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;

    Texture(const GLuint &i, const std::string &t, const std::string &p) :
        id(i),
        type(t),
        path(p)
    {

    }
};

class Material
{
public:
    Material();
    Material(const aiMaterial *ai_material, const GLboolean &animated, const std::string &path, std::vector<Texture *> &textures_loaded);

    std::vector<Texture *> loadMaterialTextures(const aiMaterial *ai_material, const aiTextureType &type, const std::string &type_name, const std::string &path, std::vector<Texture *> &textures_loaded);
    GLint textureFromFile(const std::string &directory, const GLchar *path) const;

    void sendDatas(const Shader &shader) const;

    //  Getters
    inline GLboolean hasNormalMap() const{return m_has_normal_map;}
    inline GLuint getShaderTypeIndex() const{return m_shader_type.index;}

private:
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
    GLfloat m_shininess;
    GLfloat m_roughness;
    GLfloat m_metalness;
    GLfloat m_refraction;

    GLboolean m_has_normal_map;

    std::vector<Texture *> m_textures;

    ShaderType m_shader_type;
};

#endif // MATERIAL_H