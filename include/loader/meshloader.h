#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "scenegraphnode.h"
#include "armatureloader.h"

class Scene;
class Texture;

glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &assimp_mat);

class MeshLoader
{
public:
    MeshLoader();

    void load(const aiScene *ai_scene, Scene &scene);
    SceneGraphNode *processNode(SceneGraphNode *parent, aiNode *ai_node, const aiScene *ai_scene, Scene &scene, std::vector<AnimatedModel *> *animated_models, const std::string &path, const glm::mat4 &global_inverse_transform);
    Model *processMesh(const aiMesh *ai_mesh, const aiScene *ai_scene, Scene &scene);
    void processGeometry(const aiMesh *ai_mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);

private:
    ArmatureLoader m_armatureloader;

    std::vector<Texture *> m_textures_loaded;
};

#endif // MESHLOADER_H
