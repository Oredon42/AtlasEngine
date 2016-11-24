#include "material.h"

Material::Material() :
    m_diffuse(glm::vec3(1.0, 0.0, 1.0)),
    m_specular(glm::vec3(1.0, 1.0, 1.0)),
    m_roughness(0.1),
    m_refraction(0),
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
    ai_material->Get(AI_MATKEY_SHININESS, m_shininess);
    ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, m_metalness);
    ai_material->Get(AI_MATKEY_REFRACTI, m_refraction);

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
        aiString str;
        ai_material->GetTexture(type, i, &str);
        GLboolean skip = GL_FALSE;

        //  Check if texture is not already loaded
        for(GLuint j = 0; j < textures_loaded.size(); ++j)
        {
            if(textures_loaded[j]->path == str.C_Str())
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
            Texture *texture = new Texture(textureFromFile(path, str.C_Str()), type_name, str.C_Str());
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

/*
 *  Load texture from a file
 *  Returns the id of the texture
 * */
GLint Material::textureFromFile(const std::string &directory, const GLchar *path) const
{
    //  Concatenate directory path to filename
    std::string filename;
    if(path[0] == '/')
        filename = path;
    else
        filename = directory + '/' + std::string(path);

    GLuint texture_ID;
    glGenTextures(1, &texture_ID);

    glBindTexture(GL_TEXTURE_2D, texture_ID);
    QImage image_container(filename.c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_container.width(), image_container.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image_container.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_ID;
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
            std::string name = m_textures[i]->type;
            if(name == "texture_diffuse")
                ss << diffuseNr++;
            else if(name == "texture_specular")
                ss << specularNr++;
            else if(name == "texture_normal")
                ss << normalNr++;

            number = ss.str();
            glUniform1i(glGetUniformLocation(shader.getProgram(), ("material." + name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i]->id);
        }
    }

    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.shininess"), m_shininess);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.roughness"), m_roughness);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.metalness"), 0.5f);
    glUniform1f(glGetUniformLocation(shader.getProgram(), "material.refraction"), m_refraction);
}
