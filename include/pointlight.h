#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class PointLight
{
public:
    PointLight();
    PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic, const GLfloat &intensity);
    PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &diffuse, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic);
    PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &diffuse);
    void init(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const GLfloat &constant, const GLfloat &linear, const GLfloat &quadratic);

    void sendDatas(const Shader &shader) const;

    //  Getters
    inline glm::vec3 getPosition() const{return m_position;}
    inline glm::vec3 getAmbient() const{return m_ambient;}
    inline glm::vec3 getDiffuse() const{return m_diffuse;}
    inline glm::vec3 getSpecular() const{return m_specular;}
    inline GLfloat getConstant() const{return m_constant;}
    inline GLfloat getLinear() const{return m_linear;}
    inline GLfloat getQuadratic() const{return m_quadratic;}

private:
    GLuint m_indice;

    glm::vec3 m_position;

    GLfloat m_constant;
    GLfloat m_linear;
    GLfloat m_quadratic;

    GLfloat m_intensity;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
};

#endif // POINTLIGHT_H
