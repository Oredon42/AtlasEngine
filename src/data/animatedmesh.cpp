#include "include/data/animatedmesh.h"

AnimatedMesh::AnimatedMesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const GLboolean &has_normal_map, VertexBoneData *vertex_bone_data) :
    Mesh(vertices, indices, has_normal_map)
{
    m_vertex_bone_data = vertex_bone_data;
}

AnimatedMesh::~AnimatedMesh()
{
    delete[] m_vertex_bone_data;
}

void AnimatedMesh::setupBuffers()
{
    Mesh::setupBuffers();

    glBindVertexArray(m_VAO);

    //  Vertex bones datas
    glGenBuffers(1, &m_bone_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_bone_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex_bone_data[0]) * m_indices.size(), &m_vertex_bone_data[0], GL_STATIC_DRAW);

    if(m_has_normal_map)
    {
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    }
    else
    {
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    }

    glBindVertexArray(0);
}
