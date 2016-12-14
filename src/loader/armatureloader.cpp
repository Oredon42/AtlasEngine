#include "include/loader/armatureloader.h"
#include "include/data/material.h"

ArmatureLoader::ArmatureLoader()
{

}

Armature *ArmatureLoader::load(const aiScene *ai_scene, const aiMesh *ai_mesh)
{
    std::map<std::string, GLuint> bone_mapping;
    GLuint num_bones;
    Bone *bone_tree;
    VertexBoneData *vertex_bone_data;
    fillBoneInfos(bone_mapping, num_bones, bone_tree, vertex_bone_data, ai_mesh);
    buildBoneTree(ai_scene->mRootNode, bone_tree, bone_mapping);

    Armature *armature = new Armature(bone_mapping, num_bones, bone_tree, vertex_bone_data);

    return armature;
}

/*
 * Process bones of a mesh
 * fill bones informations
 * */
void ArmatureLoader::fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *bone_tree, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh)
{
    num_bones = 0;
    bone_tree = new Bone[ai_mesh->mNumBones];
    vertex_bone_data = new VertexBoneData[ai_mesh->mNumVertices];

    //  For each bone of the ai_mesh
    for(GLuint i = 0 ; i < ai_mesh->mNumBones ; ++i)
    {
        GLuint bone_index = 0;
        std::string bone_name(ai_mesh->mBones[i]->mName.C_Str());

        //  If new bone
        if(bone_mapping.find(bone_name) == bone_mapping.end())
        {
            //  Add the new bone in the armature
            bone_index = num_bones++;
            Bone bi;
            bi.name = bone_name;
            bone_tree[bone_index] = bi;

            bone_mapping[bone_name] = bone_index;
        }
        else
            // else, get the bone index in the bone mapping
            bone_index = bone_mapping[bone_name];

        //  Copy bone offset matrix in glm format
        bone_tree[bone_index].bone_offset = assimpToGlmMat4(ai_mesh->mBones[i]->mOffsetMatrix);

        //  Store bone weights inside vertex bone data
        for(GLuint j = 0 ; j < ai_mesh->mBones[i]->mNumWeights ; ++j)
        {
            GLuint vertexID = ai_mesh->mBones[i]->mWeights[j].mVertexId;
            GLfloat weight = ai_mesh->mBones[i]->mWeights[j].mWeight;
            vertex_bone_data[vertexID].AddBoneData(bone_index, weight);
        }
    }
}

/*
 * Build model bone tree
 * from aiScene root node
 * */
void ArmatureLoader::buildBoneTree(const aiNode *ai_node, Bone *bone_tree, const std::map<std::string, GLuint> &bone_mapping)
{
    std::string bone_name = ai_node->mName.C_Str();

    //  if has bone
    if(bone_mapping.find(ai_node->mName.C_Str()) != bone_mapping.end())
    {
        for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
        {
            std::string child_name = ai_node->mChildren[i]->mName.C_Str();

            //  if has child bone
            if(bone_mapping.find(child_name) != bone_mapping.end())
            {
                GLuint  parent_index = bone_mapping.find(bone_name)->second,
                        child_index = bone_mapping.find(child_name)->second;

                //  set parent
                bone_tree[child_index].parent = parent_index;
                bone_tree[parent_index].children.push_back(child_index);

                buildBoneTree(ai_node->mChildren[i], bone_tree, bone_mapping);
            }
        }
    }
    else
        for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
            buildBoneTree(ai_node->mChildren[i], bone_tree, bone_mapping);
}

/*
 * Convert assimp format mat4 to glm format
 * */
glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &assimp_mat)
{
    glm::mat4 glm_mat;

    glm_mat[0][0] = assimp_mat.a1; glm_mat[1][0] = assimp_mat.a2; glm_mat[2][0] = assimp_mat.a3; glm_mat[3][0] = assimp_mat.a4;
    glm_mat[0][1] = assimp_mat.b1; glm_mat[1][1] = assimp_mat.b2; glm_mat[2][1] = assimp_mat.b3; glm_mat[3][1] = assimp_mat.b4;
    glm_mat[0][2] = assimp_mat.c1; glm_mat[1][2] = assimp_mat.c2; glm_mat[2][2] = assimp_mat.c3; glm_mat[3][2] = assimp_mat.c4;
    glm_mat[0][3] = assimp_mat.d1; glm_mat[1][3] = assimp_mat.d2; glm_mat[2][3] = assimp_mat.d3; glm_mat[3][3] = assimp_mat.d4;

    return glm_mat;
}
