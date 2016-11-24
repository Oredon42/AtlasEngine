#ifndef SCENEGRAPHNODE_H
#define SCENEGRAPHNODE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <QtGui/QImage>

#ifndef OGL_DEF
#define OGL_DEF
#ifdef __APPLE__
    #include <QGLWidget>
//#include <OpenGL/gl.h>
#elif __linux__
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#elif _WIN32
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif
#endif

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
    virtual ~SceneGraphNode();

    void processNode(SceneGraphNode *SceneGraphNode, glm::mat4 &global_inverse_transform, aiNode* ai_node, const aiScene* ai_scene, std::vector<Model *> (&scene_models)[NB_SHADER_TYPES], std::vector<AnimatedModel *> *animated_models, GLfloat &render_time);
    Model *processMesh(const aiMesh *mesh, const aiScene* ai_scene, std::vector<AnimatedModel *> *animated_models, GLfloat &render_time);
    void processGeometry(const aiMesh *ai_mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);

    //  Animation
    void fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *armature, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh);

    //  Getters
    inline GLuint numberOfMeshes() const{return m_number_of_models;}

    //  Setters
    void translate(const glm::vec3 &t, const std::string &name);
    void rotate(const glm::vec3 &r, const std::string &name);
    void scale(const glm::vec3 &s, const std::string &name);
    void calculateTransform(const glm::mat4 &parent_transform);

protected:
    inline void spreadTransform(const glm::mat4 &parent_transform){calculateTransform(parent_transform);for(GLuint i = 0; i < m_children.size(); ++i)m_children[i]->spreadTransform(m_transform);}

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
    SceneGraphRoot(const aiScene *ai_scene, glm::mat4 &global_inverse_transform, std::__1::string &path, std::vector<Model *> (&scene_models)[NB_SHADER_TYPES], GLfloat &render_time);
    virtual ~SceneGraphRoot();

    void loadAnimations(const aiScene *ai_scene, const std::vector<AnimatedModel *> &animated_models);
    const aiNodeAnim *FindNodeAnim(const aiAnimation *ai_animation, const std::string &node_name);
    void processAnimation(const aiAnimation *ai_animation, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLuint &time_in_ticks, const aiNode *ai_node, const std::vector<AnimatedModel *> &animated_models);


private:
    std::vector<AnimatedModel *> m_animated_models;
    std::vector<Animation *> m_animations;
};

#endif // SCENEGRAPHNODE_H
