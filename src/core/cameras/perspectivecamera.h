#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "core/a_camera.h"

class Shader;

class PerspectiveCamera : public ACamera
{
public:
    PerspectiveCamera();
    PerspectiveCamera(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp,const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fFov = 45.5f, const GLfloat &fAspect = 16.f/9.f);

    //  Getters
    inline GLfloat getFov() const{return m_fFov;}
    inline GLfloat getAspect() const{return m_fAspect;}

    //  Setters
    void setFov(const GLfloat &fFov);
    void setAspect(const GLfloat &fAspect);
    void setProjectionData(const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fFov, const GLfloat &fAspect);

protected:
    GLfloat m_fFov;
    GLfloat m_fAspect;

    //  Protected methods
    virtual void updateProjection();
};

#endif // PERSPECTIVECAMERA_H
