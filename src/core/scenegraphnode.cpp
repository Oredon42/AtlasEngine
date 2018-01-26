#include "core/data/scenegraphnode.h"

#include "core/data/animation.h"
#include "core/data/camera.h"
#include "core/data/dirlight.h"
#include "core/data/model.h"
#include "core/data/pointlight.h"
#include "core/data/spotlight.h"

#include "glm/gtc/quaternion.hpp"

#include <string>

SceneGraphNode::SceneGraphNode(glm::mat4 mGlobalInverseTransform) :
    m_pParent(0),
    m_mGlobalInverseTransform(mGlobalInverseTransform)
{

}

SceneGraphNode::~SceneGraphNode()
{
    for(size_t i = 0; i < m_apDirlights.size(); ++i)
    {
        delete m_apDirlights[i];
    }
    m_apDirlights.clear();
    for(size_t i = 0; i < m_apPointlights.size(); ++i)
    {
        delete m_apPointlights[i];
    }
    for(size_t i = 0; i < m_apSpotlights.size(); ++i)
    {
        delete m_apSpotlights[i];
    }
    m_apSpotlights.clear();
    for(size_t i = 0; i < m_apModels.size(); ++i)
    {
        delete m_apModels[i];
    }
    m_apModels.clear();

    for(std::unordered_map<std::string, SceneGraphNode *>::iterator it = m_apChildren.begin(); it != m_apChildren.end(); ++it)
    {
        delete it->second;
    }
    m_apChildren.clear();
}

void SceneGraphNode::addModel(Model *pModel)
{
    m_apModels[pModel->getShaderTypeIndex()].push_back(pModel);
}

void SceneGraphNode::addChild(SceneGraphNode *pChild, const std::string &strKey)
{
    pChild->m_pParent = this;
    pChild->m_mGlobalInverseTransform = this->m_mGlobalInverseTransform;
    pChild->spreadTransform();

    if(m_apChildren.find(strKey) != m_apChildren.end())
    {
        delete m_apChildren[strKey];
    }
    m_apChildren[strKey] = pChild;
}

SceneGraphNode *SceneGraphNode::getChild(const std::string &strKey)
{
    if(m_apChildren.empty() == false)
    {
        //  Child found in direct children
        if(m_apChildren.find(strKey) != m_apChildren.end())
        {
            return m_apChildren[strKey];
        }
        else
        {
            //  Look for it in progeny
            SceneGraphNode *pFoundNode = 0;
            std::unordered_map<std::string, SceneGraphNode *>::iterator it = m_apChildren.begin();

            while(it != m_apChildren.end())
            {
                pFoundNode = it->second->getChild(strKey);
                if(pFoundNode != 0)
                {
                    return pFoundNode;
                }
                ++it;
            }
        }
    }

    return 0;
}

virtual void SceneGraphNode::setTransform(const glm::mat4 &mTransform)
{
    Transform::setTransform(mTransform);
    spreadTransform();
}


virtual void SceneGraphNode::translate(const glm::vec3 &vPosition)
{
    Transform::translate(vPosition);
    spreadTransform();
}

virtual void SceneGraphNode::rotate(const glm::quat &qRotation)
{
    Transform::rotate(qRotation);
    spreadTransform();
}

virtual void SceneGraphNode::scale(const glm::vec3 &vScale)
{
    Transform::scale(vScale);
    spreadTransform();
}

virtual void SceneGraphNode::transform(const glm::mat4 &mTransform)
{
    Transform::transform(mTransform);
    spreadTransform();
}

/*
 * Send Transform matrix to data
 * and spread it to children
 * */
void SceneGraphNode::spreadTransform()
{
    glm::mat4 mTransform = m_mTransform;
    mTransform *= m_pParent->getTransform();

    for(size_t i = 0; i < m_apModels.size(); ++i)
    {
        m_apModels[i]->setTransform(mTransform);
    }

    for(size_t i = 0; i < m_apChildren.size(); ++i)
    {
        m_apChildren[i]->spreadTransform();
    }
}
