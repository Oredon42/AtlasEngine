#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cstring>

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

struct VertexBoneData;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    Vertex()
    {

    }

    Vertex(const glm::vec3 &p, const glm::vec2 &tc) :
        Position(p),
        Normal(0.f),
        TexCoords(tc),
        Tangent(0.f),
        Bitangent(0.f)
    {

    }
};

class Mesh
{
public:
    Mesh();
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, GLboolean has_normal_map = GL_FALSE);
    virtual ~Mesh();

    virtual void draw() const;
    virtual void setupBuffers();

    void attachArmature(const VertexBoneData *vertex_bone_data);
    void detachArmature();

    //  Getters
    inline GLuint numIndices() const{return m_indices.size();}
    inline GLuint numVertices() const{return m_vertices.size();}
    inline GLuint getIndex(const GLuint &i) const{return m_indices[i];}
    inline Vertex getVertex(const GLuint &i) const{return m_vertices[i];}

protected:
    GLuint m_VAO, m_VBO, m_EBO, m_bone_buffer;

    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    GLboolean m_has_normal_map;
};

#endif // MESH_H
