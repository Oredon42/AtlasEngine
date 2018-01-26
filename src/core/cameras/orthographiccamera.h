#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "core/a_camera.h"

class Shader;

class OrthographicCamera : public ACamera
{
public:
    OrthographicCamera();
    OrthographicCamera(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp, const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fLeft = 10.f, const GLfloat &fRight = 10.f, const GLfloat &fBottom = 10.f, const GLfloat &fTop = 10.f);

    //  Getters
    inline float getLeft() const{return m_fLeft;}
    inline float getRight() const{return m_fLeft;}
    inline float getBottom() const{return m_fLeft;}
    inline float getTop() const{return m_fLeft;}

    //  Setters
    void setDimensions(const GLfloat &fLeft, const GLfloat &fRight, const GLfloat &fBottom, const GLfloat &fTop);
    void setProjectionData(const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fLeft, const GLfloat &fRight, const GLfloat &fBottom, const GLfloat &fTop);

protected:
    GLfloat m_fLeft;
    GLfloat m_fRight;
    GLfloat m_fBottom;
    GLfloat m_fTop;

    //  Protected methods
    virtual void updateProjection();
};

#endif // ORTHOGRAPHICCAMERA_H
