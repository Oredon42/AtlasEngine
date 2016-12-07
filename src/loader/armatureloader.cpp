#include "include/loader/armatureloader.h"
#include "include/data/animatedmodel.h"
#include "include/data/material.h"

ArmatureLoader::ArmatureLoader()
{

}

AnimatedModel *ArmatureLoader::load()
{
    /*std::map<std::string, GLuint> bone_mapping;
    GLuint num_bones;
    Bone *armature;
    VertexBoneData *vertex_bone_data;
    fillBoneInfos(bone_mapping, num_bones, armature, vertex_bone_data, ai_mesh);

    Material *material = new Material(ai_material, GL_TRUE, m_path, m_textures_loaded);
    AnimatedMesh *mesh = new AnimatedMesh(vertices, indices, material->hasNormalMap(), vertex_bone_data);
    mesh->setupBuffers();
    AnimatedModel *animated_model = new AnimatedModel(mesh, bone_mapping, num_bones, armature, render_time, material);

    return animated_model;*/
    return 0;
}

/*
 * Process bones of a mesh
 * fill bones informations
 * */
void ArmatureLoader::fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *armature, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh)
{
    num_bones = 0;
    armature = new Bone[ai_mesh->mNumBones];
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
            armature[bone_index] = bi;

            bone_mapping[bone_name] = bone_index;
        }
        else
            // else, get the bone index in the bone mapping
            bone_index = bone_mapping[bone_name];

        //  Copy bone offset matrix in glm format
        //armature[bone_index].bone_offset = assimpToGlmMat4(ai_mesh->mBones[i]->mOffsetMatrix);

        //  Store bone weights inside vertex bone data
        for(GLuint j = 0 ; j < ai_mesh->mBones[i]->mNumWeights ; ++j)
        {
            GLuint vertexID = ai_mesh->mBones[i]->mWeights[j].mVertexId;
            GLfloat weight = ai_mesh->mBones[i]->mWeights[j].mWeight;
            vertex_bone_data[vertexID].AddBoneData(bone_index, weight);
        }
    }
}
