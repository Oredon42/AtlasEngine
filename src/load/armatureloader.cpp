#include "src/loader/armatureloader.h"
#include "src/core/data/material.h"
#include "src/core/data/database.h"
#include "src/core/data/armature.h"

#include "assimp/scene.h"
#include "assimp/matrix4x4.h"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include "lib/glm/glm.hpp"

ArmatureLoader::ArmatureLoader()
{

}

/*
 * Load Armature from an aiMesh
 * and insert it inside Database
 * */
Armature *ArmatureLoader::load(Database *pDatabase, const aiScene *pAiScene, const aiMesh *pAiMesh)
{
    if(pAiMesh->HasBones() == true)
    {
        std::map<std::string, unsigned int> mBoneMap;
        Bone *pBoneTree = nullptr;
        VertexBoneData *pVertexBoneData = nullptr;

        //  Load bones
        fillBoneInfos(pBoneTree, mBoneMap, pVertexBoneData, pAiMesh);

        //  Build Armature data
        buildBoneTree(pAiScene->mRootNode, pBoneTree, mBoneMap);

        //  Create Armature
        Armature *pArmature = new Armature(pBoneTree, mBoneMap, pVertexBoneData);

        //  Insert into Database
        pDatabase->insertArmature(pArmature, pAiMesh->mName.C_Str());

        return pArmature;
    }
}

/*
 * Process bones of an aiMesh
 * and fill bone map and vertex bone data
 * */
void ArmatureLoader::fillBoneInfos(Bone *pBoneTree, std::map<std::string, GLuint> &mBoneMap, VertexBoneData *&pVertexBoneData, const aiMesh *pAiMesh)
{
    unsigned int uiNumBones = 0;
    if(pBoneTree != nullptr)
    {
        delete[] pBoneTree;
    }
    pBoneTree = new Bone[pAiMesh->mNumBones];
    if(pVertexBoneData != nullptr)
    {
        delete[] pVertexBoneData;
    }
    pVertexBoneData = new VertexBoneData[pAiMesh->mNumVertices];

    //  For each bone of the aiMesh
    for(unsigned int uiBoneAssimpIndex = 0 ; uiBoneAssimpIndex < pAiMesh->mNumBones ; ++uiBoneAssimpIndex)
    {
        unsigned int uiBoneTreeIndex = 0;
        const std::string strBoneName = ai_mesh->mBones[uiBoneAssimpIndex]->mName.C_Str();

        if(mBoneMap.find(strBoneName) == mBoneMap.end())
        {
            //  If new bone, add it to the armature
            uiBoneTreeIndex = uiNumBones++;
            mBoneMap[strBoneName] = uiBoneTreeIndex;
            pBoneTree[uiBoneTreeIndex].m_strName = strBoneName;
        }
        else
        {
            //  If bone already exists, get the bone index in the bone mapping
            uiBoneTreeIndex = mBoneMap[strBoneName];
        }

        //  Get bone offset matrix in glm format
        pBoneTree[uiBoneTreeIndex].mBoneOffset = assimpToGlmMat4(pAiMesh->mBones[uiBoneAssimpIndex]->mOffsetMatrix);

        //  Store bone weights inside vertex bone data
        for(unsigned int uiWeightIndex = 0 ; uiWeightIndex < pAiMesh->mBones[uiBoneAssimpIndex]->mNumWeights; ++uiWeightIndex)
        {
            const GLuint uiVertexId = (GLuint)pAiMesh->mBones[uiBoneAssimpIndex]->mWeights[uiWeightIndex].mVertexId;
            const GLfloat fVertexWeight = (GLfloat)pAiMesh->mBones[uiBoneAssimpIndex]->mWeights[uiWeightIndex].mWeight;
            pVertexBoneData[uiVertexId].AddBoneData(uiBoneTreeIndex, fVertexWeight);
        }
    }
}

/*
 * Recursive function
 *
 * Build Bone tree (tree is an array,
 * each Bone has indices of parent and children)
 * from aiScene root node
 *
 * each aiNode is a Bone
 * */
void ArmatureLoader::buildBoneTree(const aiNode *pAiNode, Bone *pBoneTree, const std::map<std::string, GLuint> &mBoneMap)
{
    const std::string strBoneName = pAiNode->mName.C_Str();

    //  If current Bone is in Armature
    if(mBoneMap.find(strBoneName) != mBoneMap.end())
    {
        //  Look for children Bones also used in Armature
        for(unsigned int uiChildIndex = 0; uiChildIndex < pAiNode->mNumChildren; ++uiChildIndex)
        {
            const std::string strBoneChildName = pAiNode->mChildren[uiChildIndex]->mName.C_Str();

            //  If child Bone is used in Armature
            if(mBoneMap.find(strBoneChildName) != mBoneMap.end())
            {
                const GLuint    uiBoneParentIndex = mBoneMap.find(strBoneName)->second,
                                uiBoneChildIndex = mBoneMap.find(strBoneChildName)->second;

                //  Set relationship between Bones
                pBoneTree[uiChildIndex].parent = uiBoneParentIndex;
                pBoneTree[uiBoneParentIndex].children.push_back(uiBoneChildIndex);

                //  Then, recursion on children
                buildBoneTree(pAiNode->mChildren[uiChildIndex], pBoneTree, mBoneMap);
            }
        }
    }
    else
    {
        //  Current Bone is not used in Armature, recursion on children
        for(unsigned int uiChildIndex = 0; uiChildIndex < pAiNode->mNumChildren; ++uiChildIndex)
        {
            buildBoneTree(pAiNode->mChildren[uiChildIndex], pBoneTree, mBoneMap);
        }
    }
}

/*
 * Convert Assimp format mat4 to glm format
 * */
glm::mat4 assimpToGlmMat4(const aiMatrix4x4 &mAssimpMat)
{
    glm::mat4 mGlmMat;

    mGlmMat[0][0] = mAssimpMat.a1; mGlmMat[1][0] = mAssimpMat.a2; mGlmMat[2][0] = mAssimpMat.a3; mGlmMat[3][0] = mAssimpMat.a4;
    mGlmMat[0][1] = mAssimpMat.b1; mGlmMat[1][1] = mAssimpMat.b2; mGlmMat[2][1] = mAssimpMat.b3; mGlmMat[3][1] = mAssimpMat.b4;
    mGlmMat[0][2] = mAssimpMat.c1; mGlmMat[1][2] = mAssimpMat.c2; mGlmMat[2][2] = mAssimpMat.c3; mGlmMat[3][2] = mAssimpMat.c4;
    mGlmMat[0][3] = mAssimpMat.d1; mGlmMat[1][3] = mAssimpMat.d2; mGlmMat[2][3] = mAssimpMat.d3; mGlmMat[3][3] = mAssimpMat.d4;

    return mGlmMat;
}
