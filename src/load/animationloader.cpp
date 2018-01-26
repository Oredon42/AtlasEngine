#include "src/loader/animationloader.h"
#include "src/core/data/animation.h"
#include "src/core/data/database.h"

#include "assimp/scene.h"
#include "assimp/anim.h"

AnimationLoader::AnimationLoader()
{

}

/*
 * Load animations from an aiScene
 * and store in Database
 * */
void AnimationLoader::load(Database *pDatabase, const aiScene *pAiScene)
{
    if(pAiScene.HasAnimations() == true)
    {
        // Loop on every aiAnimation
        for(unsigned int uiAnimationIndex = 0; uiAnimationIndex < pAiScene->mNumAnimations; ++uiAnimationIndex)
        {
            aiAnimation *pAiAnimation = pAiScene->mAnimations[uiAnimationIndex];

            //  Get aiAnimation parameters
            const float fAnimationDuration = pAiAnimation->mDuration;
            const float fTicksPerSeconds = pAiAnimation->mTicksPerSecond != 1?pAiAnimation->mTicksPerSecond : 24.0f;  //  Default 24
            const unsigned int uiAnimationDurationInTicks = (unsigned int)(fAnimationDuration * fTicksPerSeconds);
            const std::string strAnimationName = pAiAnimation->mName.C_Str();

            //  Create a new Animation
            Animation *pAnimation = new Animation(fAnimationDuration, fTicksPerSeconds);

            //  For each tick of the animation
            //  get and store Bones Channels of every used Bone
            for(unsigned int uiCurrentTick = 0; uiCurrentTick < uiAnimationDurationInTicks; ++uiCurrentTick)
            {
                processBonesChannels(pAnimation, pAiScene->mRootNode, pAiAnimation, uiCurrentTick);
            }

            //  Add Animation to Database
            pDatabase->insertAnimation(pAnimation, strAnimationName);
        }
    }
}

/*
 * Process aiNode - each node is a bone - to retrieve the pose
 * at the current tick

 * create a Channel that will store the pose
 * */
void AnimationLoader::processBonesChannels(Animation *pAnimation, const aiNode *pBone, const aiAnimation *pAiAnimation, const unsigned int &uiCurrentTick)
{
    const std::string strBoneName = bone->mName.data;

    //  Get bone transform for current animation
    const aiNodeAnim *pAiNodeAnim = findNodeAnim(pAiAnimation, strBoneName);

    //  Is bone used in animation?
    if(pAiNodeAnim != nullptr)
    {
        //  Get Channel infos for the current Bone
        const glm::vec3 vScale = glm::vec3(pAiNodeAnim->mScalingKeys[uiCurrentTick].mValue.x, pAiNodeAnim->mScalingKeys[uiCurrentTick].mValue.y, pAiNodeAnim->mScalingKeys[uiCurrentTick].mValue.z);
        const glm::quat qRotation = glm::quat(pAiNodeAnim->mRotationKeys[uiCurrentTick].mValue.w, pAiNodeAnim->mRotationKeys[uiCurrentTick].mValue.x, pAiNodeAnim->mRotationKeys[uiCurrentTick].mValue.y, pAiNodeAnim->mRotationKeys[uiCurrentTick].mValue.z);
        const glm::vec3 vPosition = glm::vec3(pAiNodeAnim->mPositionKeys[uiCurrentTick].mValue.x, pAiNodeAnim->mPositionKeys[uiCurrentTick].mValue.y, pAiNodeAnim->mPositionKeys[uiCurrentTick].mValue.z);
        const float fTime = pAiNodeAnim->mPositionKeys[uiCurrentTick].mTime;

        //  Create new Channel
        const Channel currentChannel(vScale, qRotation, vPosition, fTime);

        //  Add Channel to the Animation
        pAnimation->setChannel(sBoneName, uiCurrentTick, currentChannel);

        //  Loop on children bones
        for(unsigned int uiChildBoneIndex = 0 ; uiChildBoneIndex < bone->mNumChildren ; ++uiChildBoneIndex)
        {
            processBonesChannels(pAnimation, bone->mChildren[uiChildBoneIndex], pAiAnimation, uiCurrentTick);
        }
    }
}

/*
 *  Return aiNodeAnim with the name node_name
 * */
const aiNodeAnim *AnimationLoader::findNodeAnim(const aiAnimation *pAiAnimation, const std::string &strNodeName)
{
    //  Loop on animation channels
    for(unsigned int uiChannelIndex = 0 ; uiChannelIndex < pAiAnimation->mNumChannels ; ++iuiChannelIndex)
    {
        const aiNodeAnim *pAiNodeAnim = pAiAnimation->mChannels[uiChannelIndex];

        if(std::string(pAiNodeAnim->mNodeName.data) == strNodeName)
        {
            return pAiNodeAnim;
        }
    }

    //  If not found
    return nullptr;
}
