#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <unordered_map>

#include <glew.h>
#include <glm/glm.hpp>

class Camera;
class Light;
class Model;
class SceneGraphNode;

class Scene
{
public:
    Scene();
    ~Scene();

    void update(const float &fRenderTime, const GLboolean (&keys)[1024]);

    //  Getters
    inline glm::vec3 getBackgroundColor() const{return m_vBackgroundColor;}
    inline Camera *getCamera(const std::string &strKey) const;
    inline Model *getModel(const std::string &strKey) const;
    inline Light *getLight(const std::string &strKey) const;
    inline Camera *getCurrentCamera() const{return m_pCurrentCamera;}

    inline size_t numberOfCameras() const{return m_mapCameras.size();}
    inline size_t numberOfLights() const{return m_mapLights.size();}
    inline size_t numberOfModels() const{return m_mapModels.size();}
    inline size_t numberOfRoots() const{return m_mapRoots.size();}

    //  Setters
    inline void setBackgroundColor(const glm::vec3 &vBackgroundColor) {m_vBackgroundColor = vBackgroundColor;}
    void setCurrentCamera(const std::string &strKey);

    void addCamera(const std::string &strKey, Camera *pCamera);
    void addLight(const std::string &strKey, Light *pLight);
    void addRoot(const std::string &strKey, SceneGraphNode *pSceneGraphNode);

protected:
    glm::vec3 m_vBackgroundColor;

    std::unordered_map<std::string, Camera *> m_mapCameras;
    std::unordered_map<std::string, Light *> m_mapLights;
    std::unordered_map<std::string, Model *> m_mapModels;
    std::unordered_map<std::string, SceneGraphNode *> m_mapRoots;

    Camera *m_pCurrentCamera;
};

#endif // SCENE_H
