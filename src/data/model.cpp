#include "include/data/model.h"
#include "include/data/camera.h"
#include "include/data/lighting/dirlight.h"
#include "include/data/lighting/spotlight.h"
#include "include/data/lighting/pointlight.h"
#include "include/data/animation/armature.h"
#include "include/data/animation/animation.h"

Model::Model() :
    m_armature(0),
    m_current_animation(0)
{

}

Model::Model(Mesh *mesh, Material *material) :
    m_material(material),
    m_armature(0),
    m_current_animation(0)
{
    m_meshes.push_back(mesh);
}

Model::~Model()
{
    for(size_t i = 0; i < m_meshes.size(); ++i)
        delete m_meshes[i];

    m_meshes.clear();

    delete m_armature;
}

void Model::draw(const Shader &shader, const GLfloat &render_time) const
{
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(m_transform));

    m_material->sendDatas(shader);

    for(size_t i = 0; i < m_meshes.size(); ++i)
    {
        if(m_armature != 0)
            m_armature->update(shader, m_current_animation, render_time);
        m_meshes[i]->draw();
    }
}

void Model::attachArmature(Armature *armature)
{
    m_armature = armature;
    for(size_t i = 0; i < m_meshes.size(); ++i)
        m_meshes[i]->attachArmature(m_armature->getVertexBoneData());
}

void Model::detachArmature()
{
    delete m_armature;
    m_armature = 0;
}

GLboolean Model::hasBone(const std::string &bone_name) const
{
    if(m_armature == 0)
        return GL_FALSE;

    return m_armature->hasBone(bone_name);
}

void Model::setAnimationInfo(const std::string &animation_name, const GLuint &duration, const GLuint &ticks_per_sec)
{
    m_animations[animation_name].setNumBones(m_armature->numberOfBones());
    m_animations[animation_name].setDuration(duration);
    m_animations[animation_name].setTicksPerSecond(ticks_per_sec);
}

void Model::setChannel(const std::string &animation_name, const std::string &bone_name, const GLuint &current_tick, const Channel &channel)
{
    const GLuint &bone_index = m_armature->getBoneIndex(bone_name);
    m_animations[animation_name].setChannel(bone_index, current_tick, channel);
}
