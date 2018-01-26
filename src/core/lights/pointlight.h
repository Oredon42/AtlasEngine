#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "core/a_light.h"

class PointLight : public ALight
{
public:
    PointLight(const glm::vec3 &vColor, const GLfloat &fIntensity, const glm::vec3 &vPosition);
    virtual ~PointLight(){}

protected:
    glm::vec3 m_vPosition;

    glm::mat4 m_amShadowTransforms[6];
};

#endif // POINTLIGHT_H
