#include "src/loader/modelloader.h"
#include "src/core/data/scenegraphnode.h"
#include "src/core/data/model.h"
#include "src/core/data/database.h"
#include "src/loader/armatureloader.h"

#include "assimp/scene.h"

ModelLoader::ModelLoader()
{

}

/*
 * Load aiScene and build
 * Scene Graph with pSceneGraphNode as root
 * */
void ModelLoader::load(Database *pDatabase, const aiScene *pAiScene)
{
    if(pAiScene->HasMeshes() == true)
    {
        const aiNode *pAiNode = pAiScene->mRootNode;

        //  Create root node
        const glm::mat4 mGlobalInverseTransform = glm::inverse(assimpToGlmMat4(pAiNode->mTransformation));
        const std::string sRootNodeName = pAiNode->mName;
        SceneGraphRoot *pSceneGraphRoot = new SceneGraphRoot(mGlobalInverseTransform);

        //  Process aiNodes
        processNode(pDatabase, pAiNode, pAiScene, pSceneGraphNode);

        //  build Scene Graph
        pSceneGraphNode->spreadTransform();

        //  Add root to Database
        pDatabase->insertSceneGraphRoot(pSceneGraphRoot, sRootNodeName);
    }
}

/*
 * Process aiNodes from a root (recursive function)
 * load and save Models
 * then loop on children nodes
 * */
void ModelLoader::processNode(Database *pDatabase, aiNode *pAiNode, const aiScene *pAiScene, SceneGraphNode *pSceneGraphNode)
{
    //  Apply aiNode transformation
    pSceneGraphNode->transform(assimpToGlmMat4(pAiNode->mTransformation));

    //  Loop on Models
    for(unsigned int uiMeshIndex = 0; uiMeshIndex < pAiNode->mNumMeshes; ++uiMeshIndex)
    {
        //  Load Model
        const aiMesh *pAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[uiMeshIndex]];
        Model *pModel = processMesh(pDatabase, pAiScene, pAiMesh, pSceneGraphNode->getPath());

        //  Add model to the node
        pSceneGraphNode->addModel(pModel);

        //  Add model to Database
        pDatabase->insertModel(pModel, pAiMesh->mName);
    }

    //  Loop on children
    for(unsigned int uiChildIndex = 0; uiChildIndex < pAiNode->mNumChildren; ++uiChildIndex)
    {
        //  If child node has meshes (avoid processing light or bone nodes)
        if(pAiNode->mChildren[uiChildIndex]->mNumMeshes > 0)
        {
            SceneGraphNode *child = new SceneGraphNode(pSceneGraphNode->getPath(), pSceneGraphNode->getGlobalInverseTransform(), pAiNode->mChildren[i]->mName.C_Str());
            processNode(pAiNode->mChildren[i], pAiScene, child, database);
            pSceneGraphNode->addChild(child);
        }
    }

    //  Add node to Database
    const std::string sNodeName = ai_node->mName.C_Str();
    pDatabase->insertSceneGraphNode(pSceneGraphNode, sNodeName);
}

/*
 * Process aiMesh
 * Return a Model that contains Vertices, Indexes,
 * Materials and eventually Armature from an aiMesh
 * */
Model *ModelLoader::processMesh(Database *pDatabase, const aiScene *pAiScene, const aiMesh *pAiMesh, const std::string &sPath)
{
    //  Geometry
    std::vector<Vertex> vVertices;
    std::vector<GLuint> vIndices;
    processGeometry(pDatabase, pAiMesh, vVertices, vIndices);


    //  Material
    aiMaterial *pAiMaterial = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

    const void *pAddress = static_cast<const void *>(pAiMaterial);
    std::stringstream ss;
    ss << address;
    std::string sMaterialName = ss.str();

    Material *pMaterial = database.getMaterial(sMaterialName);


    //  Mesh
    Mesh *pMesh = new Mesh(vVertices, vIndices, material->hasNormalMap());

    Model *pModel = new Model(pMesh, pMaterial);

    //  Bones
    if(m_process_armatures && ai_mesh->HasBones())
    {
        Armature *armature = m_armatureloader.load(ai_scene, ai_mesh);
        model->attachArmature(armature);
    }

    return model;
}

/*
 * Fill vertices and indices vectors
 * from aiMesh
 * */
void ModelLoader::processGeometry(Database *pDatabase, const aiMesh *pAiMesh, std::vector<Vertex> &vVertices, std::vector<GLuint> &vIndices)
{
    /*
     * Vertices
     * */
    for(unsigned int i = 0; i < pAiMesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.Position = glm::vec3(pAiMesh->mVertices[i].x, pAiMesh->mVertices[i].y, pAiMesh->mVertices[i].z);
        vertex.Normal = glm::vec3(pAiMesh->mNormals[i].x, pAiMesh->mNormals[i].y, pAiMesh->mNormals[i].z);

        if(pAiMesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(pAiMesh->mTextureCoords[0][i].x, pAiMesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        if(pAiMesh->HasTangentsAndBitangents())
        {
            vertex.Tangent = glm::vec3(pAiMesh->mTangents[i].x, pAiMesh->mTangents[i].y, pAiMesh->mTangents[i].z);
            vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent);
        }
        vVertices.push_back(vertex);
    }

    /*
     * Indexes
     * */
    for(unsigned int i = 0; i < pAiMesh->mNumFaces; ++i)
    {
        aiFace face = pAiMesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; ++j)
            vIndices.push_back(face.mIndices[j]);
    }
}
