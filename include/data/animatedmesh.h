#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include <vector>
#include <cstring>

#include "openglincludes.h"
#include "mesh.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

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

#endif // ANIMATEDMESH_H
