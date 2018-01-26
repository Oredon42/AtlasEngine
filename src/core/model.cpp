#include "core/model.h"

#include "core/animation.h"
#include "core/armature.h"
#include "core/material.h"
#include "core/shader.h"
#include "core/transform.h"

Model::Model() :
    m_uiVAO(0),
    m_uiVBO(0),
    m_uiEBO(0),
    m_uiBoneBO(0),
    m_pCurrentAnimation(nullptr),
    m_pArmature(nullptr),
    m_pMaterial(nullptr)
{
    m_pTransform = new Transform();
}

Model::Model(const std::vector<Vertex> &aVertices, const std::vector<GLuint> &aIndices, Material *pMaterial) :
    m_uiVAO(0),
    m_uiVBO(0),
    m_uiEBO(0),
    m_uiBoneBO(0),
    m_pCurrentAnimation(nullptr),
    m_pArmature(nullptr),
    m_pMaterial(pMaterial)
{
    m_pTransform = new Transform();

    m_aVertices = aVertices;
    m_aIndices = aIndices;

    setupBuffers();
}

Model::~Model()
{
    delete m_pTransform;

    if(m_uiVAO != 0)
    {
        glDeleteVertexArrays(1, &m_uiVAO);
    }

    if(m_uiVBO != 0)
    {
        glDeleteBuffers(1, &m_uiVBO);
    }

    if(m_uiEBO != 0)
    {
        glDeleteBuffers(1, &m_uiEBO);
    }

    if(m_uiBoneBO != 0)
    {
        glDeleteBuffers(1, &m_uiBoneBO);
    }

    for(std::unordered_map<std::string, Animation *>::iterator it = m_mapAnimations.begin(); it != m_mapAnimations.end(); ++it)
    {
        delete it->second;
    }
    m_mapAnimations.clear();

    m_pCurrentAnimation = nullptr;

    if(m_pArmature != nullptr)
    {
        delete m_pArmature;
        m_pArmature = nullptr;
    }

    if(m_pMaterial != nullptr)
    {
        delete m_pMaterial;
        m_pMaterial = nullptr;
    }
}

void Model::draw() const
{
    glBindVertexArray(m_uiVAO);
    glDrawElements(GL_TRIANGLES, m_aIndices.size(), GL_UNSIGNED_INT, static_cast<GLvoid *>(0));
}

void Model::setupBuffers()
{
    if(m_uiVAO == 0)
    {
        glGenVertexArrays(1, &m_uiVAO);
    }
    glBindVertexArray(m_uiVAO);

    if(m_uiVBO == 0)
    {
        glGenBuffers(1, &m_uiVBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
    glBufferData(GL_ARRAY_BUFFER, m_aVertices.size() * sizeof(Vertex), &m_aVertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid *>(offsetof(Vertex, vNormal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid *>(offsetof(Vertex, vUV)));

    if(m_pMaterial != 0 && m_pMaterial->hasNormalMap() == true)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid *>(offsetof(Vertex, vTangent)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<GLvoid *>(offsetof(Vertex, vBitangent)));
    }

    if(m_uiEBO == 0)
    {
        glGenBuffers(1, &m_uiEBO);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_aIndices.size() * sizeof(GLuint), &m_aIndices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Model::Vertex Model::getVertex(const unsigned int &i) const
{
    if(i < m_aVertices.size())
    {
        return m_aVertices[i];
    }
}

GLuint Model::getIndex(const unsigned int &i) const
{
    if(i < m_aIndices.size())
    {
        return m_aIndices[i];
    }
}

Animation *Model::getAnimation(const std::string &strKey) const
{
    if(m_mapAnimations.find(strKey) != m_mapAnimations.end())
    {
        return m_mapAnimations[strKey];
    }
    else
    {
        return nullptr;
    }
}

void Model::setTransform(const Transform &mTransform)
{
    *m_pTransform = mTransform;
}

//  TODO
void Model::setCurrentAnimation(const std::string &strAnimationName);

void Model::setArmature(Armature *pArmature)
{
    if(pArmature != nullptr && pArmature != m_pArmature)
    {
        if(m_pArmature != nullptr)
        {
            delete m_pArmature;
        }
        m_pArmature = pArmature;
        const VertexBoneData *pVertexBoneData = m_pArmature->getVertexBoneData();

        if(m_uiVAO == 0)
        {
            setupBuffers();
        }
        glBindVertexArray(m_uiVAO);

        //  Vertex bones data
        if(m_uiBoneBO == 0)
        {
            glGenBuffers(1, &m_uiBoneBO);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_uiBoneBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pVertexBoneData[0]) * m_aIndices.size(), &pVertexBoneData[0], GL_STATIC_DRAW);

        if(m_pMaterial != nullptr && m_pMaterial->hasNormalMap() == true)
        {
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), static_cast<const GLvoid*>0);
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), static_cast<const GLvoid*>16);
        }
        else
        {
            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), static_cast<const GLvoid*>(0));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), static_cast<const GLvoid*>16);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Model::setMaterial(Material *pMaterial)
{
    if(pMaterial != nullptr && pMaterial != m_pMaterial)
    {
        if(m_pMaterial != nullptr)
        {
            delete m_pMaterial;
        }

        m_pMaterial = pMaterial;
    }
}

//  TODO
void Model::addAnimation(Animation *pAnimation, const std::string &strAnimationName);
