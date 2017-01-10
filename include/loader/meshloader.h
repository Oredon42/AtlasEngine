#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "scenegraphnode.h"
#include "armatureloader.h"

class SceneGraphNode;
class Texture;

class MeshLoader
{
public:
    MeshLoader();

    void load(const aiScene *ai_scene, SceneGraphNode *scene_graph_node, MaterialLibrary &material_library, const GLboolean &process_armatures);

private:
    void processNode(aiNode *ai_node, const aiScene *ai_scene, SceneGraphNode *scene_graph_node, MaterialLibrary &material_library);
    Model *processMesh(const aiMesh *ai_mesh, const aiScene *ai_scene, const std::string &path, MaterialLibrary &material_library);
    void processGeometry(const aiMesh *ai_mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);

    ArmatureLoader m_armatureloader;
    std::vector<Texture *> m_textures_loaded;
    GLboolean m_process_armatures;
};

#endif // MESHLOADER_H
