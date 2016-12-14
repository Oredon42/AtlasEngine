#include <sstream>

#include "armature.h"
#include "include/data/animation.h"
#include "include/render/shader.h"

Armature::Armature(std::map<std::string, GLuint> bone_mapping, GLuint num_bones, Bone *bone_tree, VertexBoneData *vertex_bone_data) :
    m_bone_mapping(bone_mapping),
    m_num_bones(num_bones),
    m_bone_tree(bone_tree),
    m_vertex_bone_data(vertex_bone_data)
{

}

Armature::~Armature()
{
    delete[] m_bone_tree;
    delete[] m_vertex_bone_data;
}

void Armature::update(const Shader &shader, const Animation *animation, const GLfloat &render_time)
{
    updateBone(animation, 0, glm::mat4(1.f), render_time);

    //  Send bones affected by animation to the shader
    for(GLuint i = 0 ; i < animation->numberOfBones(); ++i)
    {
        std::ostringstream oss;
        oss << "gBones[" << i << "]";
        std::string name = oss.str();
        m_bone_location[i] = glGetUniformLocation(shader.getProgram(), name.c_str());
        glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, glm::value_ptr(m_bone_tree[i].final_transformation));
    }
    //  The rest of the bones is set to identity matrix
    for(GLuint i = animation->numberOfBones() ; i < MAX_BONES; ++i)
    {
        std::ostringstream oss;
        oss << "gBones[" << i << "]";
        std::string name = oss.str();
        m_bone_location[i] = glGetUniformLocation(shader.getProgram(), name.c_str());
        glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, glm::value_ptr(glm::mat4()));
    }
}

/*
 *
 * */
void Armature::updateBone(const Animation *animation, const GLuint &bone_index, const glm::mat4 &parent_transform, const GLfloat &render_time)
{
    Bone &current_bone = m_bone_tree[bone_index];

    //glm::mat4 node_transform = glm::transpose(current_bone.bone_offset);
    glm::mat4 node_transform = current_bone.bone_offset;
    glm::mat4 local_transform;

    GLuint current_tick = floor(render_time);

    const Channel &channel1 = animation->getChannel(bone_index, current_tick),
                  &channel2 = animation->getChannel(bone_index, (current_tick + 1) % animation->getDuration());

    GLfloat delta_time = (GLfloat)(channel2.time - channel1.time);
    GLfloat factor = (animation->getDuration() * animation->getTicksPerSecond() - render_time) / delta_time;

    //  Interpolations
    glm::vec3 scale = animation->calcInterpolatedScaling(channel1, channel2, animation->getDuration(), factor);
    glm::quat rotation = animation->calcInterpolatedRotation(channel1, channel2, animation->getDuration(), factor);
    glm::vec3 position = animation->calcInterpolatedPosition(channel1, channel2, animation->getDuration(), factor);

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
        updateBone(animation, current_bone.children[i], global_transform, render_time);
}
