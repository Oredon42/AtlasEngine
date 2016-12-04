#include <QImage>

#include "material.h"

Material::Material() :
    m_diffuse(glm::vec3(1.0, 0.0, 1.0)),
    m_specular(glm::vec3(1.0, 1.0, 1.0)),
    m_roughness(0.1f),
    m_metalness(1.f),
    m_refraction(1.2f),
    m_textures(0)
{

}

Material::Material(const aiMaterial *ai_material, const GLboolean &animated, const std::string &path, std::vector<Texture *> &textures_loaded)
{
    //  Attributes
    aiColor3D color;
    ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    m_diffuse = glm::vec3(color.r, color.g, color.b);
    ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    m_specular = glm::vec3(color.r, color.g, color.b);
    /*ai_material->Get(AI_MATKEY_OPACITY, m_opacity);
    ai_material->Get(AI_MATKEY_SHININESS, m_shininess);
    ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, m_metalness);
    ai_material->Get(AI_MATKEY_REFRACTI, m_refraction);*/

    //m_roughness = 1.f;

    //  Textures
    std::vector<Texture *> diffuseMaps = loadMaterialTextures(ai_material, aiTextureType_DIFFUSE, "texture_diffuse", path, textures_loaded);
    m_textures.insert(m_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture *> specularMaps = loadMaterialTextures(ai_material, aiTextureType_SPECULAR, "texture_specular", path, textures_loaded);
    m_textures.insert(m_textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture *> normalMaps = loadMaterialTextures(ai_material, aiTextureType_NORMALS, "texture_normal", path, textures_loaded);
    m_textures.insert(m_textures.end(), normalMaps.begin(), normalMaps.end());

    m_shader_type = getShaderType(animated, diffuseMaps.size(), specularMaps.size(), normalMaps.size());

    //  Normal map check
    m_has_normal_map = GL_FALSE;
    if(normalMaps.size() > 0)
        m_has_normal_map = GL_TRUE;
}

/*
 *  Load textures from aiMaterial
 * */
std::vector<Texture *> Material::loadMaterialTextures(const aiMaterial *ai_material, const aiTextureType &type, const std::string &type_name, const std::string &path, std::vector<Texture *> &textures_loaded)
{
    std::vector<Texture *> textures;

    //  For each texture
    for(GLuint i = 0; i < ai_material->GetTextureCount(type); i++)
    {
        aiString ai_texture_path;
        ai_material->GetTexture(type, i, &ai_texture_path);
        GLboolean skip = GL_FALSE;

        //  Check if texture is not already loaded
        for(GLuint j = 0; j < textures_loaded.size(); ++j)
        {
            if(textures_loaded[j]->getPath() == ai_texture_path.C_Str())
            {
                //  If texture already loaded, push it back
                textures.push_back(textures_loaded[j]);
                skip = GL_TRUE;
                break;
            }
        }
        if(!skip)
        {
            //  If not already loaded, load it
            textures.push_back(textureFromFile(path, ai_texture_path.C_Str(), type_name));
            textures_loaded.push_back(textures[textures.size()-1]);
        }
    }
    return textures;
}

/*
 *  Load texture from a file
 *  Returns the id of the texture
 * */
Texture *Material::textureFromFile(const std::string &directory, const GLchar *path, const std::string &shading_type) const
{
    //  Concatenate directory path to filename
    std::string filename;
    if(path[0] == '/')
        filename = path;
    else
        filename = directory + '/' + std::string(path);

    QImage image_container(filename.c_str());
    Texture *texture = new Texture(GL_RGBA, image_container.width(), image_container.height(), GL_BGRA, GL_UNSIGNED_BYTE, image_container.bits(), GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, path, shading_type);
    texture->generateMipmaps();
    return texture;
}

void Material::sendDatas(const Shader &shader) const
{
    if(m_textures.size() == 0)
    {
        //  No texture
        glUniform3f(glGetUniformLocation(shader.getProgram(), "material.diffuse"), m_diffuse.x, m_diffuse.y, m_diffuse.z);
        glUniform3f(glGetUniformLocation(shader.getProgram(), "material.specular"), m_specular.x, m_specular.y, m_specular.z);
    }
    else
    {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        GLuint normalNr = 1;

        for(GLuint i = 0; i < m_textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::stringstream ss;
            std::string number;
            std::string name = m_textures[i]->getShadingType();
            if(name == "texture_diffuse")
                ss << diffuseNr++;
            else if(name == "texture_specular")
                ss << specularNr++;
            else if(name == "texture_normal")
                ss << normalNr++;

            number = ss.str();
            glUniform1i(glGetUniformLocation(shader.getProgram(), ("material." + name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i]->getId());
        }
    }

    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.shininess"), m_shininess);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.roughness"), m_roughness);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.metalness"), m_metalness);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.refraction"), m_refraction);
}
