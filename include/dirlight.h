#ifndef DIRLIGHT_H
#define DIRLIGHT_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class DirLight
{
public:
    DirLight();
    DirLight(const glm::vec3 &direction, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular);

    void init(const glm::vec3 &direction, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular);

    void sendDatas(const Shader &shader) const;

    //  Getters
    inline glm::vec3 getDirection() const{return m_direction;}
    inline glm::vec3 getAmbient() const{return m_ambient;}
    inline glm::vec3 getDiffuse() const{return m_diffuse;}
    inline glm::vec3 getSpecular() const{return m_specular;}
    inline GLboolean isInitialised() const{return m_initialised;}

private:
    glm::vec3 m_direction;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    GLboolean m_initialised;
};

#endif // DIRLIGHT_H
