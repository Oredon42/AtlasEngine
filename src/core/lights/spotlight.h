#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "core/a_light.h"

class SpotLight : public ALight
{
public:
    SpotLight(const glm::vec3 &vColor, const GLfloat &fIntensity, const glm::vec3 &vPosition, const glm::vec3 &vDirection, const GLfloat &fInnerCutOff, const GLfloat &fOuterCutOff);
    virtual ~SpotLight(){}

    //  Getters
    inline glm::vec3 getDirection() const{return m_vDirection;}
    inline GLfloat getInnerCutOff() const{return m_fInnerCutOff;}
    inline GLfloat getOuterCutOff() const{return m_fOuterCutOff;}

private:
    glm::vec3 m_vPosition;
    glm::vec3 m_vDirection;

    GLfloat m_fInnerCutOff;
    GLfloat m_fOuterCutOff;

    glm::mat4 m_mLightSpaceMatrix;
};

#endif // SPOTLIGHT_H
