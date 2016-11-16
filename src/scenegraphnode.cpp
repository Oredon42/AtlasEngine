#include "include/scenegraphnode.h"
#include "include/model.h"
#include "include/camera.h"
#include "include/dirlight.h"
#include "include/spotlight.h"
#include "include/pointlight.h"

#include <string>

SceneGraphNode::SceneGraphNode(const std::string &path, glm::mat4 &global_inverse_transform) :
    m_parent(0),
    m_directory(path),
    m_global_inverse_transform(global_inverse_transform),
    m_scale(1)
{

}

SceneGraphNode::~SceneGraphNode()
{
    //  Mesh array suppression
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        for(GLuint j = 0; j < (GLuint)(m_models[i].size()); ++j)
            delete m_models[i][j];
        m_models[i].clear();
    }

    m_models->clear();


    //  Children array suppression
    for(GLuint i = 0; i < m_children.size(); ++i)
        delete m_children[i];

    m_children.clear();
}

void SceneGraphNode::translate(const glm::vec3 &t, const std::string &name)
{
    if(m_name == name)
    {
        m_position = t;
        calculateTransform(glm::mat4());
        spreadTransform(m_transform);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->translate(t, name);
}

void SceneGraphNode::calculateTransform(const glm::mat4 &parent_transform)
{
    m_transform = glm::mat4_cast(m_rotation);
    m_transform = glm::translate(m_transform, m_position);
    m_transform = glm::scale(m_transform, m_scale);

    m_transform *= parent_transform;

    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
        for(GLuint j = 0; j < m_models[i].size(); ++j)
            m_models[i][j]->setTransform(m_transform);
}

/********************
 * MODEL LOADING    *
 * ******************/

/*
 *  Process aiNode
 *  save the parent Node
 *  Loops over every mesh
 *  Loops over every node child
 * */
void SceneGraphNode::processNode(SceneGraphNode *parent, glm::mat4 &global_inverse_transform, aiNode *ai_node, const aiScene *ai_scene, std::vector<Model *> (&models)[NB_SHADER_TYPES], std::vector<AnimatedModel *> *animated_models, GLfloat &render_time)
{
    //  Get current node transformation matrix
    //m_global_inverse_transform = glm::inverse(assimpToGlmMat4(ai_node->mTransformation));

    //  Parent node
    m_parent = parent;

    m_name = ai_node->mName.C_Str();

    //  Loop on every aiMesh
    for(GLuint i = 0; i < ai_node->mNumMeshes; ++i)
    {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        Model *model = processMesh(ai_mesh, ai_scene, animated_models, render_time);
        GLuint insert_index = model->getShaderTypeIndex();
        //  Add model to node list
        m_models[insert_index].push_back(model);
        //  Add model to scene list
        models[insert_index].push_back(model);
    }

    //  Loop on every child
    for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
    {
        //  If child node has meshes (do not process light or bone nodes)
        if(ai_node->mChildren[i]->mNumMeshes > 0)
        {
            //  Process child
            SceneGraphNode *child = new SceneGraphNode(m_directory, global_inverse_transform);
            child->processNode(this, global_inverse_transform, ai_node->mChildren[i], ai_scene, models, animated_models, render_time);
            m_children.push_back(child);
        }
    }
}

/*
 *  Process aiMesh
 *  Return a Mesh that contains vertices, indexes,
 *  materials and eventually bones from an aiMesh
 * */
Model *SceneGraphNode::processMesh(const aiMesh *ai_mesh, const aiScene *ai_scene, std::vector<AnimatedModel *> *animated_models, GLfloat &render_time)
{
    /*
     *  Geometry
     * */
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    //  Loop on every vertex
    for(GLuint i = 0; i < ai_mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // Positions
        vector.x = ai_mesh->mVertices[i].x;
        vector.y = ai_mesh->mVertices[i].y;
        vector.z = ai_mesh->mVertices[i].z;
        vertex.Position = vector;
        // Normals
        vector.x = ai_mesh->mNormals[i].x;
        vector.y = ai_mesh->mNormals[i].y;
        vector.z = ai_mesh->mNormals[i].z;
        vertex.Normal = vector;
        // Texture Coordinates
        if(ai_mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = ai_mesh->mTextureCoords[0][i].x;
            vec.y = ai_mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        //  Tangents
        if(ai_mesh->HasTangentsAndBitangents())
        {
            vector.x = ai_mesh->mTangents[i].x;
            vector.y = ai_mesh->mTangents[i].y;
            vector.z = ai_mesh->mTangents[i].z;
            vertex.Tangent = vector;

            vector = glm::cross(vertex.Normal, vertex.Tangent);
            vertex.Bitangent = vector;
        }

        vertices.push_back(vertex);
    }
    //  Indexes
    for(GLuint i = 0; i < ai_mesh->mNumFaces; i++)
    {
        aiFace face = ai_mesh->mFaces[i];
        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }


    /*
     *  Material
     * */
    aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

    //  Textures

    // Diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(ai_material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // Specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(ai_material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //  Normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(ai_material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    //  Normal map check
    GLboolean has_normal_map = false;
    if(normalMaps.size() > 0)
        has_normal_map = true;

    /*
     *  Bones
     * */
    if(ai_mesh->HasBones())
    {
        std::map<std::string, GLuint> bone_mapping;
        GLuint num_bones = 0;
        Bone *armature = new Bone[ai_mesh->mNumBones];
        VertexBoneData *vertex_bone_data;

        fillBoneInfos(bone_mapping, num_bones, armature, vertex_bone_data, ai_mesh);

        ShaderType shader_type = getShaderType(GL_TRUE, diffuseMaps.size(), specularMaps.size(), normalMaps.size());
        AnimatedMesh *mesh = new AnimatedMesh(vertices, indices, has_normal_map, vertex_bone_data);
        mesh->setupBuffers();
        AnimatedModel *model = new AnimatedModel(mesh, textures, shader_type, bone_mapping, num_bones, armature, has_normal_map, render_time);

        //  Adding the AnimatedMesh inside the AnimatedMesh list
        animated_models->push_back(model);
        return model;
    }

    //  No bone
    ShaderType shader_type = getShaderType(GL_FALSE, diffuseMaps.size(), specularMaps.size(), normalMaps.size());
    Mesh *mesh = new Mesh(vertices, indices, has_normal_map);
    mesh->setupBuffers();
    return new Model(mesh, textures, shader_type, has_normal_map);
}

/*
 *  Load textures from aiMaterial
 * */
std::vector<Texture> SceneGraphNode::loadMaterialTextures(const aiMaterial *mat, const aiTextureType &type, const std::string &type_name)
{
    std::vector<Texture> textures;

    //  For each texture
    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        GLboolean skip = GL_FALSE;

        //  Check if texture is not already loaded
        for(GLuint j = 0; j < m_textures_loaded.size(); j++)
        {
            if(m_textures_loaded[j].path == str.C_Str())
            {
                //  If texture already loaded
                //  Push it back
                textures.push_back(m_textures_loaded[j]);
                skip = GL_TRUE;
                break;
            }
        }
        if(!skip)
        {
            //  If not already loaded
            //  Load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str());
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            m_textures_loaded.push_back(texture);
        }
    }
    return textures;
}

/*
 *  Load texture from a file
 *  Returns the id of the texture
 * */
GLint SceneGraphNode::TextureFromFile(const GLchar *path)
{
    //  Concat directory path to filename
    std::string filename = m_directory + '/' + std::string(path);

    GLuint texture_ID;
    glGenTextures(1, &texture_ID);

    glBindTexture(GL_TEXTURE_2D, texture_ID);
    QImage image_container(filename.c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_container.width(), image_container.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image_container.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_ID;
}


/************
 * ANIMATION *
 * **********/

/*
 * Build model bone tree
 * from aiScene root node
 * */
void SceneGraphNode::buildBoneTree(AnimatedModel *model, const aiNode *ai_node)
{
    if(model->hasBone(std::string(ai_node->mName.C_Str())))
    {
        for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
        {
            model->setBoneParent(std::string(ai_node->mName.C_Str()), std::string(ai_node->mChildren[i]->mName.C_Str()));

            buildBoneTree(model, ai_node->mChildren[i]);
        }
    }
    else
        for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
            buildBoneTree(model, ai_node->mChildren[i]);
}

/*
 * Process bones of a mesh
 * fill bones informations
 * */
void SceneGraphNode::fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *armature, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh)
{
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
        armature[bone_index].bone_offset = assimpToGlmMat4(ai_mesh->mBones[i]->mOffsetMatrix);

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
 * Load animations from an aiScene
 * fill informations of meshes inside the current SceneGraph
 * */
void SceneGraphNode::loadAnimations(const aiScene *ai_scene, const std::vector<AnimatedModel *> &animated_models)
{
    /*
     * This loop will store animation pose
     * for each tick of every animation
     * So we don't have to traverse the aiScene tree at runtime
     * (I've chosen to iterate over animation and traverse tree as it is more current to have 1 animation per file)
     * */
    for(GLuint animation_index = 0; animation_index < ai_scene->mNumAnimations; ++animation_index)
    {
        buildBoneTree(animated_models[animation_index], ai_scene->mRootNode);

        //  Get animation parameters
        GLfloat animation_time = ai_scene->mAnimations[animation_index]->mDuration;
        GLfloat ticks_per_seconds = ai_scene->mAnimations[animation_index]->mTicksPerSecond != 1?ai_scene->mAnimations[0]->mTicksPerSecond : 24.0f;
        //  Total time of the animation in ticks
        GLuint time_in_ticks = animation_time * ticks_per_seconds;

        aiAnimation *ai_animation = ai_scene->mAnimations[animation_index];

        //  For each tick of the animation
        //  Retrieve animation pose
        for(GLuint current_tick = 0; current_tick < time_in_ticks; ++current_tick)
            processAnimation(ai_animation, "default", ticks_per_seconds, current_tick, current_tick * time_in_ticks, time_in_ticks, ai_scene->mRootNode, animated_models);
    }
}

/*
 *  Return aiNodeAnim with the name node_name
 * */
const aiNodeAnim* SceneGraphNode::FindNodeAnim(const aiAnimation *ai_animation, const std::string &node_name)
{
    //  Loop over animation channels
    for(GLuint i = 0 ; i < ai_animation->mNumChannels ; ++i)
    {
        const aiNodeAnim* ai_node_anim = ai_animation->mChannels[i];

        if(std::string(ai_node_anim->mNodeName.data) == node_name)
            return ai_node_anim;
    }

    //  If not found
    return NULL;
}

/*
 *  Process aiNode - each node is a bone
 *
 *  for the current animation and the current time in ticks
 *  create interpolated transformation matrix
 *  store matrix into concerned animated meshes
 * */
void SceneGraphNode::processAnimation(const aiAnimation *ai_animation, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLfloat &total_time, const aiNode *ai_node, const std::vector<AnimatedModel *> &animated_models)
{
    std::string bone_name = ai_node->mName.data;

    //  Try to find the bone inside animation
    const aiNodeAnim *ai_node_anim = FindNodeAnim(ai_animation, bone_name);

    glm::vec3 scaling;
    glm::quat rotation;
    glm::vec3 position;

    //  If bone found
    if(ai_node_anim)
    {
        scaling = glm::vec3(ai_node_anim->mScalingKeys[current_tick].mValue.x, ai_node_anim->mScalingKeys[current_tick].mValue.y, ai_node_anim->mScalingKeys[current_tick].mValue.z);
        rotation = glm::quat(ai_node_anim->mRotationKeys[current_tick].mValue.x, ai_node_anim->mRotationKeys[current_tick].mValue.y, ai_node_anim->mRotationKeys[current_tick].mValue.z, ai_node_anim->mRotationKeys[current_tick].mValue.w);
        position = glm::vec3(ai_node_anim->mPositionKeys[current_tick].mValue.x, ai_node_anim->mPositionKeys[current_tick].mValue.y, ai_node_anim->mPositionKeys[current_tick].mValue.z);
    }
    else
    {
        scaling = glm::vec3(1);
        rotation = glm::quat(0.f, 0.f, 0.f, 0.f);
        position = glm::vec3(0);
    }

    //  Loop over every animated model
    for(GLuint i = 0; i < animated_models.size(); ++i)
    {
        //  If current model has the current bone
        //  Send animation datas
        if(animated_models[i]->hasBone(bone_name))
            animated_models[i]->setAnimationInfo(animation_name, ai_animation->mNumChannels, total_time, ticks_per_second, bone_name, current_tick, current_time, scaling, rotation, position);
    }

    //  Loop over children bones
    for(GLuint i = 0 ; i < ai_node->mNumChildren ; ++i)
       processAnimation(ai_animation, animation_name, ticks_per_second, current_tick, current_time, total_time, ai_node->mChildren[i], animated_models);
}

/*
 * Scene graph root creation from aiScene
 * */
SceneGraphRoot::SceneGraphRoot(const aiScene *scene, glm::mat4 &global_inverse_transform, const std::string &path, std::vector<Model *> (&models)[NB_SHADER_TYPES], GLfloat &render_time) :
    SceneGraphNode(path, global_inverse_transform)
{
    m_directory = path;

    //  Process root node
    //  Fills the mesh vector
    processNode(0, m_global_inverse_transform, scene->mRootNode, scene, models, &m_animated_models, render_time);


    //  Animation
    if(scene->HasAnimations())
        loadAnimations(scene, m_animated_models);
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
