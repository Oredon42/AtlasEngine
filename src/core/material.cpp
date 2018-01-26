#include "core/data/material.h"

#include "core/data/texture.h"

Material::Material(const glm::vec3 &vAlbedo, const GLfloat &fRoughness, const GLfloat &fMetalness, const GLfloat &fRefraction, const GLfloat &fOpacity) :
    m_vAlbedo(vAlbedo),
    m_fRoughness(fRoughness),
    m_fMetalness(fMetalness),
    m_fRefraction(fRefraction),
    m_fOpacity(fOpacity),
    m_pAlbedoMap(nullptr),
    m_pMetalnessMap(nullptr),
    m_pRoughnessMap(nullptr),
    m_pNormalMap(nullptr)
{

}

Material::~Material()
{
    if(m_pAlbedoMap != nullptr)
    {
        delete m_pAlbedoMap;
    }

    if(m_pMetalnessMap != nullptr)
    {
        delete m_pMetalnessMap;
    }

    if(m_pRoughnessMap != nullptr)
    {
        delete m_pRoughnessMap;
    }

    if(m_pRefractionMap != nullptr)
    {
        delete m_pRefractionMap;
    }

    if(m_pOpacityMap != nullptr)
    {
        delete m_pOpacityMap;
    }

    if(m_pNormalMap != nullptr)
    {
        delete m_pNormalMap;
    }
}

void Material::sendDatas(Shader &shader) const
{
    //  TODO: export shader code
    if(m_pAlbedoTexture == nullptr)
    {
        glUniform3f(glGetUniformLocation(shader.getProgram(), "material.albedo"), m_vAlbedo.r, m_vAlbedo.g, m_vAlbedo.b);
    }
    else
    {
        const std::string strNameNumber = ss.str();
        const std::string strTexture = "material." + strNameNumber;
        glUniform1i(glGetUniformLocation(shader.getProgram(), strTexture.c_str()), shader.getActiveTextureUint());
        glActiveTexture(shader.getActiveTexture());
        shader.setNextTexture();
        m_aTextures[i]->bind();
    }
    if(m_pMetallicTexture == nullptr)
    {
        glUniform1f(glGetUniformLocation(shader.getProgram(), "material.metalness"), m_fMetalness);
    }
    else
    {
        const std::string strNameNumber = ss.str();
        const std::string strTexture = "material." + strNameNumber;
        glUniform1i(glGetUniformLocation(shader.getProgram(), strTexture.c_str()), shader.getActiveTextureUint());
        glActiveTexture(shader.getActiveTexture());
        shader.setNextTexture();
        m_aTextures[i]->bind();
    }
    if(m_pRoughnessTexture == nullptr)
    {
        glUniform1f(glGetUniformLocation(shader.getProgram(), "material.roughness"), m_fRoughness);
    }
    else
    {
        const std::string strNameNumber = ss.str();
        const std::string strTexture = "material." + strNameNumber;
        glUniform1i(glGetUniformLocation(shader.getProgram(), strTexture.c_str()), shader.getActiveTextureUint());
        glActiveTexture(shader.getActiveTexture());
        shader.setNextTexture();
        m_aTextures[i]->bind();
    }
}

void Material::setAlbedoMap(Texture *pAlbedoMap)
{
    if(m_pAlbedoMap != nullptr)
    {
        delete m_pAlbedoMap;
    }

    m_pAlbedoMap = pAlbedoMap;
}

void Material::setMetalnessMap(Texture *pMetalnessMap)
{
    if(m_pMetalnessMap != nullptr)
    {
        delete m_pMetalnessMap;
    }

    m_pMetalnessMap= pMetalnessMap;
}

void Material::setRoughnessMap(Texture *pRoughnessMap)
{
    if(m_pRoughnessMap != nullptr)
    {
        delete m_pRoughnessMap;
    }

    m_pRoughnessMap = pRoughnessMap;
}

void Material::setRefractionMap(Texture *pRefractionMap)
{
    if(m_pRefractionMap != nullptr)
    {
        delete m_pRefractionMap;
    }

    m_pRefractionMap = pRefractionMap;
}

void Material::setOpacityMap(Texture *pOpacityMap)
{
    if(m_pOpacityMap != nullptr)
    {
        delete m_pOpacityMap;
    }

    m_pOpacityMap = pOpacityMap;
}

void Material::setNormalMap(Texture *pNormalMap)
{
    if(m_pNormalMap != nullptr)
    {
        delete m_pNormalMap;
    }

    m_pNormalMap = pNormalMap;
}

Material& Material::operator=(const Material& other)
{
    m_pAlbedoMap = other.m_pAlbedoMap;
    m_fRoughness = other.m_fRoughness;
    m_fMetalness = other.m_fMetalness;
    m_fRefraction = other.m_fRefraction;
    m_fOpacity = other.m_fOpacity;

    setAlbedoMap(other.m_pAlbedoMap);
    setMetalnessMap(other.m_pMetalnessMap);
    setRoughnessMap(other.m_pRoughnessMap);
    setRefractionMap(other.m_pRefractionMap);
    setOpacityMap(other.m_pOpacityMap);
    setNormalMap(other.m_pNormalMap);

    return *this;
}

Material& Material::operator=(const Material& other)
{
    this = *other;
}
