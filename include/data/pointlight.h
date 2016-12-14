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
    PointLight(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &color, const GLfloat &intensity);
    void init(const GLuint &indice, const glm::vec3 &position, const glm::vec3 &color, const GLfloat &intensity);

    void sendViewDatas(const Shader &shader, const glm::mat4 &view) const;
    void sendDatas(const Shader &shader) const;

    //  Getters
    inline glm::vec3 getPosition() const{return m_position;}
    inline glm::vec3 getColor() const{return m_color;}
    inline GLfloat getConstant() const{return m_constant;}
    inline GLfloat getLinear() const{return m_linear;}
    inline GLfloat getQuadratic() const{return m_quadratic;}

private:
    GLuint m_indice;

    glm::vec3 m_position;

    GLfloat m_intensity;

    GLfloat m_constant;
    GLfloat m_linear;
    GLfloat m_quadratic;

    glm::vec3 m_color;
};

#endif // POINTLIGHT_H
