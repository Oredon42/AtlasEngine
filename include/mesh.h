#ifndef MESH_H
#define MESH_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const GLboolean &has_normal_map);
    virtual ~Mesh();

    void draw();
    virtual void setupBuffers();

protected:
    GLuint m_VAO, m_VBO, m_EBO;

    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    GLboolean m_has_normal_map;
};

/******************
 * ANIMATED MESH
 * ************** */

#define NUM_BONES_PER_VERTEX 4

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

class AnimatedMesh : public Mesh
{
public:
    AnimatedMesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const GLboolean &has_normal_map, VertexBoneData *vertex_bone_data);
    virtual ~AnimatedMesh();

    virtual void setupBuffers();

private:
    GLuint m_bone_buffer;

    VertexBoneData *m_vertex_bone_data;
};


#endif // MESH_H
