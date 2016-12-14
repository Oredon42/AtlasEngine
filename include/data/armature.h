#ifndef ARMATURE_H
#define ARMATURE_H

#include <map>

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "lib/assimp/Importer.hpp"
#include "lib/assimp/scene.h"
#include "lib/assimp/postprocess.h"

#define NUM_BONES_PER_VERTEX 4
#define MAX_BONES 100

class Shader;
class Animation;

struct Bone
{
    GLuint parent;
    std::string name;
    std::vector<GLuint> children;

    glm::mat4 bone_offset;
    glm::mat4 final_transformation;

    GLboolean used_in_animation;

    Bone()
    {
        parent = 0;
        bone_offset = glm::mat4();
        final_transformation = glm::mat4();
        used_in_animation = GL_FALSE;
    }
};

/*
 * Bone ID and weight for each vertex
 * */
struct VertexBoneData
{
    GLuint IDs[NUM_BONES_PER_VERTEX];
    GLfloat weights[NUM_BONES_PER_VERTEX];

    VertexBoneData()
    {
        Reset();
    }

    void Reset()
    {
        memset(IDs, 0, sizeof(IDs));
        memset(weights, 0, sizeof(weights));
    }

    /*
     * Add new bone data (bone ID + weight) inside arrays
     * if the number of bones already added equals NUM_BONES_PER_VERTEX
     * then nothing is added
     * */
    void AddBoneData(const GLuint &boneID, const GLfloat &weight)
    {
        for(GLuint i = 0 ; i < NUM_BONES_PER_VERTEX; ++i)
        {
            //  1st empty data in bone array
            if(weights[i] == 0.0)
            {
                IDs[i] = boneID;
                weights[i] = weight;
                return;
            }
        }
    }
};

class Armature
{
public:
    Armature(std::map<std::string, GLuint> bone_mapping, GLuint num_bones, Bone *bone_tree, VertexBoneData *vertex_bone_data);
    ~Armature();

    void update(const Shader &shader, const Animation *animation, const GLfloat &render_time);
    void updateBone(const Animation *animation, const GLuint &bone_index, const glm::mat4 &parent_transform, const GLfloat &render_time);

    //  Getters
    inline VertexBoneData *getVertexBoneData() const{return m_vertex_bone_data;}
    inline GLuint numberOfBones() const{return m_num_bones;}
    inline GLuint getBoneIndex(const std::string &bone_name){return m_bone_mapping[bone_name];}
    inline GLboolean hasBone(const std::string &bone_name) const{return m_bone_mapping.find(bone_name) != m_bone_mapping.end();}

private:
    GLuint m_bone_location[MAX_BONES];
    std::map<std::string, GLuint> m_bone_mapping;
    GLuint m_num_bones;
    Bone *m_bone_tree;
    VertexBoneData *m_vertex_bone_data;
};

#endif // ARMATURE_H
