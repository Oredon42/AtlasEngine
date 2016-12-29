#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "openglincludes.h"
#include "pointlight.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class SpotLight : public PointLight
{
public:
    SpotLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cut_off, const GLfloat &outer_cut_off);

    void init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position, const glm::vec3 &direction, const GLfloat &cut_off, const GLfloat &outer_cut_off);

    virtual void sendViewDatas(const Shader &shader, const glm::mat4 &view) const;
    virtual void sendDatas(const Shader &shader) const;

    //  Getters
    inline glm::vec3 getDirection() const{return m_direction;}
    inline GLfloat getCutOff() const{return m_cut_off;}
    inline GLfloat getOuterCutOff() const{return m_outer_cut_off;}

private:
    glm::vec3 m_direction;

    GLfloat m_cut_off;
    GLfloat m_outer_cut_off;
};

#endif // SPOTLIGHT_H
