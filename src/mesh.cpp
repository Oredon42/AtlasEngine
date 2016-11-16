#include "include/mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const GLboolean &has_normal_map) :
    m_VAO(0),
    m_VBO(0),
    m_EBO(0),
    m_has_normal_map(has_normal_map)
{
    m_vertices = vertices;
    m_indices = indices;
}

Mesh::~Mesh()
{

}

void Mesh::setupBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    if(m_has_normal_map)
    {
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
    }

    glBindVertexArray(0);
}


/*****************
 * ANIMATED MESH
 * ************* */

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
