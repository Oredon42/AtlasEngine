#ifndef SCENE_H
#define SCENE_H

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "scenegraphnode.h"
#include "camera.h"
#include "shader.h"
#include "pointlight.h"
#include "dirlight.h"
#include "spotlight.h"
#include <string>

class Model;
class Skybox;

class Scene
{
public:
    Scene(std::string &path, GLfloat &render_time);
    ~Scene();

    void init(const std::string &path);
    void clear();

    void drawForward(const Shader (&shaders)[NB_SHADER_TYPES], const GLuint &window_width, const GLuint &window_height, const GLfloat &render_time);
    void drawDeferred(const Shader (&shaders)[NB_SHADER_TYPES], GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height);

    void buildKdTree();

    void sendViewSpacePointLightDatas(const Shader &shader);

    inline void updateCamera(const GLfloat &xoffset, const GLfloat &yoffset){m_cameras[m_current_camera]->setOffset(xoffset, yoffset);}
    inline void addDirLight(const glm::vec3 &direction, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular){m_dirlights.push_back(DirLight(direction, ambient, diffuse, specular));}
    inline void addDirLight(const DirLight &d){m_dirlights.push_back(d);}
    inline void addSpotLight(const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cutOff, const GLfloat &outerCutOff, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic){m_spotlights.push_back(SpotLight(position, direction, cutOff, outerCutOff, ambient, diffuse, specular, constant, linear, quadratic));}
    inline void addSpotLight(const SpotLight &s){m_spotlights.push_back(s);}
    inline void addPointLight(const glm::vec3 &position, const glm::vec3 &color, const GLfloat &intensity){m_pointlights.push_back(PointLight(m_pointlights.size(), position, color, intensity));}
    inline void addPointLight(const PointLight &p){m_pointlights.push_back(p);}
    inline void addCamera(const glm::vec3 &pos, const glm::vec3 &front, const glm::vec3 &up, const GLfloat &speed, const GLfloat &fov){m_cameras.push_back(new Camera(pos, front, up, speed, fov));}
    inline void addCamera(Camera *c){m_cameras.push_back(c);}

    //  Getters
    inline GLfloat &getRenderTime(){return m_render_time;}
    inline Model *getModel(const GLuint &shader_index, const GLuint &model_index) const{return m_models[shader_index][model_index];}
    inline SceneGraphRoot *getLastRoot()const{return m_roots[m_roots.size() - 1];}
    inline SceneGraphRoot *getRoot(const GLuint &k) const{return m_roots[k];}
    inline glm::vec3 getBackgroundColor() const{return m_background_color;}
    inline std::vector<PointLight> getPointLights() const{return m_pointlights;}
    inline std::string getPath() const{return m_path;}
    inline Camera *getCurrentCamera() const{return m_cameras[m_current_camera];}
    inline GLuint numberOfRoots() const{return m_roots.size();}
    inline GLuint numberOfPointLights() const{return m_pointlights.size();}
    inline GLuint numberOfDirights() const{return m_dirlights.size();}
    inline GLuint numberOfSpotLights() const{return m_spotlights.size();}
    inline GLuint numberOfModels(const GLuint &shader_index) const{return m_models[shader_index].size();}

    //  Setters
    inline void setBackgroundColor(const glm::vec3 &background_color){m_background_color = background_color;}
    inline void addSceneGraphRoot(SceneGraphRoot *s){m_roots.push_back(s);}
    inline void sendPointLightDatas(const GLuint &i, const Shader &shader){m_pointlights[i].sendDatas(shader);}
    inline void sendDirLightDatas(const GLuint &i, const Shader &shader){m_dirlights[i].sendDatas(shader);}
    inline void sendSpotLightDatas(const GLuint &i, const Shader &shader){m_spotlights[i].sendDatas(shader);}
    inline void sendCameraToShader(const Shader &shader, const GLfloat &screen_width, const GLfloat &screen_height){m_cameras[m_current_camera]->sendDatas(shader, screen_width, screen_height);}
    inline void moveCamera(GLboolean (&keys)[1024], const GLfloat &render_time){m_cameras[m_current_camera]->orientate(); m_cameras[m_current_camera]->move(keys, render_time);}
    inline void insertModel(Model *model, const GLuint &shader_index){m_models[shader_index].push_back(model);}

    inline void translate(const glm::vec3 &t, const std::string &node_name){m_roots[m_roots.size() - 1]->translate(t, node_name);}
    inline void rotate(const glm::vec3 &r, const std::string &node_name){m_roots[m_roots.size() - 1]->rotate(r, node_name);}
    inline void scale(const glm::vec3 &s, const std::string &node_name){m_roots[m_roots.size() - 1]->scale(s, node_name);}

protected:
    std::string &m_path;
    std::vector<PointLight> m_pointlights;
    std::vector<DirLight> m_dirlights;
    std::vector<SpotLight> m_spotlights;
    std::vector<Camera *> m_cameras;
    std::vector<SceneGraphRoot *> m_roots;
    std::vector<Model *> m_models[NB_SHADER_TYPES];

    Skybox *m_skybox;
    glm::vec3 m_background_color;

    GLuint m_current_camera;

    GLfloat &m_render_time;
};

#endif // SCENE_H
