#include "include/data/scenegraphnode.h"
#include "include/data/model.h"
#include "include/data/camera.h"
#include "include/data/dirlight.h"
#include "include/data/spotlight.h"
#include "include/data/pointlight.h"
#include "include/data/animation.h"
#include "include/data/animatedmesh.h"

#include <string>

SceneGraphNode::SceneGraphNode(std::string &path, glm::mat4 &global_inverse_transform) :
    m_parent(0),
    m_path(path),
    m_global_inverse_transform(global_inverse_transform),
    m_scale(1)
{

}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent, std::string &name, std::string &path, glm::mat4 &global_inverse_transform, glm::mat4 &transform) :
    m_parent(parent),
    m_name(name),
    m_path(path),
    m_global_inverse_transform(global_inverse_transform),
    m_node_transform(transform)
{

}

SceneGraphNode::~SceneGraphNode()
{
    //  Mesh array suppression
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        for(GLuint j = 0; j < (GLuint)(m_models[i].size()); ++j)
            delete m_models[i][j];
        m_models[i].clear();
    }

    m_models->clear();

    //  Texture loaded suppression
    for(GLuint i = 0; i < m_textures_loaded.size(); ++i)
        delete m_textures_loaded[i];

    m_textures_loaded.clear();


    //  Children array suppression
    for(GLuint i = 0; i < m_children.size(); ++i)
        delete m_children[i];

    m_children.clear();
}

void SceneGraphNode::setMaterial(const Material& material, const std::string &name)
{
    if(m_name == name)
    {
        for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
            for(GLuint j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->setMaterial(material);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->setMaterial(material, name);
}

/*
 * If name is this node name
 * translate this node with t
 * and spread to every child
 * */
void SceneGraphNode::translate(const glm::vec3 &t, const std::string &name)
{
    if(m_name == name)
    {
        m_position = t;
        calculateTransform(glm::mat4());
        spreadTransform(m_transform);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->translate(t, name);
}

/*
 * If name is this node name
 * rotate this node with r
 * and spread to every child
 * */
void SceneGraphNode::rotate(const glm::vec3 &r, const std::string &name)
{
    if(m_name == name)
    {
        m_rotation = glm::quat(r);
        calculateTransform(glm::mat4());
        spreadTransform(m_transform);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->rotate(r, name);
}

/*
 * If name is this node name
 * scale this node with s
 * and spread to every child
 * */
void SceneGraphNode::scale(const glm::vec3 &s, const std::string &name)
{
    if(m_name == name)
    {
        m_scale = s;
        calculateTransform(glm::mat4());
        spreadTransform(m_transform);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->scale(s, name);
}

/*
 * Calculate node transform matrix
 * with saved transform,
 * saved rotation, saved scale and saved translation
 * And set every models of this node transforms
 * */
void SceneGraphNode::calculateTransform(const glm::mat4 &parent_transform)
{
    m_transform = m_node_transform;
    m_transform *= glm::mat4_cast(m_rotation);
    m_transform = glm::translate(m_transform, m_position);
    m_transform = glm::scale(m_transform, m_scale);

    m_transform *= parent_transform;

    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
        for(GLuint j = 0; j < m_models[i].size(); ++j)
            m_models[i][j]->setTransform(m_transform);
}


/*****************
 * SCENEGRAPHROOT
 * ************* */

/*
 * Scene graph root creation from aiScene
 * */
SceneGraphRoot::SceneGraphRoot(const aiScene *ai_scene, glm::mat4 &global_inverse_transform, std::string &path, std::vector<Model *> (&scene_models)[NB_SHADER_TYPES], GLfloat &render_time) :
    SceneGraphNode(path, global_inverse_transform)
{

}

SceneGraphRoot::~SceneGraphRoot()
{
    //  Animations suppression
    for(GLuint i = 0; i < m_animations.size(); ++i)
        delete m_animations[i];

    m_animations.clear();
}

