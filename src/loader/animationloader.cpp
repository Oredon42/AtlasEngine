#include "include/loader/animationloader.h"
#include "include/data/animation/animation.h"
#include "include/data/scene.h"
#include "include/data/model.h"

AnimationLoader::AnimationLoader()
{

}

/*
 * Load animations from an aiScene
 * fill informations of models inside the current SceneGraph
 * */
void AnimationLoader::load(const aiScene *ai_scene, const Scene *scene)
{
    /*
     * This loop will store animation pose
     * for each tick of every animation
     * So we don't have to traverse the aiScene tree at runtime
     * (I've chosen to iterate over animation and traverse tree
     * than traverse tree once and iterate over animations
     * as it is more current to have 1 animation per file)
     * */
    for(unsigned int animation_index = 0; animation_index < ai_scene->mNumAnimations; ++animation_index)
    {
        //  Get animation parameters
        GLfloat total_time = ai_scene->mAnimations[animation_index]->mDuration;
        GLfloat ticks_per_seconds = ai_scene->mAnimations[animation_index]->mTicksPerSecond != 1?ai_scene->mAnimations[0]->mTicksPerSecond : 24.0f;
        //  Total time of the animation in ticks
        GLuint total_time_in_ticks = (GLuint)(total_time * ticks_per_seconds);
        GLfloat seconds_per_tick = 1.f / ticks_per_seconds;

        aiAnimation *ai_animation = ai_scene->mAnimations[animation_index];

        //  For each tick of the animation
        //  Retrieve animation pose
        for(GLuint current_tick = 0; current_tick < total_time_in_ticks; ++current_tick)
            processAnimation(ai_animation, scene, "default", ticks_per_seconds, current_tick, current_tick * seconds_per_tick, total_time_in_ticks, ai_scene->mRootNode);
    }
}

/*
 *  Process aiNode - each node is a bone
 *
 *  for the current animation and the current time in ticks
 *  create interpolated transformation matrix
 *  store matrix into concerned animated meshes
 * */
void AnimationLoader::processAnimation(const aiAnimation *ai_animation, const Scene *scene, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLuint &total_time_in_ticks, const aiNode *ai_node)
{
    std::string bone_name = ai_node->mName.data;

    //  Is bone used in animation?
    const aiNodeAnim *ai_node_anim = findNodeAnim(ai_animation, bone_name);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    GLfloat time;

    //  If bone found
    if(ai_node_anim)
    {
        scale = glm::vec3(ai_node_anim->mScalingKeys[current_tick].mValue.x, ai_node_anim->mScalingKeys[current_tick].mValue.y, ai_node_anim->mScalingKeys[current_tick].mValue.z);
        rotation = glm::quat(ai_node_anim->mRotationKeys[current_tick].mValue.w, ai_node_anim->mRotationKeys[current_tick].mValue.x, ai_node_anim->mRotationKeys[current_tick].mValue.y, ai_node_anim->mRotationKeys[current_tick].mValue.z);
        position = glm::vec3(ai_node_anim->mPositionKeys[current_tick].mValue.x, ai_node_anim->mPositionKeys[current_tick].mValue.y, ai_node_anim->mPositionKeys[current_tick].mValue.z);
        time = ai_node_anim->mPositionKeys[current_tick].mTime;
    }
    else
    {
        scale = glm::vec3(1.f);
        rotation = glm::quat(0.f, 0.f, 0.f, 0.f);
        position = glm::vec3(0.f);
        time = 0.f;
    }

    Channel current_channel;
    current_channel.position = position;
    current_channel.rotation = rotation;
    current_channel.scale = scale;
    current_channel.time = time;

    //  Loop over every model
    /*for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        for(GLuint j = 0; j < scene->numberOfModels(i); ++j)
        {
            Model *model = scene->getModel(i,j);
            if(model->hasBone(bone_name))
            {
                    model->setAnimationInfo(animation_name, total_time_in_ticks, ticks_per_second);
                    model->setChannel(animation_name, bone_name, current_tick, current_channel);
            }
        }
    }*/

    //  Loop over children bones
    for(unsigned int i = 0 ; i < ai_node->mNumChildren ; ++i)
       processAnimation(ai_animation, scene, animation_name, ticks_per_second, current_tick, current_time, total_time_in_ticks, ai_node->mChildren[i]);
}

/*
 *  Return aiNodeAnim with the name node_name
 * */
const aiNodeAnim *AnimationLoader::findNodeAnim(const aiAnimation *ai_animation, const std::string &node_name)
{
    //  Loop over animation channels
    for(unsigned int i = 0 ; i < ai_animation->mNumChannels ; ++i)
    {
        const aiNodeAnim* ai_node_anim = ai_animation->mChannels[i];

        if(std::string(ai_node_anim->mNodeName.data) == node_name)
            return ai_node_anim;
    }

    //  If not found
    return NULL;
}
