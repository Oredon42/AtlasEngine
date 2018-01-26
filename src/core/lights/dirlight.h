#ifndef DIRLIGHT_H
#define DIRLIGHT_H

#include "core/a_light.h"

class DirLight : public ALight
{
public:
    DirLight(const glm::vec3 &vColor, const GLfloat &fIntensity, const glm::vec3 &vDirection);
    virtual ~DirLight(){}

    //  Getters
    inline glm::vec3 getDirection() const{return m_vDirection;}

private:
    glm::vec3 m_vDirection;
    glm::mat4 m_mLightSpaceMatrix;
};

#endif // DIRLIGHT_H
