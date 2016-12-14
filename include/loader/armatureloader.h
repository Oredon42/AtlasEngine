#ifndef ARMATURELOADER_H
#define ARMATURELOADER_H

#include <map>

#include "openglincludes.h"
#include "include/data/armature.h"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class AnimatedModel;
struct Bone;
struct VertexBoneData;

glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &assimp_mat);

class ArmatureLoader
{
public:
    ArmatureLoader();

    Armature *load(const aiScene *ai_scene, const aiMesh *ai_mesh);
    void fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *bone_tree, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh);
    void buildBoneTree(const aiNode *ai_node, Bone *bone_tree, const std::map<std::string, GLuint> &bone_mapping);
};

#endif // ARMATURELOADER_H
