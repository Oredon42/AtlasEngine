#include "include/data/geometry/mesh.h"
#include "include/data/animation/armature.h"

Mesh::Mesh() :
    m_VAO(0),
    m_VBO(0),
    m_EBO(0),
    m_has_normal_map(GL_FALSE)
{

}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, GLboolean has_normal_map) :
    m_VAO(0),
    m_VBO(0),
    m_EBO(0),
    m_bone_buffer(0),
    m_has_normal_map(has_normal_map)
{
    m_vertices = vertices;
    m_indices = indices;

    computeAABB();

    setupBuffers();
}

Mesh::~Mesh()
{

}

void Mesh::draw() const
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
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

void Mesh::attachArmature(const VertexBoneData *vertex_bone_data)
{
    if(m_bone_buffer != 0)
        detachArmature();

    glBindVertexArray(m_VAO);

    //  Vertex bones datas
    glGenBuffers(1, &m_bone_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_bone_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_bone_data[0]) * m_indices.size(), &vertex_bone_data[0], GL_STATIC_DRAW);

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

void Mesh::detachArmature()
{
    if(m_bone_buffer != 0)
    {
        glDeleteBuffers(1, &m_bone_buffer);
        m_bone_buffer = 0;
    }
}

void Mesh::computeAABB()
{
    for(GLuint i = 0; i < m_vertices.size(); ++i)
        m_box.clipPoint(m_vertices[i].Position);
}
