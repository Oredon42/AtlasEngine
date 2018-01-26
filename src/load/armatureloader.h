#ifndef ARMATURELOADER_H
#define ARMATURELOADER_H

#include <map>

#include "src/core/openglincludes.h"

class AnimatedModel;
class Database;
class Armature;
struct Bone;
struct VertexBoneData;
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMatrix4x4;

glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &mAssimpMat);

class ArmatureLoader
{
public:
    ArmatureLoader();

    Armature *load(Database *pDatabase, const aiScene *pAiScene, const aiMesh *pAiMesh);

private:
    void fillBoneInfos(Bone *pBoneTree, std::map<std::string, GLuint> &mBoneMap, VertexBoneData *&pVertexBoneData, const aiMesh *pAiMesh);
    void buildBoneTree(const aiNode *pAiNode, Bone *pBoneTree, const std::map<std::string, GLuint> &mBoneMap);
};

#endif // ARMATURELOADER_H
