#include "armature.h"

#include "animation.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <sstream>

VertexBoneData::VertexBoneData()
{
    Reset();
}

void VertexBoneData::Reset()
{
    memset(aIds, 0, sizeof(aIds));
    memset(aWeights, 0, sizeof(aWeights));
}

/*
 * Add new bone data (bone ID + weight) inside arrays
 * if the number of bones already added equals MAX_BONES_PER_VERTEX
 * then nothing is added
 * */
void VertexBoneData::AddBoneData(const unsigned int &uiBoneId, const float &fWeight)
{
    for(unsigned int i = 0 ; i < MAX_BONES_PER_VERTEX; ++i)
    {
        //  1st empty data in bone array
        if(abs(afWeights[i]) < EPSILON)
        {
            auiIds[i] = uiBoneId;
            afWeights[i] = fWeight;
            return;
        }
    }
}


Armature::Armature(Bone *aBoneTree, std::map<std::string, unsigned int> &mapBoneMapping, VertexBoneData *pVertexBoneData) :
    m_mapBoneMapping(mapBoneMapping),
    m_aBoneTree(aBoneTree),
    m_pVertexBoneData(pVertexBoneData)
{
    m_strBoneShaderString.reserve(m_mapBoneMapping.size());

    for(size_t i = 0 ; i < m_mapBoneMapping.size(); ++i)
    {
        std::ostringstream oss;
        oss << "bones_transforms[" << i << "]";
        m_strBoneShaderString[i] = oss.str();
    }
}

Armature::~Armature()
{
    delete[] m_aBoneTree;
    delete[] m_pVertexBoneData;

    m_mapBoneMapping.clear();
    m_strBoneShaderString.clear();
}

void Armature::update(const Shader &shader, const Animation *pAnimation, const float &fRenderTime)
{
    //  Update bone tree
    updateBoneTransform(pAnimation, 0, glm::mat4(1.f), fRenderTime);

//  TODO: export shader code
    //  Send bones to the shader
    for(size_t i = 0 ; i < m_mapBoneMapping.size(); ++i)
    {
        m_auiBoneLocation[i] = glGetUniformLocation(shader.getProgram(), m_strBoneShaderString[i].c_str());
        glUniformMatrix4fv(m_auiBoneLocation[i], 1, GL_TRUE, glm::value_ptr(m_aBoneTree[i].mFinalTransform));
    }
}

/*
 * Update bone transform at index bone_index
 * using animation at render_time
 * and affects the transform to childrens
 * */
void Armature::updateBoneTransform(const Animation *pAnimation, const unsigned int &uiBoneIndex, const glm::mat4 &mParentTransform, const float &fRenderTime)
{
    const Bone &currentBone = m_aBoneTree[uiBoneIndex];
    const std::string strCurrentBoneName = currentBone.strName;

    glm::mat4 mGlobalTransform = mParentTransform;

    //  If bone is used in Animation, calculate interpolated transform
    if(pAnimation->isBoneUsed(strCurrentBoneName))
    {
        const unsigned int uiCurrentTick = floor(fRenderTime);

        const Channel &channel1 = pAnimation->getChannel(strCurrentBoneName, uiCurrentTick);
        const Channel &channel2 = pAnimation->getChannel(strCurrentBoneName, (uiCurrentTick + 1) % pAnimation->getDuration());

        const float fDeltaTime = channel2.fTime - channel1.fTime;
        const float fFactor = (pAnimation->getDuration() * pAnimation->getTicksPerSecond() - fRenderTime) / fDeltaTime;

        //  Interpolations
        const glm::vec3 vScale = Channel::calcInterpolatedScaling(channel1, channel2, pAnimation->getDuration(), fFactor);
        const glm::quat qRotation = Channel::calcInterpolatedRotation(channel1, channel2, pAnimation->getDuration(), fFactor);
        const glm::vec3 vPosition = Channel::calcInterpolatedPosition(channel1, channel2, pAnimation->getDuration(), fFactor);

        const glm::mat4 mScaleMatrix = glm::scale(glm::mat4(1), vScale);
        const glm::mat4 mRotationMatrix = glm::mat4_cast(qRotation);
        const glm::mat4 mPositionMatrix = glm::translate(glm::mat4(1), vPosition);

        //  Combine the above transform
        const glm::mat4 mLocalTransform = mPositionMatrix * mRotationMatrix * mScaleMatrix;

        //  Combine parent transform and interpolated transform
        mGlobalTransform *= mLocalTransform;
    }

    const glm::mat4 &mNodeTransform = currentBone.mBoneOffset;

    //  Update bone transform
    currentBone.mFinalTransform = mGlobalTransform * mNodeTransform;

    //  Loop over children bones
    for(size_t i = 0 ; i < currentBone.auiChildren.size() ; ++i)
    {
        updateBoneTransform(pAnimation, currentBone.auiChildren[i], mGlobalTransform, fRenderTime);
    }
}
