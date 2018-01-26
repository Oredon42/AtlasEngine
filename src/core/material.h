#ifndef MATERIAL_H
#define MATERIAL_H

#include <glew.h>
#include <glm/glm.hpp>

class Texture;
struct aiMaterial;
struct aiTextureType;

class Material
{
public:
    Material(const glm::vec3 &vAlbedo = glm::vec3(1.f, 0.f, 1.f), const GLfloat &fRoughness = 1.f, const GLfloat &fMetalness = 1.f, const GLfloat &fRefraction = 1.f, const GLfloat &fOpacity = 1.f);
    ~Material();

    //  Getters
    inline GLboolean hasAlbedoMap() const{return m_pAlbedoMap != nullptr;}
    inline GLboolean hasMetalnessMap() const{return m_pMetalnessMap != nullptr;}
    inline GLboolean hasRoughnessMap() const{return m_pRoughnessMap != nullptr;}
    inline GLboolean hasRefractionMap() const{return m_pRefractionMap != nullptr;}
    inline GLboolean hasOpacityMap() const{return m_pOpacityMap != nullptr;}
    inline GLboolean hasNormalMap() const{return m_pNormalMap != nullptr;}

    //  Setters
    void setAlbedoMap(Texture *pAlbedoMap);
    void setMetalnessMap(Texture *pMetalnessMap);
    void setRoughnessMap(Texture *pRoughnessMap);
    void setRefractionMap(Texture *pRefractionMap);
    void setOpacityMap(Texture *pOpacityMap);
    void setNormalMap(Texture *pNormalMap);

    Material& operator=(const Material& other);
    Material& operator=(const Material* other);

private:
    glm::vec3 m_vAlbedo;
    GLfloat m_fRoughness;
    GLfloat m_fMetalness;
    GLfloat m_fRefraction;
    GLfloat m_fOpacity;

    Texture *m_pAlbedoMap;
    Texture *m_pMetalnessMap;
    Texture *m_pRoughnessMap;
    Texture *m_pRefractionMap;
    Texture *m_pOpacityMap;
    Texture *m_pNormalMap;
};

#endif // MATERIAL_H
