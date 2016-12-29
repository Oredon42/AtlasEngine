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
#include "model.h"

class Material;
class AnimatedModel;
class Camera;
class DirLight;
class SpotLight;
class PointLight;
class Animation;

struct Bone;
struct VertexBoneData;

class Texture;

glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &assimp_mat);

class SceneGraphNode
{
    friend class Scene;
public:
    SceneGraphNode(std::string &path, glm::mat4 &global_inverse_transform, const std::string &name);
    SceneGraphNode(SceneGraphNode *parent, const std::string &name, const std::string &path, const glm::mat4 &global_inverse_transform, const glm::mat4 &transform);
    SceneGraphNode(const std::string &name, glm::mat4 transform = glm::mat4(1.f));
    virtual ~SceneGraphNode();

    void spreadTransform();
    inline void insertModel(Model *model){m_models[model->getShaderTypeIndex()].push_back(model);}
    void addChild(SceneGraphNode *child);
    virtual void calculateTransform();

    //  Getters
    SceneGraphNode *getNode(const std::string &name);
    inline std::string getName() const{return m_name;}
    inline GLuint numberOfMeshes() const{return m_number_of_models;}
    inline glm::mat4 getGlobalInverseTransform() const{return m_global_inverse_transform;}
    inline glm::mat4 getTransform() const{return m_transform;}

    //  Setters
    void setMaterial(const Material& material, const std::string &name);
    inline void setParent(SceneGraphNode *parent){m_parent = parent;}
    inline void setName(const std::string &name){m_name = name;}
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}
    inline void setPosition(const glm::vec3 &position){m_position = position;spreadTransform();}
    inline void setRotation(const glm::quat &rotation){m_rotation = rotation;spreadTransform();}
    inline void setScale(const glm::vec3 &scale){m_scale = scale;spreadTransform();}

protected:
    std::string m_path;
    std::string m_name;
    SceneGraphNode *m_parent;
    std::vector<SceneGraphNode *> m_children;

    std::vector<Model *> m_models[NB_SHADER_TYPES];
    GLuint m_number_of_models;

    std::vector<Texture *> m_textures_loaded;

    glm::mat4 m_global_inverse_transform;
    glm::mat4 m_node_transform;
    glm::mat4 m_transform;
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
};

/*
 * SceneGraphRoot
 *
 * Root of the scene graph
 * Save a list of animated meshes in the scene,
 * will be useful when adding new animations
 * */
class SceneGraphRoot : public SceneGraphNode
{
public:
    SceneGraphRoot(const std::string &name, const std::string &path, const glm::mat4 &global_inverse_transform, const glm::mat4 &transform);
    virtual ~SceneGraphRoot();

    void translate(const glm::vec3 &t, const std::string &name);
    void rotate(const glm::vec3 &r, const std::string &name);
    void scale(const glm::vec3 &s, const std::string &name);

    virtual void calculateTransform();

private:
    std::vector<Animation *> m_animations;
};

#endif // SCENEGRAPHNODE_H
