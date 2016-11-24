#include "include/model.h"
#include "include/camera.h"
#include "include/dirlight.h"
#include "include/spotlight.h"
#include "include/pointlight.h"
#include "include/animation.h"

Model::Model()
{

}

Model::Model(Mesh *mesh, Material *material) :
    m_material(material)
{
    m_meshes.push_back(mesh);
}

Model::~Model()
{
    for(GLuint i = 0; i < m_meshes.size(); ++i)
        delete m_meshes[i];

    m_meshes.clear();

    delete m_material;
}


/*
 * Deferred shading drawing
 * */
void Model::draw(const Shader &shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(m_transform));

    m_material->sendDatas(shader);

    for(GLuint i = 0; i < m_meshes.size(); ++i)
        m_meshes[i]->draw();

    for(GLuint i = 0; i < m_textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


/******************
 * ANIMATED MODEL
 **************** */

AnimatedModel::AnimatedModel(AnimatedMesh *mesh, const std::map<std::string, GLuint> &bone_mapping, const GLuint &num_bones, Bone *&armature, GLfloat &render_time, Material *material) :
    m_render_time(render_time),
    Model(mesh, material)
{
    m_current_animation = "default";
    m_num_bones = num_bones;
    if(m_num_bones > 0)
    {
        m_bone_mapping = bone_mapping;
        m_armature = armature;
    }
}

/*
 * Deferred shading draw
 * */
void AnimatedModel::draw(const Shader &shader)
{
    Animation &anim = m_animations[m_current_animation];

    GLfloat tick = std::fmod(m_render_time / anim.ticks_per_second, anim.duration);
    updateArmature(anim, m_armature[0], 0, glm::mat4(1), tick);

    //  Send bones affected by animation to the shader
    for(GLuint i = 0 ; i < anim.num_bones; ++i)
    {
        std::ostringstream oss;
        oss << "gBones[" << i << "]";
        std::string name = oss.str();
        m_bone_location[i] = glGetUniformLocation(shader.getProgram(), name.c_str());
        glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, glm::value_ptr(m_armature[i].final_transformation));
    }
    //  The rest of the bones is set to identity matrix
    for(GLuint i = anim.num_bones ; i < MAX_BONES; ++i)
    {
        std::ostringstream oss;
        oss << "gBones[" << i << "]";
        std::string name = oss.str();
        m_bone_location[i] = glGetUniformLocation(shader.getProgram(), name.c_str());
        glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, glm::value_ptr(glm::mat4()));
    }

    Model::draw(shader);
}

void AnimatedModel::setAnimationInfo(const std::string &animation_name, const GLuint &duration, const GLuint &ticks_per_sec)
{
    m_animations[animation_name].setNumBones(m_num_bones);
    m_animations[animation_name].setDuration(duration);
    m_animations[animation_name].ticks_per_second = ticks_per_sec;
}

void AnimatedModel::setChannel(const std::string &animation_name, const std::string &bone_name, const GLuint &current_tick, const Channel &channel)
{
    const GLuint &bone_index = m_bone_mapping[bone_name];
    m_animations[animation_name].setChannel(bone_index, current_tick, channel);
}

/*
 * Change current animation
 * */
void AnimatedModel::setAnimation(const std::string &current_animation)
{
    //  Set all bones unused in animation
    for(GLuint i = 0; i < m_num_bones; ++i)
        m_armature[i].used_in_animation = GL_FALSE;

    m_current_animation = current_animation;
}

/*
 *
 * */
void AnimatedModel::updateArmature(const Animation &anim, Bone &current_bone, const GLuint &bone_index, const glm::mat4 &parent_transform, const GLfloat &render_time)
{
    //  Get current transformation matrix in glm format
    //glm::mat4 node_transform = glm::transpose(current_bone.bone_offset);
    glm::mat4 node_transform = current_bone.bone_offset;
    glm::mat4 local_transform;

    GLuint current_tick = floor(render_time);

    Channel &channel1 = m_animations[m_current_animation].channels[bone_index][current_tick],
            &channel2 = m_animations[m_current_animation].channels[bone_index][(current_tick + 1) % anim.duration];

    GLfloat delta_time = (GLfloat)(channel2.time - channel1.time);
    GLfloat factor = (anim.duration * anim.ticks_per_second - render_time) / delta_time;

    //  Interpolations
    glm::vec3 scale = CalcInterpolatedScaling(channel1, channel2, anim.duration, factor);
    glm::quat rotation = CalcInterpolatedRotation(channel1, channel2, anim.duration, factor);
    glm::vec3 position = CalcInterpolatedPosition(channel1, channel2, anim.duration, factor);

    glm::mat4 scale_matrix = glm::scale(glm::mat4(1), channel1.scale);
    glm::mat4 rotation_matrix = glm::mat4_cast(channel1.rotation);
    glm::mat4 position_matrix = glm::translate(glm::mat4(1), channel1.position);

    //  Combine the above transformation
    local_transform = position_matrix * rotation_matrix * scale_matrix;

    //  Combine parent transform and interpolated transform
    glm::mat4 global_transform = parent_transform * local_transform;

    current_bone.final_transformation = global_transform * node_transform;

    //  Loop over children bones
    for(GLuint i = 0 ; i < current_bone.children.size() ; ++i)
        updateArmature(anim, m_armature[current_bone.children[i]], current_bone.children[i], global_transform, render_time);
}


/*
 *  Calculates the interpolation of the position
 *  at the time animation_time
 * */
glm::vec3 AnimatedModel::CalcInterpolatedPosition(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const
{
    if(animation_duration == 1)
        return channel1.position;

    glm::vec3 delta = channel2.position - channel1.position;
    return channel1.position + factor * delta;
}


/*
 *  Calculates the interpolation of the rotation
 *  at the time animation_time
 * */
glm::quat AnimatedModel::CalcInterpolatedRotation(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const
{
    if(animation_duration == 1)
        return channel1.rotation;

    return glm::normalize(glm::slerp(channel1.rotation, channel2.rotation, factor));
}


/*
 *  Calculates the interpolation of the scale
 *  at the time animation_time
 * */
glm::vec3 AnimatedModel::CalcInterpolatedScaling(const Channel &channel1, const Channel &channel2, const GLuint &animation_duration, const GLfloat &factor) const
{
    if(animation_duration == 1)
        return channel1.scale;

    glm::vec3 delta = channel2.scale - channel1.scale;
    return channel1.scale + factor * delta;
}

/*
 * Build model bone tree
 * from aiScene root node
 * */
void AnimatedModel::buildBoneTree(const aiNode *ai_node)
{
    std::string bone_name = ai_node->mName.C_Str();

    //  If has bone
    if(m_bone_mapping.find(ai_node->mName.C_Str()) != m_bone_mapping.end())
    {
        for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
        {
            std::string child_name = ai_node->mChildren[i]->mName.C_Str();

            if(m_bone_mapping.find(child_name) != m_bone_mapping.end())
            {
                GLuint  parent_index = m_bone_mapping.find(bone_name)->second,
                        child_index = m_bone_mapping.find(child_name)->second;

                m_armature[child_index].parent = parent_index;
                m_armature[parent_index].children.push_back(child_index);

                buildBoneTree(ai_node->mChildren[i]);
            }
        }
    }
    else
        for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
            buildBoneTree(ai_node->mChildren[i]);
}
