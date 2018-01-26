#include "core/models/renderingquad.h"

RenderingQuad::RenderingQuad(const GLfloat &fSize) :
    Model::Model(),
    m_fSize(fSize)
{
    const glm::vec3 vNormal = glm::vec3(0.f, 0.f, 0.f, 1.f);
    const glm::vec3 vTangent = glm::vec3(1.f, 0.f, 0.f, 0.f);
    const glm::vec3 vBitangent = glm::vec3(0.f, 0.f, 1.f, 0.f);
    m_aVertices.push_back({glm::vec3(-m_fSize, m_fSize, 0.f), vNormal, glm::vec2(0.f, 1.f), vTangent, vBitangent});
    m_aVertices.push_back({glm::vec3(-m_fSize, -m_fSize, 0.f), vNormal, glm::vec2(0.f, 0.f), vTangent, vBitangent});
    m_aVertices.push_back({glm::vec3(m_fSize, m_fSize, 0.f), vNormal, glm::vec2(1.f, 1.f), vTangent, vBitangent});
    m_aVertices.push_back({glm::vec3(m_fSize, -m_fSize, 0.f), vNormal, glm::vec2(1.f, 0.f), vTangent, vBitangent});

    setupBuffers();
}

void RenderingQuad::draw() const
{
    glBindVertexArray(m_uiVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderingQuad::setupBuffers()
{
    glGenVertexArrays(1, &m_uiVAO);
    glGenBuffers(1, &m_uiVBO);
    glBindVertexArray(m_uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
    glBufferData(GL_ARRAY_BUFFER, m_aVertices.size() * sizeof(Vertex), &m_aVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, vNormal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, vUV));
}
