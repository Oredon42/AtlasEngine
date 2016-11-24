#include "include/scenegraphnode.h"
#include "include/model.h"
#include "include/camera.h"
#include "include/dirlight.h"
#include "include/spotlight.h"
#include "include/pointlight.h"
#include "include/animation.h"

#include <string>

SceneGraphNode::SceneGraphNode(std::string &path, glm::mat4 &global_inverse_transform) :
    m_parent(0),
    m_path(path),
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

    //  Texture loaded suppression
    for(GLuint i = 0; i < m_textures_loaded.size(); ++i)
        delete m_textures_loaded[i];

    m_textures_loaded.clear();


    //  Children array suppression
    for(GLuint i = 0; i < m_children.size(); ++i)
        delete m_children[i];

    m_children.clear();
}

/*
 * If name is this node name
 * translate this node with t
 * and spread to every child
 * */
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

/*
 * If name is this node name
 * rotate this node with r
 * and spread to every child
 * */
void SceneGraphNode::rotate(const glm::vec3 &r, const std::string &name)
{
    if(m_name == name)
    {
        m_rotation = glm::quat(r);
        calculateTransform(glm::mat4());
        spreadTransform(m_transform);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->rotate(r, name);
}

/*
 * If name is this node name
 * scale this node with s
 * and spread to every child
 * */
void SceneGraphNode::scale(const glm::vec3 &s, const std::string &name)
{
    if(m_name == name)
    {
        m_scale = s;
        calculateTransform(glm::mat4());
        spreadTransform(m_transform);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->scale(s, name);
}

/*
 * Calculate node transform matrix
 * with saved transform,
 * saved rotation, saved scale and saved translation
 * And set every models of this node transforms
 * */
void SceneGraphNode::calculateTransform(const glm::mat4 &parent_transform)
{
    m_transform = m_node_transform;
    m_transform *= glm::mat4_cast(m_rotation);
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
void SceneGraphNode::processNode(SceneGraphNode *parent, glm::mat4 &global_inverse_transform, aiNode *ai_node, const aiScene *ai_scene, std::vector<Model *> (&scene_models)[NB_SHADER_TYPES], std::vector<AnimatedModel *> *animated_models, GLfloat &render_time)
{
    m_parent = parent;
    m_name = ai_node->mName.C_Str();
    m_node_transform = assimpToGlmMat4(ai_node->mTransformation);

    //  Loop on every aiMesh
    for(GLuint i = 0; i < ai_node->mNumMeshes; ++i)
    {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        Model *model = processMesh(ai_mesh, ai_scene, animated_models, render_time);
        GLuint insert_index = model->getShaderTypeIndex();
        m_models[insert_index].push_back(model);
        scene_models[insert_index].push_back(model);
    }

    //  Loop on every child
    for(GLuint i = 0; i < ai_node->mNumChildren; ++i)
    {
        //  If child node has meshes (do not process light or bone nodes)
        if(ai_node->mChildren[i]->mNumMeshes > 0)
        {
            //  Process child
            SceneGraphNode *child = new SceneGraphNode(m_path, global_inverse_transform);
            child->processNode(this, global_inverse_transform, ai_node->mChildren[i], ai_scene, scene_models, animated_models, render_time);
            m_children.push_back(child);
        }

        //  Put armature process here?
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
    processGeometry(ai_mesh, vertices, indices);


    /*
     *  Material
     * */
    aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

    /*
     *  Bones
     * */
    if(ai_mesh->HasBones())
    {
        std::map<std::string, GLuint> bone_mapping;
        GLuint num_bones;
        Bone *armature;
        VertexBoneData *vertex_bone_data;
        fillBoneInfos(bone_mapping, num_bones, armature, vertex_bone_data, ai_mesh);

        //  Create AnimatedModel
        Material *material = new Material(ai_material, GL_TRUE, m_path, m_textures_loaded);
        AnimatedMesh *mesh = new AnimatedMesh(vertices, indices, material->hasNormalMap(), vertex_bone_data);
        mesh->setupBuffers();
        AnimatedModel *animated_model = new AnimatedModel(mesh, bone_mapping, num_bones, armature, render_time, material);

        //  Adding the AnimatedMesh inside the AnimatedMesh list
        animated_models->push_back(animated_model);
        return animated_model;
    }

    //  No bone
    Material *material = new Material(ai_material, GL_FALSE, m_path, m_textures_loaded);
    Mesh *mesh = new Mesh(vertices, indices, material->hasNormalMap());
    mesh->setupBuffers();
    return new Model(mesh, material);
}

/*
 * Fill vertices and indices vectors
 * from aiMesh
 * */
void SceneGraphNode::processGeometry(const aiMesh *ai_mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices)
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


/************
 * ANIMATION *
 * **********/

/*
 * Process bones of a mesh
 * fill bones informations
 * */
void SceneGraphNode::fillBoneInfos(std::map<std::string, GLuint> &bone_mapping, GLuint &num_bones, Bone *armature, VertexBoneData *&vertex_bone_data, const aiMesh *ai_mesh)
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


/*****************
 * SCENEGRAPHROOT
 * ************* */

/*
 * Scene graph root creation from aiScene
 * */
SceneGraphRoot::SceneGraphRoot(const aiScene *ai_scene, glm::mat4 &global_inverse_transform, std::string &path, std::vector<Model *> (&scene_models)[NB_SHADER_TYPES], GLfloat &render_time) :
    SceneGraphNode(path, global_inverse_transform)
{
    //  Process root node
    //  Fills the mesh vector
    processNode(0, m_global_inverse_transform, ai_scene->mRootNode, ai_scene, scene_models, &m_animated_models, render_time);

    if(ai_scene->HasAnimations())
    {
        //  Build bone trees of models
        for(GLuint i = 0; i < m_animated_models.size(); ++i)
            m_animated_models[i]->buildBoneTree(ai_scene->mRootNode);

        //  Load and store animations
        loadAnimations(ai_scene, m_animated_models);
    }
}

SceneGraphRoot::~SceneGraphRoot()
{
    //  Animations suppression
    for(GLuint i = 0; i < m_animations.size(); ++i)
        delete m_animations[i];

    m_animations.clear();
}

/*
 * Load animations from an aiScene
 * fill informations of models inside the current SceneGraph
 * */
void SceneGraphRoot::loadAnimations(const aiScene *ai_scene, const std::vector<AnimatedModel *> &animated_models)
{
    /*
     * This loop will store animation pose
     * for each tick of every animation
     * So we don't have to traverse the aiScene tree at runtime
     * (I've chosen to iterate over animation and traverse tree
     * than traverse tree once and iterate over animations
     * as it is more current to have 1 animation per file)
     * */
    for(GLuint animation_index = 0; animation_index < ai_scene->mNumAnimations; ++animation_index)
    {
        //  Get animation parameters
        GLfloat animation_time = ai_scene->mAnimations[animation_index]->mDuration;
        GLfloat ticks_per_seconds = ai_scene->mAnimations[animation_index]->mTicksPerSecond != 1?ai_scene->mAnimations[0]->mTicksPerSecond : 24.0f;
        //  Total time of the animation in ticks
        GLuint time_in_ticks = animation_time * ticks_per_seconds;
        GLfloat seconds_per_tick = 1.f / ticks_per_seconds;

        aiAnimation *ai_animation = ai_scene->mAnimations[animation_index];

        //  For each tick of the animation
        //  Retrieve animation pose
        for(GLuint current_tick = 0; current_tick < time_in_ticks; ++current_tick)
            processAnimation(ai_animation, "default", ticks_per_seconds, current_tick, current_tick * seconds_per_tick, time_in_ticks, ai_scene->mRootNode, animated_models);
    }
}

/*
 *  Process aiNode - each node is a bone
 *
 *  for the current animation and the current time in ticks
 *  create interpolated transformation matrix
 *  store matrix into concerned animated meshes
 * */
void SceneGraphRoot::processAnimation(const aiAnimation *ai_animation, const std::string &animation_name, const GLfloat &ticks_per_second, const GLuint &current_tick, const GLfloat &current_time, const GLuint &time_in_ticks, const aiNode *ai_node, const std::vector<AnimatedModel *> &animated_models)
{
    std::string bone_name = ai_node->mName.data;

    //  Try to find the bone inside animation
    const aiNodeAnim *ai_node_anim = FindNodeAnim(ai_animation, bone_name);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    GLfloat time;

    //  If bone found
    if(ai_node_anim)
    {
        scale = glm::vec3(ai_node_anim->mScalingKeys[current_tick].mValue.x, ai_node_anim->mScalingKeys[current_tick].mValue.y, ai_node_anim->mScalingKeys[current_tick].mValue.z);
        rotation = glm::quat(ai_node_anim->mRotationKeys[current_tick].mValue.w, ai_node_anim->mRotationKeys[current_tick].mValue.x, ai_node_anim->mRotationKeys[current_tick].mValue.y, ai_node_anim->mRotationKeys[current_tick].mValue.z);
        position = glm::vec3(ai_node_anim->mPositionKeys[current_tick].mValue.x, ai_node_anim->mPositionKeys[current_tick].mValue.y, ai_node_anim->mPositionKeys[current_tick].mValue.z);
        time = ai_node_anim->mPositionKeys[current_tick].mTime;
    }
    else
    {
        scale = glm::vec3(1.f);
        rotation = glm::quat(0.f, 0.f, 0.f, 0.f);
        position = glm::vec3(0.f);
        time = 0.f;
    }

    Channel current_channel;
    current_channel.position = position;
    current_channel.rotation = rotation;
    current_channel.scale = scale;
    current_channel.time = time;

    //  Loop over every animated model
    for(GLuint i = 0; i < animated_models.size(); ++i)
    {
        //  If current model has the current bone
        //  Send animation datas (anim info + channel)
        if(animated_models[i]->hasBone(bone_name))
        {
            animated_models[i]->setAnimationInfo(animation_name, time_in_ticks, ticks_per_second);
            animated_models[i]->setChannel(animation_name, bone_name, current_tick, current_channel);
        }
    }

    //  Loop over children bones
    for(GLuint i = 0 ; i < ai_node->mNumChildren ; ++i)
       processAnimation(ai_animation, animation_name, ticks_per_second, current_tick, current_time, time_in_ticks, ai_node->mChildren[i], animated_models);
}

/*
 *  Return aiNodeAnim with the name node_name
 * */
const aiNodeAnim* SceneGraphRoot::FindNodeAnim(const aiAnimation *ai_animation, const std::string &node_name)
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
