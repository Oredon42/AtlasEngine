#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <unordered_map>

#include <glew.h>
#include <glm.hpp>

class Animation;
class Armature;
class Material;
class Transform;

class Model
{
    struct Vertex
    {
        glm::vec3 vPosition;
        glm::vec3 vNormal;
        glm::vec2 vUV;
        glm::vec3 vTangent;
        glm::vec3 vBitangent;
    };

public:
    Model();
    Model(const std::vector<Vertex> &aVertices, const std::vector<GLuint> &aIndices, Material *pMaterial);
    virtual ~Model();

    virtual inline void draw() const;
    virtual void setupBuffers();

    //  Getters
    Vertex getVertex(const unsigned int &i) const;
    GLuint getIndex(const unsigned int &i) const;
    Animation *getAnimation(const std::string &strKey) const;
    inline Animation *getCurrentAnimation() const{return m_pCurrentAnimation;}
    inline Armature *getArmature() const{return m_pArmature;}
    inline Material *getMaterial() const{return m_pMaterial;}

    inline size_t numberOfIndices() const{return m_aIndices.size();}
    inline size_t numberOfVertices() const{return m_aVertices.size();}
    inline size_t numberOfAnimations() const{return m_mapAnimations.size();}

    inline bool hasArmature() const{return m_pArmature != nullptr;}
    inline bool hasMaterial() const{return m_pMaterial != nullptr;}

    //  Setters
    void setTransform(const Transform &mTransform);
    void setCurrentAnimation(const std::string &strAnimationName);
    void setArmature(Armature *pArmature);
    void setMaterial(Material *pMaterial);

    void addAnimation(Animation *pAnimation, const std::string &strAnimationName);

protected:
    Transform *m_pTransform;

    //  Buffers
    GLuint m_uiVAO;
    GLuint m_uiVBO;
    GLuint m_uiEBO;
    GLuint m_uiBoneBO;

    //  Datas
    std::vector<Vertex> m_aVertices;
    std::vector<GLuint> m_aIndices;

    //  Animation
    std::unordered_map<std::string, Animation *> m_mapAnimations;
    Animation *m_pCurrentAnimation;
    Armature *m_pArmature;

    //  Material
    Material *m_pMaterial;
};

#endif // MODEL_H
