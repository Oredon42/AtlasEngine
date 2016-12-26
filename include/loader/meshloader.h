#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "scenegraphnode.h"
#include "armatureloader.h"

class Scene;
class Texture;

class MeshLoader
{
public:
    MeshLoader();

    void load(const aiScene *ai_scene, Scene *scene, const GLboolean &process_armatures);
    SceneGraphNode *processNode(aiNode *ai_node, const aiScene *ai_scene, Scene *scene);
    Model *processMesh(const aiMesh *ai_mesh, const aiScene *ai_scene, Scene *scene);
    void processGeometry(const aiMesh *ai_mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);

private:
    ArmatureLoader m_armatureloader;

    std::vector<Texture *> m_textures_loaded;

    GLboolean m_process_armatures;
};

#endif // MESHLOADER_H
