#include "include/data/scene.h"
#include "include/loader/meshloader.h"
#include "include/data/scenegraphnode.h"

MeshLoader::MeshLoader()
{

}

void MeshLoader::load(const aiScene *ai_scene, Scene &scene)
{
    glm::mat4 inverse_transform = glm::inverse(assimpToGlmMat4(ai_scene->mRootNode->mTransformation));

    std::vector<AnimatedModel *> *animated_models = new std::vector<AnimatedModel *>;

    //scene.addSceneGraphRoot(processNode(0, inverse_transform, ai_scene->mRootNode, ai_scene, scene, animated_models));
}

/*
 *  Process aiNode
 *  save the parent Node
 *  Loops over every mesh
 *  Loops over every node child
 * */
SceneGraphNode *MeshLoader::processNode(SceneGraphNode *parent, aiNode *ai_node, const aiScene *ai_scene, Scene &scene, std::vector<AnimatedModel *> *animated_models, const std::string &path, const glm::mat4 &global_inverse_transform)
{
    /*SceneGraphNode *scene_graph_node = new SceneGraphNode(parent, ai_node->mName.C_Str(), path, global_inverse_transform, assimpToGlmMat4(ai_node->mTransformation));

    //  Loop on every aiMesh
    for(GLuint i = 0; i < ai_node->mNumMeshes; ++i)
    {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        Model *model = processMesh(ai_mesh, ai_scene, scene, animated_models);
        GLuint shader_index = model->getShaderTypeIndex();
        scene_graph_node->insertModel(model, shader_index);
        scene.insertModel(model, shader_index);
    }

    //  Loop on every child
    for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
    {
        //  If child node has meshes (do not process light or bone nodes)
        if(ai_node->mChildren[i]->mNumMeshes > 0)
            scene_graph_node->addChild(processNode(scene_graph_node, ai_node->mChildren[i], ai_scene, scene, animated_models, path, global_inverse_transform));
    }*/
    return 0;
}

/*
 *  Process aiMesh
 *  Return a Mesh that contains vertices, indexes,
 *  materials and eventually bones from an aiMesh
 * */
Model *MeshLoader::processMesh(const aiMesh *ai_mesh, const aiScene *ai_scene, Scene &scene)
{
    //  Geometry
    /*std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    processGeometry(ai_mesh, vertices, indices);


    //  Material
    aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];


    //  Bones
    if(ai_mesh->HasBones())
    {
        AnimatedModel *animated_model = m_armatureloader.load();
        scene.getLastRoot()->addAnimatedModel(animated_model);
        return animated_model;
    }

    //  No bone
    Material *material = new Material(ai_material, GL_FALSE, m_path, m_textures_loaded);
    Mesh *mesh = new Mesh(vertices, indices, material->hasNormalMap());
    mesh->setupBuffers();
    return new Model(mesh, material);*/
    return 0;
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
