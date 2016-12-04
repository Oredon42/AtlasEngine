#ifndef MATERIAL_H
#define MATERIAL_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#include "shader.h"
#include "texture.h"

class Material
{
public:
    Material();
    Material(const aiMaterial *ai_material, const GLboolean &animated, const std::string &path, std::vector<Texture *> &textures_loaded);

    std::vector<Texture *> loadMaterialTextures(const aiMaterial *ai_material, const aiTextureType &type, const std::string &type_name, const std::string &path, std::vector<Texture *> &textures_loaded);
    Texture *textureFromFile(const std::string &directory, const GLchar *path, const std::string &shading_type) const;

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
    GLfloat m_opacity;

    GLboolean m_has_normal_map;

    std::vector<Texture *> m_textures;

    ShaderType m_shader_type;
};

#endif // MATERIAL_H
