#ifndef SCENEGRAPHNODE_H
#define SCENEGRAPHNODE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <QtGui/QImage>

#include "include/openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#include "shader.h"
#include "mesh.h"

class Model;
class Material;
class AnimatedModel;
class Camera;
class DirLight;
class SpotLight;
class PointLight;
class Animation;

struct Bone;
struct VertexBoneData;

struct Texture;

glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &assimp_mat);

class SceneGraphNode
{
    friend class Scene;
public:
    SceneGraphNode();
    SceneGraphNode(std::string &path, glm::mat4 &global_inverse_transform);
    SceneGraphNode(SceneGraphNode *parent, std::string &name, std::string &path, glm::mat4 &global_inverse_transform, glm::mat4 &transform);
    virtual ~SceneGraphNode();

    //  Getters
    inline GLuint numberOfMeshes() const{return m_number_of_models;}

    //  Setters
    void setMaterial(const Material& material, const std::string &name);
    inline void setParent(SceneGraphNode *parent){m_parent = parent;}
    inline void setName(const std::string &name){m_name = name;}
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}
    void translate(const glm::vec3 &t, const std::string &name);
    void rotate(const glm::vec3 &r, const std::string &name);
    void scale(const glm::vec3 &s, const std::string &name);
    void calculateTransform(const glm::mat4 &parent_transform);
    inline void spreadTransform(const glm::mat4 &parent_transform){calculateTransform(parent_transform);for(GLuint i = 0; i < m_children.size(); ++i)m_children[i]->spreadTransform(m_transform);}
    inline void insertModel(Model *model, const GLuint &shader_index){m_models[shader_index].push_back(model);}
    inline void addChild(SceneGraphNode *child){m_children.push_back(child);}

protected:
    SceneGraphNode *m_parent;
    std::vector<SceneGraphNode *> m_children;

    std::vector<Model *> m_models[NB_SHADER_TYPES];

    std::string &m_path;
    std::vector<Texture *> m_textures_loaded;

    std::string m_name;

    GLuint m_number_of_models;

    glm::mat4 &m_global_inverse_transform;

    glm::mat4 m_node_transform;

    glm::mat4 m_transform;
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
};

/*
 * Root of the scene graph
 * Save a list of animated meshes in the scene,
 * will be useful when adding new animations
 * */
class SceneGraphRoot : public SceneGraphNode
{
public:
    SceneGraphRoot(const aiScene *ai_scene, glm::mat4 &global_inverse_transform, std::string &path, std::vector<Model *> (&scene_models)[NB_SHADER_TYPES], GLfloat &render_time);
    virtual ~SceneGraphRoot();

    //  Getters
    inline std::vector<AnimatedModel *> getAnimatedModels()const {return m_animated_models;}

    //  Setters
    inline void addAnimatedModel(AnimatedModel* animated_model){m_animated_models.push_back(animated_model);}

private:
    std::vector<AnimatedModel *> m_animated_models;
    std::vector<Animation *> m_animations;
};

#endif // SCENEGRAPHNODE_H
