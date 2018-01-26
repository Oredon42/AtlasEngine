#include "core/scene.h"

#include "core/camera.h"
#include "core/light.h"
#include "core/model.h"
#include "core/scenegraphnode.h"

Scene::Scene() :
    m_pCurrentCamera(nullptr)
{

}

Scene::~Scene()
{
    for(std::unordered_map<std::string, Camera *>::iterator it = m_mapCameras.begin(); it != m_mapCameras.end(); ++it)
    {
        delete it->second;
    }
    m_mapCameras.clear();

    for(std::unordered_map<std::string, Light *>::iterator it = m_mapLights.begin(); it != m_mapLights.end(); ++it)
    {
        delete it->second;
    }
    m_mapLights.clear();

    for(std::unordered_map<std::string, Model *>::iterator it = m_mapModels.begin(); it != m_mapModels.end(); ++it)
    {
        delete it->second;
    }
    m_mapModels.clear();

    for(std::unordered_map<std::string, SceneGraphNode *>::iterator it = m_mapRoots.begin(); it != m_mapRoots.end(); ++it)
    {
        delete it->second;
    }
    m_mapRoots.clear();

    m_pCurrentCamera = nullptr;
}

void Scene::update(const float &fRenderTime, const GLboolean (&keys)[1024])
{
    m_pCurrentCamera->move(fRenderTime, keys);
}

Camera *Scene::getCamera(const std::string &strKey) const
{
    if(m_mapCameras.find(strKey) != m_mapCameras.end())
    {
        return m_mapCameras[strKey];
    }
    else
    {
        return nullptr;
    }
}

Model *Scene::getModel(const std::string &strKey) const
{
    if(m_mapModels.find(strKey) != m_mapModels.end())
    {
        return m_mapModels[strKey];
    }
    else
    {
        return nullptr;
    }
}

Light *Scene::getLight(const std::string &strKey) const
{
    if(m_mapLights.find(strKey) != m_mapLights.end())
    {
        return m_mapLights[strKey];
    }
    else
    {
        return nullptr;
    }
}

void Scene::setCurrentCamera(const std::string &strKey)
{
    if(m_mapCameras.find(strKey) != m_mapCameras.end())
    {
        m_pCurrentCamera = m_mapCameras[strKey];
    }
}

void Scene::addCamera(const std::string &strKey, Camera *pCamera)
{
    if(pCamera != nullptr && m_mapCameras.find(strKey) == m_mapCameras.end())
    {
        m_mapCameras[strKey] = pCamera;
    }
}

void Scene::addLight(const std::string &strKey, Light *pLight)
{
    if(pLight != nullptr && m_mapLights.find(strKey) == m_mapLights.end())
    {
        m_mapLights[strKey] = pLight;
    }
}

void Scene::addRoot(const std::string &strKey, SceneGraphNode *pSceneGraphNode)
{
    if(pSceneGraphNode != nullptr && m_mapRoots.find(strKey) == m_mapRoots.end())
    {
        m_mapRoots[strKey] = pSceneGraphNode;
    }
}
