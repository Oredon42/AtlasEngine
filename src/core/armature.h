#ifndef ARMATURE_H
#define ARMATURE_H

#include <map>
#include <vector>

#include <glew.h>
#include <glm/glm.hpp>

constexpr unsigned int MAX_BONES_PER_VERTEX = 4;
constexpr unsigned int NUM_BONES_PER_ARMATURE = 100;
constexpr float EPSILON = 0.001f;

class Shader;
class Animation;

/*
 * Element of the Bone Tree
 * */
struct Bone
{
    unsigned int uiParent;
    std::vector<unsigned int> auiChildren;

    std::string strName;

    glm::mat4 mBoneOffset;
    glm::mat4 mFinalTransform;

    Bone() : uiParent(0) {}
};

/*
 * Bone ID and weight for a Vertex
 * */
struct VertexBoneData
{
    unsigned int auiIds[MAX_BONES_PER_VERTEX];
    float afWeights[MAX_BONES_PER_VERTEX];

    VertexBoneData();

    void Reset();
    void AddBoneData(const unsigned int &uiBoneId, const float &fWeight);
};

class Armature
{
public:
    Armature(Bone *aBoneTree, std::map<std::string, unsigned int> &mapBoneMapping, VertexBoneData *pVertexBoneData);
    ~Armature();

    void update(const Shader &shader, const Animation *pAnimation, const float &fRenderTime);

    //  Getters
    inline VertexBoneData *getVertexBoneData() const{return m_pVertexBoneData;}
    inline size_t numberOfBones() const{return m_mapBoneMapping.size();}
    inline unsigned int getBoneIndex(const std::string &strBoneName) const{return m_mapBoneMapping[strBoneName];}
    inline bool hasBone(const std::string &strBoneName) const{return m_mapBoneMapping.find(strBoneName) != m_mapBoneMapping.end();}

private:
    GLuint m_auiBoneLocation[NUM_BONES_PER_ARMATURE];
    std::map<std::string, unsigned int> m_mapBoneMapping;
    Bone *m_aBoneTree;
    VertexBoneData *m_pVertexBoneData;

    std::vector<std::string> m_strBoneShaderString;

    void updateBoneTransform(const Animation *pAnimation, const unsigned int &uiBoneIndex, const glm::mat4 &mParentTransform, const float &fRenderTime);
};

#endif // ARMATURE_H
