#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>

#include <glew.h>
#include <glm/glm.hpp>

class Model;
class Texture;
class Database;
class SceneGraphNode;
class TextureLoader;
class ArmatureLoader;
struct aiScene;
struct aiNode;
struct aiMesh;

class ModelLoader
{
public:
    ModelLoader();

    void load(Database *pDatabase, const aiScene *pAiScene);

private:
    void processNode(Database *pDatabase, aiNode *pAiNode, const aiScene *pAiScene, SceneGraphNode *pSceneGraphNode);
    Model *processMesh(Database *pDatabase, const aiScene *pAiScene, const aiMesh *pAiMesh, const std::string &sPath);
    void processGeometry(Database *pDatabase, const aiMesh *pAiMesh, std::vector<Vertex> &vVertices, std::vector<GLuint> &vIndices);

    ArmatureLoader *m_pArmatureLoader;
    std::vector<Texture *> m_vTexturesLoaded;
};

#endif // MODELLOADER_H
