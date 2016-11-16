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

class Model;
class AnimatedModel;
class Camera;
class DirLight;
class SpotLight;
class PointLight;

struct Bone;
struct VertexBoneData;

struct Texture;

glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &assimp_mat);

class SceneGraphNode
{
    friend class Scene;
public:
    SceneGraphNode();
    SceneGraphNode(const std::string &path, glm::mat4 &global_inverse_transform);
    ~SceneGraphNode();

    void processNode(SceneGraphNode *SceneGraphNode, glm::mat4 &global_inverse_transform, aiNode* ai_node, const aiScene* ai_scene, std::vector<Model *> (&models)[NB_SHADER_TYPES], std::vector<AnimatedModel *> *animated_models, GLfloat &render_time);
    Model *processMesh(const aiMesh *mesh, const aiScene* ai_scene, std::vector<AnimatedModel *> *animated_models, GLfloat &render_time);
    std::vector<Texture> loadMaterialTextures(const aiMaterial *mat, const aiTextureType &type, const std::string &type_name);

    GLint TextureFromFile(const GLchar *path);

    //  Animation
    void buildBoneTree(AnimatedModel *model, const aiNode *ai_root_node);
    void fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *armature, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh);
    void loadAnimations(const aiScene *ai_scene, const std::vector<AnimatedModel *> &animated_models);
    const aiNodeAnim *FindNodeAnim(const aiAnimation *ai_animation, const std::string &node_name);
    GLuint FindPosition(const GLuint &animation_time, const aiNodeAnim *ai_node_anim);
    GLuint FindRotation(const GLuint &animation_time, const aiNodeAnim *ai_node_anim);
    GLuint FindScaling(const GLuint &animation_time, const aiNodeAnim *ai_node_anim);
    void processAnimation(const aiAnimation *ai_animation, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLfloat &total_time, const aiNode *ai_node, const std::vector<AnimatedModel *> &animated_models);

    //  Getters
    inline GLuint numberOfMeshes() const{return m_number_of_models;}

    //  Setters
    void translate(const glm::vec3 &t, const std::string &name);
    void calculateTransform(const glm::mat4 &parent_transform);

protected:
    inline void spreadTransform(const glm::mat4 &parent_transform){calculateTransform(parent_transform);for(GLuint i = 0; i < m_children.size(); ++i)m_children[i]->spreadTransform(m_transform);}

    SceneGraphNode *m_parent;
    std::vector<SceneGraphNode *> m_children;

    std::vector<Model *> m_models[NB_SHADER_TYPES];

    std::string m_directory;
    std::vector<Texture> m_textures_loaded;

    std::string m_name;

    GLuint m_number_of_models;

    glm::mat4 &m_global_inverse_transform;

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
    SceneGraphRoot(const aiScene *scene, glm::mat4 &global_inverse_transform, const std::string &path, std::vector<Model *> (&models)[NB_SHADER_TYPES], GLfloat &render_time);

private:
    std::vector<AnimatedModel *> m_animated_models;
};

#endif // SCENEGRAPHNODE_H
