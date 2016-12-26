#include "include/loader/meshloader.h"
#include "include/data/scene.h"
#include "include/data/model.h"
#include "include/data/scenegraphnode.h"

MeshLoader::MeshLoader()
{

}

void MeshLoader::load(const aiScene *ai_scene, Scene *scene, const GLboolean &process_armatures)
{
    m_process_armatures = process_armatures;

    glm::mat4 global_inverse_transform = glm::inverse(assimpToGlmMat4(ai_scene->mRootNode->mTransformation));

    SceneGraphRoot *scene_graph_root = new SceneGraphRoot(ai_scene->mRootNode->mName.C_Str(), scene->getPath(), global_inverse_transform, assimpToGlmMat4(ai_scene->mRootNode->mTransformation));
    scene->addSceneGraphRoot(scene_graph_root);

    aiNode *ai_node = ai_scene->mRootNode;

    //  Loop on every aiMesh
    for(GLuint i = 0; i < ai_node->mNumMeshes; ++i)
    {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        Model *model = processMesh(ai_mesh, ai_scene, scene);
        scene->getLastRoot()->insertModel(model);
        scene->insertModel(model);
    }

    //  Loop on every child
    for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
    {
        //  If child node has meshes (do not process light or bone nodes)
        if(ai_node->mChildren[i]->mNumMeshes > 0)
            scene->getLastRoot()->addChild(processNode(ai_node->mChildren[i], ai_scene, scene));
    }

    scene->getLastRoot()->spreadTransform(glm::mat4(1));
}

/*
 *  Process aiNode
 *  save the parent Node
 *  Loops over every mesh
 *  Loops over every node child
 * */
SceneGraphNode *MeshLoader::processNode(aiNode *ai_node, const aiScene *ai_scene, Scene *scene)
{
    std::string node_name = ai_node->mName.C_Str();
    SceneGraphNode *scene_graph_node = new SceneGraphNode(node_name, assimpToGlmMat4(ai_node->mTransformation));

    //  Loop on every aiMesh
    for(GLuint i = 0; i < ai_node->mNumMeshes; ++i)
    {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        Model *model = processMesh(ai_mesh, ai_scene, scene);
        scene_graph_node->insertModel(model);
        scene->insertModel(model);
    }

    //  Loop on every child
    for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
    {
        //  If child node has meshes (do not process light or bone nodes)
        if(ai_node->mChildren[i]->mNumMeshes > 0)
            scene_graph_node->addChild(processNode(ai_node->mChildren[i], ai_scene, scene));
    }
    return scene_graph_node;
}

/*
 *  Process aiMesh
 *  Return a Model that contains Vertices, Indexes,
 *  Materials and eventually Armature from an aiMesh
 * */
Model *MeshLoader::processMesh(const aiMesh *ai_mesh, const aiScene *ai_scene, Scene *scene)
{
    //  Geometry
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    processGeometry(ai_mesh, vertices, indices);


    //  Material
    aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    Material *material = new Material(ai_material, ai_mesh->HasBones(), scene->getPath(), m_textures_loaded);

    //  Mesh
    Mesh *mesh = new Mesh(vertices, indices, material->hasNormalMap());

    Model *model = new Model(mesh, material);

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
void MeshLoader::processGeometry(const aiMesh *ai_mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices)
{
    /*
     * Vertices
     * */
    for(GLuint i = 0; i < ai_mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position = glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z);

        if(ai_mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        if(ai_mesh->HasTangentsAndBitangents())
        {
            vertex.Tangent = glm::vec3(ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z);
            vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent);
        }
        vertices.push_back(vertex);
    }

    /*
     * Indexes
     * */
    for(GLuint i = 0; i < ai_mesh->mNumFaces; ++i)
    {
        aiFace face = ai_mesh->mFaces[i];
        for(GLuint j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }
}
