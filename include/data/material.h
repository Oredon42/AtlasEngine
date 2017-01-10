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

#include <unordered_map>

class Material
{
public:
    Material();
    Material(const aiMaterial *ai_material, const GLboolean &animated, const std::string &path, std::vector<Texture *> &textures_loaded);
    Material(const glm::vec3 &color, const GLfloat &roughness, const GLfloat &metalness, const GLfloat &refraction, const GLfloat &opacity, GLboolean has_normal_map = GL_FALSE, std::vector<Texture *> textures = std::vector<Texture *>(), ShaderType shader_type = ShaderType());
    std::vector<Texture *> loadMaterialTextures(const aiMaterial *ai_material, const aiTextureType &type, const std::string &type_name, const std::string &path, std::vector<Texture *> &textures_loaded);
    Texture *textureFromFile(const std::string &directory, const GLchar *path, const std::string &shading_type) const;

    void sendDatas(const Shader &shader) const;

    //  Getters
    inline GLboolean hasNormalMap() const{return m_has_normal_map;}
    inline GLuint getShaderTypeIndex() const{return m_shader_type.index;}

    //  Setters
    void copy(const Material &material);
    inline void copy(const Material *material){copy(*material);}

private:
    glm::vec3 m_color;
    GLfloat m_roughness;
    GLfloat m_metalness;
    GLfloat m_refraction;
    GLfloat m_opacity;

    GLboolean m_has_normal_map;

    std::vector<Texture *> m_textures;

    ShaderType m_shader_type;
};

struct MaterialLibrary
{
    std::unordered_map<std::string, Material *> materials;
    std::unordered_map<std::string, Texture *> textures_loaded;

    ~MaterialLibrary()
    {
        for(std::unordered_map<std::string, Material *>::iterator it = materials.begin(); it != materials.end(); ++it)
            delete it->second;

        materials.clear();

        for(std::unordered_map<std::string, Texture *>::iterator it = textures_loaded.begin(); it != textures_loaded.end(); ++it)
            delete it->second;

        textures_loaded.clear();
    }

    void addMaterial(const aiMaterial *ai_material, const std::string &name, const GLboolean &animated, const std::string &path)
    {
        if(materials.find(name) == materials.end())
        {
            glm::vec3 color;

            GLfloat opacity,
                    metalness,
                    roughness,
                    refraction;

            //  Attributes
            aiColor3D ai_color;
            ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
            color = glm::vec3(color.r, color.g, color.b);
            ai_material->Get(AI_MATKEY_OPACITY, opacity);
            aiColor3D specular;
            ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
            metalness = 1-specular.r;
            ai_material->Get(AI_MATKEY_SHININESS, roughness);
            roughness = 1.f - fmin(0.99f, (roughness * specular.r));
            ai_material->Get(AI_MATKEY_REFRACTI, refraction);


            std::vector<Texture *> textures;

            //  Textures
            std::vector<Texture *> diffuse_maps = loadMaterialTextures(ai_material, aiTextureType_DIFFUSE, "texture_diffuse", path);
            textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
            std::vector<Texture *> specular_maps = loadMaterialTextures(ai_material, aiTextureType_SPECULAR, "texture_specular", path);
            textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
            std::vector<Texture *> normal_maps = loadMaterialTextures(ai_material, aiTextureType_NORMALS, "texture_normal", path);
            textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

            ShaderType shader_type = getShaderType(animated, diffuse_maps.size(), specular_maps.size(), normal_maps.size());

            //  Normal map check
            GLboolean has_normal_map = GL_FALSE;
            if(normal_maps.size() > 0)
                has_normal_map = GL_TRUE;

            materials[name] = new Material(color, roughness, metalness, refraction, opacity, has_normal_map, textures, shader_type);
        }
    }

    void addMaterial(Material *material, const std::string &name)
    {
        if(materials.find(name) == materials.end())
            materials[name] = material;
    }

    /*
     *  Load textures from aiMaterial
     * */
    std::vector<Texture *> loadMaterialTextures(const aiMaterial *ai_material, const aiTextureType &type, const std::string &type_name, const std::string &path)
    {
        std::vector<Texture *> out_textures;
        //  For each texture
        for(GLuint i = 0; i < ai_material->GetTextureCount(type); i++)
        {
            aiString ai_texture_path;
            ai_material->GetTexture(type, i, &ai_texture_path);
            std::string texture_path = ai_texture_path.C_Str();

            //  Check if texture is not already loaded
            if(textures_loaded.find(texture_path) == textures_loaded.end())
                textures_loaded[texture_path] = new Texture(path, ai_texture_path.C_Str(), type_name);

            out_textures.push_back(textures_loaded[texture_path]);
        }
        return out_textures;
    }

    inline Material *getMaterial(const std::string &name) const{return materials.at(name);}
};

#endif // MATERIAL_H
