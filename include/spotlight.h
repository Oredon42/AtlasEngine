#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class SpotLight
{
public:
    SpotLight();
    SpotLight(const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cutOff, const GLfloat &outerCutOff, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic);

    void init(const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cutOff, const GLfloat &outerCutOff, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic);

    void sendDatas(const Shader &shader) const;
    void updatePosDir(const glm::vec3 &position, const glm::vec3 &direction);

    //  Getters
    inline glm::vec3 getPosition() const{return m_position;}
    inline glm::vec3 getDirection() const{return m_direction;}
    inline GLfloat getCutOff() const{return m_cutOff;}
    inline GLfloat getOuterCutOff() const{return m_outerCutOff;}
    inline GLfloat getConstant() const{return m_constant;}
    inline GLfloat getLinear() const{return m_linear;}
    inline GLfloat getQuadratic() const{return m_quadratic;}
    inline glm::vec3 getAmbient() const{return m_ambient;}
    inline glm::vec3 getDiffuse() const{return m_diffuse;}
    inline glm::vec3 getSpecular() const{return m_specular;}
    inline GLboolean isInitialised() const{return m_initialised;}

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;

    GLfloat m_cutOff;
    GLfloat m_outerCutOff;

    GLfloat m_constant;
    GLfloat m_linear;
    GLfloat m_quadratic;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    GLboolean m_initialised;
};

#endif // SPOTLIGHT_H
