#ifndef ARMATURELOADER_H
#define ARMATURELOADER_H

#include <map>

#include "openglincludes.h"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class AnimatedModel;
class Bone;
class VertexBoneData;

class ArmatureLoader
{
public:
    ArmatureLoader();

    AnimatedModel *load();
    void fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *armature, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh);
};

#endif // ARMATURELOADER_H
